//
//  mipsGenerator.h
//  Compiler
//
//  Created by ohazyi on 2017/12/11.
//  Copyright © 2017年 ohazyi. All rights reserved.
//

#ifndef mipsGenerator_h
#define mipsGenerator_h
#include "headers.h"

//ofstream asm_out("asm.txt");
ofstream asm_out("/Users/Mr.ZY/Desktop/asm.txt");
//#define asm_out asm_out
int reg_id_1, reg_id_2, reg_id_3;

void updateOffset(int pos, int program_id, int& offset){
    //    if (pos < index_proc[1]) //代表是全局变量区 //0x2ffc
    //        start = 1;
    int start = index_proc[program_id];
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
}
void calcOffset(string name, int program_id, string& start_pos, int& offset){
    int pos = locateVariable(name, program_id, offset); //如果是临时变量,pos = 0//把相对于函数的偏移量保存到offset //到四元式这一步，肯定是有定义了
    if (pos == -2){ //以防万一，!!!可删 -1是RET,虽然已经先处理过了RET了
        asm_out << "!!!ERROR:::NOT DEFINED$$$" << endl;
        cout << name << "!!!ERROR:Not Defined_ADD$$$" << endl;
        return ;
    }
    
    if (pos > 0 && pos < index_proc[1])
        start_pos = "gp";
    else
        start_pos = "fp";
    
    updateOffset(pos, program_id, offset);
}
void getArray(int reg_t, string name, int program_id, int line_id){
    int offset;
    string start_pos;
    int pos = locateVariable(name, program_id, offset); //如果是临时变量,pos = 0//把相对于函数的偏移量保存到offset //到四元式这一步，肯定是有定义了
    if (pos == -2){ //以防万一，!!!可删 -1是RET,虽然已经先处理过了RET了
        asm_out << "!!!ERROR:::NOT DEFINED$$$" << endl;
        cout << name << "!!!ERROR:Not Defined_para$$$" << endl;
        return ;
    }
    if (pos > 0 && pos < index_proc[1])
        start_pos = "gp";
    else
        start_pos = "fp";
    
    updateOffset(pos, program_id, offset);
    asm_out << "subi\t" << mp_reg_name[reg_t] << ",$" << start_pos << "," << offset << endl;
//    asm_out << "lw\t" << mp_reg_name[reg_t] << ",-" << offset << "($" << start_pos << ")" << endl;
    return ;
}
string getT(string name, int program_id, int line_id, int& reg_id, bool is_load, bool is_modify){
    int offset;
    string start_pos;
    if (isNumber(name) || isChar(name)){
        return name;
    }
    
    if (name == "RET_int" || name == "RET_char"){
        
//        asm_out << "move\t$t" << reg_t << ",$v1" << endl;
        //asm_out << "sw\t$v1,0($t1)" << endl; //把v1的值加载到t1所在的位置
        return "$v1";
    }
    
    if (name.find("[") != string::npos && name.find("]") != string::npos){ //检测是不是数组元素
        size_t name_pos1 = name.find("[");
        size_t name_pos2 = name.find("]");
        string name_array = name.substr(0,name_pos1);
        string name_offset = name.substr(name_pos1+1, name_pos2-name_pos1-1);
        getArray(25, name_array, program_id, line_id); //$t9
//        getVariableMips(1, name_array, program_id, false); //!!!寄存器编号直接回收
//        getVariableMips(2, name_offset, program_id, true);
        int reg_name_offset;
        string reg_offset;
        reg_offset = getT(name_offset, program_id, line_id, reg_name_offset, true, false);
        cout << "reg_name_offset:" << reg_name_offset << endl;
        if (isNumber(reg_offset) || isChar(reg_offset)){ //!!!数组的下标可以是字符吗 a['2']？
//            asm_out << "li\t$t8," << mystoi(reg_offset)*4 << endl; //节约一条 li $t8, 2
            asm_out << "sub\t$t9,$t9," << mystoi(reg_offset)*4 << endl;
        }else{
            asm_out << "sll\t$t8," << reg_offset << ",2" << endl;
            asm_out << "sub\t$t9,$t9,$t8" << endl;
        }
        if (is_load){ //!!!
            asm_out << "lw\t$t9,0($t9)" << endl;
        }
        reg_id = 25;
        return "$t9"; //mp_reg_name[25]
    }
    
    
    //如果分配了全局寄存器，就直接给
    if (mp_reg_global[program_id][name] >= S_START && mp_reg_global[program_id][name] <= S_END){
        reg_id_1 = mp_reg_global[program_id][name];
        return mp_reg_name[mp_reg_global[program_id][name]];
    }
    
    
    int res = -1;//reg_id = -1;
    bool has_alloc = false, is_full = true;
    for (auto it: mp_reg){
        cout << it.first << "----" << it.second.first << "----" << mp_reg_line[it.first] << endl;
        if (it.second.first == name && it.second.second == program_id){ //不仅要名字相同，而且也要程序块对应是一致的(防止全局变量和局部变量重名的情况)
            has_alloc = true;
            res = reg_id = it.first;
            mp_reg_line[res] = line_id; //更新一下出现的时间，以防被LRU给T掉
            
            
            
            
//            if (!is_modify)
//            { //!!!!!!!!!!!!要更新！！！！！！！！因为可能从其它的地方跳到这个地方
//                calcOffset(name, program_id, start_pos, offset);
//                
//                asm_out << "lw\t" << mp_reg_name[reg_id] << ",-" << offset << "($" << start_pos << ")" << endl;
//            }
//            mp_v_dirty[program_id][name]++;
            
            
            
            
            
            
            break;
        }
    }
    if (has_alloc){ //代表已经有寄存器分配给这个变量了，那么直接退出就好
        return mp_reg_name[res];
    }
    rep (i, T_START, T_END){
        if (!reg[i]){
            is_full = false;
            reg_id = i;
            break;
        }
    }
    if (!is_full){
        mp_reg[reg_id] = make_pair(name,program_id);
        mp_reg_line[reg_id] = line_id;
        reg[reg_id] = true;
        
//        if (mp_v_dirty[program_id][name] > 0 || name == "p" || name == "q") //!!!!!!!!!!在加入参数和全局变量之前，貌似没有问题。。。非常想加，但有风险
        if (!is_modify)
        { //这个变量之前被修改过，我们分配临时寄存器的时候就得先把值给加载了
            calcOffset(name, program_id, start_pos, offset);
            
            asm_out << "lw\t" << mp_reg_name[reg_id] << ",-" << offset << "($" << start_pos << ")" << endl;
        }
        mp_v_dirty[program_id][name]++;
        return mp_reg_name[reg_id];
    }
    reg_id = T_START;
    int min_line = mp_reg_line[T_START];
    //否则通过LRU算法找出一个最近最久未使用的寄存器进行踢出去的操作
    rep (i, T_START, T_END){
        if (mp_reg_line[i] < min_line){
            min_line = mp_reg_line[i];
            reg_id = i;
        }
    }
    
    //把这个变量写回到内存空间里去
    calcOffset(mp_reg[reg_id].first, program_id, start_pos, offset);
    asm_out << "sw\t" << mp_reg_name[reg_id] << ",-" << offset << "($" << start_pos << ")" << endl;
    
    mp_reg[reg_id] = make_pair(name, program_id);
    mp_reg_line[reg_id] = line_id;
//    if (mp_v_dirty[program_id][name] > 0 || name == "p" || name == "q")
    if (!is_modify)
    //!!!!!非常想保留
    { //这个变量之前被修改过，我们分配临时寄存器的时候就得先把值给加载了
        calcOffset(name, program_id, start_pos, offset);
        
        asm_out << "lw\t" << mp_reg_name[reg_id] << ",-" << offset << "($" << start_pos << ")" << endl;
    }
    mp_v_dirty[program_id][name]++;
    return mp_reg_name[reg_id];
}
void allocateZero(){
    asm_out << "sw\t$0,0($sp)" << endl;
    asm_out << "addi\t$sp,$sp,-4" << endl;
    #ifdef mips
    asm_out << endl;
    #endif
}
void allocateConst(const Quat& q, int const_i){ //常量 //不允许修改i,并且是个引用，不需要复制
    #ifdef mips
    asm_out << "#\tconst " << q.type.substr(6, q.type.size()-6) << " " << q.op1 << " = " << q.op2 << endl;
    #endif
    int t_id = 25; //"$t9" //1; //getT();
    asm_out << "li\t" << mp_reg_name[t_id] << "," << q.op2 << endl; //t_id // li t0, num
    asm_out << "sw\t" << mp_reg_name[t_id] << ",-" << 4*(const_i-1) << "($sp)" << endl;
//    asm_out << "addi\t$sp,$sp,-4" << endl;
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
//    int label_print;//t_id = getT()
//    asm_out << "li\t$t0,0" << endl;
//    asm_out << "li\t$t1," << q.op2 << endl;
//    newLabel(label_print);
//    asm_out << "LABEL_" << label_print << ":" << endl;
//    allocateZero();
//    asm_out << "addi\t$t0,$t0,1" << endl;
//    asm_out << "bne\t$t0,$t1,LABEL_" <<  label_print << endl << endl;
    //本来是把数组的元素都初始化的，后来觉得好像没啥必要，直接求出来$sp
    if (mystoi(q.op2) != 0) //窥孔优化无谓指令
        asm_out << "addi\t$sp,$sp,-" << 4*mystoi(q.op2) << endl;
}
void allocateFunction(const Quat& q){ //函数/过程的分配，主要是要保存fp和ra信息
    cout << const_i << endl;
    cout << mp_quat_para_num[q.program_id] << endl;
    if (const_i+mp_proc_variable[q.program_id] != 0) //窥孔优化无谓指令
        asm_out << "subi\t$sp,$sp," << 4*(const_i+mp_proc_variable[q.program_id]) << endl; //把常数开辟的空间全部都移到这儿减掉
    
    #ifdef mips
    asm_out << "#END Const&Variable define" << endl;
    #endif
/*
    asm_out <<"sw\t$s1,0($sp)"<<endl; //保存sp
    asm_out <<"addi\t$sp,$sp,-4"<<endl; //

    asm_out <<"sw\t$ra,0($sp)"<<endl; //保存返回地址

    //    asm_out <<"addi\t$fp,$sp,0"<<endl; //这个函数里的fp是一个基准线的作用，保存当前函数的顶部
    asm_out <<"addi\t$sp,$sp,-4"<<endl;

    asm_out <<"sw\t$s0,0($sp)"<<endl; //压栈保存一下之前的fp在哪，因为之前已经先在函数定义的时候保存了
    asm_out <<"addi\t$sp,$sp,-4"<<endl;
    
    if (mp_quat_cnt_temp[q.program_id] !=0) //窥孔优化无谓指令
        asm_out << "addi\t$sp,$sp,-" << 4*mp_quat_cnt_temp[q.program_id] << endl; //***把中间代码产生的空间跳过去
    //    asm_out << "move\t$fp,$s0" << endl; //!!!
 */
    asm_out <<"sw\t$s1,0($sp)"<<endl; //Important保存sp
    //asm_out <<"addi\t$sp,$sp,-4"<<endl;
    
    asm_out <<"sw\t$ra,-4($sp)"<<endl; //保存返回地址
    
    //asm_out <<"addi\t$sp,$sp,-4"<<endl;
    
    asm_out <<"sw\t$s0,-8($sp)"<<endl; //压栈保存一下之前的fp在哪，因为之前已经先在函数定义的时候保存了
    //asm_out <<"addi\t$sp,$sp,-4"<<endl;
    int tot_offset = 12;
    
    asm_out << "addi\t$sp,$sp,-" << tot_offset+4*mp_quat_cnt_temp[q.program_id] << endl; //***把中间代码产生的空间跳过去
    //    asm_out << "move\t$fp,$s0" << endl; //!!!
}

