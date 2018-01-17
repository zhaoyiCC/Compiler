//
//  enter.h
//  Compiler
//
//  Created by ohazyi on 2017/12/11.
//  Copyright © 2017年 ohazyi. All rights reserved.
//

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
void enter(string name, string kind, string type, int value, int addr, int para_num){
    if (kind == "void" || kind == "function") { //代表是过程或者函数
        rep (i, 1, cnt_proc){
            if (tab[index_proc[i]].name == name){ //重复定义函数
                error(100);
                return ;
            }
        }
        index_proc[++cnt_proc] = cnt_tab+1; //分程序的索引表指向当前的这个符号表的位置(我们马上就会存这个信息了)，注意这个是从0开始存的
    }else{ //不是过程或者函数，说明是局部变量或者全局变量
        //如果是全局变量，其index_proc[0] = 0
        rep (i, index_proc[cnt_proc], cnt_tab){ //检查之前定义的参数和局部变量有没有名字相同的
            if (tab[i].name == name){
                error(101);
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
    
    cout << "cnt_tab " << cnt_tab << " ::: " << name << " " << kind << " " << type << " " << value << " " << addr << " " << para_num << " &&& " << cnt_proc << endl;
}

int locateVariable(string name, int program_id, int& offset,bool is_mips=true){ //找到这个变量在哪里定义的，返回在符号表的位置，未找到则是-1 //默认参数，因为我是后来才加上要判断语法分析的合法性，因此
    int program_end = index_proc[program_id+1]-1;
    if (!is_mips)
        program_end = cnt_tab;
    //    if (program_id == cnt_proc) //因为我加上了统一操作，因此不需要了
    //        program_end = cnt_tab;
    if (is_mips && name.size() > 3 && name.substr(0,4) == "RET_"){ // "RET_int" or "RET_char"
        return -1;
    }
    if (is_mips && name.size()>1 && name[0] == '#'){
        offset = mp_tmp[name].second - 1; //-1的目的是因为我们的栈顶指向的是函数存放的第一个参数，而由于之前存放在符号表的第一个是函数名，因此addr相当于多了一个，以此同理
        return 0;
    }
    rep (i, index_proc[program_id]+1, program_end){ //因为第一个是函数的名字，防止出现局部变量名和函数名同名的情况
        if (tab[i].name == name){
            offset = tab[i].addr - 1;
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
    return -2;
}

void addQuat(string type, string op1, string op2, string op3){
    cout << "QUAT!!!" << " " << type << " " << op1 << " " << op2 << " " << op3 << " ::: " << cnt_proc << endl;
    ++cnt_quat;
    quat[cnt_quat].type = type;
    quat[cnt_quat].op1 = op1;
    quat[cnt_quat].op2 = op2;
    quat[cnt_quat].op3 = op3;
    quat[cnt_quat].program_id = cnt_proc;
}
void printQuat(){
    cout << "------------------------------" << endl;
    cout << "------------------------------" << endl;
    cout << "------------------------------" << endl;
    if (!quat[cnt_quat+1].label.empty()){
        addQuat("nop", "", "", "");
    }
    
    rep (i,1,cnt_quat) {
        
        if (!quat[i].label.empty())// != -1)
            for (auto j: quat[i].label)//cout << "LABEL_" << quat[i].label << " ";
                cout << "LABEL_" << j << " ";
        switch (mp_quat[quat[i].type]){
            case 100: cout << quat[i].type << " " << quat[i].op1 << endl; break;
            case 1: cout << "const " << quat[i].type.substr(6, quat[i].type.size()-6) << " " << quat[i].op1 << " = " << quat[i].op2 << endl; break;
            case 3: cout << "var " << quat[i].type.substr(9, quat[i].type.size()-9) << " " << quat[i].op1 << endl; break;
            case 5: cout << quat[i].type.substr(9, quat[i].type.size()-9) << " " << quat[i].op1 << "()" << endl; break;
            case 7: cout << "para " << quat[i].type.substr(10, quat[i].type.size()-10) << " " << quat[i].op1 << endl; break;
            case 9: cout << "void " << quat[i].op1 << endl; break;
            case 10: cout << quat[i].op1 << " = " << quat[i].op2 << " " << quat[i].type << " " << quat[i].op3 << endl; break;
            case 20: cout << quat[i].op1 << " " << quat[i].type << " " << quat[i].op2 << endl; break;
            case 30: cout << quat[i].op1 << " = " << quat[i].op2 << "[" << quat[i].op3 << "]" << endl; break;
            case 40: cout << "var " << quat[i].type.substr(9,quat[i].type.size()-2-9) << " " << quat[i].op1 << "[" << quat[i].op2 << "]" << endl; break;
            case 50: cout << quat[i].type << endl;  break;
            default: cout << "@@@" << quat[i].type << " " << mp_quat[quat[i].type] << endl; cout << quat[i].type << " " << quat[i].op1 << " " << quat[i].op2 << " " << quat[i].op3 << endl;
        }
    }
}
void programTable(){
    cout << "------------------------------" << endl;
    cout << "------------------------------" << endl;
    cout << "------------------------------" << endl;
    rep (i,1,cnt_proc){
        cout << "Program " << i << " :  " << index_proc[i] << endl;
    }
    cout << "AddByMe Program " << cnt_proc+1 << " :  " << index_proc[cnt_proc+1] << endl;
}

void symbolTable(){
    cout << "------------------------------" << endl;
    cout << "------------------------------" << endl;
    cout << "------------------------------" << endl;
    rep (i,1,cnt_tab){
        cout << "Symbol " << i << " :  " << tab[i].name << " " << tab[i].kind << " " << tab[i].type << " " << tab[i].value << " " << tab[i].addr << " " << tab[i].para_num << " " << tab[i].program_id << endl;
    }
}
void calcTmp(){
    cout << "------------------------------" << endl;
    cout << "------------------------------" << endl;
    cout << "------------------------------" << endl;
    map<int, int> mp_proc_num, mp_proc_cnt;
    rep (i,1,cnt_proc){
        mp_proc_cnt[i] = index_proc[i+1] - index_proc[i]-1;//空出每段的第一个符号即函数自己，
    }
    rep (i,1,cnt_quat){
        if (quat[i].type == "function_int" || quat[i].type == "function_char" || quat[i].type == "void_"){
            rep (j,1,cnt_proc){
                if (tab[index_proc[j]].name == quat[i].op1){
                    mp_quat_para_num_with_local[i] = index_proc[j+1] - index_proc[j]-1;
                    rep (k,index_proc[j]+1,index_proc[j+1]-1){
                        if (tab[k].type == "int[]" || tab[k].type=="char[]"){
                            mp_quat_para_num_with_local[i] += tab[k].para_num-1; //****少算了这么多的单位 之前只把数组算了1因此ra的绝对地址就算消了
                        }
                    }
                    mp_quat_para_num[i] = tab[index_proc[j]].para_num;
                    break;
                }
            }
        }
        if (quat[i].op1.size()>1&&quat[i].op1[0]=='#'){ //四元式产生的中间代码一定是第一个操作数
            if (mp_tmp.count(quat[i].op1)>0){
                cout << "SAME!!!" << endl;
                continue;
            }
            mp_proc_num[quat[i].program_id] ++;
            mp_tmp[quat[i].op1] = make_pair(quat[i].program_id, 3+mp_proc_cnt[quat[i].program_id]+mp_proc_num[quat[i].program_id]);
            cout << quat[i].op1 << " ::: " << mp_tmp[quat[i].op1].first << " " << mp_tmp[quat[i].op1].second << endl;
        }
    }
    mp_quat_cnt_temp = mp_proc_num;
    cout << "------------------------------" << endl;
    cout << "------------------------------" << endl;
    cout << "------------------------------" << endl;
    
}
#endif /* enter_h */
