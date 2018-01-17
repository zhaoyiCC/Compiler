#ifndef dataFlow_h
#define dataFlow_h
#define fDEBUG
#define MAX_VARIABLE 10010
#include "headers.h"
int typeUseDef(string& s, string s_type){ //1:Def 2:Use 0:null
    if (s!="=[]" && s!="[]=" && !isString(s) && s.find("[") != string::npos && s.find("]") != string::npos){
        string array_name, subscript;
        splitArrayName(s, array_name, subscript);
        s = subscript;
        if (s == "" || isNumber(s) || isChar(s) || s.find("[") != string::npos || s.find("\"") != string::npos || s== "-string" || s=="int" || s=="char" || s=="RET_int" || s=="RET_char")
            return 0;
        return 2;
    }
    if (s == "" || isNumber(s) || isChar(s) || s.find("[") != string::npos || s.find("\"") != string::npos || s== "-string" || s=="int" || s=="char" || s=="RET_int" || s=="RET_char")
        return 0;
    if (s_type == "=" || s_type == "-" || s_type == "*" || s_type == "+" || s_type == "/" || s_type == "READ") //读入是得放在def集而不是use集吧
        return 1;
    if (mp_mips[s_type] == 21 || mp_mips[s_type] == 103 || mp_mips[s_type] == 104 || mp_mips[s_type] == 106 || mp_mips[s_type] == 107 || mp_mips[s_type] == 101) //比较运算符 读入输出 返回 PUSH
        //        {"PUSH", 101}, {"BZ", 102}, {"PRINT", 103}, {"READ", 104}, {"GOTO", 105}, {"ret", 106}, {"call", 105}, {"SWITCH", 107}, {"nop", 108},
        return 2;
    return 0;
}
void calcLineDefUse(int i){ //计算每一行的Def集和Use集
    string op1 = quat[i].op1, op2 = quat[i].op2, op3 = quat[i].op3;
    if (typeUseDef(op2, quat[i].type)){
        use_line[i].push_back(op2);
    }
    if (typeUseDef(op3, quat[i].type)){
        use_line[i].push_back(op3);
    }
    int typ = typeUseDef(op1, quat[i].type);
    if (typ == 1)
        def_line[i].push_back(op1);
    else if (typ == 2)
        use_line[i].push_back(op1);
}
void calcDefUse(int quat_start, int quat_end, int block_id){
    map<string, int> vis;
    rep (i, quat_start, quat_end){
        //        quat[i].block_pos = block_id;//之前的一个while循环已经计算过了
        string op1 = quat[i].op1, op2 = quat[i].op2, op3 = quat[i].op3;
        if (typeUseDef(op2, quat[i].type) && vis[op2] == 0){
            use[block_id].push_back(op2);
            vis[op2]++;
        }
        if (typeUseDef(op3, quat[i].type) && vis[op3] == 0){
            use[block_id].push_back(op3);
            vis[op3]++;
        }
        if (vis[op1] == 0){
            int typ = typeUseDef(op1, quat[i].type);
            if (typ == 1)
                def[block_id].push_back(op1);
            else if (typ == 2)
                use[block_id].push_back(op1);
            vis[op1]++;
        }
    }
    uniqueVector(def[block_id]); //应该是无用的，因为我的vis标记保证了只会加入到def集和use集一次
    uniqueVector(use[block_id]);
}
void outVector(vector<string>& v, string s, ofstream& o_out){
    o_out << s << "::: ";
    for (auto k: v)
        o_out << k << " ";
    o_out << endl;
}
bool cmp(pair<string, int> a, pair<string, int> b){
    return a.second < b.second;
}
void removeNode(string s){
    for (auto&& it: v_node){ //否则只是拷贝，erase不能改变原来的值
        for (auto&& i = it.second.begin(); i != it.second.end();){
            if (*i == s){
                i = it.second.erase(i);
            }else
                i++;
        }
    }
    v_node.erase(s);
}
bool checkIfParaGlobal(string p, int program_id){
    int offset;
    int pos = locateVariable(p, program_id, offset); //如果是临时变量,pos = 0//把相对于函数的偏移量保存到offset //到四元式这一步，肯定是有定义了
    if (pos == -2){ //以防万一，!!!可删 -1是RET,虽然已经先处理过了RET了
        cout << p << "!!!ERROR:Not DEFINED____dataflow$$$" << endl;
        return false;
    }
    if (pos > 0 && tab[pos].kind == "parameter"){ //参数不允许分配全局变量!
        return false;
    }
    if (pos > 0 && pos < index_proc[1]){ //pos=0是局部变量#1这些了，可以分配//全局变量不逊于分配全局寄存器!
        return false;
    }
    return true;
}
void flowPerWork(int f_start, int f_end, int program_id){
    flow_line_out << "---------------------------Program_id:" << program_id << " Line " << f_start << "~" << f_end << "--------------------" << endl;
    int quat_start = 0, quat_now, block_cnt = 0;
    vector<string> v_deg;
    v_deg.clear();
    v_set_flow.clear();
    v_node.clear();
    quat_start = f_start; //1;
    block_cnt = 0;
    while (quat_start <= f_end){
        quat_now = quat_start;
        while (quat[quat_now].block_pos == quat[quat_now+1].block_pos && quat_now < f_end){
            quat_now++;
        }
        calcDefUse(quat_start, quat_now, quat[quat_start].block_pos); //计算def集和use集
        quat_start = quat_now+1;
    }
    vector<string> v_diff;
    //迭代进行计算IN 和 OUT集
    bool is_changed = true;
    while (is_changed){
        is_changed = false;
        rep (i, quat[f_start].block_pos, quat[f_end].block_pos){
            flow_in_old[i] = flow_in[i];
            flow_out_old[i] = flow_out[i];
        }
        for (int i = quat[f_end].block_pos; i >= quat[f_start].block_pos; --i){
            for (auto j: suffix[i])
                if (j >= quat[f_start].block_pos && j <= quat[f_end].block_pos){ //拒绝跨函数的跨基本块(call不需要考虑数据流)
                flow_out[i].insert(flow_out[i].end(), flow_in[j].begin(), flow_in[j].end());
            }
            uniqueVector(flow_out[i]);
            flow_in[i] = use[i];
            calcDifference(flow_out[i], def[i], v_diff);
            flow_in[i].insert(flow_in[i].end(), v_diff.begin(), v_diff.end());
            uniqueVector(flow_out[i]);
            uniqueVector(flow_in[i]);
            flow_block_out << "-----------Block" << i << "-----------" << endl;
            if (flow_in[i] != flow_in_old[i] || flow_out[i] != flow_out_old[i]){
//                outVector(flow_out_old[i], "Out_Old", flow_block_out);
//                outVector(flow_in_old[i], "In_Old", flow_block_out);
                is_changed = true;
            }
            outVector(flow_out[i], "Out", flow_block_out);
            outVector(use[i], "Use", flow_block_out);
            outVector(def[i], "Def", flow_block_out);
            outVector(flow_in[i], " In", flow_block_out);
        }
    }
    
}
void flowWork(){ //首先划分基本块
    rep (i,0,MAX_QUAT){
        quat[i].block_id = quat[i].block_pos = -1;
    }
    int quat_start = 0, quat_now, block_cnt = 0;
    rep (i, 1, cnt_quat){
        if (quat[i].type=="BEGIN" || quat[i].type=="GOTO" || quat[i].type=="call" || quat[i].type == "BZ" || quat[i].type == "ret"){ //|| quat[i].type == "READ"
            quat[i].block_pos = block_cnt;
            quat[i+1].block_id = ++block_cnt; //quat[i+1].block_pos //0; //!!!BEGIN下面的那一句(即定义后的第一句话)是块的入口语句
            if (quat_start == 0)
                quat_start = i + 1;
        }
        if (quat[i].label.size() > 0 || quat[i].op3=="[]=" || quat[i].op3=="=[]"){
            if (quat[i].block_id == -1){
                quat[i].block_id = ++block_cnt;//= quat[i].block_pos //0; //!!!LABEL对应的这一句必然是入口语句
            }
            if (quat_start == 0)
                quat_start = i + 1;
        }
    }
    
    //计算每个块的block_pos
    quat_start = 1;
    block_cnt = 0;
    while (quat_start <= cnt_quat){
        quat_now = quat_start+1;
        while (quat[quat_now].block_id == -1 && quat_now <= cnt_quat)
            quat_now++;
        block_cnt++;
        rep (i,quat_start, quat_now-1)
        quat[i].block_pos = block_cnt;
        quat_start = quat_now;
    }
    flow_block_out << "----------------------Start Line Analysis--------------" << endl;
    rep (i, 1, cnt_quat){ //计算前驱集合和后继集合
        if (quat[i].type=="GOTO" || quat[i].type == "BZ" || quat[i].type=="call"){
            rep (j, 1, cnt_quat){
                if ((quat[j].type == "function_int" || quat[j].type == "function_char" || quat[j].type == "void_") && (quat[j].op1 == quat[i].op1)){
                    suffix[quat[i].block_pos].push_back(quat[j].block_pos+1); //j要加一是因为函数定义的地方还没加上去  只有在BEGIN结束之后才会加上去
                    flow_block_out << "Line" << i << "(" << quat[i].type << " " << quat[i].op1 << ")    Block:";
                    flow_block_out << quat[i].block_pos << " --> " << quat[j].block_pos+1 << endl;
                    prefix[quat[j].block_pos+1].push_back(quat[i].block_pos);
                }
                if (quat[j].label.size() > 0 && find(quat[j].label.begin(), quat[j].label.end(), calcLabel(quat[i].op1)) != quat[j].label.end()){
                    flow_block_out << "Line" << i << "(" << quat[i].type << " " << quat[i].op1 << ")    Block:";
                    flow_block_out << quat[i].block_pos << " --> " << quat[j].block_pos << endl;
                    suffix[quat[i].block_pos].push_back(quat[j].block_pos);
                    prefix[quat[j].block_pos].push_back(quat[i].block_pos);
                }
            }
        }
    }
    flow_block_out << endl;
    rep (i, 1, cnt_quat-1){ //最后一条语句不需要和出口相连边!!!//计算前驱集合和后继集合
        if (quat[i].block_pos != quat[i+1].block_pos){
            if (quat[i].type != "GOTO" && quat[i].type != "ret"){
                flow_block_out << "Line" << i << " (Block" << quat[i].block_pos << ")    NEXT: " << quat[i+1].block_pos << endl;
                suffix[quat[i].block_pos].push_back(quat[i+1].block_pos);
                prefix[quat[i+1].block_pos].push_back(quat[i].block_pos);
            }
        }
    }
    flow_block_out << "----------------------End Line Analysis-----------------" << endl << endl;
    flow_block_out << "----------------------Print Block suffix----------------" << endl;
    rep (i,1,block_cnt+1){
        flow_block_out << "Block " << i << " ----------> ";
        uniqueVector(suffix[i]);
        for (auto j: suffix[i]){
            flow_block_out << j << ",";
        }
        flow_block_out << endl;
    }
    flow_block_out << "----------------------End Block suffix------------------" << endl << endl;
    int q_start, q_end, q_now=1;
    while (q_now <= cnt_quat){
        if (quat[q_now].type == "BEGIN"){
            q_start = q_now + 1;
            while (q_now <= cnt_quat && quat[q_now].type != "function_int" && quat[q_now].type != "function_char" && quat[q_now].type != "void_"){
                q_now++;
            }
            q_end = q_now-1;
            flow_block_out << q_start << "***" << q_end << endl;
            flowPerWork(q_start, q_end, quat[q_start].program_id);
        }
        q_now++;
    }
}
#endif /* dataFlow_h */
