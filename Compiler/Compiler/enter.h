#ifndef enter_h
#define enter_h
#include "headers.h"
#include "myStd.h"
#include "error.h"
void newTmp(string& res){
    res = "#" + int2string(++cnt_tmp);
}
void newLabel(int& res){
    res = (++cnt_label);
}
void enter(string name, string kind, string type, int value, int addr, int para_num, int line_pos){
    #ifdef constDelete
    if (kind == "const"){
        const_cnt[cnt_proc]++;
    }
    #endif
    if (kind == "void" || kind == "function") { //代表是过程或者函数
        rep (i, 1, cnt_proc){
            if (tab[index_proc[i]].name == name){ //重复定义函数
                error(100, line_pos);
                return ;
            }
        }
        index_proc[++cnt_proc] = cnt_tab+1; //分程序的索引表指向当前的这个符号表的位置(我们马上就会存这个信息了)，注意这个是从0开始存的
    }else{ //不是过程或者函数，说明是局部变量或者全局变量
        //如果是全局变量，其index_proc[0] = 0
        rep (i, index_proc[cnt_proc], cnt_tab){ //检查之前定义的参数和局部变量有没有名字相同的
            if (tab[i].name == name){
                error(101, line_pos);
                return ;
            }
        }
    }
    cnt_tab++;
    tab[cnt_tab].name = name;
    tab[cnt_tab].kind = kind;
    tab[cnt_tab].type = type;
    tab[cnt_tab].value = value;
    tab[cnt_tab].addr = addr;
    tab[cnt_tab].para_num = para_num;
    tab[cnt_tab].program_id = cnt_proc;
    #ifdef debug
    cout << "cnt_tab " << cnt_tab << " ::: " << name << " " << kind << " " << type << " " << value << " " << addr << " " << para_num << " &&& " << cnt_proc << endl;
    #endif // debug
}