string transPara(int reg_t, string name, int program_id, int line_id){
    int offset, reg_id = -1;
    string start_pos;
    if (isChar(name) || isNumber(name)){
        asm_out << "li\t" << mp_reg_name[reg_t] << "," << name << endl;
        return mp_reg_name[reg_t];//return name;
    }
    if (name == "RET_int" || name == "RET_char"){
        return "$v1";
    }
    //如果分配了全局寄存器，就直接给
    if (mp_reg_global[program_id][name] >= S_START && mp_reg_global[program_id][name] <= S_END){
        reg_id_1 = mp_reg_global[program_id][name];
        return mp_reg_name[mp_reg_global[program_id][name]];
    }
    
    
    bool has_alloc = false;
    for (auto it: mp_reg){
        cout << it.first << "::::" << it.second.first << "::::" << mp_reg_line[it.first] << endl;
        if (it.second.first == name && it.second.second == program_id){ //不仅要名字相同，而且也要程序块对应是一致的(防止全局变量和局部变量重名的情况)
            has_alloc = true;
            reg_id = it.first;
            mp_reg_line[reg_id] = line_id; //更新一下出现的时间，以防被LRU给T掉
            break;
        }
    }
    if (has_alloc){ //代表这个变量已经分配了参数了
        return mp_reg_name[reg_id];
    }
    //否则就是还在内存区里，我们直接加载到reg_t($t9)里
    int pos = locateVariable(name, program_id, offset); //如果是临时变量,pos = 0//把相对于函数的偏移量保存到offset //到四元式这一步，肯定是有定义了
    if (pos == -2){ //以防万一，!!!可删 -1是RET,虽然已经先处理过了RET了
        asm_out << "!!!ERROR:::NOT DEFINED$$$" << endl;
        cout << name << "!!!ERROR:Not Defined_para$$$" << endl;
        return "";
    }
    if (pos > 0 && pos < index_proc[1])
        start_pos = "gp";
    else
        start_pos = "fp";
    
    updateOffset(pos, program_id, offset);
    asm_out << "lw\t" << mp_reg_name[reg_t] << ",-" << offset << "($" << start_pos << ")" << endl;
    return mp_reg_name[reg_t];//直接返回传进来的这个寄存器的编号
}

