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

#include <iostream>
#include <cstdio>
#include <cstring>
#include <string>
#include <map>
#include <iostream>
#include <cmath>
#include <vector>
#include <set>
#include <algorithm>
#include <queue>
#include <fstream>
#include <streambuf>
#include <ctype.h>
#define rep(i,a,b) for (int i = (a); i <= (b); ++i)
using namespace std;
void expression(string& str), statement(string& str), funcCall(string& str), statExecution(string& str, bool is_multi_statement);

int id, cnt = 999, now = 0, siz, last;
string sym, str;
//string sym; //获取到的接下来的一个字符串
vector<char> oper_rela = {'<', '>', '='};
vector<char> oper = {'+', '-', '*', '/'};
vector<char> oper_bra = {'(', ')', '[', ']', '{', '}'};
vector<char> oper_punc = {';', ',', ':'};

map<string,int> mp = {
    {"<", 1},{"<=", 2},{">", 3},{">=", 4},{"!=", 5},{"==", 6},
    
    {"+", 7},{"-", 8},{"*", 9},{"/", 10},
    
    {"'", 11},{"\"", 12},{"(", 13},{")", 14},{"[", 15},{"]", 16},{"{", 17},{"}", 18},{";", 19},{",", 20},{":", 21},
    
    {"=", 22},
    
    {"int", 100},{"char", 101},{"const", 102},{"if", 103},{"else", 104},{"for", 105},{"switch", 106},{"case", 107},{"void", 108},{"return", 109},{"main", 110},
    {"scanf", 111}, {"printf", 112},
    
};
map<string,string> mp_out = {
    {"<", "blt"},{"<=", "blt"},{">", "bgt"},{">=", "bge"},{"!=", "bne"},{"==", "beq"},
    
    {"+", "add"},{"-", "sub"},{"*", "mul"},{"/", "div"},
    
    {"(", "LeftParentheis"},{")", "RightParenthesis"},{"[", "LeftBracket"},{"]","RightBracket"},{"{", "LeftBrace"},{"}", "RightBrace"},
    
    {";", "Semicolon"},{",", "Comma"},{":", "colon"},
    
    {"=", "assign"}
};
map<int,string> error_msg = {
    {1, "const must be a int or char"},
    {2, "const must need a ="},
    {3, "const must need a variable name"},
    {4, "variable definition must need a variable name"},
    {5, "[ must follow a number"},
    {6, "Array can't be with 0"},
    
    
    {9, "void must have a ("},
    {10, "void papameter list must have a )"},
    
    {12, "if must have a ("},
    {13, "expression must have a )"},
    {14, "wrong format of expression"},
    {15, "missing ["},
    {16, "function call must have a ("},
    {17, "missing )"},
    {18, "for missing a ("},
    {19, "for should have a identifier"},
    {20, "for missing a ="},
    {21, "for missing a ;"},
    {22, "for must have a +|-"},
    {23, "for step must be a integer"},
    {24, "for step cannot have leading zeros"},
    {25, "for step must have a )"},
    {26, "scanf must have a ("},
    {27, "scanf must have a identifier"},
    {28, "scanf must have a )"},
    {29, "printf must have a ("},
    {30, "printf must have a )"},
    {31, "return must start with a ("},
    {32, "return must end with a )"},
    {33, "switch must have a ("},
    {34, "switch must have a )"},
    {35, "switch must have a {"},
    {36, "switch must have a }"},
    {37, "case must have at least a case"},
    {38, "case must follow a constant"},
    {40, "case must have a :"},
    {41, "switch must have a default statement"},
    {42, "default must have a :"},
    
    {50, "assign missing a ]"},
    {51, "assign missing a ="},
    
    {55, "const missing a ;"},
    
    {56, "missing a {"},
    {57, "const must be a int if const int . ="},
    {57, "const must be a char if const char . ="},
    
    {59, "case constant must have a unsigned int after +/-"},
    
    {72, "call missing a )"},
    
    {88, "!!!Error: statement:Unknown Indentifier!!!"},
    
    {100, "missing a ;"},
    {101, "extra content after program ends."}
};


