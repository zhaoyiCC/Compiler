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
#include "mipsGenerator.h"
int main() {
    int i, Int = 0;
    printf("Int=%d\n",Int);
    for (i = Int; i <= 1000-Int; i = i+1){
//        for (i = Int; i <= i-1; i = i+1){
//            Int = Int+1;
//        }
        Int = Int+1;
    }
    printf("Int=%d\n",Int);
    
//    freopen("out.txt","w",stdout);
//    ofstream asm_out("asm.txt");
//    asm_out << "123" << endl;
    
    cout << "Please input a file:" << endl;
//    cin >> route;
//    ifstream r_t(route);
//    if (!r_t){
//        cout << "File Not exist" << endl;
//        return 0;
//    }
    route = "14011100_test.txt";//"15.txt";
    std::ifstream t(route);//"");
    std::string s((std::istreambuf_iterator<char>(t)),
                  std::istreambuf_iterator<char>());
    
    str = s;
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
    
    test({"("}, 45); //main()
    id = lexicalAnalysis(str, sym);
    test({")"}, 46);
    id = lexicalAnalysis(str, sym);
    
    enter("main","void","",0,0,0); //把void main加入到符号表
    addQuat("void_", "main", "", "");
    main_pos = cnt_quat; //标记住main函数开始的地方
    addr = 0;
    statExecution(str, true, true); //是复合语句，即可以有const和变量定义
    index_proc[cnt_proc+1] = cnt_tab+1; //为了统一操作，在最后一个加上起始
    if (now < siz)
        error(99);
    cout << "siz = " << siz << " now = " << now << endl << endl;
    
    printQuat();
    programTable();
    symbolTable();
    calcTmp(); //cout << mp_tmp["#1"].second << endl;
    quatMips();
    
    return 0;
}

