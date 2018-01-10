//
//  quatOptimizer.h
//  Compiler
//
//  Created by ohazyi on 2017/12/23.
//  Copyright © 2017年 ohazyi. All rights reserved.
//

#ifndef quatOptimizer_h
#define quatOptimizer_h
#define fDEBUG
#include "headers.h"
int cnt_dag, cnt_op, cnt_node, cnt_quat_new = 0; //cnt_node是节点表里的节点个数 cnt_dag是所有DAG图里的节点个数 cnt_op是dag图里的操作符也就是非叶节点的个数(因为所有的非叶节点都是运算符)
set<int> father[MAX_QUAT];
vector<pair<string, int>> route_dag;
map<string, int> mp_node;
map<int, int> mp_dag_print;
bool vis[MAX_QUAT];
struct Node{
    string value, pick="", temp = ""; //标识符的名字
    int l=-1, r = -1, cnt_print = 0;
    vector<string> names;
}dag[MAX_QUAT];
struct Quat quat_new[MAX_QUAT+10];
//bool dagFind(int k, string s){
//    for (auto i: dag[k]){
//        if (i == s){
//            return i;
//        }
//    }
//    return -1;
//}
int addLeaf(string s){
    cout << "ADDLEAF: " << s << endl;
    for (int i = cnt_dag; i >= 1; --i){
        if (dag[i].value == s || find(dag[i].names.begin(), dag[i].names.end(), s) != dag[i].names.end()){
            return i; //代表是第i个dag图的节点是s
        }
    }
    cnt_dag = cnt_dag+1;
    dag[cnt_dag].value = dag[cnt_dag].pick = s; //+"0"; //a0
    mp_node[s] = ++cnt_node;
    return cnt_dag;
}
void addOp(string op, string s, int l, int r){
    if (op=="="){
        if (dag[l].l != -1 || dag[l].r != -1){ //非叶子节点
            dag[l].names.insert(dag[l].names.begin(), s);
            mp_node[s] = l;
            return ;
        }
    }
    for (int i = cnt_dag; i >=1; --i){
        if (dag[i].value == op && dag[i].l == l && dag[i].r == r){
            dag[i].names.insert(dag[i].names.begin(), s); //插入到头部，即逆向插入 //dag[i].names.push_back(s);
            mp_dag[s] = i; //!!!更新节点表
            cout << s << "&&&" << i << endl;
            return ;
        }
    }
    cnt_dag++;
    cnt_op++; //非叶节点数
    dag[cnt_dag].value = op;
    dag[cnt_dag].names.push_back(s);
    dag[cnt_dag].l = l;
    dag[cnt_dag].r = r;
    mp_node[s] = ++cnt_node;
    if (l != -1)
        father[l].insert(cnt_dag);
    if (r != -1)
        father[r].insert(cnt_dag);
}
bool checkDAG(int k){ //检查中间节点k是否可以导出
    if (k == -1 || (dag[k].l == -1&&dag[k].r==-1) || vis[k]) //导出的必然是之前没有导出中间节点
        return false;
    for (auto i: father[k]){
        if (!vis[i]){
            return false;
        }
    }
    return true;
}
void addNode(int k){
    cout << k << " " << dag[k].value << " " << dag[k].pick << endl;
    for (auto i: dag[k].names){
        cout << i << " ";
    }
    cout << endl;
    int cnt_export = 0, cnt_temp = 0;
    string s_temp;
    for (auto i: dag[k].names){
        if (i.size() >0 && i[0] == '#'){
            if (cnt_temp == 0){
                s_temp = i; //选取第一个临时变量作为我们中间节点的导出的点
            }
            cnt_temp++;
        }else{
            cnt_export++;
            route_dag.push_back(make_pair(i, k)); //{a, 3}
            cout << "ADD_TO_ROUTE:(" << i << "," << k << ")" << endl;
            dag[k].pick = i; //随便选取一个标识符的名字作为这个中间节点的名字代表 //用最后面那个也就是第一次插入的那个，因为names是每次插入到头部即逆向插入的
        }
        //!!!对于所有的全局和out集合里的点，都需要导出到节点序列 然后如果只有临时变量只需要导出一个
    }
    
    
    if (cnt_export == 0 || dag[k].cnt_print > 0){ //!!!如果这个节点有需要输出，那么必须给一个临时变量(这样做是为了防止输出pick，但是pick的值变了的情况)
        if (cnt_temp == 0)
            cout << "!!!ERROR:DAG!!!" << endl;
        route_dag.push_back(make_pair(s_temp, k));
        cout << "ADD_TO_ROUTE:(" << s_temp << "," << k << ")" << endl;
//        if (cnt_export == 0) //!!!
        dag[k].temp = dag[k].pick = s_temp;
//        else
//            dag[k].temp = s_temp; //!!!(可以省去看效果)如果是已经有对应的了，就不要用临时变量来更新了。感觉还是用局部变量代替比较权威。
    }
    
    
}
void dfs(int k){
    int v;
    addNode(k);
    vis[k] = true;
    if ((dag[k].l != -1 || dag[k].r != -1) && !vis[k]){ //只会dfs非叶节点
        v = dag[k].l; //按照该启发式
        if (checkDAG(v)){
            dfs(v);
        }
    }
}
void vecGenerator(int program_id){
    while (route_dag.size() < cnt_op){
        for (int i = cnt_dag; i >= 1; --i){
            if (checkDAG(i)){
//                addNode(i);
                dfs(i);
            }
        }
    }
    
    for (int i = (int)route_dag.size()-1; i >= 0; --i){
        int v = route_dag[i].second;
        int l = dag[v].l, r = dag[v].r;
        cnt_quat_new++;
        quat_new[cnt_quat_new].type = "=";
        if (dag[v].value == "="){
            quat_new[cnt_quat_new].op1 = route_dag[i].first;
            quat_new[cnt_quat_new].op2 = dag[l].pick;
            quat_new[cnt_quat_new].program_id = program_id;
            cout << route_dag[i].first << "=" << dag[l].pick << endl;
        }
        else{
            quat_new[cnt_quat_new].type = dag[v].value;
            quat_new[cnt_quat_new].op1 = route_dag[i].first;
            quat_new[cnt_quat_new].op2 = dag[l].pick;
            quat_new[cnt_quat_new].op3 = dag[r].pick;
            quat_new[cnt_quat_new].program_id = program_id;
            cout << route_dag[i].first << "=" << dag[l].pick << dag[v].value << dag[r].pick << endl;
        }
    }
}
void dagBlock(int quat_start, int quat_end){
    if (quat_start == 46){
        int pp;
        pp = 1;
    }
    cout << "***************" << quat_start << "    " << quat_end << "**************" << endl;
    int l, r;
    string s1, s2;
    memset(vis, false, sizeof(vis));
    mp_node.clear();
    route_dag.clear();
    cnt_dag = cnt_op = cnt_node = 0;
    rep (i,quat_start,quat_end){//1,cnt_quat) {
        switch (mp_dag[quat[i].type]){
            case 10: { // + - * /
                l = addLeaf(quat[i].op2);
                r = addLeaf(quat[i].op3);
                addOp(quat[i].type, quat[i].op1, l, r);
                break;
            }
            case 20: {// =
                //!!!  之前没有注释掉这段，我很好奇是怎么还能对的
//                if (quat[i].op3 == "=[]"){
//                    splitArrayName(quat[i].op2, s1, s2);
//                    l = addLeaf(s1);
//                    r = addLeaf(s2);
//                    addOp(quat[i].type, quat[i].op1, l, r);
//                }else if (quat[i].op3 != "[]=")
                
                {
                    l = addLeaf(quat[i].op2);
                    addOp(quat[i].type, quat[i].op1, l, -1);
                }
                break;
            }
            case 60: { //PRINT
                if (quat[i].op2=="string")
                    break;
                int pos = -1;
                for (auto it = mp_node.begin(); it != mp_node.end(); ++it){
                    cout << it->first << ":::" << it->second << endl;
                    if (it->first == quat[i].op1) {
                        pos = it->second;
                        break;
                    }
                    if (find(dag[it->second].names.begin(), dag[it->second].names.end(), quat[i].op1) != dag[it->second].names.end()){
                        pos = it->second;
                        break;
                    }
                }
                if (pos == -1){ //加上了一个临时变量来保存输出后，应该不会再出现pos=-1的情况了
                    cout << "!!!ERROR:dag missing print temp!!!" << endl;
                }else{
                    dag[pos].cnt_print++; //把这个中间节点对应的输出次数加一
                    mp_dag_print[i] = pos; //输出的是第几号临时变量 //因为临时变量是不会在节点表上更新的，因此我们可以找到保存之后一起输出
                }
                break;
            }
//            default:
//                quat_new[++cnt_quat_new] = quat[i];
        }
    }
    if (quat[quat_start].label.size() > 0 && (mp_dag[quat[quat_start].type] == 10 || mp_dag[quat[quat_start].type]== 20)){ //防止该基本块的原来第一条语句是有label, 而且新的基本块是我们导出的，这样label信息就要保留
        cout << "%^&#*!@$@" << quat_start << endl;
        quat_new[cnt_quat_new+1].label = quat[quat_start].label;
    }
    int quat_cnt_new_now = cnt_quat_new+1;
    vecGenerator(quat[quat_start].program_id);
    
    
    int test_cnt = 0;
    
    rep (i,quat_start,quat_end){
        
        if (quat[i].label.size() > 0)
            test_cnt++;
        
        if (quat[i].type=="PRINT"){
            if (quat[i].op2=="string"){ //addQuat("PRINT", prin_str, "string", "");
//                cnt_quat_new++;
//                quat_new[cnt_quat_new].type = "PRINT";
//                quat_new[cnt_quat_new].op1 = quat[i].op1;
//                quat_new[cnt_quat_new].op2 = "";
                quat_new[++cnt_quat_new] = quat[i];
                
                
                quat_new[cnt_quat_new].label.clear();
                
                
                cout << "PRINT_string " << quat[i].op1 << endl;
            }
            else{
                quat_new[++cnt_quat_new] = quat[i];
                
                
                quat_new[cnt_quat_new].label.clear();
                
                
                quat_new[cnt_quat_new].op1 = dag[mp_dag_print[i]].temp;
                cout << "PRINT_" << quat[i].op2 << " " << dag[mp_dag_print[i]].temp << endl;
            }
        }else if (mp_dag[quat[i].type] != 10 && mp_dag[quat[i].type] != 20){ // 不是= +-*/
            cout << quat[i].type << " ~~~~" << endl;
            quat_new[++cnt_quat_new] = quat[i];
            
            quat_new[cnt_quat_new].label.clear();
            
            
        }
    }
    
    cout << "()()" << quat_start << "()()" << quat_end << "()()" << test_cnt << endl;
    
    int cnt_this_quat = 0;
    rep (i, quat_start, quat_end){
        if (quat[i].label.size() > 0){
            quat_new[quat_cnt_new_now].label = quat[i].label;
            
            cnt_this_quat++;
        }
        if (cnt_this_quat>1){
            cout << "!!!ERROR:more than a label in a basic block$$$" << endl;
        }
    }
    
    
    
    rep (i, 0, cnt_dag){
        father[i].clear();
        dag[i].value = dag[i].pick = dag[i].temp = ""; //标识符的名字
        dag[i].l = dag[i].r = -1;
        dag[i].cnt_print = 0;
        dag[i].names.clear();
    }
    cout << "------------------------------" << endl;
}
void dagWork(){ //处理DAG图，首先划分基本块
    int quat_start = 0, quat_now;
    printSide();
    rep (i, 1, cnt_quat){
        if (quat[i].type=="BEGIN" || quat[i].type=="GOTO" || quat[i].type=="call" || quat[i].type == "BZ" || quat[i].type == "ret" || quat[i].type == "READ"){ //quat[i].op3=="[]=" || quat[i].op3=="=[]"
            cout << "^^^SEP" << i+1 << endl;
            if (i+1==73){
                int fuck;
                fuck = 250;
            }
            quat[i+1].block_id = 0; //!!!BEGIN下面的那一句(即定义后的第一句话)是块的入口语句
            if (quat_start == 0)
                quat_start = i + 1;
        }
        if (quat[i].label.size() > 0 || quat[i].op3=="[]=" || quat[i].op3=="=[]"){ // 数组放哪儿想清楚，之前只把a[i] = j放在最后好像没问题，现在决定放在这儿//因为不能#1 = a[i]; 然后到一个新的基本块print #1
            cout << "%%%SEP" << i << "^^" << endl;
            quat[i].block_id = 0; //!!!LABEL对应的这一句必然是入口语句
            if (quat_start == 0)
                quat_start = i + 1;
        }
    }
    rep (i,1,quat_start-1){
        quat_new[++cnt_quat_new] = quat[i];
    }
    while (quat_start <= cnt_quat){
        quat_now = quat_start+1;
        while (quat[quat_now].block_id == -1 && quat_now <= cnt_quat)
            quat_now++;
        dagBlock(quat_start, quat_now-1);
//        if (quat_now <= cnt_quat)
//            dagBlock(quat_start, quat_now-1);
//        else{ //更新到新的quat里面
//            rep (i, quat_start,cnt_quat)
//                quat_new[++cnt_quat_new] = quat[i];
//        }
        quat_start = quat_now;
    }
//    memcpy(quat, quat_new, sizeof(quat)); //!!!不知道为什么的 执行这句就会return后crash
    rep (i,0,MAX_QUAT){
        quat[i] = quat_new[i];
    }
    cnt_quat = cnt_quat_new;
    printQuat(); //quat_new, cnt_quat_new
}

#endif /* quatOptimizer_h */