int lexicalAnalysis(string& str, string& s){ //词法分析
    if (now>=siz) exit(0); //代表已经处理完成了
    s = "";
    
    while (isspace(str[now]))
        now++;
    last = now; //在之前的while语句后面是为了避免语法分析输出This is a ...的时候前面有很多多余的回车或空格
    if (now >= siz){
        return -2;
    }
    s += str[now++];
    if (isalpha(str[now-1]) || str[now-1] == '_'){ //标识符的第一个必须是文法里的字母(即字母或数字)
        while (isalnum(str[now]) || str[now] == '_'){ //isalnum代表是字母或数字
            s += str[now++];
        }
        if (mp.count(s) > 0){ //已经在表中
            return mp[s];
        }else{ //需要新增加的标识符
            mp[s] = ++cnt;
            return cnt;
        }
    }
    if (isdigit(str[now-1])){ //如果是数字，就一直判断直至不是数字
        while (isdigit(str[now])){
            s += str[now++];
        }
        return 99;
    }
    if (str[now-1]=='<' || str[now-1]=='>' || str[now-1] == '=' || str[now-1]=='!'){//遇到<,>,=,!，则判断其后面那个符号是不是等于号
        if (str[now] == '='){
            s += str[now++]; //<= >=
        }
        return mp[s];
    }
    if (find(oper.begin(), oper.end(), str[now-1]) != oper.end()){ //str[now] == '+' or '-' or '*' or '/'
        return mp[s];
    }
    if (str[now-1] == '\''){ //处理单引号
        s += str[now++]; //这个把真实的字符常量加进去答案
        s += str[now++]; //这个把后面那个单引号加进去
        return 98;//代表是字符常量
    }
    if (str[now-1] == '"'){ //处理双引号，一直读到后面一个双引号，认定这一部分是字符串。
        while (str[now] != '"'){
            s += str[now++];
        }
        s += str[now++];
        return 97;//代表是字符串
    }
    if (find(oper_bra.begin(), oper_bra.end(), str[now-1]) != oper_bra.end() || find(oper_punc.begin(), oper_punc.end(), str[now-1]) != oper_punc.end()){ //代表是括号或者分号或者逗号，然后这个是单个处理的，即左括号输出一次，右括号输出一次
        return mp[s];
    }
    return -1;
}
void lexical(string& str){
    string s; // s是语义分析里的sym的作用
    int lex_line = 0;
    while (now < siz){
        int value = lexicalAnalysis(str, sym);
        if (value == -2){} //代表是最后终止了，直接跳过
        else if (value == -1){ //代表没有找到，报wrong
            printf("%d !!!WRONG!!! %s\n", ++lex_line, sym.data());
        }
        else if (value >= 100 && value < 1000){//保留字
            printf("%d KeyWord %s\n", ++lex_line, sym.data());
        }
        else if (value >= 1000){ //标识符
            printf("%d Identifier %s\n", ++lex_line, sym.data());
        }else if (value >= 1 && value <= 50){ //代表是一些运算符等 !!!!!50代修改
            printf("%d %s %s\n", ++lex_line, mp_out[sym].data(), sym.data());
        }
        else if (value == 99){ //代表是整数常量
            printf("%d ConstantInt %s\n", ++lex_line, sym.data());
        }
        else if (value == 98){ //代表是字符常量
            printf("%d ConstantChar %s\n", ++lex_line, sym.data());
        }
        else if (value == 97){ //代表是字符串常量
            printf("%d ConstantString %s\n", ++lex_line, sym.data());
        }else { //其它的返回值
            printf("%d !!!NotFound!!! %s\n", ++lex_line, sym.data());
        }
    }
}

