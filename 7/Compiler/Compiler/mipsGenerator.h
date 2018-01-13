#ifndef mipsGenerator_h
#define mipsGenerator_h
#include "headers.h"
ofstream asm_out("asm.txt");
//ofstream asm_out("/Users/Mr.ZY/Desktop/asm.txt");
//#define asm_out asm_out
int getT(){
    return 0;
}
void allocateZero(){
    asm_out << "sw\t$0,0($sp)" << endl;
    asm_out << "addi\t$sp,$sp,-4" << endl << endl;
}
void allocateConst(const Quat& q){ //常量 //不允许修改i,并且是个引用，不需要复制
    #ifdef mips
    asm_out << "#\tconst " << q.type.substr(6, q.type.size()-6) << " " << q.op1 << " = " << q.op2 << endl;
    #endif
    int t_id = 1; //getT();
    asm_out << "li\t$t" << t_id << "," << q.op2 << endl; // li t0, num
    asm_out << "sw\t$t" << t_id << ",0($sp)" << endl;
    asm_out << "addi\t$sp,$sp,-4\n" << endl;
}
void allocateVariable(const Quat& q){
    #ifdef mips
    asm_out << "#var " << q.type.substr(9, q.type.size()-9) << " " << q.op1 << endl;
    #endif
    allocateZero();
}
void allocateArray(const Quat& q){
    #ifdef mips
    asm_out << "#var " << q.type.substr(9,q.type.size()-2-9) << " " << q.op1 << "[" << q.op2 << "]" << endl;
    #endif
    //本来是把数组的元素都初始化的，后来觉得好像没啥必要，直接求出来$sp
    asm_out << "addi\t$sp,$sp,-" << 4*mystoi(q.op2) << endl;
}
void allocateFunction(const Quat& q){ //函数/过程的分配，主要是要保存fp和ra信息
    //    asm_out << q.type.substr(9, q.type.size()-9) << " " << q.op1 << "()" << endl;
    #ifdef mips
    asm_out << "#END Const&Variable define" << endl;
    #endif
    
    asm_out <<"sw\t$s1,0($sp)"<<endl; //***保存sp
    asm_out <<"addi\t$sp,$sp,-4\n"<<endl; //***
    
    asm_out <<"sw\t$ra,0($sp)"<<endl; //保存返回地址
    
    //    asm_out <<"addi\t$fp,$sp,0"<<endl; //这个函数里的fp是一个基准线的作用，保存当前函数的顶部
    asm_out <<"addi\t$sp,$sp,-4\n"<<endl;
    
    asm_out <<"sw\t$s0,0($sp)"<<endl; //压栈保存一下之前的fp在哪，因为之前已经先在函数定义的时候保存了
    asm_out <<"addi\t$sp,$sp,-4\n"<<endl;
    
    asm_out << "addi\t$sp,$sp,-" << 4*mp_quat_cnt_temp[q.program_id] << endl; //***把中间代码产生的空间跳过去
}
void getVariableMips(int reg_t, string name, int program_id, bool is_load){ //is_load:是否要取出值
    string name_array, name_offset, start_pos = "fp";
    #ifdef mips
    asm_out << "#~~~" << name << endl; //~~~a[yyy]
    #endif
    int offset, start; //offset = tab[pos].addr
    if (isChar(name)){
        asm_out << "li\t$t" << reg_t << "," << int(name[1]) << endl; //输出字符串的值
        return ;
    }
    if (isNumber(name)){ //如果是一个整数
        asm_out << "li\t$t" << reg_t << "," << mystoi(name) << endl;
        return ;
    }
    if (name == "RET_int" || name == "RET_char"){
        if (is_load){
            asm_out << "move\t$t" << reg_t << ",$v1" << endl;
        }
        //asm_out << "sw\t$v1,0($t1)" << endl; //把v1的值加载到t1所在的位置
        return ;
    }
    size_t name_pos1, name_pos2; //string::size_type
    if (name.find("[") != string::npos && name.find("]") != string::npos){ //检测是不是数组元素
        name_pos1 = name.find("[");
        name_pos2 = name.find("]");
        name_array = name.substr(0,name_pos1);
        name_offset = name.substr(name_pos1+1, name_pos2-name_pos1-1);
        getVariableMips(1, name_array, program_id, false); //!!!寄存器编号直接回收
        getVariableMips(2, name_offset, program_id, true);
        asm_out << "sll\t$t2,$t2,2" << endl;
        asm_out << "sub\t$t1,$t1,$t2" << endl;
        if (is_load){
            asm_out << "lw\t$t" << reg_t << ",0($t1)" << endl;
        }
        return ;
    }
    int pos = locateVariable(name, program_id, offset); //如果是临时变量,pos = 0//把相对于函数的偏移量保存到offset //到四元式这一步，肯定是有定义了
    if (pos == -2){ //以防万一，!!!可删 -1是RET,虽然已经先处理过了RET了
        asm_out << "!!!ERROR:::NOT DEFINED$$$" << endl;
        cout << name << "!!!ERROR:Not Defined$$$" << endl;
        return ;
    }
    
    if (pos > 0 && pos < index_proc[1])
        start_pos = "gp";
    start = index_proc[program_id];
    if (pos < index_proc[1]) //代表在全局区找到了
        start = 1;
    if (program_id == 0)
        start = 1;
    rep (i,start,pos-1){
        if (tab[i].type == "int[]" || tab[i].type == "char[]"){
            offset += tab[i].para_num-1;
        }
    }
    offset*=4;
    //asm_out << "subi\t$t" << reg_t << ",$" << start_pos << "," << offset << endl;
    if (is_load){
        asm_out << "lw\t$t" << reg_t << ",-" << offset << "($" << start_pos << ")" << endl;
    }else
        asm_out << "subi\t$t" << reg_t << ",$" << start_pos << "," << offset << endl;
}
void allocateParameter(const Quat& q, int para_i){ //参数的分配，标准的分配方法是：前4个压到a0-a3，后面的压到栈上
    #ifdef mips
    asm_out << "#\t" << q.type << " " << q.op1 << endl;
    #endif
    int t_reg_1 = 1, t_reg_2 = 2;;

    getVariableMips(t_reg_1, q.op1, q.program_id, true);
    
    asm_out << "sw\t$t" << t_reg_1 << ",0($sp)" << endl;
    asm_out << "addi\t$sp,$sp,-4\n" <<endl;
}
void assiMips(const Quat& q){ // y = x//赋值语句的转化
    #ifdef mips
    asm_out << "#" << q.op1 << " " << q.type << " " << q.op2 << endl;
    #endif
    int t_reg_1 = getT(), t_reg_2 = getT();
    //!!!
    t_reg_1 = 1, t_reg_2 = 2; //空出t0给li用
    if (q.op1.find("[")!=string::npos){ //a[i] = j 必须先算a[i] 否则t2被后面的给挤掉了
        getVariableMips(t_reg_1, q.op1, q.program_id, false); //get y.address to t1
        getVariableMips(t_reg_2, q.op2, q.program_id, true); //get x to t2
    }else{
        getVariableMips(t_reg_2, q.op2, q.program_id, true); //get x to t2
        getVariableMips(t_reg_1, q.op1, q.program_id, false); //get y.address to t1
    }
    asm_out << endl << "sw\t$t" << t_reg_2 << ",0($t" << t_reg_1 << ")" << endl; //sw $t2,0($t1)
}
void addMips(const Quat& q, string operation){ //add sub //#12 = x + 1
    #ifdef mips
    asm_out << "#" << q.op1 << " = " << q.op2 << " " << q.type << " " << q.op3 << endl;
    #endif
    int t_reg_1 = getT(), t_reg_2 = getT(), t_reg_3;
    //!!!
    t_reg_1 = 1, t_reg_2 = 2, t_reg_3 = 3; //空出t0给li用
    if (q.op1.find("[")!=string::npos){ //a[i] + j 必须先算a[i] 否则t2被后面的给挤掉了
        getVariableMips(t_reg_1, q.op2, q.program_id, true);
        getVariableMips(t_reg_2, q.op3, q.program_id, true);
    }else{ // j + a[i]
        getVariableMips(t_reg_2, q.op3, q.program_id, true);
        getVariableMips(t_reg_1, q.op2, q.program_id, true);
    }
    asm_out << operation << "\t$t" << t_reg_1 << ",$t" << t_reg_1 << ",$t" << t_reg_2 << endl; //最终结果保存在t1里
    getVariableMips(t_reg_3, q.op1, q.program_id, false);
    asm_out << endl << "sw\t$t" << t_reg_1 << ",0($t" << t_reg_3 << ")" << endl; //把t1写到结果t3里 //sw $t1,0($t3)
}
void compMips(const Quat& q, string operation){ //add sub //#12 = x + 1
    map<string, string> mp_comp = {{">=", "sge"}, {">", "sgt"}, {"<=", "sle"}, {"<", "slt"}, {"==", "seq"}, {"!=", "sne"}}; //sne seq  sge sgt sle有常数 slt没有
    #ifdef mips
    asm_out << "#\t" << q.op1 << " " << q.type << " " << q.op2 << endl;
    #endif
    int t_reg_1 = getT(), t_reg_2 = getT(), t_reg_3;
    //!!!
    t_reg_1 = 1, t_reg_2 = 2; //空出t0给li用
    getVariableMips(t_reg_1, q.op1, q.program_id, true);
    getVariableMips(t_reg_2, q.op2, q.program_id, true);
    asm_out << mp_comp[operation] << "\t$t0,$t" << t_reg_1 << ",$t" << t_reg_2 << endl;
}
void jumpMips(const Quat& q){ //BZ LABEL_2 //READ x
    #ifdef mips
    asm_out << "#\t" << q.type << " " << q.op1 << endl;
    #endif
    asm_out << "bne\t$t0,1," << q.op1 << endl; //BZ是不满足就跳转，所以就是不等于1就跳转
}
void gotoMips(const Quat& q){ //BZ LABEL_2 //READ x
    #ifdef mips
    asm_out << "#\t" << q.type << " " << q.op1 << endl;
    #endif
    asm_out << "jal\t" << q.op1 << endl; //BZ是不满足就跳转，所以就是不等于1就跳转
}
void reprMips(const Quat& q, bool is_read){ //BZ LABEL_2 //READ x
    #ifdef mips
    asm_out << "#\t" << q.type << " " << q.op1 << endl;
    #endif
    if (q.type == "PRINTLN"){ //li $a0, '\n' //li $v0, 11 //syscall
        asm_out << "li\t$a0,'\\n'" << endl;
        asm_out << "li\t$v0,11" << endl;
        asm_out << "syscall" << endl;
        return ;
    }
    if (q.op1.size() == 0)
        return ;
    if (!is_read && q.op1[0] == '"'){ //q.op2 == "-string" 也可以 //输出的内容是字符串，找到对应的是几号str，这个在之前的.data段定义过了
        //        cout << "%%%%PRINT q.op2=" << q.op2 << endl;
        asm_out << "la\t$a0,str" << mp_s[q.op1] << endl;
        asm_out <<"li\t$v0,4"<<endl;
        asm_out <<"syscall\n"<<endl;
        return ;
    }
    int t_reg_1 = 1;//getT()!!!
    getVariableMips(t_reg_1, q.op1, q.program_id, !is_read);
    int offset, print_type=0;
    int pos = locateVariable(q.op1, q.program_id, offset);
    if (pos == -3 || (pos == -1 && q.op1 == "RET_int") || (pos == 0&&q.op2=="int") || (pos>0&&tab[pos].type == "int")){ //pos==0代表是四元式产生的局部变量
        print_type = is_read ? 5 : 1; //mips输出整数
    }else if (pos == -4 || (pos == -1 && q.op1 == "RET_char") || (pos == 0&&q.op2=="char") || (pos>0&&tab[pos].type == "char")){
        print_type = is_read ? 12 : 11;
    }else{
        if (pos>=0)
            asm_out << tab[pos].type;
        asm_out << "!!!ERRRROR!!!" << endl;
    }
    if (!is_read)
        asm_out << "move\t$a0,$t" << t_reg_1 << endl;
    asm_out << "li\t$v0," << print_type << endl;
    asm_out << "syscall\n" << endl;
    if (is_read)
        asm_out << "sw\t$v0,0($t" << t_reg_1 << ")" << endl;
    #ifdef mips
    cout << endl;
    #endif
}
void retuMips(const Quat& q){ //add sub //#12 = x + 1
    #ifdef mips
    asm_out << "#\t" << q.op1 << " " << q.type << " " << q.op2 << endl;
    #endif
    if (q.program_id == cnt_proc){ //如果是主函数，就不要恢复现场了，直接退出程序
        asm_out << "li\t$v0,10" << endl;
        asm_out << "syscall" << endl;
        return ;
    }
    if (q.op1!=""){ //代表不是过程
        int t_reg_1 = 1;//!!!t_reg_1 = 1
        getVariableMips(t_reg_1, q.op1, q.program_id, true); //我们直接加载到值，因为考虑到存在ret 常数的情况***
        asm_out << "move\t$v1,$t" << t_reg_1 << endl;
        //asm_out << "lw\t$v1,0($t" << t_reg_1 << ")" << endl; //***
    }
    
    asm_out << "lw\t$sp,-" << int2string(4*(para_now_cnt)) << "($fp)" << endl; //****
    asm_out << "addi\t$sp,$sp," << 4*(mp_quat_para_num[q.program_id]) << endl; //加上$fp参数的个数
    
    asm_out << "lw\t$ra,-" << int2string(4*(para_now_cnt+1)) << "($fp)" << endl; //这两个顺序不能反，因为取ra要用到目前的fp，因此不能先恢复fp现场
    
    asm_out << "lw\t$fp,-" << int2string(4*(para_now_cnt+2)) << "($fp)" << endl;
    
    asm_out << "jr\t$ra" << endl;
}