//sw	$t0,0($t1)
void getVariableMips(int reg_t, string name, int program_id, bool is_load){ //is_load:是否要取出值
    string name_array, name_offset, start_pos = "fp";
    asm_out << "#~~~" << name << endl; //~~~a[yyy]
    int offset, start; //offset = tab[pos].addr
    if (isChar(name)){
        asm_out << "li\t$a" << reg_t << "," << int(name[1]) << endl; //输出字符串的值
        return ;
    }
    if (isNumber(name)){ //如果是一个整数
        asm_out << "li\t$a" << reg_t << "," << mystoi(name) << endl;
        return ;
    }
    if (name == "RET_int" || name == "RET_char"){
        if (is_load){
            asm_out << "move\t$a" << reg_t << ",$v1" << endl;
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
    /*
    if (pos > 0 && pos < index_proc[1]){//        beg = "0xx2ffc";
        asm_out << "move\t$t" << reg_t << ",$gp" << endl;
    }else {//        asm_out << "li\t$t" << reg_t << ",0" << endl;
        asm_out << "move\t$t" << reg_t << ",$fp" << endl;
    }*/
    if (pos > 0 && pos < index_proc[1])
        start_pos = "gp";
    
    updateOffset(pos, program_id, offset);
    
    //asm_out << "subi\t$t" << reg_t << ",$" << start_pos << "," << offset << endl;
    if (is_load){
        asm_out << "lw\t$t" << reg_t << ",-" << offset << "($" << start_pos << ")" << endl;
    }else
        asm_out << "subi\t$t" << reg_t << ",$" << start_pos << "," << offset << endl;

}
void allocateParameter(const Quat& q, int quat_i, int para_i){ //参数的分配，标准的分配方法是：前4个压到a0-a3，后面的压到栈上
    #ifdef mips
    asm_out << "#\t" << q.type << " " << q.op1 << endl;
    #endif
    string reg_res = transPara(25, q.op1, q.program_id, quat_i);
    asm_out << "sw\t" << reg_res << ",0($sp)" << endl;
    asm_out << "addi\t$sp,$sp,-4" <<endl; //!!!感觉很难进行优化 因为不是push完参数就一定call的，比如gcd(m, mod(m,n)) 是
                               //PUSH m
                               //PUSH n
                               //PUSH m
                               //call mod
                               //#65 = RET_int
                               //PUSH #65
                               //call gcd
                                //所以不可以在call的时候减去位置
}
void assiMips(const Quat& q, int quat_i){ // y = x//赋值语句的转化
    #ifdef mips
    asm_out << "#" << q.op1 << " " << q.type << " " << q.op2 << endl;
    #endif
    if (q.op1=="#30" && q.op2 == "a[#28]"){
        int lyj;
        lyj = 233;
    }
    /*int t_reg_1 = getT(), t_reg_2 = getT();
    
    t_reg_1 = 1, t_reg_2 = 2; //空出t0给li用
    if (q.op1.find("[")!=string::npos){ //a[i] = j 必须先算a[i] 否则t2被后面的给挤掉了
        getVariableMips(t_reg_1, q.op1, q.program_id, false); //get y.address to t1
        getVariableMips(t_reg_2, q.op2, q.program_id, true); //get x to t2
    }else{
        getVariableMips(t_reg_2, q.op2, q.program_id, true); //get x to t2
        getVariableMips(t_reg_1, q.op1, q.program_id, false); //get y.address to t1
    }
    asm_out << endl << "sw\t$t" << t_reg_2 << ",0($t" << t_reg_1 << ")" << endl; //sw $t2,0($t1)
     */
    
//    if (q.op1.find("[")!=string::npos){ //a[i] = j 必须先算a[i] 否则t2被后面的给挤掉了
////        getVariableMips(t_reg_1, q.op1, q.program_id, false); //get y.address to t1
////        getVariableMips(t_reg_2, q.op2, q.program_id, true); //get x to t2
//        
//        
//    }else{
//        getVariableMips(t_reg_2, q.op2, q.program_id, true); //get x to t2
//        getVariableMips(t_reg_1, q.op1, q.program_id, false); //get y.address to t1
//    }
    string t_reg_2 = getT(q.op2, q.program_id, quat_i, reg_id_2, true, false), t_reg_1 = getT(q.op1, q.program_id, quat_i, reg_id_1, false, true);
    if (reg_id_1 >= T_START && reg_id_1 <= T_END)
        dirty[reg_id_1] = true; //脏位修改为true， 代表被修改过了
    else //还有可能是t9
        cout << "OOOOOOO:" << reg_id_1 << endl;
    //!!!还没考虑数组的情况
    if (q.op1.find("[") != string::npos && q.op1.find("]") != string::npos){ // a[i] = x
        if (isNumber(t_reg_2) || isChar(t_reg_2)){ //a[i] = 100
            asm_out << "li\t$t8," << t_reg_2 << endl; //必须先li $t8, 100
            t_reg_2 = "$t8";
        }
        asm_out << "sw\t" << t_reg_2 << ",0(" << t_reg_1 << ")" << endl; //sw $ ,0($t1)
        return ;
    }
    
    
    
    if (isNumber(t_reg_2) || isChar(t_reg_2))
        asm_out << "li\t" << t_reg_1 << "," << t_reg_2 << endl;
    else
        asm_out << "move\t" << t_reg_1 << "," << t_reg_2 << endl;
}
void addMips(const Quat& q, int quat_i, string operation){
    if (q.op1=="j" && q.op2=="m" && q.op3=="i"){
        int xr;
        xr = 520;
    }
//##22 = 1 + 2
//    add	$t0,1,2
    //add sub //#12 = x + 1
    #ifdef mips
    asm_out << "#" << q.op1 << " = " << q.op2 << " " << q.type << " " << q.op3 << endl;
    #endif
//    int t_reg_1 = getT(), t_reg_2 = getT(), t_reg_3;
//    //!!!
//    t_reg_1 = 1, t_reg_2 = 2, t_reg_3 = 3; //空出t0给li用
//    if (q.op1.find("[")!=string::npos){ //a[i] + j 必须先算a[i] 否则t2被后面的给挤掉了
//        getVariableMips(t_reg_1, q.op2, q.program_id, true);
//        getVariableMips(t_reg_2, q.op3, q.program_id, true);
//    }else{ // j + a[i]
//        getVariableMips(t_reg_2, q.op3, q.program_id, true);
//        getVariableMips(t_reg_1, q.op2, q.program_id, true);
//    }
//    asm_out << operation << "\t$t" << t_reg_1 << ",$t" << t_reg_1 << ",$t" << t_reg_2 << endl; //最终结果保存在t1里
//    getVariableMips(t_reg_3, q.op1, q.program_id, false);
//    asm_out << endl << "sw\t$t" << t_reg_1 << ",0($t" << t_reg_3 << ")" << endl; //把t1写到结果t3里 //sw $t1,0($t3)
    string t_reg_2 = getT(q.op2, q.program_id, quat_i, reg_id_2, true, false), t_reg_3 = getT(q.op3, q.program_id, quat_i, reg_id_3, true, false), t_reg_1 = getT(q.op1, q.program_id, quat_i, reg_id_1, false, true);
    //0-1这种情况 因为-带来的
    if ((isNumber(t_reg_2) || isChar(t_reg_2)) && (isNumber(t_reg_3) || isChar(t_reg_3))){
        string res = "";
        if (operation == "add")
            res = int2string(mystoi(t_reg_2) + mystoi(t_reg_3));
        else if (operation == "sub")
            res = int2string(mystoi(t_reg_2) - mystoi(t_reg_3));
        else if (operation == "mul")
            res = int2string(mystoi(t_reg_2) * mystoi(t_reg_3));
        else if (operation == "div")
            res = int2string(mystoi(t_reg_2) / mystoi(t_reg_3));
        else
            cout << "!!!ERROR: WRONG operand !!!" << endl;
        if (q.op1.find("[") != string::npos && q.op1.find("]") != string::npos){ // a[i] = x+y
            asm_out << "li\t$t8," << res << endl;
            asm_out << "sw\t$t8,0(" << t_reg_1 << ")" << endl; //sw $ ,0($t1)
            return ;
        }
        asm_out << "li\t" << t_reg_1 << "," << res << endl;
        dirty[reg_id_1] = true;
        return ;
    }
    if (isNumber(t_reg_2) || isChar(t_reg_2)){
        if (operation == "add" || operation == "mul"){ //2+x ==> x+2
            swap(t_reg_2, t_reg_3);
        }else{
            if (q.op1.find("[") != string::npos && q.op1.find("]") != string::npos){ // a[i] = 2-x
                asm_out << "li\t$t8," << t_reg_2 << endl; //li $t8, 2
                asm_out << operation << "\t$t8,$t8," << t_reg_3 << endl; //sub $t8, $t8, $..
                asm_out << "sw\t$t8,0(" << t_reg_1 << ")" << endl; //sw $ ,0($t1)
                return ;
            }
            asm_out << "li\t" << t_reg_1 << "," << t_reg_2 << endl; //li $.., 2
            asm_out << operation << "\t" << t_reg_1 << "," << t_reg_1 << "," << t_reg_3 << endl;//sub $.., $.., $..
            dirty[reg_id_1] = true;
            return ;
        }
    }
    //！！！窥孔优化之后不会有x=2+3这样的情况了吧 也不会有a[i] = . + . 的情况了吧
    if (q.op1.find("[") != string::npos && q.op1.find("]") != string::npos){ // a[i] = x+y
        asm_out << operation << "\t$t8," << t_reg_2 << "," << t_reg_3 << endl;
        asm_out << "sw\t$t8,0(" << t_reg_1 << ")" << endl; //sw $ ,0($t1)
        return ;
    }
    dirty[reg_id_1] = true;
    asm_out << operation << "\t" << t_reg_1 << "," << t_reg_2 << "," << t_reg_3 << endl;
}
bool checkRight(string sx, string sy, string op){
    int x, y;
    
    x = mystoi(sx);//mystoi做了统一处理，故可以直接返回字符的ascii值
    y = mystoi(sy);

    if (op == ">=")
        return (x >= y);
    if (op == "<=")
        return (x <= y);
    if (op == ">")
        return (x > y);
    if (op == "<")
        return (x <= y);
    if (op == "==")
        return (x == y);
    if (op == "!=")
        return (x != y);
    cout << op << "!!!ERROR: Wrong operation!!!" << endl;
    return true;
}
void compMips(const Quat& q, int quat_i, string operation){ //!!!暂时把比较的结果放在了$t9里，之前是$t0 //add sub //#12 = x + 1
    //map<string, string> mp_comp = {{">=", "sge"}, {">", "sgt"}, {"<=", "sle"}, {"<", "slt"}, {"==", "seq"}, {"!=", "sne"}};
    map<string, string> mp_comp = {{">=", "blt"}, {"<", "bge"}, {">", "ble"}, {"<=", "bgt"},
           {"==", "bne"}, {"!=", "beq"}};
    map<string, string> mp_comp_opposite = {{">=", "bgt"}, {"<", "ble"}, {">", "bge"}, {"<=", "blt"},
        {"==", "bne"}, {"!=", "beq"}};
    #ifdef mips
    asm_out << "#\t" << q.op1 << " " << q.type << " " << q.op2 << endl;
    asm_out << "#\t" << quat[quat_i+1].type << " " << quat[quat_i+1].op1 << endl;
    #endif

    if (q.op1=="#19" && q.op2=="1"){
        int zhuangbi;
        zhuangbi = 1;
    }
    string t_reg_1 = getT(q.op1, q.program_id, quat_i, reg_id_1, true, false), t_reg_2 = getT(q.op2, q.program_id,quat_i, reg_id_2, true, false);
    /*if (isNumber(t_reg_1) || isChar(t_reg_1)){ !!!非常迫切的想优化掉 //如果只是t_reg_1换一些位置？再变一下类型？
        asm_out << "li\t$t9," << t_reg_1 << endl;
        t_reg_1 = "$t9";
    }
    if (isNumber(t_reg_2) && mp_comp[operation] == "slt"){ slti $t9,1
        asm_out << mp_comp[operation] << "i\t$t9," << t_reg_1 << "," << t_reg_2 << endl;
        return ;
    }
    asm_out << mp_comp[operation] << "\t$t9," << t_reg_1 << "," << t_reg_2 << endl;*/
    
    if ((isNumber(t_reg_1) || isChar(t_reg_1)) && (isNumber(t_reg_2) || isChar(t_reg_2))){//2 == 3
        if (!checkRight(t_reg_1, t_reg_2, operation)){
            asm_out << "#ALWAYS satisfied " << quat[quat_i+1].op1 << endl;
            asm_out << "j\t" << quat[quat_i+1].op1 << endl;
            cout << "j\t" << quat[quat_i+1].op1 << endl;
        }else{
            asm_out << "#NEVER j " << quat[quat_i+1].op1 << endl;
            cout << "#NEVER j " << quat[quat_i+1].op1 << endl;
        }
        return ;
    }
    
    if ((isNumber(t_reg_1) || isChar(t_reg_1)) && !(isNumber(t_reg_2) || isChar(t_reg_2))){//2048 == i，交换次序，对于大于这些换成相反的，==直接交换保持bne就好
        asm_out << mp_comp_opposite[operation] << "\t" << t_reg_2 << "," << t_reg_1 << "," << quat[quat_i+1].op1 << endl;
        cout << mp_comp_opposite[operation] << "\t" << t_reg_2 << "," << t_reg_1 << "," << quat[quat_i+1].op1 << endl;
        return ;
    }
    
    
    asm_out << mp_comp[operation] << "\t" << t_reg_1 << "," << t_reg_2 << "," << quat[quat_i+1].op1 << endl;
    cout << mp_comp[operation] << "\t" << t_reg_1 << "," << t_reg_2 << "," << quat[quat_i+1].op1 << endl;
}


void jumpMips(const Quat& q){ //BZ LABEL_2 //READ x
    //将这个一个和之前的条件比较的四元式进行合并成一条mips语句，即i==0 BZ LABEL_4 直接合并成 bne $i,0,LABEL_4
    //#ifdef mips
    // asm_out << "#\t" << q.type << " " << q.op1 << endl;
    //#endif
    //asm_out << "bne\t$t9,1," << q.op1 << endl; BZ是不满足就跳转，所以就是不等于1就跳转
}

void gotoMips(const Quat& q){ //GOTO LABEL_1
    #ifdef mips
    asm_out << "#\t" << q.type << " " << q.op1 << endl;
    #endif
    asm_out << "j\t" << q.op1 << endl;
}
void callMips(const Quat& q, int quat_i){ //GOTO LABEL_1
#ifdef mips
    asm_out << "#\t" << q.type << " " << q.op1 << endl;
#endif
    cout << "#\t" << q.type << " " << q.op1 << endl;
    int offset;
    //string s_reg;
    for (auto i: flow_in_line[quat_i]){
        cout << i << endl;
        int pos = locateVariable(i, q.program_id, offset); //如果是临时变量,pos = 0//把相对于函数的偏移量保存到offset //到四元式这一步，肯定是有定义了
        if (pos == -2){ //以防万一，!!!可删 -1是RET,虽然已经先处理过了RET了
            asm_out << "!!!ERROR:::NOT DEFINED______CALL_Restore$$$" << endl;
            cout << i << "!!!ERROR:Not Defined______CALL_Restore#$$" << endl;
            return ;
        }
        if (pos > 0 && pos < index_proc[1]){
            cout << i << " _global variable Should not be allocated with a global register" << endl;
            continue;
        }
        if (tab[pos].kind == "parameter"){
            cout << i << " _parameter Should not be allocated with a global register" << endl;
            continue;
        }
        updateOffset(pos, q.program_id, offset);
        cout << mp_reg_global[q.program_id][i] << endl;
        asm_out << "sw\t" << mp_reg_name[mp_reg_global[q.program_id][i]] << ",-" << offset << "($fp)" << endl; //可以对脏位进行判断一下，即在call这句话的in集合里并且是脏位才sw //全局寄存器不会分配给全局变量
    }
    asm_out << "jal\t" << q.op1 << endl;
    for (auto i: flow_in_line[quat_i]){
        cout << i << endl;
        int pos = locateVariable(i, q.program_id, offset); //如果是临时变量,pos = 0//把相对于函数的偏移量保存到offset //到四元式这一步，肯定是有定义了
        if (pos > 0 && pos < index_proc[1]){
            cout << i << " _global variable Should not be allocated with a global register" << endl;
            continue;
        }
        if (tab[pos].kind == "parameter"){
            cout << i << " _parameter Should not be allocated with a global register" << endl;
            continue;
        }
        updateOffset(pos, q.program_id, offset);
        asm_out << "lw\t" << mp_reg_name[mp_reg_global[q.program_id][i]] << ",-" << offset << "($fp)" << endl; //可以对脏位进行判断一下，即在call这句话的in集合里并且是脏位才sw //全局寄存器不会分配给全局变量
    }
}
void reprMips(const Quat& q, int quat_i, bool is_read){ //BZ LABEL_2 //READ x
    #ifdef mips
    asm_out << "#\t" << q.type << " " << q.op1 << endl;
    #endif
    
    if (q.type == "PRINTLN"){ //li $a0, '\n' //li $v0, 11 //syscall
        #ifdef println
        asm_out << "li\t$a0,'\\n'" << endl;
        asm_out << "li\t$v0,11" << endl;
        asm_out << "syscall" << endl;
        #endif
        return ;
    }
    if (q.op1.size() == 0)
        return ;
    if (!is_read && q.op1[0] == '"'){ //q.op2 == "-string" 也可以 //输出的内容是字符串，找到对应的是几号str，这个在之前的.data段定义过了
//        cout << "%%%%PRINT q.op2=" << q.op2 << endl;
        asm_out << "la\t$a0,str" << mp_s[q.op1] << endl;
        asm_out << "li\t$v0,4" <<endl;
        asm_out << "syscall" <<endl;
        return ;
    }
//    int t_reg_1 = 1;//getT()!!!
//    getVariableMips(t_reg_1, q.op1, q.program_id, !is_read);
//    int offset, print_type=0;
//    int pos = locateVariable(q.op1, q.program_id, offset);
//    if (pos == -3 || (pos == -1 && q.op1 == "RET_int") || (pos == 0&&q.op2=="int") || (pos>0&&tab[pos].type == "int")){ //pos==0代表是四元式产生的局部变量
//        print_type = is_read ? 5 : 1; //mips输出整数
//    }else if (pos == -4 || (pos == -1 && q.op1 == "RET_char") || (pos == 0&&q.op2=="char") || (pos>0&&tab[pos].type == "char")){
//        print_type = is_read ? 12 : 11;
//    }else{
//        if (pos>=0)
//            asm_out << tab[pos].type;
//        asm_out << "!!!ERRRROR!!!" << endl;
//    }
//    if (!is_read)
//        asm_out << "move\t$a0,$t" << t_reg_1 << endl;
//    asm_out << "li\t$v0," << print_type << endl;
//    asm_out << "syscall\n" << endl;
//    if (is_read)
//        asm_out << "sw\t$v0,0($t" << t_reg_1 << ")" << endl << endl;
    string t_reg_1 = getT(q.op1, q.program_id, quat_i, reg_id_1, true, is_read); //!!!最后是这样的吗 read就会修改
    dirty[reg_id_1] = true;
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
        asm_out << "move\t$a0," << t_reg_1 << endl;
    asm_out << "li\t$v0," << print_type << endl;
    asm_out << "syscall" << endl;
    if (is_read)
        asm_out << "move\t" << t_reg_1 << ",$v0" << endl; //读入的内容保存在$v0里面
}
void retuMips(const Quat& q, int quat_i){ //add sub //#12 = x + 1
    #ifdef mips
    asm_out << "#\t" << q.op1 << " " << q.type << " " << q.op2 << endl;
    #endif
    if (q.program_id == cnt_proc){ //如果是主函数，就不要恢复现场了，直接退出程序
        asm_out << "li\t$v0,10" << endl;
        asm_out << "syscall" << endl;
        return ;
    }
    if (q.op1!=""){ //代表不是过程
//        int t_reg_1 = 1;//!!!t_reg_1 = 1
//        getVariableMips(t_reg_1, q.op1, q.program_id, true); //我们直接加载到值，因为考虑到存在ret 常数的情况***
//        asm_out << "move\t$v1,$t" << t_reg_1 << endl;
        string t_reg_1 = getT(q.op1, q.program_id, quat_i, reg_id_1, true, false);
        if (isNumber(t_reg_1) || isChar(t_reg_1)){
            asm_out << "li\t$v1," << t_reg_1 << endl; //li $v1, 'O'
        }else
            asm_out << "move\t$v1," << t_reg_1 << endl;
    }

    asm_out << "lw\t$sp,-" << int2string(4*(para_now_cnt)) << "($fp)" << endl; //****
    if (mp_quat_para_num[q.program_id] != 0) //窥孔优化无谓指令
        asm_out << "addi\t$sp,$sp," << 4*(mp_quat_para_num[q.program_id]) << endl; //加上$fp参数的个数

    asm_out << "lw\t$ra,-" << int2string(4*(para_now_cnt+1)) << "($fp)" << endl; //这两个顺序不能反，因为取ra要用到目前的fp，因此不能先恢复fp现场

    asm_out << "lw\t$fp,-" << int2string(4*(para_now_cnt+2)) << "($fp)" << endl;

    asm_out << "jr\t$ra" << endl;
}
void init_reg(int i){
    int offset;
    string start_pos;
    rep (j, T_START, T_END){
        if (dirty[j]) { //进行写回内存
            int pos = locateVariable(mp_reg[j].first, quat[i].program_id, offset); //如果是临时变量,pos = 0//把相对于函数的偏移量保存到offset //到四元式这一步，肯定是有定义了
            if (pos == -2){ //以防万一，!!!可删 -1是RET,虽然已经先处理过了RET了
                asm_out << "!!!ERROR:::NOT DEFINED$$$" << endl;
                cout << mp_reg[j].first << "!!!ERROR:Not Defined_REG$$$" << endl;
                return ;
            }
            /*
             if (pos > 0 && pos < index_proc[1]){//        beg = "0xx2ffc";
             asm_out << "move\t$t" << reg_t << ",$gp" << endl;
             }else {//        asm_out << "li\t$t" << reg_t << ",0" << endl;
             asm_out << "move\t$t" << reg_t << ",$fp" << endl;
             }*/
            if (pos > 0 && pos < index_proc[1])
                start_pos = "gp";
            else
                start_pos = "fp";
            
            updateOffset(pos, quat[i].program_id, offset);
            
            asm_out << "sw\t" << mp_reg_name[j] << ",-" << offset << "($" << start_pos << ")" << endl;
        }
    }
    memset(dirty, false, sizeof(dirty));
    memset(reg, false, sizeof(reg));
    mp_reg.clear();
    mp_reg_line.clear();
}
void quatMips(){
    int offset;
    asm_out << ".data" << endl;
    #ifdef mips
    asm_out << endl;
    #endif
    rep (i, 1, cnt_quat){
        if (quat[i].type == "PRINT" && quat[i].op1[0] =='"'){
            if (mp_s.count(quat[i].op1) == 0){ //重复的就不要加了
                mp_s[quat[i].op1] = ++cnt_mp_s;
            }
        }
    }
    string start_pos;
    for (auto i: mp_s){
        asm_out << "str" << i.second << ":\t.asciiz" << " " << i.first << endl;
            #ifdef mips
        asm_out << endl;
            #endif
    }

    asm_out << ".text" << endl;
    #ifdef mips
    asm_out << endl;
    #endif
    asm_out << "move\t$gp, $sp" << endl;
    
    memset(dirty, false, sizeof(dirty));
    memset(reg, false, sizeof(reg));
    mp_reg.clear();
    mp_reg_line.clear();
    rep (i, 1, cnt_quat){//main_pos-1){
        cout << i << "mp_reg.size() = " << mp_reg.size() << endl;
        if (i==18){
            int lll;
            lll = 1;
            cout << mp_reg.size() << endl;
            
        }
        cout << quat[i].type << "*******" << quat[i].op1 << "*******" << quat[i].block_pos << "**" << i << endl;
        if (i == index_proc[1]-const_cnt[0]){ //前面的都是全局的常变量。全局常变量占在一个固定的坑里，不会出来了，之后遇到全局变量也可以直接根据$sp(0x2ffc)-addr来找到 //在第一个分程序前定义的常变量都是全局的常变量，并且正好对应一条四元式
            if (const_i+mp_proc_variable[quat[0].program_id] != 0) //去除无谓指令，例如subi $sp,$sp,0这种 !!!窥孔优化
                asm_out << "subi\t$sp,$sp," << 4*(const_i+mp_proc_variable[quat[0].program_id]) << endl; //重大bug:之前没加上全局常量的偏移 即const_i+
            asm_out << "j\tmain" << endl; //声明常变量后，先跳转到main进行!!!有必要吗
                #ifdef mips
            asm_out << endl;
                #endif
        }
        if (!quat[i].label.empty())//(quat[i].label != -1)
            for (auto j : quat[i].label)//asm_out << "LABEL_" << quat[i].label << ":" << endl;
                asm_out << "LABEL_" << j << ":" << endl;
        
        
        if  ((mp_mips[quat[i].type] == 21) || (quat[i].block_pos != quat[i+1].block_pos && (quat[i].type == "call" || quat[i].type == "BEGIN" || quat[i].type == "ret" || quat[i].type == "GOTO"))){ //quat[i].type == "BZ"当前基本块结束了
            if (quat[i].type == "BEGIN")
                cout << "~~~~~~~START BEGIN" <<  endl;
            else
                cout << "~~~" << quat[i].type << endl;
            init_reg(i);
            if (quat[i].type == "BEGIN")
                cout << "~~~~~~~END BEGIN" << endl;
        }
        
        if (quat[i].block_pos != quat[i-1].block_pos){
            memset(dirty, false, sizeof(dirty));
            memset(reg, false, sizeof(reg));
            mp_reg.clear();
            mp_reg_line.clear();
        }
        
        
        switch (mp_mips[quat[i].type]){
            case 100: break;
            case 1: allocateConst(quat[i], ++const_i); break; //"const int"
            case 3:  break; //allocateVariable(quat[i]);
            case 5: const_i = para_i = 0; asm_out << quat[i].op1 << ":" << endl; asm_out << "move\t$s1,$sp" << endl; asm_out << "move\t$s0,$fp" << endl; asm_out << "addi\t$fp,$sp," << 4*(mp_quat_para_num[quat[i].program_id]) << endl; para_now_cnt = mp_quat_para_num_with_local[quat[i].program_id]; break; //asm_out << "addi\t$fp,$sp," << 4*(mp_quat_para_num[i]) << endl;
            case 7: break; //参数，直接无视，因为在PUSH里干了
            case 9: asm_out << quat[i].op1 << ":" << endl; break;
            case 10: addMips(quat[i], i, "add"); break; //处理加号
            case 11: addMips(quat[i], i, "sub"); break; //处理减号
            case 12: addMips(quat[i], i, "mul"); break; //处理乘号
            case 13: addMips(quat[i], i, "div"); break; //处理除号
            case 20: assiMips(quat[i], i); break; // 赋值语句
            case 21: compMips(quat[i], i, quat[i].type); break; //小于等于 //#6 >= #10 //BZ LABEL_2
            case 40: break; //allocateArray(quat[i]);
            case 50:
//                if (mp_proc_variable[quat[i].program_id] != 0) //窥孔优化无谓指令
//                        asm_out << "subi\t$sp,$sp," << 4*mp_proc_variable[quat[i].program_id] << endl;
                allocateFunction(quat[i]); break; //在END...前
            case 101: allocateParameter(quat[i], i, ++para_i); break; //PUSH
            case 102: jumpMips(quat[i]); break;
            case 103: reprMips(quat[i], i, false); break;
            case 104: reprMips(quat[i], i, true); break;
            case 1000: reprMips(quat[i], i, true); break;
            case 105: gotoMips(quat[i]); break; //GOTO label_1
            case 106: retuMips(quat[i], i); break;
            case 107: asm_out << "#\t" << quat[i].type << " " << quat[i].op1 << endl; break;
            case 108: asm_out << quat[i].type << endl; break; //nop
            case 109: callMips(quat[i], i); break; //call label_1
            default: asm_out << "???" << quat[i].type << " " << quat[i].op1 << endl;
        }
        
        if (quat[i].block_pos != quat[i+1].block_pos && !(quat[i].type == "call" || quat[i].type == "BEGIN" || quat[i].type == "ret" || quat[i].type == "GOTO" || quat[i].type == "BZ")){ //当前基本块结束了
            cout << quat[i].type << "$$$" << quat[i].op1 << "$$$" << quat[i].op2 << "$$$" << quat[i].op3 << endl;
            init_reg(i);
        }
    }
}

#endif /* mipsGenerator_h */
