#ifndef error_h
#define error_h
int test(vector<string> v, int errorId, vector<string> v_skip, int line_pos){ //判断必须是这个里面的内容，否则就是第errorId号错误
    if (find(v.begin(), v.end(), sym) == v.end()){ //find出的位置(迭代器)在这个集合的末尾，代表没找到
        error(errorId, line_pos);
        v.push_back(";");
        v.push_back("}");
        v.insert(v.end(), v_skip.begin(), v_skip.end()); //把v_skip的内容加到v里面，即都可以跳过
        if (now >= siz) //{ printQuat(); exit(0); } //代表已经处理完了
            return 0;
        while (find(v.begin(), v.end(), sym) == v.end()){
            id = lexicalAnalysis(str, sym);
        }
        if (now >= siz) //{ printQuat(); exit(0); } //代表已经处理完了
            return 0;
        return 0;
        //!!!skip(v, errorId);???
    }
    return 1;
}

void error(int errorId, int line_pos){
    cout << "!!!ERROR!!!" << endl;
    cout << error_msg[errorId] << "   Line:" << line_pos << endl << endl;
}
#endif /* error_h */
