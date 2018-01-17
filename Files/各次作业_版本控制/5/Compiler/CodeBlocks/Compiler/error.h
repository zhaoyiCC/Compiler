//
//  error.h
//  Compiler
//
//  Created by ohazyi on 2017/12/11.
//  Copyright © 2017年 ohazyi. All rights reserved.
//

#ifndef error_h
#define error_h
int test(vector<string> v, int errorId){ //判断必须是这个里面的内容，否则就是第errorId号错误
    if (find(v.begin(), v.end(), sym) == v.end()){ //find出的位置(迭代器)在这个集合的末尾，代表没找到
        error(errorId);
        v.push_back(";");
        v.push_back("}");
        while (find(v.begin(), v.end(), sym) == v.end()){
            id = lexicalAnalysis(str, sym);
        }
        if (now >= siz) //{ printQuat(); exit(0); } //代表已经处理完了
            return 0;
        //!!!skip(v, errorId);???
    }
    return 1;
}
void error(int errorId){
    cout << "!!!ERROR!!!" << endl;
    cout << error_msg[errorId] << endl << endl;
    //    switch (errorId) {
    //        case 1: cout << error_msg[errorId] << endl << endl; break;
    //        case 2: cout << error_msg[errorId] << endl << endl; break;
    //    }
}
#endif /* error_h */