void quatMips(){
    asm_out << ".data" << endl;
    #ifdef mips
    cout << endl;
    #endif
    rep (i, 1, cnt_quat){
        if (quat[i].type == "PRINT" && quat[i].op1[0] =='"'){
            if (mp_s.count(quat[i].op1) == 0){ //重复的就不要加了
                mp_s[quat[i].op1] = ++cnt_mp_s;
            }
        }
    }
    string str_out_trans;
    for (auto i: mp_s){
        asm_out << "str" << i.second << ":\t.asciiz" << " " << i.first << endl;
          #ifdef mips
        cout<<endl;
          #endif
    }
    
    asm_out << ".text" << endl << endl;
    asm_out << "move\t$gp, $sp" << endl;
    rep (i, 1, cnt_quat){//main_pos-1){
        if (i == index_proc[1]){ //前面的都是全局的常变量。全局常变量占在一个固定的坑里，不会出来了，之后遇到全局变量也可以直接根据$sp(0x2ffc)-addr来找到 //在第一个分程序前定义的常变量都是全局的常变量，并且正好对应一条四元式
            asm_out << "subi\t$sp,$sp," << 4*mp_proc_variable[quat[0].program_id] << endl;
            asm_out << "j\tmain" << endl;
                #ifdef mips
            cout << endl; //声明常变量后，先跳转到main进行!!!有必要吗
                #endif
        }
        if (!quat[i].label.empty())//(quat[i].label != -1)
            for (auto j : quat[i].label)//asm_out << "LABEL_" << quat[i].label << ":" << endl;
                asm_out << "LABEL_" << j << ":" << endl;
        
        switch (mp_mips[quat[i].type]){
            case 100: break;
            case 1: allocateConst(quat[i]); break;
            case 3:  break; //allocateVariable(quat[i]);
            case 5: para_i = 0; asm_out << quat[i].op1 << ":" << endl; asm_out << "move\t$s1,$sp" << endl; asm_out << "move\t$s0,$fp" << endl; asm_out << "addi\t$fp,$sp," << 4*(mp_quat_para_num[quat[i].program_id]) << endl; para_now_cnt = mp_quat_para_num_with_local[quat[i].program_id]; break; //asm_out << "addi\t$fp,$sp," << 4*(mp_quat_para_num[i]) << endl;
            case 7: break; //参数，直接无视，因为在PUSH里干了
            case 9: asm_out << quat[i].op1 << ":" << endl; break;
            case 10: addMips(quat[i], "add"); break; //处理加号
            case 11: addMips(quat[i], "sub"); break; //处理减号
            case 12: addMips(quat[i], "mul"); break; //处理乘号
            case 13: addMips(quat[i], "div"); break; //处理除号
            case 20: assiMips(quat[i]); break; // 赋值语句
            case 21: compMips(quat[i], quat[i].type); break; //小于等于 //#6 >= #10 //BZ LABEL_2
            case 40: break; //allocateArray(quat[i]);
            case 50: asm_out << "subi\t$sp,$sp," << 4*mp_proc_variable[quat[i].program_id] << endl; allocateFunction(quat[i]); break; //在END...前
            case 101: allocateParameter(quat[i], ++para_i); break; //PUSH
            case 102: jumpMips(quat[i]); break;
            case 103: reprMips(quat[i], false); break;
            case 104: reprMips(quat[i], true); break;
            case 1000: reprMips(quat[i], true); break;
            case 105: gotoMips(quat[i]); break;
            case 106: retuMips(quat[i]); break;
            case 107: asm_out << "#\t" << quat[i].type << " " << quat[i].op1 << endl; break;
            case 108: asm_out << quat[i].type << endl; break; //nop
            default: asm_out << "???" << quat[i].type << " " << quat[i].op1 << endl;
        }
    }
}

#endif /* mipsGenerator_h */