int locateVariable(string name, int program_id, int& offset,bool is_mips=true){ //找到这个变量在哪里定义的，返回在符号表的位置，未找到则是-1 //默认参数，因为我是后来才加上要判断语法分析的合法性，因此
    int program_end = index_proc[program_id+1]-1;
    if (!is_mips)
        program_end = cnt_tab;
    if (is_mips && name.size() > 3 && name.substr(0,4) == "RET_"){ // "RET_int" or "RET_char"
        return -1;
    }
    if (isChar(name)){ //如果是一个字符
        return -4;
    }
    if (isNumber(name)){ //如果是一个整数
        return -3;
    }
    if (is_mips && name.size()>1 && name[0] == '#'){
        offset = mp_tmp[name].second - 1; //-1的目的是因为我们的栈顶指向的是函数存放的第一个参数，而由于之前存放在符号表的第一个是函数名，因此addr相当于多了一个，以此同理
        return 0;
    }
    rep (i, index_proc[program_id]+1, program_end){ //因为第一个是函数的名字，防止出现局部变量名和函数名同名的情况
        if (tab[i].name == name){
            offset = tab[i].addr - 1; //第0号是函数名，实际情况是第一个变量的偏移量是0 因此要减一
            return i;
        }
    }
    //代表所在程序的局部变量区没有，去全局区找一下
    rep (i, 1, index_proc[1]-1){
        if (tab[i].name == name){
            offset = tab[i].addr - 1;
            //            program_id = 0; //把原调用的函数的program_id的信息更新，代表在全局函数
            return i;
        }
    }
    if (name.size() >= 1 && name[0] == '#') //中间变量
        return -5;
    return -2;
}
string constValue(string s){
    int offset;
    int pos = locateVariable(s, cnt_proc, offset, false);
    if (pos <= 0)
        return s;
    if (tab[pos].kind == "const"){
        if (tab[pos].type == "char"){
            char c = tab[pos].value;
            string res;
            res.push_back('\'');
            res.push_back(c);
            res.push_back('\'');
            return res; //'C'
        }else
            return int2string(tab[pos].value);
    }
    return s;
}
void addQuat(string type, string op1, string op2, string op3){
    #ifdef debug
    cout << "QUAT!!!" << " " << type << " " << op1 << " " << op2 << " " << op3 << " ::: " << cnt_proc << endl;
    #endif // debug
    #ifdef constDelete
    if (type == "const_int" || type == "const_char")
        return;
    #endif
    ++cnt_quat;
    quat[cnt_quat].type = type;
    quat[cnt_quat].op1 = op1;
    quat[cnt_quat].op2 = op2;
    quat[cnt_quat].op3 = op3;
    #ifdef constDelete
    quat[cnt_quat].op1 = constValue(op1);
    quat[cnt_quat].op2 = constValue(op2);
    quat[cnt_quat].op3 = constValue(op3);
    #endif
    quat[cnt_quat].program_id = cnt_proc;
}
void printSide(ofstream& o_out){
    #ifdef debug
    o_out << "------------------------------" << endl;
    o_out << "------------------------------" << endl;
    o_out << "------------------------------" << endl;
    #endif // debug
}
void printQuat(ofstream& quat_out){ //struct Quat quat[MAX_QUAT],int cnt_quat
    if (!quat[cnt_quat+1].label.empty()){
        addQuat("nop", "", "", "");
    }
    rep (i,1,cnt_quat) {
        if (!quat[i].label.empty())// != -1)
            for (auto j: quat[i].label)//cout << "LABEL_" << quat[i].label << " ";
                quat_out << "LABEL_" << j << " ";
        switch (mp_quat[quat[i].type]){
            case 1000: quat_out << quat[i].type << endl; break;
            case 100: quat_out << quat[i].type << " " << quat[i].op1 << endl; break;
            case 1: quat_out << "const " << quat[i].type.substr(6, quat[i].type.size()-6) << " " << quat[i].op1 << " = " << quat[i].op2 << endl; break;
            case 3: quat_out << "var " << quat[i].type.substr(9, quat[i].type.size()-9) << " " << quat[i].op1 << endl; break;
            case 5: quat_out << quat[i].type.substr(9, quat[i].type.size()-9) << " " << quat[i].op1 << "()" << endl; break;
            case 7: quat_out << "para " << quat[i].type.substr(10, quat[i].type.size()-10) << " " << quat[i].op1 << endl; break;
            case 9: quat_out << "void " << quat[i].op1 << "()" << endl; break;
            case 10: quat_out << quat[i].op1 << " = " << quat[i].op2 << " " << quat[i].type << " " << quat[i].op3 << endl; break;
            case 20: quat_out << quat[i].op1 << " " << quat[i].type << " " << quat[i].op2 << endl; break;
            case 30: quat_out << quat[i].op1 << " = " << quat[i].op2 << "[" << quat[i].op3 << "]" << endl; break;
            case 40: quat_out << "var " << quat[i].type.substr(9,quat[i].type.size()-2-9) << " " << quat[i].op1 << "[" << quat[i].op2 << "]" << endl; break;
            case 50: quat_out << quat[i].type << endl;  break;
            case 60: quat_out << quat[i].type << "_" << quat[i].op2 << " " << quat[i].op1 << endl; break;
            default: quat_out << "@@@" << quat[i].type << " " << mp_quat[quat[i].type] << endl; quat_out << quat[i].type << " " << quat[i].op1 << " " << quat[i].op2 << " " << quat[i].op3 << endl;
        }
    }
}
void programTable(){
    printSide(symbol_table_out);//#ifdef debug
    rep (i,1,cnt_proc){
        symbol_table_out << "Program " << i << " :  " << index_proc[i] << endl;
    }
    symbol_table_out << "AddByMe Program " << cnt_proc+1 << " :  " << index_proc[cnt_proc+1] << endl;//#endif // debug
}
void symbolTable(){
    symbol_table_out << "      id name kind type value type addr para_num program_id" << endl;
    rep (i,1,cnt_tab){
        symbol_table_out << "Symbol  " << i << " :   " << tab[i].name << "   " << tab[i].kind << "   " << tab[i].type << "   " << tab[i].value << "   " << tab[i].addr << "   " << tab[i].para_num << "  " << tab[i].program_id << endl;
    }//#endif
}
void calcTmp(){
    printSide(symbol_table_out);
    map<int, int> mp_proc_cnt;
    rep (i,1,cnt_proc){
        mp_proc_cnt[i] = index_proc[i+1] - index_proc[i]-const_cnt[i]-1; //这个过程块所有的参数局部变量。空出每段的第一个符号即函数自己
    }
    rep (i,1,cnt_quat){
        if (quat[i].type == "variable_int" || quat[i].type == "variable_char")
            mp_proc_variable[quat[i].program_id]++;
        if (quat[i].type == "variable_int[]" || quat[i].type == "variable_char[]")
            mp_proc_variable[quat[i].program_id]+=mystoi(quat[i].op2);
        if (quat[i].type == "function_int" || quat[i].type == "function_char" || quat[i].type == "void_"){
            rep (j,1,cnt_proc){
                if (tab[index_proc[j]].name == quat[i].op1){
                    mp_quat_para_num_with_local[quat[i].program_id] = index_proc[j+1] - index_proc[j]-const_cnt[j]-1;
                    rep (k,index_proc[j]+1,index_proc[j+1]-1){
                        if (tab[k].type == "int[]" || tab[k].type=="char[]"){
                            mp_quat_para_num_with_local[quat[i].program_id] += tab[k].para_num-1; //****少算了这么多的单位 之前只把数组算了1因此ra的绝对地址就算消了
                        }
                    }
                    mp_quat_para_num[quat[i].program_id] = tab[index_proc[j]].para_num; //^^^mp_quat_para_num[i] = tab[index_proc[j]].para_num;
//                    symbol_table_out << i << " " << tab[index_proc[j]].para_num << " " <<  quat[i].program_id << endl;
                    break;
                }
            }
        }
        if (quat[i].op1.size()>1&&quat[i].op1[0]=='#'){ //四元式产生的中间代码一定是第一个操作数
            if (mp_tmp.count(quat[i].op1)>0){
                continue;
            }
            mp_quat_cnt_temp[quat[i].program_id] ++; //代表是这个过程块的四元式产生的第几个变量
            mp_tmp[quat[i].op1] = make_pair(quat[i].program_id, 3+mp_quat_para_num_with_local[quat[i].program_id]+mp_quat_cnt_temp[quat[i].program_id]); //3+mp_proc_cnt[quat[i].program_id]+mp_quat_cnt_temp[quat[i].program_id]
            symbol_table_out << quat[i].op1 << " ::: (Program_id:" << mp_tmp[quat[i].op1].first << " ,Offset:" << mp_tmp[quat[i].op1].second << ")" << endl;
        }
    }
}
#endif /* enter_h */
