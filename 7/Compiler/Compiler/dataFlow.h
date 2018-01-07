//
//  dataFlow.h
//  Compiler
//
//  Created by ohazyi on 2018/1/5.
//  Copyright © 2018年 ohazyi. All rights reserved.
//

#ifndef dataFlow_h
#define dataFlow_h
#define fDEBUG
#include "headers.h"

vector<string> flow_in[MAX_QUAT], flow_out[MAX_QUAT], flow_in_old[MAX_QUAT], flow_out_old[MAX_QUAT], def[MAX_QUAT], use[MAX_QUAT];
vector<int> suffix[MAX_QUAT], prefix[MAX_QUAT];

void calcDefUse(int quat_start, int quat_end, int block_id){
    
    cout << "***************" << quat_start << "    " << quat_end << "    " << block_id << "**************" << endl;
    map<string, int> vis;
    
    rep (i, quat_start, quat_end){
        quat[i].block_pos = block_id;
        string op1 = quat[i].op1, op2 = quat[i].op2, op3 = quat[i].op3;
        cout << quat[i].type << "!!!" << op1 << "!!!" << op2 << "!!!" << op3 << endl;
        if (op2 != "" && vis[op2] == 0){
            use[block_id].push_back(op2);
            vis[op2]++;
        }
        if (op3 != "" && vis[op3] == 0){
            use[block_id].push_back(op3);
            vis[op3]++;
        }
        if (op1 != "" && vis[op1] == 0){
            def[block_id].push_back(op1);
            vis[op1]++;
        }
    }
    
    cout << "------------------------------" << endl;
}

void flowWork(){ //首先划分基本块
    rep (i,0,MAX_QUAT){
        quat[i].block_id = quat[i].block_pos = -1;
    }
    int quat_start = 0, quat_now, block_cnt = 0;
    printSide();
    rep (i, 1, cnt_quat){
        if (quat[i].type=="BEGIN" || quat[i].type=="GOTO" || quat[i].type=="call" || quat[i].type == "BZ" || quat[i].type == "ret" || quat[i].type == "READ" || quat[i].op3=="[]="){
            quat[i].block_pos = block_cnt;
            quat[i+1].block_id = ++block_cnt; //quat[i+1].block_pos //0; //!!!BEGIN下面的那一句(即定义后的第一句话)是块的入口语句
            cout << "&&&SEP" << i+1 << endl;
            if (i+1==72){
                int ily;
                ily = 1;
            }
            if (quat_start == 0)
                quat_start = i + 1;
        }
        if (quat[i].label.size() > 0){
            if (quat[i].block_id == -1){
                quat[i].block_id = ++block_cnt;//= quat[i].block_pos //0; //!!!LABEL对应的这一句必然是入口语句
                cout << "&&&" << i << endl;
            }
            if (quat_start == 0)
                quat_start = i + 1;
        }
//        if (quat[i].block_id == -1)
//            quat[i].block_pos = block_cnt;
    }
    
//    rep (i, 1, cnt_quat){ //计算前驱集合和后继集合
//        if (quat[i].type=="GOTO" || quat[i].type == "BZ" || quat[i].type=="call"){
//            int ppppppp = 1;
//            ppppppp ++;
//            cout << quat[i].op1 << endl;
//            rep (j, 1, cnt_quat){
//                if ((quat[j].type == "function_int" || quat[j].type == "function_char" || quat[j].type == "void_") && (quat[j].op1 == quat[i].op1)){
//                    suffix[quat[i].block_pos].push_back(quat[j].block_pos+1); //j要加一是因为函数定义的地方还没加上去  只有在BEGIN结束之后才会加上去
//                    cout << quat[i].type << " " << quat[i].op1 << endl;
//                    cout << quat[i].block_pos << " --> " << quat[j].block_pos+1 << endl;
//                    prefix[quat[j].block_id+1].push_back(quat[i].block_pos);
//                    
//                }
//                
//                if (quat[j].label.size() > 0 && find(quat[j].label.begin(), quat[j].label.end(), calcLabel(quat[i].op1)) != quat[j].label.end())
//                {
//                    suffix[quat[i].block_id].push_back(quat[j].block_pos);
//                    prefix[quat[j].block_id].push_back(quat[i].block_pos);
//                }
//            }
//        }
//        
//    }
    
    block_cnt = 0;
    while (quat_start <= cnt_quat){
        quat_now = quat_start+1;
        while (quat[quat_now].block_id == -1 && quat_now <= cnt_quat)
            quat_now++;
        calcDefUse(quat_start, quat_now-1, ++block_cnt); //计算def集和use集
        quat_start = quat_now;
    }
    
    /*
    rep (i, 1, cnt_quat){ //计算前驱集合和后继集合
        if (quat[i].type=="GOTO" || quat[i].type == "BZ" || quat[i].type=="call"){
            cout << quat[i].op1 << endl;
            rep (j, 1, cnt_quat){
                if ((quat[j].type == "function_int" || quat[j].type == "function_char" || quat[j].type == "void_") && (quat[j].op1 == quat[i].op1)){
                    suffix[quat[i].block_pos].push_back(quat[j].block_pos+1); //j要加一是因为函数定义的地方还没加上去  只有在BEGIN结束之后才会加上去
                    cout << quat[i].type << " " << quat[i].op1 << endl;
                    cout << quat[i].block_pos << " --> " << quat[j].block_pos+1 << endl;
                    prefix[quat[j].block_pos+1].push_back(quat[i].block_pos);
                }
                
                if (quat[i].op1.substr(0,6)=="LABEL_")
                if (quat[j].label.size() > 0 && find(quat[j].label.begin(), quat[j].label.end(), calcLabel(quat[i].op1)) != quat[j].label.end())
                {
                    suffix[quat[i].block_pos].push_back(quat[j].block_pos+1);
                    prefix[quat[j].block_pos+1].push_back(quat[i].block_pos);
                    cout << quat[i].type << " $$$ " << quat[i].op1 << endl;
                    cout << quat[i].block_pos << " --> " << quat[j].block_pos+1 << endl;
                }
//                cout << j << endl;
            }
        }
        if (quat[i].type == "BZ"){
            suffix[quat[i].block_pos].push_back(quat[i].block_pos+1);
            prefix[quat[i].block_pos+1].push_back(quat[i].block_pos);
        }
    }
    
    vector<string> v_diff;
    //迭代进行计算IN 和 OUT集
    bool is_changed = true;
    while (is_changed){
        is_changed = false;
        memcpy(flow_in_old, flow_in, sizeof(flow_in));
        memcpy(flow_out_old, flow_out, sizeof(flow_out));
        for (int i = block_cnt; i >= 1; --i){
            for (auto j: suffix[i]){
                flow_out[i].insert(flow_out[i].end(), flow_in[j].begin(), flow_in[j].end());
            }
            flow_in[i] = use[i];
            calcDifference(flow_out[i], def[i], v_diff);
            flow_in[i].insert(flow_in[i].end(), v_diff.begin(), v_diff.end());
            if (flow_in[i] != flow_in_old[i] || flow_out[i] != flow_out_old[i])
                is_changed = true;
        }
    }
    
    memcpy(quat, quat_new, sizeof(quat));
    cnt_quat = cnt_quat_new;
    printQuat(); //quat_new, cnt_quat_new
     */
}

#endif /* dataFlow_h */