//string getNext(){
//
//}
void enter(string type){
    /*
     if (mp.count(sym) > 0) {//之前已经存储过 // determine if duplicate
     //! 错误处理
     cout << sym << "Has defind" << endl << endl;
     }
     mp[sym] = ++cnt;
     */
    //tab[] = type;
}
void error(int errorId){
    cout << "!!!ERROR!!!" << endl;
    cout << error_msg[errorId] << endl << endl;
    //    switch (errorId) {
    //        case 1: cout << error_msg[errorId] << endl << endl; break;
    //        case 2: cout << error_msg[errorId] << endl << endl; break;
    //    }
}
int test(vector<string> v, int errorId){ //判断必须是这个里面的内容，否则就是第errorId号错误
    //    for (auto& i: v){
    //        cout << i << endl << endl;
    //    }
    if (find(v.begin(), v.end(), sym) == v.end()){ //find出的位置(迭代器)在这个集合的末尾，代表没找到
        error(errorId);
        v.push_back(";");
        v.push_back("}");
        while (find(v.begin(), v.end(), sym) == v.end()){
            id = lexicalAnalysis(str, sym);
        }
        if (now >= siz) exit(0); //代表已经处理完了
        //!!!skip(v, errorId);???
    }
    return 1;
}
void consDeclarion(string& str) { // ＜常量说明＞ ::=  const＜常量定义＞;{ const＜常量定义＞;}
    //＜常量定义＞   ::=   int＜标识符＞＝＜整数＞{,＜标识符＞＝＜整数＞}
    //char＜标识符＞＝＜字符＞{,＜标识符＞＝＜字符＞}
    int pos_line_header, const_cnt;
    while (sym == "const") { // 重复处理多个const语句
        const_cnt = 0;
        pos_line_header = last;
        id = lexicalAnalysis(str, sym);//getNext(); // int | char, else wrong
        test({"int","char"}, 1); //必须是int或者char类型
        string cons_type = "const_" + sym; //"constint" 代表是个int类型的常量，“constchar" ...
        do{
            id = lexicalAnalysis(str, sym); //读入标识符的名字
            enter("constValue"); //代表这个标识符的类型是常量
            
            id = lexicalAnalysis(str, sym); //sym = getNext(); //读入到=
            test({"="}, 2);
            id = lexicalAnalysis(str, sym);
            if (cons_type == "const_int"){ //对于const int ... 跟上的是一个(有符号)整数，因此要判断有无 + -
                if (sym == "+" || sym =="-"){
                    id = lexicalAnalysis(str, sym);
                }
                if (id != 99) //代表不是(无符号)整数
                    error(56);
            }else{ //代表是const char ...
                if (id != 98)//代表不是是字符常量
                    error(57);
            }
            id = lexicalAnalysis(str, sym); //跟上的是一个分号或者逗号
            const_cnt++;
        }while (sym == ",");
        
        test({";"}, 55);
        cout << "This is a " + cons_type + " statement::: " << const_cnt << "elements::: " << str.substr(pos_line_header, last-pos_line_header) << endl << endl;
        id = lexicalAnalysis(str, sym); //getNext();
    }
}
void variDeclation(string& str){
    int variable_cnt, pos_line_header = last; //pos_line_header为这一行的行首在哪，给后来做回退用
    while (sym == "int" || sym == "char"){
        variable_cnt = 0;
        test({"int","char"}, 1); //必须是int或者char类型
        string type = "variable_" + sym; //"constint" 代表是个int类型的常量，“constchar" ...
        do{
            id = lexicalAnalysis(str, sym); //得到变量名
            if (id < 1000) //如果不是标识符，就报错
                error(4);
            id = lexicalAnalysis(str, sym);
            if (sym == "["){ //考虑数组的定义，即int a[100] 注意我们的文法与C语言不同没有函数的声明只有函数的定义
                id = lexicalAnalysis(str, sym);
                //cout << sym << endl << endl;
                if (id != 99)
                    error(5);
                else if (stoi(sym)==0) //把字符串转换为整数，等效于sym=="0"
                    error(6);
                id = lexicalAnalysis(str, sym);
                test({"]"}, 7);
                id = lexicalAnalysis(str, sym);
            }else if (sym == "("){ //带(即参数的int定义显然是函数或者过程，回退到这一行行首
                if (variable_cnt > 0)//代表之前已经有变量被定义了，报错 int i, work();
                    error(7);
                now = pos_line_header;
                id = lexicalAnalysis(str, sym);
                return ;//直接退出了，不用再继续判断变量定义了
            }
            variable_cnt++;
        }while (sym == ",");
        test({";"}, 8);
        cout << "This is a "+ type +" statement::: " << variable_cnt << " variables::: " << str.substr(pos_line_header, last-pos_line_header) << endl << endl;
        pos_line_header = now;
        id = lexicalAnalysis(str, sym);
    }
}
void parameter(string& str){ //＜值参数表＞   ::= ＜表达式＞{,＜表达式＞}｜＜空＞
    int para_cnt = 0, pos_line_header = last;
    if (sym==")") {//应对值参数表为空的情况，因为表达式不会以)开始 也许有些多余，因为怕现在处理出错
        //id = lexicalAnalysis(str, sym);
        return ;
    }
    else{
        do{
            if (para_cnt != 0){ //因为第一次在函数调用前已经读进来了标识符，因此不需要读入标识符了，之后的都需要跳过逗号
                id = lexicalAnalysis(str, sym); //得到变量名
            }
            expression(str);
            para_cnt++;
        }while (sym == ",");
    }
    cout << "This is a parameter with ::: " << para_cnt << " parameters::: " << str.substr(pos_line_header, last-pos_line_header) << endl << endl;
}
void funcCall(string& str){ //函数调用 //＜有（无）返回值函数调用语句＞ ::= ＜标识符＞‘(’＜值参数表＞‘)’
    string func_name = sym; //把函数名保留一下
    int pos_line_header = last;
    id = lexicalAnalysis(str, sym);
    test({"("},16);
    id = lexicalAnalysis(str, sym);
    parameter(str);//调用值参数表
    test({")"}, 17);
    
    id = lexicalAnalysis(str, sym);
    cout << "This is function_call statement::: " << str.substr(pos_line_header, last-pos_line_header) << endl << endl;
}
void factor(string& str){ //＜因子＞    ::= ＜标识符＞｜＜标识符＞‘[’＜表达式＞‘]’｜＜整数＞|＜字符＞｜＜有返回值函数调用语句＞|‘(’＜表达式＞‘)’
    int pos_line_header = last;
    //string res = sym;
    if (id == 98 || id == 99){ //代表是字符或者(无符号)整数
        id = lexicalAnalysis(str, sym);
    }
    else if (sym == "("){
        id = lexicalAnalysis(str, sym);
        expression(str);
        test({")"}, 13);
        id = lexicalAnalysis(str, sym);
        //return ;
    }else if (sym == "+" || sym =="-"){
        id = lexicalAnalysis(str, sym);
        if (id == 99){ //代表是(无符号)整数
            id = lexicalAnalysis(str, sym);
        }
    }else if (id < 1000){ //代表不是标识符或者函数名
        error(14);
        id = lexicalAnalysis(str, sym);
        //return ;
    }else{//现在是最后的情况即是一个标识符，之前已经读过标识符了
        id = lexicalAnalysis(str, sym);
        if (sym == "["){
            id = lexicalAnalysis(str, sym);
            expression(str);
            test({"]"},15);
            id = lexicalAnalysis(str, sym);
        }else if (sym == "("){ //代表是函数调用语句 ???如果遇到a = work(1); work是一个过程，怎么报错
            //!!!
            now = pos_line_header; //会退到读函数名的地方
            id = lexicalAnalysis(str, sym);
            funcCall(str);
            //return ;
        }
        //否则就是一个单纯的标识符，也就是变量名，不需要做任何事
    }
    cout << "This is a factor statemnt::: " << str.substr(pos_line_header, last-pos_line_header) << endl << endl;
}
void term(string& str){ //＜项＞     ::= ＜因子＞{＜乘法运算符＞＜因子＞}
    int pos_line_header = last;
    factor(str);
    while (sym == "*" || sym == "/"){ //对于后面可能出现的多个乘除法接着的因子项，递归调用
        id = lexicalAnalysis(str, sym);
        factor(str);
    }
    cout << "This is a term statemnt::: " << str.substr(pos_line_header, last-pos_line_header) << endl << endl;
}
void expression(string& str){ //＜表达式＞    ::= ［＋｜－］＜项＞{＜加法运算符＞＜项＞}
    int pos_line_header = last;
    if (sym == "+" || sym == "-"){ //遇到前导的正号或者负号，先提取出来
        id = lexicalAnalysis(str, sym);
    }
    term(str);
    while (sym == "+" || sym == "-"){ //遇到前导的正号或者负号，先提取出来
        id = lexicalAnalysis(str, sym);
        term(str);
    }
    cout << "This is a expression statemnt::: " << str.substr(pos_line_header, last-pos_line_header) << endl << endl;
}
void condStatement(string& str){ //＜条件＞    ::=  ＜表达式＞＜关系运算符＞＜表达式＞｜＜表达式＞ //表达式为0条件为假，否则为真
    int pos_line_header = last;
    expression(str);
    if (id >= 1 && id <= 6) { //代表是关系运算符
        id = lexicalAnalysis(str, sym);
        expression(str);
    }
    cout << "This is a condition ::: " << str.substr(pos_line_header, last-pos_line_header) << endl << endl;
}
void ifelStatement(string& str){ //＜条件语句＞  ::=  if ‘(’＜条件＞‘)’＜语句＞［else＜语句＞］
    int pos_line_header = last;
    string ifel_type = "if";
    id = lexicalAnalysis(str, sym);
    test({"("}, 12);
    id = lexicalAnalysis(str, sym); //读入下一个标识符给条件处理的语句使用
    condStatement(str);
    test({")"}, 17);
    id = lexicalAnalysis(str, sym);
    statement(str);
    if (sym == "else") {
        ifel_type += "_else";
        id = lexicalAnalysis(str, sym);
        statement(str);
    }
    cout << "This is a " << ifel_type << "statement::: " << str.substr(pos_line_header, last-pos_line_header) << endl << endl;
}
void assiStatement(string& str){ //＜赋值语句＞   ::=  ＜标识符＞＝＜表达式＞|＜标识符＞‘[’＜表达式＞‘]’=＜表达式＞
    int pos_line_header = last;
    id = lexicalAnalysis(str, sym); //
    if (sym == "["){
        id = lexicalAnalysis(str, sym);
        expression(str);
        test({"]"}, 50);
        id = lexicalAnalysis(str, sym);
    }
    test({"="}, 51);
    id = lexicalAnalysis(str, sym);
    expression(str);
    cout << "This is an assign statement::: " << str.substr(pos_line_header, last-pos_line_header) << endl << endl;
}
int step(string& str){//＜步长＞    ::=  ＜非零数字＞｛＜数字＞｝
    //cout << "STEP::: " << sym << endl << endl;
    if (id != 99){
        error(23);
        return 0;
    }
    if (sym[0]=='0' && sym.length()>1){ //代表出现了前导0
        error(24);
        return 0;
    }
    id = lexicalAnalysis(str, sym);
    return 1;
}
void loopStatement(string &str){ //＜循环语句＞::=for‘(’＜标识符＞＝＜表达式＞;＜条件＞;＜标识符＞＝＜标识符＞(+|-)＜步长＞‘)’＜语句＞
    int pos_line_header = last;
    id = lexicalAnalysis(str, sym); //之前已经读进来过for了
    test({"("},18);
    id = lexicalAnalysis(str, sym);
    if (id < 1000) //必须是标识符
        error(19);
    id = lexicalAnalysis(str, sym);
    test({"="},20);
    id = lexicalAnalysis(str, sym);
    expression(str);
    test({";"},21);
    id = lexicalAnalysis(str, sym);
    condStatement(str);
    test({";"},21);
    id = lexicalAnalysis(str, sym); //处理到=前面的标识符
    if (id < 1000) //必须是标识符
        error(19);
    id = lexicalAnalysis(str, sym); //处理到=
    test({"="},20);
    id = lexicalAnalysis(str, sym); //处理到=后面的标识符
    if (id < 1000) //必须是标识符
        error(19);
    id = lexicalAnalysis(str, sym); //处理到+ -
    test({"+", "-"},20);
    id = lexicalAnalysis(str, sym);
    step(str);
    test({")"}, 25);
    id = lexicalAnalysis(str, sym);
    statement(str);
    cout << "This is a loop statement::: " << str.substr(pos_line_header, last-pos_line_header) << endl << endl;
}
void scanStatement(string& str){ //＜读语句＞    ::=  scanf ‘(’＜标识符＞{,＜标识符＞}‘)’
    int pos_line_header = last;
    id = lexicalAnalysis(str, sym);
    test({"("}, 26);
    int scan_cnt = 0;
    do{
        //        if (scan_cnt != 0){ //除了第一次不需要读进来表达式外，之后的都需要跳过逗号
        //            id = lexicalAnalysis(str, sym); //得到变量名
        //        }
        id = lexicalAnalysis(str, sym); //得到变量名
        if (id < 1000)
            error(27);
        scan_cnt++;
        id = lexicalAnalysis(str, sym); //读入到逗号或者)
    }while (sym == ",");
    test({")"}, 28);
    id = lexicalAnalysis(str, sym);
    cout << "This is a scanf statement::: " << scan_cnt << " variables::: " << str.substr(pos_line_header, last-pos_line_header) << endl << endl;
}
void prinStatement(string& str){ //＜写语句＞    ::= printf ‘(’ ＜字符串＞,＜表达式＞ ‘)’| printf ‘(’＜字符串＞ ‘)’| printf ‘(’＜表达式＞‘)’
    int pos_line_header = last;
    id = lexicalAnalysis(str, sym);
    test({"("}, 29);
    id = lexicalAnalysis(str, sym);
    if (id == 97) { //代表是字符串
        id = lexicalAnalysis(str, sym);
        if (sym == ","){ //＜字符串＞,＜表达式＞
            id = lexicalAnalysis(str, sym);
            expression(str);
        }
    }else{
        expression(str);
    }
    test({")"}, 30);
    id = lexicalAnalysis(str, sym);
    cout << "This is a printf statement::: " << str.substr(pos_line_header, last-pos_line_header) << endl << endl;
}
void retuStatement(string& str){ //＜返回语句＞   ::=  return[‘(’＜表达式＞‘)’]
    int pos_line_header = last;
    id = lexicalAnalysis(str, sym);
    if (sym == ";"){ //代表是没有任何(表达式)即return ;的直接返回
        cout << "This is a return statement::: " << str.substr(pos_line_header, last-pos_line_header) << endl << endl;
        return ;
    }
    test({"("}, 31);
    id = lexicalAnalysis(str, sym);
    expression(str);
    test({")"}, 32);
    id = lexicalAnalysis(str, sym); //读到了最后的分号
    cout << "This is a return statement::: " << str.substr(pos_line_header, last-pos_line_header) << endl << endl;
}
void caseStatement(string& str){ //＜情况表＞   ::=  ＜情况子语句＞{＜情况子语句＞}
    //＜情况子语句＞  ::=  case＜常量＞：＜语句＞
    int case_cnt = 0, pos_line_header = last;
    test({"case"}, 37); //还没有检测，所以要先检测下
    //    id = lexicalAnalysis(str, sym);
    do {
        //        if (case_cnt != 0){ //除了第一次不需要读进来表达式外，之后的都需要跳过case
        //            id = lexicalAnalysis(str, sym); //得到变量名
        //        }
        id = lexicalAnalysis(str, sym); //得到常量
        if (sym == "+" || sym =="-"){ //因为case后面跟的是常量，常量是由(有符号)整数和字符构成的，所以要判断+ -号
            id = lexicalAnalysis(str, sym);
            if (id != 99) //代表是(无符号)整数
                error(59);
        }else { //代表处理到的是无符号整数或者字符
            if (id != 98 && id != 99) //代表既不是是字符常量也不是整数常量
                error(38);
        }
        id = lexicalAnalysis(str, sym); //读到:
        test({":"}, 40);
        id = lexicalAnalysis(str, sym);
        statement(str);
        case_cnt++;
    }while (sym == "case");
    cout << "This is cases statement with::: " << case_cnt << " cases::: " << str.substr(pos_line_header, last-pos_line_header) << endl << endl;
}
void defuStatement(string& str){ //＜缺省＞   ::=  default : ＜语句＞
    int pos_line_header = last;
    test({"default"}, 41); //还没有检测，所以要先检测下
    id = lexicalAnalysis(str, sym);
    test({":"}, 42);
    id = lexicalAnalysis(str, sym);
    statement(str);
    cout << "This is a default statement::: " << str.substr(pos_line_header, last-pos_line_header) << endl << endl;
}
void switStatement(string& str){ //＜情况语句＞  ::=  switch ‘(’＜表达式＞‘)’ ‘{’＜情况表＞＜缺省＞‘}’
    int pos_line_header = last;
    id = lexicalAnalysis(str, sym);
    test({"("}, 33);
    id = lexicalAnalysis(str, sym);
    expression(str);
    test({")"}, 34);
    id = lexicalAnalysis(str, sym);
    test({"{"}, 35);
    id = lexicalAnalysis(str, sym);
    caseStatement(str);
    defuStatement(str);
    test({"}"}, 36);
    id = lexicalAnalysis(str, sym);
    cout << "This is a switch statement::: " << str.substr(pos_line_header, last-pos_line_header) << endl << endl;
}
void statement(string& str){ //＜语句＞    ::= ＜条件语句＞｜＜循环语句＞| ‘{’＜语句列＞‘}’｜＜有返回值函数调用语句＞;| ＜无返回值函数调用语句＞;｜＜赋值语句＞;｜＜读语句＞;｜＜写语句＞;｜＜空＞;|＜情况语句＞｜＜返回语句＞;
    int pos_line_header = last;
    switch (mp[sym]) { //不能用字符串的case,因此不得不map一下得到一个对应的索引值
        case 103: ifelStatement(str);  break; //"if"
        case 105: loopStatement(str);  break; //"for"
        case 111: scanStatement(str); test({";"}, 100); id = lexicalAnalysis(str, sym); break; //"scanf"
        case 112: prinStatement(str); test({";"}, 100); id = lexicalAnalysis(str, sym); break; //"printf"
        case 109: retuStatement(str); test({";"}, 100); id = lexicalAnalysis(str, sym); break; //"return"
        case 19:  id = lexicalAnalysis(str, sym); break; //";" 即对应的是<空>;
        case 106: switStatement(str); break; //情况语句，也就是switch-case语句
        case 17: statExecution(str, false); break;//"{" 语句列即不是复合语句(即不能有const、变量定义)
        default: { //剩下的就是一个标识符了，可能是赋值语句或者函数调用语句
            if (id < 1000){
                test({},88);
                break;
            }
            id = lexicalAnalysis(str, sym);
            if (sym == "=" || sym == "["){ //赋值语句 //＜赋值语句＞   ::=  ＜标识符＞＝＜表达式＞|＜标识符＞‘[’＜表达式＞‘]’=＜表达式＞
                now = pos_line_header;
                id = lexicalAnalysis(str, sym);
                assiStatement(str);
            }else{ //函数调用 !!!如何保证调用的函数是已经定义过的
                now = pos_line_header;
                id = lexicalAnalysis(str, sym);
                funcCall(str);
            }
            test({";"}, 100);
            id = lexicalAnalysis(str, sym);
        }
    }
}
void statExecution(string& str, bool is_multi_statement){ //处理复合语句 //＜复合语句＞   ::=  ［＜常量说明＞］［＜变量说明＞］＜语句列＞
    test({"{"}, 56);
    id = lexicalAnalysis(str, sym);
    if (is_multi_statement){
        consDeclarion(str);
        variDeclation(str);
    }
    while (sym != "}"){ //＜语句列＞ ::=｛＜语句＞｝ //调用语句列的地方只有一个就是语句里有 ‘{’＜语句列＞‘}’
        statement(str);
    }
    id = lexicalAnalysis(str, sym);
}
int procMainExec(string& str){
    int tot = 0;
    if (id < 1000) //如果不是标识符，就报错
        error(4);
    id = lexicalAnalysis(str, sym); //读到(
    if (sym != "(")
        error(9);
    id = lexicalAnalysis(str, sym);
    while (sym == "int" || sym == "char"){
        tot += (tot==0);//如果是第一次0就加一，因为后面的tot++统计的是逗号的个数
        id = lexicalAnalysis(str, sym); //得到变量名
        if (id < 1000) //如果不是标识符，就报错
            error(4);
        id = lexicalAnalysis(str, sym);
        if (sym==","){
            tot++;
            id = lexicalAnalysis(str, sym);
            continue;
        }
        break;//这么写而不是遇到）break是为了防止出现一些错误符号例如;
    }
    test({")"}, 10);
    id = lexicalAnalysis(str, sym);
    return tot;
}
void voidDeclartion(string& str, string type){ //有返回值函数定义和无返回值函数定义 //处理void的定义, 注意已经在procDeclartion里面
    int pos_line_header = last;
    id = lexicalAnalysis(str, sym); //得到过程名
    string void_name = sym;
    
    int variable_tot = procMainExec(str);
    
    cout << "This is a "+ type +" statement " << void_name << " ::: " << variable_tot << " parameters::: " << str.substr(pos_line_header, last-pos_line_header) << endl << endl;
    
    statExecution(str, true); //是复合语句，即可以有const和变量定义
    cout << "siz = " << siz << " now = " << now << endl << endl;
}
//void funcDeclartion(string& str, string type){ //处理void的定义, 注意已经在procDeclartion里面
//
//    id = lexicalAnalysis(str, sym); //得到过程名
//    string func_name = sym;
//
//    int variable_tot = procMainExec(str);
//
//    cout << "This is a "+ type +" statement::: " << variable_tot << " variables!" << endl << endl;
//    id = lexicalAnalysis(str, sym);
//    statExec(str);
//}
void procDeclartion(string& str){ //处理所有的函数和过程的定义，直至做到void main为止
    int pos_line_header;
    while (sym == "int" || sym == "char" || sym == "void"){
        pos_line_header = last;
        if (sym == "void"){
            id = lexicalAnalysis(str, sym); //得到过程名
            if (sym == "main"){
                id = lexicalAnalysis(str, sym); //读到了void main的{
                return ;
            }
            now = pos_line_header;
            id = lexicalAnalysis(str, sym);
            voidDeclartion(str, "void");
        }else{
            voidDeclartion(str, "function_" + sym);
        }
    }
}
int main() {
    //freopen("out.txt","w",stdout);
    std::ifstream t("14011100_test.txt");
    std::string s((std::istreambuf_iterator<char>(t)),
                    std::istreambuf_iterator<char>());
    
    str = s;
    
    siz = str.size();
    //cout << str << endl << endl;
    //lexical(str); //语义分析
    now = 0;
    id = lexicalAnalysis(str, sym); //getNext();
    
    consDeclarion(str);
    variDeclation(str);
    procDeclartion(str);
    
    test({"("}, 45);
    id = lexicalAnalysis(str, sym);
    test({")"}, 46);
    id = lexicalAnalysis(str, sym);
    
    statExecution(str, true); //是复合语句，即可以有const和变量定义
    if (now < siz)
        error(101);
    cout << "siz = " << siz << " now = " << now << endl << endl;
    return 0;
}
