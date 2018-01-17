//
//  main.cpp
//  LexicalAnalysis
//
//  Created by ohazyi on 2017/11/22.
//  Copyright © 2017 ohazyi. All rights reserved.
//  C++11!!!
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

int cnt = 999, now = 0, siz;
string s;
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

};
map<string,string> mp_out = {
    {"<", "blt"},{"<=", "blt"},{">", "bgt"},{">=", "bge"},{"!=", "bne"},{"==", "beq"},

    {"+", "add"},{"-", "sub"},{"*", "mul"},{"/", "div"},

    {"(", "LeftParentheis"},{")", "RightParenthesis"},{"[", "LeftBracket"},{"]","RightBracket"},{"{", "LeftBrace"},{"}", "RightBrace"},

    {";", "Semicolon"},{",", "Comma"},{":", "colon"},

    {"=", "assign"}
};

int lexicalAnalysis(string& str){ //词法分析
    s = "";

    while (isspace(str[now]))
        now++;
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
int main() {
    //freopen("out.txt","w",stdout);

    std::ifstream t("14011100_test.txt");
    std::string str((std::istreambuf_iterator<char>(t)),
                    std::istreambuf_iterator<char>());
    //cout << str << endl;
    siz = str.size();
    int lex_line = 0;
    while (now < siz){
        int value = lexicalAnalysis(str);
        if (value == -2){} //代表是最后终止了，直接跳过
        else
        if (value == -1){ //代表没有找到，报wrong
            printf("%d !!!WRONG!!! %s\n", ++lex_line, s.data());
        }
        else if (value >= 100 && value < 1000){//保留字
            printf("%d KeyWord %s\n", ++lex_line, s.data());
        }
        else if (value >= 1000){ //标识符
            printf("%d Identifier %s\n", ++lex_line, s.data());
        }else if (value >= 1 && value <= 50){ //代表是一些运算符等 !!!!!50代修改
            printf("%d %s %s\n", ++lex_line, mp_out[s].data(), s.data());
        }
        else if (value == 99){ //代表是整数常量
            printf("%d ConstantInt %s\n", ++lex_line, s.data());
        }
        else if (value == 98){ //代表是字符常量
            printf("%d ConstantChar %s\n", ++lex_line, s.data());
        }
        else if (value == 97){ //代表是字符串常量
            printf("%d ConstantString %s\n", ++lex_line, s.data());
        }else { //其它的返回值
            printf("%d !!!NotFound!!! %s\n", ++lex_line, s.data());
        }
    }
    return 0;
}
