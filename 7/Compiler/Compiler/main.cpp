//
//  main.cpp
//  LexicalAnalysis
//
//  Created by ohazyi on 2017/11/22.
//  Copyright © 2017 ohazyi. All rights reserved.
//  C++11!!!
//  函数名尽量驼峰，变量名尽量下划线
/*
 consDeclartion：处理常量定义
 variDeclartion：处理变量定义
 */
//函数名也是和变量名一样放在mp里吗？？？
#include "enter.h"
#include "lexicalHandler.h"
#include "grammarHandler.h"
#include "mipsGenerator_new.h"
#include "quatOptimizer.h"
#include "dataFlow.h"

int main() {
    
    vector<string> one = {"1","2","3","4","5"};
    vector<string> two = {"4","5", "6"};
    vector<string> result;
    calcDifference(one, two, result);
    for (auto p: result){
        cout << p << endl;
    }
    
//    cout << calcLabel("LABEL_100") << endl;
    
//    freopen("out.txt","w",stdout);

//    ofstream mcfile; //创建个对象
//    mcfile.open("asm.txt"); //创建文件 //那个在mipsGenerator的文件是不会创建的，但是之前有asm.txt是写的进去的
    cout << "Please input a file:" << endl;
    cin >> route;
    ifstream r_t(route);
    if (!r_t){
        cout << "File Not exist" << endl;
        return 0;
    }
//    route = "14011100_test.txt";

//    std::ifstream t(route);//"");
//    std::string s((std::istreambuf_iterator<char>(t)),
//                  std::istreambuf_iterator<char>());
//    transform(s.begin(), s.end(), s.begin(), ::tolower); //全部转成小写，应付while文法的！

    string s = "";
    FILE *fp;
    char c;
    fp = fopen(route.data(), "r");
    int line_num = 1, str_id = 0;
    while ((c = fgetc(fp)) != EOF){
        if (c == '\\'){ //转义字符要原样输出
            s = s+c;
            mp_line[str_id++] = line_num;
        }
        s = s+c;
        mp_line[str_id++] = line_num;
        if (c=='\n'){
            line_num++;
        }
    }
    str = s;
//    return 0;

    siz = str.size();   //cout << str << endl << endl;
    //lexical(str); //语义分析
    now = 0;
    index_proc[0] = 1; //第0个分程序表的第一个变量是1
    id = lexicalAnalysis(str, sym); //getNext();

    consDeclarion(str);
    variDeclation(str);
    addr = 0;
    is_global = false; //之后进入函数/过程定义，都不是全局变量
    procDeclartion(str);

    test({"("}, 45, {}, last); //main()
    id = lexicalAnalysis(str, sym);
    test({")"}, 46, {}, last);
    id = lexicalAnalysis(str, sym);

    enter("main","void","",0,0,0, last); //把void main加入到符号表
    addQuat("void_", "main", "", "");
    main_pos = cnt_quat; //标记住main函数开始的地方
    addr = 0;
    statExecution(str, true, true); //是复合语句，即可以有const和变量定义
    index_proc[cnt_proc+1] = cnt_tab+1; //为了统一操作，在最后一个加上起始
    if (now < siz)
        error(99, last);
    #ifdef debug
    cout << "siz = " << siz << " now = " << now << endl << endl;
    #endif // debug
    printQuat(); //(quat, cnt_quat)
    programTable();
    symbolTable();
    calcTmp(); //cout << mp_tmp["#1"].second << endl;
    
    
    dagWork();
    flowWork();
    
    
    quatMips();

    return 0;
}
