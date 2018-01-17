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
#include <ostream>
#include <sstream>
#define rep(i,a,b) for (int i = (a); i <= (b); ++i)
#define MAX_QUAT 10000
#define MAX_TAB 10000
using namespace std;
void expression(string& str, string& res), statement(string& str), funcCall(string& str), statExecution(string& str, bool is_multi_statement, bool is_main), printQuat();

int id, cnt = 999, now = 0, last;
size_t siz;
int addr = 0, cnt_quat, cnt_tab = 0, cnt_proc = 0, index_proc[1010], cnt_tmp = 0, cnt_label, para_i, para_now_cnt; //index_proc为分程序索引表，里面存的是每个程序的第一个定义的变量在符号表的位置(就是过程/函数自己，因为自己也会存在这个里面的)
string route, sym, str;
//string sym; //获取到的接下来的一个字符串
vector<char> oper_rela = {'<', '>', '='};
vector<char> oper = {'+', '-', '*', '/'};
vector<char> oper_bra = {'(', ')', '[', ']', '{', '}'};
vector<char> oper_punc = {';', ',', ':'};

struct Tab{ //符号表
    string name, kind, type; // kind: 标识符种类：0常量\1变量\2函数\3参数 type: 0-int\1-char\2-void\3-int[]\4-char[]
    int value; //存储常量值
    int addr; //地址，其中所有的都是0，其它的在此基础上逐渐增加，比如work里的i,j,k分别为1，2，3(包括参数，const，局部变量全体一起依次增加)
    int para_num; //参数个数，对数组而言为元素个数
    int program_id; //位于第几个程序块，当然也可以通过index来求出来
}tab[MAX_TAB]; //符号表信息保存在0..cnt_tab-1里

struct Quat{ //四元式
    string type, op1, op2, op3;
    int program_id;//label=-1,
    vector<int> label;
}quat[MAX_QUAT+10];

//struct Temp{
//    int addr, program_id;
//};
vector<int> case_v;
map<string, int> mp_s;
int cnt_mp_s = 0, sp = 0, main_pos;
bool is_global = true; //是否是全局变量，在全局变量定义完之后置为false
bool reg[41]; //寄存器使用状态，true代表正在被使用
map<string, pair<int,int>> mp_tmp; //存放四元式产生的临时变量对应的在第几个程序块和在程序块的addr
map<int, int> mp_quat_para_num, mp_quat_para_num_with_local, mp_quat_cnt_temp;
struct Func{ //存放函数的结构体，其中type有三种类型，int, char, "" 其中第三个代表是过程
    int tab_id; //在符号表的登录位置
    string type;
    int para_num;
};
map<string, Func> mp_func; //函数名为键，值为对应的函数/过程信息
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
    
    {98, "missing a ;"},
    {99, "extra content after program ends."},
    
    {100, "Duplicate name of a proc"},
    {101, "local variable same name with another local variable"},
    {103, "if must have a condition operation"},
};
int mystoi(string s){
    int res = 0, d = 1, sign_s = 1, s_end = 0;
    if (s[0]=='+'||s[0]=='-'){
        s_end = 1;
        if (s[0] == '-') sign_s = -1;
    }
    for (int i = (int)s.size() -1; i >= s_end; --i){
        res += d * (s[i]-'0');
        d*=10;
    }
    return res*sign_s;
}
string int2string(int x){//    return to_string(x); //c++11
    stringstream istr;
    istr << x;
    return istr.str();//    char c[21];//    int length = sprintf(c, "%d", x); //    cout<<c<<endl;//    return c;
}
bool isChar(const std::string& s){
    if (s.size()!=3)
        return false;
    if (s[0]!='\''||s[2]!='\'')
        return false;
    return true;//检测合法!!!
}
bool isNumber(const std::string& s)
{
    if (s=="-" || s=="+") return false;
    std::string::const_iterator it = s.begin();
    if (*it=='-'||*it=='+') it++;
    while (it != s.end() && std::isdigit(*it)) ++it;
    return !s.empty() && it == s.end();
}
void newTmp(string& res){
    res = "#" + int2string(++cnt_tmp);
}
void newLabel(int& res){
    res = (++cnt_label);
}
int lexicalAnalysis(string& str, string& s){ //词法分析
    if (now>=siz) //{ printQuat(); exit(0); }//代表已经处理完成了
        return 0;
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
//void enter(string type){
    /*
     if (mp.count(sym) > 0) {//之前已经存储过 // determine if duplicate
     //! 错误处理
     cout << sym << "Has defind" << endl << endl;
     }
     mp[sym] = ++cnt;
     */
    //tab[] = type;
//}
void error(int errorId){
    cout << "!!!ERROR!!!" << endl;
    cout << error_msg[errorId] << endl << endl;
    //    switch (errorId) {
    //        case 1: cout << error_msg[errorId] << endl << endl; break;
    //        case 2: cout << error_msg[errorId] << endl << endl; break;
    //    }
}

void enter(string name, string kind, string type, int value, int addr, int para_num){
    if (kind == "void" || kind == "function") { //代表是过程或者函数
        rep (i, 1, cnt_proc){
            if (tab[index_proc[i]].name == name){ //重复定义函数
                error(100);
                return ;
            }
        }
        index_proc[++cnt_proc] = cnt_tab+1; //分程序的索引表指向当前的这个符号表的位置(我们马上就会存这个信息了)，注意这个是从0开始存的
    }else{ //不是过程或者函数，说明是局部变量或者全局变量
        //如果是全局变量，其index_proc[0] = 0
        rep (i, index_proc[cnt_proc], cnt_tab){ //检查之前定义的参数和局部变量有没有名字相同的
            if (tab[i].name == name){
                error(101);
                return ;
            }
        }
    }
    
    cnt_tab++;
    tab[cnt_tab].name = name;
    tab[cnt_tab].kind = kind;
    tab[cnt_tab].type = type;
    tab[cnt_tab].value = value;
    tab[cnt_tab].addr = addr;
    tab[cnt_tab].para_num = para_num;
    tab[cnt_tab].program_id = cnt_proc;
    
    cout << "cnt_tab " << cnt_tab << " ::: " << name << " " << kind << " " << type << " " << value << " " << addr << " " << para_num << " &&& " << cnt_proc << endl;
    
}

void addQuat(string type, string op1, string op2, string op3){
    cout << "QUAT!!!" << " " << type << " " << op1 << " " << op2 << " " << op3 << " ::: " << cnt_proc << endl;
    ++cnt_quat;
    quat[cnt_quat].type = type;
    quat[cnt_quat].op1 = op1;
    quat[cnt_quat].op2 = op2;
    quat[cnt_quat].op3 = op3;
    quat[cnt_quat].program_id = cnt_proc;
}

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
            //enter("constValue"); //代表这个标识符的类型是常量
            string const_name = sym;
            id = lexicalAnalysis(str, sym); //sym = getNext(); //读入到=
            test({"="}, 2);
            id = lexicalAnalysis(str, sym);
            int const_value = 0;
            if (cons_type == "const_int"){ //对于const int ... 跟上的是一个(有符号)整数，因此要判断有无 + -
                const_value = mystoi(sym);
                if (sym == "+" || sym == "-"){
                    id = lexicalAnalysis(str, sym);
                    const_value = mystoi(sym);
                    if (sym == "-")
                        const_value *= -1;
                }
                if (id != 99) //代表不是(无符号)整数
                    error(56);
            }else{ //代表是const char ...
                const_value = (int)sym[1]; //'8' //!!!此时sym的长度必为3，我们把ascii值存入符号表
                if (id != 98)//代表不是是字符常量
                    error(57);
            }
            enter(const_name, "const", cons_type.substr(6, cons_type.size()-6), const_value, ++addr, 0);
            addQuat(cons_type, const_name, int2string(const_value), "");
            id = lexicalAnalysis(str, sym); //跟上的是一个分号或者逗号
            const_cnt++;
        }while (sym == ",");
        
        test({";"}, 55);
        cout << "This is a " + cons_type + " statement::: " << const_cnt << "elements::: " << str.substr(pos_line_header, last-pos_line_header) << endl << endl;
        id = lexicalAnalysis(str, sym); //getNext();
    }
}
void variDeclation(string& str){
    string vari_name, op2 = "";
    int variable_cnt, pos_line_header = last; //pos_line_header为这一行的行首在哪，给后来做回退用
    while (sym == "int" || sym == "char"){
        variable_cnt = 0;
        test({"int","char"}, 1); //必须是int或者char类型
        string type, type_sentence = "variable_" + sym; //"variable_int" 代表是个int类型的常量，“constchar" ...
        do{
            type = type_sentence;
            id = lexicalAnalysis(str, sym); //得到变量名
            vari_name = sym;
            if (id < 1000) //如果不是标识符，就报错
                error(4);
            id = lexicalAnalysis(str, sym);
            if (sym == "["){ //考虑数组的定义，即int a[100] 注意我们的文法与C语言不同没有函数的声明只有函数的定义
                id = lexicalAnalysis(str, sym);
                if (id != 99)
                    error(5);
                else if (sym=="0") //等效于stoi(sym)==0,把字符串转换为整数
                    error(6);
                int p_num = mystoi(sym);
                op2 = int2string(p_num);
                id = lexicalAnalysis(str, sym);
                test({"]"}, 7);
                type+="[]";
                enter(vari_name, "variable", type.substr(9, (int)type.size() - 9), 0, ++addr, p_num);
                id = lexicalAnalysis(str, sym);
                
            }else if (sym == "("){ //带(即参数的int定义显然是函数或者过程，回退到这一行行首
                if (variable_cnt > 0) //代表之前已经有变量被定义了，报错 int i, work();
                    error(7);
                now = pos_line_header;
                id = lexicalAnalysis(str, sym);
                return ;//直接退出了，不用再继续判断变量定义了
            }else{ //否则就是单个变量了
                enter(vari_name, "variable", type.substr(9, (int)type.size() - 9), 0, ++addr, 0);
            }
            addQuat(type,vari_name,op2,"");
            variable_cnt++;
        }while (sym == ",");
        test({";"}, 8);
        cout << "This is a "+ type +" statement::: " << variable_cnt << " variables::: " << str.substr(pos_line_header, last-pos_line_header) << endl << endl;
        pos_line_header = now;
        id = lexicalAnalysis(str, sym);
    }
}
void parameter(string& str){ //＜值参数表＞::= ＜表达式＞{,＜表达式＞}｜＜空＞
    string op1, op2, op3, res;
    int para_cnt = 0, pos_line_header = last, para_id = 0;
    if (sym==")") {//应对值参数表为空的情况，因为表达式不会以)开始 也许有些多余，因为怕现在处理出错
        //id = lexicalAnalysis(str, sym);
        return ;
    }
    else{
        do{
            if (para_cnt != 0){ //因为第一次在函数调用前已经读进来了标识符，因此不需要读入标识符了，之后的都需要跳过逗号
                id = lexicalAnalysis(str, sym); //得到变量名
            }
            expression(str, res);
            addQuat("PUSH", res, int2string(para_cnt+1), "");
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
    addQuat("call", func_name, "", ""); //call mymax
}
void factor(string& str, string& res){ //＜因子＞    ::= ＜标识符＞｜＜标识符＞‘[’＜表达式＞‘]’｜＜整数＞|＜字符＞｜＜有返回值函数调用语句＞|‘(’＜表达式＞‘)’
    string op1, op2, op3, factor_name, factor_sign;
    int pos_line_header = last;
    //string res = sym;
    if (id == 98 || id == 99){ //代表是字符或者(无符号)整数
        res = sym;
        id = lexicalAnalysis(str, sym);
    }
    else if (sym == "("){
        id = lexicalAnalysis(str, sym);
        expression(str, res);
        test({")"}, 13);
        id = lexicalAnalysis(str, sym);
        //return ;
    }else if (sym == "+" || sym =="-"){
        factor_sign = sym;
        id = lexicalAnalysis(str, sym);
        if (id == 99){ //代表是(无符号)整数
            res = factor_sign + sym;
            id = lexicalAnalysis(str, sym);
        }else{
            cout << "!!!FactorErr!!!" << endl;
        }
        
    }else if (id < 1000){ //代表不是标识符或者函数名
        error(14);
        id = lexicalAnalysis(str, sym);
        //return ;
    }else{//现在是最后的情况即是一个标识符，之前已经读过标识符了
        res = op2 = factor_name = sym; //三连等赋值
        id = lexicalAnalysis(str, sym);
        
        if (sym == "["){
            id = lexicalAnalysis(str, sym);
            expression(str, op3);
            test({"]"},15);
            id = lexicalAnalysis(str, sym);
            newTmp(op1);
            //cout << "@@@@@@@" << endl;
            addQuat("=", op1, op2+"["+op3+"]", "");//!!!addQuat("load", op1, op2, op3); // t1 = op2[op3]
            res = op1;
        }else if (sym == "("){ //代表是函数调用语句 ???如果遇到a = work(1); work是一个过程，怎么报错
            //!!!
            now = pos_line_header; //会退到读函数名的地方
            id = lexicalAnalysis(str, sym);
            funcCall(str);
//            newTmp(op1);
//            addQuat("assign", op1, factor_name, ""); //t1 =
            if (mp_func.count(factor_name) == 0)
                cout << "!!!ERR: " << factor_name << " Not found" << endl;
            else if (mp_func[factor_name].type == "")
                cout << "!!!ERR You can't assign a void " << factor_name << " value" << endl;
            else
                res = "RET_"+mp_func[factor_name].type; //!!!
            //return ;
        }
        //否则就是一个单纯的标识符，也就是变量名，不需要做任何事
        
    }
    cout << "This is a factor statemnt::: " << str.substr(pos_line_header, last-pos_line_header) << endl << endl;
}
void term(string& str, string& res){ //＜项＞     ::= ＜因子＞{＜乘法运算符＞＜因子＞}
    string op1, op2, op3, term_name, sign_term;
    int pos_line_header = last;
    factor(str, res);
    op1 = res;
    while (sym == "*" || sym == "/"){ //对于后面可能出现的多个乘除法接着的因子项，递归调用
        sign_term = sym;
        op2 = op1; //上个的结果是现在的第一个操作数
        id = lexicalAnalysis(str, sym);
        term_name = sym;
        
        factor(str, res);
        op3 = res;
        newTmp(op1);
        addQuat(sign_term, op1, op2, op3);
    }
    cout << "This is a term statemnt::: " << str.substr(pos_line_header, last-pos_line_header) << endl << endl;
    res = op1;
}
void expression(string& str, string& res){ //＜表达式＞    ::= ［＋｜－］＜项＞{＜加法运算符＞＜项＞}
    string op1, op2, op3, expression_sign;
    int pos_line_header = last;
    if (sym == "+" || sym == "-"){ //遇到前导的正号或者负号，先提取出来
        id = lexicalAnalysis(str, sym);
        expression_sign = sym;
    }
    term(str, res);
    op1 = res;
    if (expression_sign == "-"){
        newTmp(op1);
        op2 = res;
        addQuat("*", op1, "-1", op2); //-1!!! //t1 = -1*op2
    }
    while (sym == "+" || sym == "-"){ //遇到前导的正号或者负号，先提取出来
        expression_sign = sym;
        op2 = op1;
        
        id = lexicalAnalysis(str, sym);
        term(str, res);
        op3 = res;
        newTmp(op1);
        addQuat(expression_sign, op1, op2, op3);
    }
    cout << "This is a expression statemnt::: " << str.substr(pos_line_header, last-pos_line_header) << endl << endl;
    res = op1;
}
void condStatement(string& str, bool is_loop, Quat& q){ //＜条件＞    ::=  ＜表达式＞＜关系运算符＞＜表达式＞｜＜表达式＞ //表达式为0条件为假，否则为真
    string op1, op2, op3, res, cond;
    int pos_line_header = last;
    expression(str, res);
    cond = sym;
    op1 = res;
    if (id >= 1 && id <= 6) { //代表是关系运算符
        id = lexicalAnalysis(str, sym);
        expression(str, res);
        op2 = res;
    }else
        error(103);
    if (!is_loop)
        addQuat(cond, op1, op2, ""); //"==" ">="
    else{
        q.type = cond;
        q.op1 = op1;
        q.op2 = op2;
        q.op3 = "";
        q.program_id = cnt_proc;
    }
    cout << "This is a condition ::: " << str.substr(pos_line_header, last-pos_line_header) << endl << endl;
}
void ifelStatement(string& str){ //＜条件语句＞  ::=  if ‘(’＜条件＞‘)’＜语句＞［else＜语句＞］
    int pos_line_header = last, label1, label2, ifel_pos;
    Quat ifel_quat; //没作用，只是为了配合for的演出
    string ifel_type = "if";
    newLabel(label1);
    newLabel(label2);
    id = lexicalAnalysis(str, sym);
    test({"("}, 12);
    id = lexicalAnalysis(str, sym); //读入下一个标识符给条件处理的语句使用
    condStatement(str, false, ifel_quat); //ifel_quat没作用，只是为了配合for的演出
    test({")"}, 17);
    addQuat("BZ", "LABEL_"+int2string(label1), "", ""); //紧跟在 == 后面
    id = lexicalAnalysis(str, sym);
    statement(str);
    if (sym == "else") {
        addQuat("GOTO", "LABEL_"+int2string(label2), "", "");//if判断条件之后直接跳到else后面一句
        ifel_type += "_else";
        id = lexicalAnalysis(str, sym);
        ifel_pos = cnt_quat;
        statement(str);
        quat[ifel_pos+1].label.push_back(label1); //label1; //在else语句的第一句话打上label1标记
        quat[cnt_quat+1].label.push_back(label2);//= label2; //在else语句的后一句话打上label2标记 !!!如果最后一个打上了label语句，但是没有内容，我们必须得插入一条空指令
    }else{ //代表没有else语句
        quat[cnt_quat+1].label.push_back(label1); //= label1;
    }
    
    cout << "This is a " << ifel_type << "statement::: " << str.substr(pos_line_header, last-pos_line_header) << endl << endl;
}
void assiStatement(string& str){ //＜赋值语句＞   ::=  ＜标识符＞＝＜表达式＞|＜标识符＞‘[’＜表达式＞‘]’=＜表达式＞
    string op1, op2, op3, res;
    op1 = sym;
    int pos_line_header = last;
    id = lexicalAnalysis(str, sym); //一定是等于号，因为之前推过了
    
    if (sym == "["){
        id = lexicalAnalysis(str, sym);
        expression(str, res);
        op1+="["+res+"]";
        test({"]"}, 50);
        id = lexicalAnalysis(str, sym);
    }
    test({"="}, 51);
    id = lexicalAnalysis(str, sym);
    expression(str, res);
    addQuat("=", op1, res, "");
    cout << "This is an assign statement::: " << str.substr(pos_line_header, last-pos_line_header) << endl << endl;
}
int step(string& str, string& num){//＜步长＞    ::=  ＜非零数字＞｛＜数字＞｝
    //cout << "STEP::: " << sym << endl << endl;
    if (id != 99){
        error(23);
        return 0;
    }
    if (sym[0]=='0' && sym.length()>1){ //代表出现了前导0
        error(24);
        return 0;
    }
    num = sym;
    id = lexicalAnalysis(str, sym);
    return 1;
}
void loopStatement(string &str){ //＜循环语句＞::=for‘(’＜标识符＞＝＜表达式＞;＜条件＞;＜标识符＞＝＜标识符＞(+|-)＜步长＞‘)’＜语句＞
    string op1, op2, op3, res, loop_variment, step_num;
    Quat loop_cond_quat, loop_step;
    int pos_line_header = last, loop_pos, label_first, label_second;
    id = lexicalAnalysis(str, sym); //之前已经读进来过for了
    test({"("},18);
    id = lexicalAnalysis(str, sym);
    if (id < 1000) //必须是标识符
        error(19);
    loop_variment = sym;
    id = lexicalAnalysis(str, sym);
    test({"="},20);
    id = lexicalAnalysis(str, sym);
    expression(str, res);
    addQuat("=", loop_variment, res, ""); //i = 1
    
    loop_pos = cnt_quat;
    newLabel(label_first);
    quat[loop_pos+1].label.push_back(label_first);//= label_first; //在i=1的下一句加上标记
    
    test({";"},21);
    id = lexicalAnalysis(str, sym);
    condStatement(str, true, loop_cond_quat);
    
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
    loop_step.type = sym; //+ -
    id = lexicalAnalysis(str, sym);
    step(str, step_num);
    test({")"}, 25);
    loop_step.op1 = loop_variment;
    loop_step.op2 = loop_variment;
    loop_step.op3 = step_num;
    loop_step.program_id = cnt_proc;
    //loop_step.label = quat[cnt_quat+1].label; //!!! 重要 //!!!时间错误
    id = lexicalAnalysis(str, sym);
    statement(str);
    loop_step.label = quat[cnt_quat+1].label;
    quat[++cnt_quat] = loop_step;
    loop_cond_quat.label = quat[cnt_quat+1].label;
    quat[++cnt_quat] = loop_cond_quat;
    newLabel(label_second);
    addQuat("BZ", "LABEL_"+int2string(label_second),"","");
    addQuat("GOTO", "LABEL_"+int2string(label_first),"","");
    quat[cnt_quat+1].label.push_back(label_second);//= label_second;
    cout << "This is a loop statement::: " << str.substr(pos_line_header, last-pos_line_header) << endl << endl;
}
void scanStatement(string& str){ //＜读语句＞    ::=  scanf ‘(’＜标识符＞{,＜标识符＞}‘)’
    int pos_line_header = last;
    id = lexicalAnalysis(str, sym);
    test({"("}, 26);
    int scan_cnt = 0;
    do{
        id = lexicalAnalysis(str, sym); //得到变量名
        if (id < 1000)
            error(27);
        scan_cnt++;
        addQuat("READ", sym, "", "");
        id = lexicalAnalysis(str, sym); //读入到逗号或者)
    }while (sym == ",");
    test({")"}, 28);
    id = lexicalAnalysis(str, sym);
    cout << "This is a scanf statement::: " << scan_cnt << " variables::: " << str.substr(pos_line_header, last-pos_line_header) << endl << endl;
}
void prinStatement(string& str){ //＜写语句＞    ::= printf ‘(’ ＜字符串＞,＜表达式＞ ‘)’| printf ‘(’＜字符串＞ ‘)’| printf ‘(’＜表达式＞‘)’
    string op1, op2, op3, res;
    int pos_line_header = last;
    id = lexicalAnalysis(str, sym);
    test({"("}, 29);
    id = lexicalAnalysis(str, sym);
    if (id == 97) { //代表是字符串
        addQuat("PRINT", sym, "", "");
        id = lexicalAnalysis(str, sym);
        if (sym == ","){ //＜字符串＞,＜表达式＞
            id = lexicalAnalysis(str, sym);
            expression(str, res);
            addQuat("PRINT", res, "", "");
        }
    }else{
        expression(str, res);
        addQuat("PRINT", res, "", "");
    }
    test({")"}, 30);
    id = lexicalAnalysis(str, sym);
    cout << "This is a printf statement::: " << str.substr(pos_line_header, last-pos_line_header) << endl << endl;
}
void retuStatement(string& str){ //＜返回语句＞   ::=  return[‘(’＜表达式＞‘)’]
    string op1, op2, op3, res;
    int pos_line_header = last;
    id = lexicalAnalysis(str, sym);
    if (sym == ";"){ //代表是没有任何(表达式)即return ;的直接返回
        addQuat("ret", "", "", "");
        cout << "This is a return statement::: " << str.substr(pos_line_header, last-pos_line_header) << endl << endl;
        return ;
    }
    test({"("}, 31);
    id = lexicalAnalysis(str, sym);
    expression(str, res);
    test({")"}, 32);
    id = lexicalAnalysis(str, sym); //读到了最后的分号
    cout << "This is a return statement::: " << str.substr(pos_line_header, last-pos_line_header) << endl << endl;
    addQuat("ret", res, "", "");
}
void caseStatement(string& str, string swit_variment){ //＜情况表＞   ::=  ＜情况子语句＞{＜情况子语句＞}
    //＜情况子语句＞  ::=  case＜常量＞：＜语句＞
    case_v.clear();
    string op1, op2, op3, res, case_constant;
    int case_cnt = 0, pos_line_header = last, label_new = 0, label_now, case_pos;
    test({"case"}, 37); //还没有检测，所以要先检测下
    //    id = lexicalAnalysis(str, sym);
    do {
        id = lexicalAnalysis(str, sym); //得到常量
        if (sym == "+" || sym =="-"){ //因为case后面跟的是常量，常量是由(有符号)整数和字符构成的，所以要判断+ -号
            if (sym == "-")
                case_constant = "-";
            id = lexicalAnalysis(str, sym);
            case_constant += sym;
            if (id != 99) //代表是(无符号)整数
                error(59);
        }else { //代表处理到的是无符号整数或者字符
            if (id != 98 && id != 99) //代表既不是是字符常量也不是整数常量
                error(38);
            case_constant = sym;
        }
        if (case_cnt == 0) //第一次的case要新建一个当前分支的标号
            newLabel(label_now);
        else
            label_now = label_new; //其它的当前分支都是上一次的新的
        newLabel(label_new); //表示下一个分支的标号
        addQuat("==", swit_variment, case_constant, "");
        if (case_cnt!=0) //除了第一个case分支，在==的第一句话打上label标记
            quat[cnt_quat].label.push_back(label_now);// = label_now;
        addQuat("BZ", "LABEL_"+int2string(label_new), "", "");
        id = lexicalAnalysis(str, sym); //读到:
        test({":"}, 40);
        id = lexicalAnalysis(str, sym);
//        case_pos = cnt_quat;
        statement(str);
        addQuat("GOTO", "", "", ""); //op1是最后的default开始的位置，我们等之后再填
        case_v.push_back(cnt_quat);
        
        case_cnt++;
    }while (sym == "case");
//    for (auto i : case_v){
//        quat[i].op1 = "LABEL_" + int2string(label_new);
//    }
    quat[cnt_quat+1].label.push_back(label_new); //= label_new; //default:
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
    string op1, op2, op3, res, swit_variment;
    int pos_line_header = last, label_after_default;
    id = lexicalAnalysis(str, sym);
    test({"("}, 33);
    id = lexicalAnalysis(str, sym);
    expression(str, res);
    addQuat("SWITCH", res, "", "");
    swit_variment = res;
    test({")"}, 34);
    id = lexicalAnalysis(str, sym);
    test({"{"}, 35);
    id = lexicalAnalysis(str, sym);
    caseStatement(str, swit_variment);
    defuStatement(str);
    
    newLabel(label_after_default);
    quat[cnt_quat+1].label.push_back(label_after_default);
    for (auto i : case_v){
        quat[i].op1 = "LABEL_" + int2string(label_after_default);
    }
    
    test({"}"}, 36);
    id = lexicalAnalysis(str, sym);
    cout << "This is a switch statement::: " << str.substr(pos_line_header, last-pos_line_header) << endl << endl;
}
void statement(string& str){ //＜语句＞    ::= ＜条件语句＞｜＜循环语句＞| ‘{’＜语句列＞‘}’｜＜有返回值函数调用语句＞;| ＜无返回值函数调用语句＞;｜＜赋值语句＞;｜＜读语句＞;｜＜写语句＞;｜＜空＞;|＜情况语句＞｜＜返回语句＞;
    int pos_line_header = last;
    switch (mp[sym]) { //不能用字符串的case,因此不得不map一下得到一个对应的索引值
        case 103: ifelStatement(str);  break; //"if"
        case 105: loopStatement(str);  break; //"for"
        case 111: scanStatement(str); test({";"}, 98); id = lexicalAnalysis(str, sym); break; //"scanf"
        case 112: prinStatement(str); test({";"}, 98); id = lexicalAnalysis(str, sym); break; //"printf"
        case 109: retuStatement(str); test({";"}, 98); id = lexicalAnalysis(str, sym); break; //"return"
        case 19:  id = lexicalAnalysis(str, sym); break; //";" 即对应的是<空>;
        case 106: switStatement(str); break; //情况语句，也就是switch-case语句
        case 17: statExecution(str, false, false); break;//"{" 语句列即不是复合语句(即不能有const、变量定义)
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
            }else{ //函数调用（既可以是有返回值，也可以是无返回值） !!!如何保证调用的函数是已经定义过的 //!!!用mp_func
                now = pos_line_header;
                id = lexicalAnalysis(str, sym);
                funcCall(str);
            }
            test({";"}, 98);
            id = lexicalAnalysis(str, sym);
        }
    }
}
void statExecution(string& str, bool is_multi_statement, bool is_main){ //处理复合语句 //＜复合语句＞   ::=  ［＜常量说明＞］［＜变量说明＞］＜语句列＞
    test({"{"}, 56);
    id = lexicalAnalysis(str, sym);
    
    if (is_multi_statement){
        consDeclarion(str);
        variDeclation(str);
//        if (!is_main)
            addQuat("BEGIN", "", "", "");
    }
    while (sym != "}"){ //＜语句列＞ ::=｛＜语句＞｝ //调用语句列的地方只有一个就是语句里有 ‘{’＜语句列＞‘}’
        statement(str);
    }
    id = lexicalAnalysis(str, sym);
}
int procMainExec(string& str){
    int tot = 0;
    string para_type, para_name;
    if (id < 1000) //如果不是标识符，就报错
        error(4);
    id = lexicalAnalysis(str, sym); //读到(
    if (sym != "(")
        error(9);
    id = lexicalAnalysis(str, sym);
    para_type = sym;
    addr = 0; //每一个函数/过程的addr都为0
    while (sym == "int" || sym == "char"){
        tot += (tot==0);//如果是第一次0就加一，因为后面的tot++统计的是逗号的个数
        id = lexicalAnalysis(str, sym); //得到变量名
        para_name = sym;
        if (id < 1000) //如果不是标识符，就报错
            error(4);
        enter(sym, "parameter", para_type, 0, ++addr, 0);
        addQuat("parameter_"+para_type, para_name, "", "");
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
void voidDeclartion(string& str, string kind, string type){ //有返回值函数定义和无返回值函数定义 //处理void的定义, 注意已经在procDeclartion里面
    int void_pos, pos_line_header = last;
    id = lexicalAnalysis(str, sym); //得到过程名
    string void_name = sym;
    
    enter(sym, kind, type, 0, 0, 0);//variable_tot); //必须要先塞进去
    
    void_pos = cnt_tab;
    addQuat(kind+"_"+type, void_name, "", ""); //"void_", "function_int" "function_char"
    int variable_tot = procMainExec(str);
    
    cout << "This is a " + kind + type + " statement " << void_name << " ::: " << variable_tot << " parameters::: " << str.substr(pos_line_header, last-pos_line_header) << endl << endl;
    tab[void_pos].para_num = variable_tot;
    mp_func[void_name] = Func{void_pos, type, variable_tot};
    statExecution(str, true, false); //是复合语句，即可以有const和变量定义
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
            voidDeclartion(str, "void", "");
            if (quat[cnt_quat].type != "ret") //如果void的最后没有return ; 我们会帮它加上一句ret，代表自然结束了，这样便于之后的恢复现场
                addQuat("ret", "", "", "");
        }else{
            voidDeclartion(str, "function", sym);
        }
    }
}
map<string,int> mp_quat = {
    {"const_int", 1}, {"const_char", 1},
    {"variable_int", 3}, {"variable_char", 3},
    {"function_int", 5}, {"function_char", 5},
    {"parameter_int", 7}, {"parameter_char", 7},
    {"void_", 9},
    {"+", 10}, {"-", 10}, {"*", 10}, {"/", 10},
    {"=",20}, {">=", 20}, {"==", 20}, {"<=", 20}, {">", 20}, {"<", 20},
    {"variable_int[]", 40}, {"variable_char[]", 40},
    {"BEGIN", 50}, //这个是我人为加上的一个标记，代表的是函数中变量定义结束的位置，在这个时候我进行了ra和sp压到运行栈的步骤
    {"GOTO", 100}, {"BZ", 100}, {"PRINT", 100}, {"READ", 100}, {"PUSH", 100}, {"ret", 100}, {"call", 100}, {"SWITCH", 100},
    
};

int locateVariable(string name, int program_id, int& offset){ //找到这个变量在哪里定义的，返回在符号表的位置，未找到则是-1
    int program_end = index_proc[program_id+1]-1;
//    if (program_id == cnt_proc) //因为我加上了统一操作，因此不需要了
//        program_end = cnt_tab;
    if (name.size() > 3 && name.substr(0,4) == "RET_"){ // "RET_int" or "RET_char"
        return -1;
    }
    if (name.size()>1 && name[0] == '#'){
        offset = mp_tmp[name].second - 1; //-1的目的是因为我们的栈顶指向的是函数存放的第一个参数，而由于之前存放在符号表的第一个是函数名，因此addr相当于多了一个，以此同理
        return 0;
    }
    rep (i, index_proc[program_id]+1, program_end){ //因为第一个是函数的名字，防止出现局部变量名和函数名同名的情况
        if (tab[i].name == name){
            offset = tab[i].addr - 1;
            return i;
        }
    }
    //代表所在程序的局部变量区没有，去全局区找一下
    rep (i, 1, index_proc[1]-1){
        if (tab[i].name == name){
            offset = tab[i].addr - 1;
//            program_id = 0; //把原调用的函数的program_id的信息更新，代表在全局函数
            return i;
        }
    }
    return -2;
}
void printQuat(){
    cout << "------------------------------" << endl;
    cout << "------------------------------" << endl;
    cout << "------------------------------" << endl;
    rep (i,1,cnt_quat) {
        
        if (!quat[i].label.empty())// != -1)
            for (auto j: quat[i].label)//cout << "LABEL_" << quat[i].label << " ";
                cout << "LABEL_" << j << " ";
        switch (mp_quat[quat[i].type]){
            case 100: cout << quat[i].type << " " << quat[i].op1 << endl; break;
            case 1: cout << "const " << quat[i].type.substr(6, quat[i].type.size()-6) << " " << quat[i].op1 << " = " << quat[i].op2 << endl; break;
            case 3: cout << "var " << quat[i].type.substr(9, quat[i].type.size()-9) << " " << quat[i].op1 << endl; break;
            case 5: cout << quat[i].type.substr(9, quat[i].type.size()-9) << " " << quat[i].op1 << "()" << endl; break;
            case 7: cout << "para " << quat[i].type.substr(10, quat[i].type.size()-10) << " " << quat[i].op1 << endl; break;
            case 9: cout << "void " << quat[i].op1 << endl; break;
            case 10: cout << quat[i].op1 << " = " << quat[i].op2 << " " << quat[i].type << " " << quat[i].op3 << endl; break;
            case 20: cout << quat[i].op1 << " " << quat[i].type << " " << quat[i].op2 << endl; break;
            case 30: cout << quat[i].op1 << " = " << quat[i].op2 << "[" << quat[i].op3 << "]" << endl; break;
            case 40: cout << "var " << quat[i].type.substr(9,quat[i].type.size()-2-9) << " " << quat[i].op1 << "[" << quat[i].op2 << "]" << endl; break;
            case 50: cout << quat[i].type << endl;  break;
            default: cout << "@@@" << quat[i].type << " " << mp_quat[quat[i].type] << endl; cout << quat[i].type << " " << quat[i].op1 << " " << quat[i].op2 << " " << quat[i].op3 << endl;
        }
    }
}
int getT(){
//    rep (i,8,15){
//        if (!reg[i]){
//            reg[i] = true;
//            return i;
//        }
//    }
//    return -1;
    return 0;
}
void allocateZero(){
    cout << "sw\t$0,0($sp)" << endl;
    cout << "addi\t$sp,$sp,-4" << endl << endl;
}
void allocateConst(const Quat& q){ //常量 //不允许修改i,并且是个引用，不需要复制
    cout << "#\tconst " << q.type.substr(6, q.type.size()-6) << " " << q.op1 << " = " << q.op2 << endl;
    int t_id = 1; //getT();
    cout << "li\t$t" << t_id << "," << q.op2 << endl; // li t0, num
    cout << "sw\t$t" << t_id << ",0($sp)" << endl;
    cout << "addi\t$sp,$sp,-4\n" << endl;
}
void allocateVariable(const Quat& q){
    allocateZero();
}
void allocateArray(const Quat& q){
    int label_print;//t_id = getT()
    cout << "li\t$t0,0" << endl;
    cout << "li\t$t1," << q.op2 << endl;
    newLabel(label_print);
    cout << "LABEL_" << label_print << ":" << endl;
    allocateZero();
    cout << "addi\t$t0,$t0,1" << endl;
    cout << "bne\t$t0,$t1,LABEL_" <<  label_print << endl << endl;
}
void allocateFunction(const Quat& q){ //函数/过程的分配，主要是要保存fp和ra信息
//    cout << q.type.substr(9, q.type.size()-9) << " " << q.op1 << "()" << endl;
    cout << "#END Const&Variable define" << endl;
    
    
    cout <<"sw\t$s1,0($sp)"<<endl; //***保存sp
    cout <<"addi\t$sp,$sp,-4\n"<<endl; //***
    
    
    cout <<"sw\t$ra,0($sp)"<<endl; //保存返回地址

//    cout <<"addi\t$fp,$sp,0"<<endl; //这个函数里的fp是一个基准线的作用，保存当前函数的顶部
    cout <<"addi\t$sp,$sp,-4\n"<<endl;
    
    cout <<"sw\t$s0,0($sp)"<<endl; //压栈保存一下之前的fp在哪，因为之前已经先在函数定义的时候保存了
    cout <<"addi\t$sp,$sp,-4\n"<<endl;
    
    cout << "addi\t$sp,$sp,-" << 4*mp_quat_cnt_temp[q.program_id] << endl;//***
//    cout << "move\t$fp,$s0" << endl; //!!!
}
//# =,hhh,,yyy
//# get hhh
//li	$t0,0
//
//addi	$t0,$t0,0
//sll	$t0,$t0,2
//sub	$t0,$t9,$t0
//lw	$t0,0($t0)
//
//# get yyy
//li	$t1,0
//
//addi	$t1,$t1,13
//sll	$t1,$t1,2
//sub	$t1,$fp,$t1
//sw	$t0,0($t1)
void getVariableMips(int reg_t, string name, int program_id, bool is_load){ //is_load:是否要取出值
    string name_array, name_offset;
    cout << "#~~~" << name << endl; //~~~a[yyy]
    if (name == "i"){
        int kk;
        kk = 1;
    }
    int offset, start; //offset = tab[pos].addr
    if (isChar(name)){
        cout << "li\t$t" << reg_t << "," << int(name[1]) << endl; //输出字符串的值
        return ;
    }
    if (isNumber(name)){ //如果是一个整数
        cout << "li\t$t" << reg_t << "," << mystoi(name) << endl;
        return ;
    }
    if (name == "RET_int" || name == "RET_char"){
        if (is_load){
            cout << "move\t$t" << reg_t << ",$v1" << endl;
        }
        //cout << "sw\t$v1,0($t1)" << endl; //把v1的值加载到t1所在的位置
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
        cout << "sll\t$t2,$t2,2" << endl;
        cout << "sub\t$t1,$t1,$t2" << endl;
        if (is_load){
            cout << "lw\t$t" << reg_t << ",0($t1)" << endl;
        }
        return ;
    }
    int pos = locateVariable(name, program_id, offset); //如果是临时变量,pos = 0//把相对于函数的偏移量保存到offset //到四元式这一步，肯定是有定义了
    if (pos == -2){ //以防万一，!!!可删 -1是RET,虽然已经先处理过了RET了
        cout << "!!!ERRNOT DEFINED!!!" << endl;
        return ;
    }
    if (pos > 0 && pos < index_proc[1]){//        beg = "0xx2ffc";
        //cout << "li\t$t" << reg_t << ",0x2ffc" << endl;
        cout << "move\t$t" << reg_t << ",$gp" << endl;
    }else {//        cout << "li\t$t" << reg_t << ",0" << endl;
        cout << "move\t$t" << reg_t << ",$fp" << endl;
    }
    
//    if (pos < index_proc[1]) //代表是全局变量区 //0x2ffc
//        start = 1;
    start = index_proc[program_id];
    if (pos < index_proc[1]) //代表在全局区找到了
        start = 1;
    if (program_id == 0)
        start = 1;
    rep (i,start,pos-1){
        if (tab[i].type == "int[]" || tab[i].type == "char[]"){
            offset += tab[i].para_num;
        }
    }
    offset*=4;
    cout << "subi\t$t" << reg_t << ",$t" << reg_t << "," << offset << endl;
    if (is_load){
        cout << "lw\t$t" << reg_t << ",0($t" << reg_t << ")" << endl;
    }
}
void allocateParameter(const Quat& q, int para_i){ //参数的分配，标准的分配方法是：前4个压到a0-a3，后面的压到栈上
    cout << "#\t" << q.type << " " << q.op1 << endl;
    int t_reg_1 = 1, t_reg_2 = 2;;
    //!!!
    getVariableMips(t_reg_1, q.op1, q.program_id, true);
//    cout << "move\t$t" << t_reg_2 << ",$sp" << endl;
//    cout << "subi\t$t" << t_reg_2 << ",$t" << t_reg_2 << "," << 4*para_i << endl;

//    cout << "sw\t$t" << t_reg_1 << ",0($t" << t_reg_2 << ")" << endl;
    
    
//    cout << "move\t$t" << t_reg_2 << ",$fp" << endl;
//    cout << "subi\t$t" << t_reg_2 << ",$t" << t_reg_2 << "," << 4*(para_i-1) << endl;
//    cout << "sw\t$t" << t_reg_1 << ",0($t" << t_reg_2 << ")" << endl; //sw $t1,0($t2)
    
    /*
    int temp_op2 = mystoi(q.op2);
    cout << "###Para" << temp_op2 << " " << para_i << endl;
    cout << "move\t$sp,$fp" << endl;
    cout << "subi\t$sp,$sp," << 4*(temp_op2-1) << endl;
    cout << "sw\t$t" << t_reg_1 << ",0($sp)" << endl;
    cout << "addi\t$sp,$sp,-4\n" <<endl;
    */
    
    cout << "sw\t$t" << t_reg_1 << ",0($sp)" << endl;
    cout << "addi\t$sp,$sp,-4\n" <<endl;
}
void assiMips(const Quat& q){ // y = x//赋值语句的转化
    cout << "#" << q.op1 << " " << q.type << " " << q.op2 << endl;
    int t_reg_1 = getT(), t_reg_2 = getT();
    //!!!
    t_reg_1 = 1, t_reg_2 = 2; //空出t0给li用
    if (q.op1.find("[")!=string::npos){ //a[i] = j 必须先算a[i] 否则t2被后面的给挤掉了
        getVariableMips(t_reg_1, q.op1, q.program_id, false); //get y.address to t1
        getVariableMips(t_reg_2, q.op2, q.program_id, true); //get x to t2
    }else{
        getVariableMips(t_reg_2, q.op2, q.program_id, true); //get x to t2
        getVariableMips(t_reg_1, q.op1, q.program_id, false); //get y.address to t1
    }
    cout << endl << "sw\t$t" << t_reg_2 << ",0($t" << t_reg_1 << ")" << endl; //sw $t2,0($t1)
}
void addMips(const Quat& q, string operation){ //add sub //#12 = x + 1
    cout << "#" << q.op1 << " = " << q.op2 << " " << q.type << " " << q.op3 << endl;
    int t_reg_1 = getT(), t_reg_2 = getT(), t_reg_3;
    //!!!
    t_reg_1 = 1, t_reg_2 = 2, t_reg_3 = 3; //空出t0给li用
    if (q.op1.find("[")!=string::npos){ //a[i] + j 必须先算a[i] 否则t2被后面的给挤掉了
        getVariableMips(t_reg_1, q.op2, q.program_id, true);
        getVariableMips(t_reg_2, q.op3, q.program_id, true);
    }else{ // j + a[i]
        getVariableMips(t_reg_2, q.op3, q.program_id, true);
        getVariableMips(t_reg_1, q.op2, q.program_id, true);
    }
    cout << operation << "\t$t" << t_reg_1 << ",$t" << t_reg_1 << ",$t" << t_reg_2 << endl; //最终结果保存在t1里
    getVariableMips(t_reg_3, q.op1, q.program_id, false);
    cout << endl << "sw\t$t" << t_reg_1 << ",0($t" << t_reg_3 << ")" << endl; //把t1写到结果t3里 //sw $t1,0($t3)
}
void compMips(const Quat& q, string operation){ //add sub //#12 = x + 1
    map<string, string> mp_comp = {{">=", "sge"}, {">", "sgt"}, {"<=", "sle"}, {"<", "slt"}, {"==", "seq"}, {"!=", "sne"}};
    cout << "#\t" << q.op1 << " " << q.type << " " << q.op2 << endl;
    int t_reg_1 = getT(), t_reg_2 = getT(), t_reg_3;
    //!!!
    t_reg_1 = 1, t_reg_2 = 2; //空出t0给li用
    getVariableMips(t_reg_1, q.op1, q.program_id, true);
    getVariableMips(t_reg_2, q.op2, q.program_id, true);
    cout << mp_comp[operation] << "\t$t0,$t" << t_reg_1 << ",$t" << t_reg_2 << endl;
}
void jumpMips(const Quat& q){ //BZ LABEL_2 //READ x
    cout << "#\t" << q.type << " " << q.op1 << endl;
    cout << "bne\t$t0,1," << q.op1 << endl; //BZ是不满足就跳转，所以就是不等于1就跳转
}
void gotoMips(const Quat& q){ //BZ LABEL_2 //READ x
    cout << "#\t" << q.type << " " << q.op1 << endl;
    cout << "jal\t" << q.op1 << endl; //BZ是不满足就跳转，所以就是不等于1就跳转
}
void reprMips(const Quat& q, bool is_read){ //BZ LABEL_2 //READ x
    cout << "#\t" << q.type << " " << q.op1 << endl;
    if (q.op1.size() == 0)
        return ;
    if (!is_read && q.op1[0] == '"'){ //输出的内容是字符串，找到对应的是几号str，这个在之前的.data段定义过了
        cout << "la\t$a0,str" << mp_s[q.op1] << endl;
        cout <<"li\t$v0,4"<<endl;
        cout <<"syscall\n"<<endl;
        return ;
    }
    int t_reg_1 = 1;//getT()!!!
    getVariableMips(t_reg_1, q.op1, q.program_id, !is_read);
    int offset, print_type=0;
    int pos = locateVariable(q.op1, q.program_id, offset);
    if ((pos == -1 && q.op1 == "RET_int") || (pos == 0 || tab[pos].type == "int")){ //pos==0代表是四元式产生的局部变量
        print_type = is_read ? 5 : 1;
    }else if ((pos == -1 && q.op1 == "RET_char") || (tab[pos].type == "char")){
        print_type = is_read ? 12 : 11;
    }else{
        if (pos>=0)
            cout << tab[pos].type;
        cout << "!!!ERRRROR!!!" << endl;
    }
    if (!is_read)
        cout << "move\t$a0,$t" << t_reg_1 << endl;
    cout << "li\t$v0," << print_type << endl;
    cout << "syscall\n" << endl;
    if (is_read)
        cout << "sw\t$v0,0($t" << t_reg_1 << ")" << endl << endl;
}
void retuMips(const Quat& q){ //add sub //#12 = x + 1
    cout << "#\t" << q.op1 << " " << q.type << " " << q.op2 << endl;
    if (q.program_id == cnt_proc){ //如果是主函数，就不要恢复现场了，直接退出程序
        cout << "li\t$v0,10" << endl;
        cout << "syscall" << endl;
        return ;
    }
    if (q.op1!=""){ //代表不是过程
        int t_reg_1 = 1;//!!!t_reg_1 = 1
        getVariableMips(t_reg_1, q.op1, q.program_id, false);
        cout << "lw\t$v1,0($t" << t_reg_1 << ")" << endl;
    }
    
    cout << "lw\t$sp,-" << int2string(4*(para_now_cnt)) << "($fp)" << endl; //****
    cout << "addi\t$sp,$sp," << 4*(mp_quat_para_num[q.program_id]) << endl;
    
    cout << "lw\t$ra,-" << int2string(4*(para_now_cnt+1)) << "($fp)" << endl; //这两个顺序不能反，因为取ra要用到目前的fp，因此不能先恢复fp现场
    
    
    
    cout << "lw\t$fp,-" << int2string(4*(para_now_cnt+2)) << "($fp)" << endl;
    
    
    
    cout << "jr\t$ra" << endl;
}
//void printGlobal(){
//    cout << "TOTAL GLOBAL = " << index_proc[1] << endl;
//    rep (i,1,index_proc[1]-1){ //在第一个分程序前定义的常变量都是全局的常变量，并且正好对应一条四元式
//        printVariable(i);
//    }
//}
void calcTmp(){
    cout << "------------------------------" << endl;
    cout << "------------------------------" << endl;
    cout << "------------------------------" << endl;
    map<int, int> mp_proc_num, mp_proc_cnt;
    rep (i,1,cnt_proc){
        mp_proc_cnt[i] = index_proc[i+1] - index_proc[i]-1;//空出每段的第一个符号即函数自己，
   }
    rep (i,1,cnt_quat){
        if (quat[i].type == "function_int" || quat[i].type == "function_char" || quat[i].type == "void_"){
            rep (j,1,cnt_proc){
                if (tab[index_proc[j]].name == quat[i].op1){
                    mp_quat_para_num_with_local[i] = index_proc[j+1] - index_proc[j]-1;
                    mp_quat_para_num[i] = tab[index_proc[j]].para_num;
                    break;
                }
            }
        }
        if (quat[i].op1.size()>1&&quat[i].op1[0]=='#'){ //四元式产生的中间代码一定是第一个操作数
            if (mp_tmp.count(quat[i].op1)>0){
                cout << "SAME!!!" << endl;
                continue;
            }
            mp_proc_num[quat[i].program_id] ++;
            mp_tmp[quat[i].op1] = make_pair(quat[i].program_id, 3+mp_proc_cnt[quat[i].program_id]+mp_proc_num[quat[i].program_id]);
            cout << quat[i].op1 << " ::: " << mp_tmp[quat[i].op1].first << " " << mp_tmp[quat[i].op1].second << endl;
        }
    }
    mp_quat_cnt_temp = mp_proc_num;
}
void programTable(){
    cout << "------------------------------" << endl;
    cout << "------------------------------" << endl;
    cout << "------------------------------" << endl;
    rep (i,1,cnt_proc){
        cout << "Program " << i << " :  " << index_proc[i] << endl;
    }
    cout << "AddByMe Program " << cnt_proc+1 << " :  " << index_proc[cnt_proc+1] << endl;
}

void symbolTable(){
    cout << "------------------------------" << endl;
    cout << "------------------------------" << endl;
    cout << "------------------------------" << endl;
    rep (i,1,cnt_tab){
        cout << "Symbol " << i << " :  " << tab[i].name << " " << tab[i].kind << " " << tab[i].type << " " << tab[i].value << " " << tab[i].addr << " " << tab[i].para_num << " " << tab[i].program_id << endl;
    }
}
map<string,int> mp_mips = {
    {"const_int", 1}, {"const_char", 1},
    {"variable_int", 3}, {"variable_char", 3},
    {"function_int", 5}, {"function_char", 5},
    {"parameter_int", 7}, {"parameter_char", 7},
    {"void_", 5},
    {"+", 10}, {"-", 11}, {"*", 12}, {"/", 13},
    {"=",20}, {">=", 21}, {"==", 21}, {"<=", 21}, {">", 21}, {"<", 21},
    
    {"variable_int[]", 40}, {"variable_char[]", 40},
    {"BEGIN", 50},
    {"PUSH", 101}, {"BZ", 102}, {"PRINT", 103}, {"READ", 104}, {"GOTO", 105}, {"ret", 106}, {"call", 105}, {"SWITCH", 107},
};
void quatMips(){
    cout << "------------------------------" << endl;
    cout << "------------------------------" << endl;
    cout << "------------------------------" << endl;
    
    cout << ".data" << endl << endl;
    rep (i, 1, cnt_quat){
        if (quat[i].type == "PRINT" && quat[i].op1[0] =='"'){
            if (mp_s.count(quat[i].op1) == 0){ //重复的就不要加了
                mp_s[quat[i].op1] = ++cnt_mp_s;
            }
        }
    }
    for (auto i: mp_s){
        cout << "str" << i.second << ":\t.asciiz" << " " << i.first << endl <<endl;
    }
    
    rep (i,1,index_proc[1]-1){ //全局常变量占在一个固定的坑里，不会出来了，之后遇到全局变量也可以直接根据$sp(0x2ffc)-addr来找到 //在第一个分程序前定义的常变量都是全局的常变量，并且正好对应一条四元式
        switch (mp_mips[quat[i].type]){
            case 1: allocateConst(quat[i]); break;
            case 3: allocateVariable(quat[i]); break;
            case 40: allocateArray(quat[i]); break;
            default: cout << "!!!ErrorNOTDefined!!!" << mp_mips[quat[i].type] << endl;
        }
    }
    
    cout << ".text" << endl << endl;
    cout << "move\t$gp, $sp" << endl;
    rep (i, 1, cnt_quat){//main_pos-1){
        if (i == index_proc[1])
            cout << "j\tmain" << endl << endl; //声明常变量后，先跳转到main进行!!!有必要吗
        if (!quat[i].label.empty())//(quat[i].label != -1)
            for (auto j : quat[i].label)//cout << "LABEL_" << quat[i].label << ":" << endl;
                cout << "LABEL_" << j << ":" << endl;
        if (i==14){
            int llll;
            llll = 1;
        }
        switch (mp_mips[quat[i].type]){
            case 100: break;
            case 1: allocateConst(quat[i]); break;
            case 3: allocateVariable(quat[i]); break;
            case 5: para_i = 0; cout << quat[i].op1 << ":" << endl; cout << "move\t$s1,$sp" << endl; cout << "move\t$s0,$fp" << endl; cout << "addi\t$fp,$sp," << 4*(mp_quat_para_num[i]) << endl; para_now_cnt = mp_quat_para_num_with_local[i]; break;
            case 7: break; //参数，直接无视，因为在PUSH里干了
            case 9: cout << quat[i].op1 << ":" << endl; break;
            case 10: addMips(quat[i], "add"); break; //处理加号
            case 11: addMips(quat[i], "sub"); break; //处理减号
            case 12: addMips(quat[i], "mul"); break; //处理乘号
            case 13: addMips(quat[i], "div"); break; //处理除号
            case 20: assiMips(quat[i]); break; // 赋值语句
            case 21: compMips(quat[i], quat[i].type); break; //小于等于 //#6 >= #10 //BZ LABEL_2
            case 40: allocateArray(quat[i]); break;
            case 50: allocateFunction(quat[i]); break;
            case 101: allocateParameter(quat[i], ++para_i); break; //PUSH
            case 102: jumpMips(quat[i]); break;
            case 103: reprMips(quat[i], false); break;
            case 104: reprMips(quat[i], true); break;
            case 105: gotoMips(quat[i]); break;
            case 106: retuMips(quat[i]); break;
            case 107: cout << "#\t" << quat[i].type << " " << quat[i].op1 << endl; break;
            default: cout << "???" << quat[i].type << " " << quat[i].op1 << endl;
        }
    }
}

int main() {
    freopen("out.txt","w",stdout);
    ofstream asm_out("asm.txt");
    asm_out << "123" << endl;
    
    cout << "Please input a file:" << endl;
    cin >> route;
    ifstream r_t(route);
    if (!r_t){
        cout << "File Not exist" << endl;
        return 0;
    }
    
    std::ifstream t(route);//"14011100_test.txt");
    std::string s((std::istreambuf_iterator<char>(t)),
                  std::istreambuf_iterator<char>());
    
    str = s;
    siz = str.size();
    //cout << str << endl << endl;
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
    calcTmp();
    cout << mp_tmp["#1"].second << endl;
    quatMips();
    
    return 0;
}

/*
 ＜加法运算符＞ ::= +｜-
 ＜乘法运算符＞  ::= *｜/
 ＜关系运算符＞  ::=  <｜<=｜>｜>=｜!=｜==
 ＜字母＞   ::= ＿｜a｜．．．｜z｜A｜．．．｜Z
 ＜数字＞   ::= ０｜＜非零数字＞
 ＜非零数字＞  ::= １｜．．．｜９
 ＜字符＞    ::=   '＜加法运算符＞'｜'＜乘法运算符＞'｜'＜字母＞'｜'＜数字＞'
 ＜字符串＞   ::=  "｛十进制编码为32,33,35-126的ASCII字符｝"
 ＜程序＞    ::= ［＜常量说明＞］［＜变量说明＞］{＜有返回值函数定义＞|＜无返回值函数定义＞}＜主函数＞
 ＜常量说明＞ ::=  const＜常量定义＞;{ const＜常量定义＞;}
 ＜常量定义＞   ::=   int＜标识符＞＝＜整数＞{,＜标识符＞＝＜整数＞}
 | char＜标识符＞＝＜字符＞{,＜标识符＞＝＜字符＞}
 ＜无符号整数＞  ::= ＜非零数字＞｛＜数字＞｝
 ＜整数＞        ::= ［＋｜－］＜无符号整数＞｜０
 ＜标识符＞    ::=  ＜字母＞｛＜字母＞｜＜数字＞｝
 ＜声明头部＞   ::=  int＜标识符＞ |char＜标识符＞
 ＜变量说明＞  ::= ＜变量定义＞;{＜变量定义＞;}
 ＜变量定义＞  ::= ＜类型标识符＞(＜标识符＞|＜标识符＞‘[’＜无符号整数＞‘]’){,(＜标识符＞|＜标识符＞‘[’＜无符号整数＞‘]’) }
 ＜常量＞   ::=  ＜整数＞|＜字符＞
 ＜类型标识符＞      ::=  int | char
 ＜有返回值函数定义＞  ::=  ＜声明头部＞‘(’＜参数＞‘)’ ‘{’＜复合语句＞‘}’
 ＜无返回值函数定义＞  ::= void＜标识符＞‘(’＜参数＞‘)’‘{’＜复合语句＞‘}’
 ＜复合语句＞   ::=  ［＜常量说明＞］［＜变量说明＞］＜语句列＞
 ＜参数＞    ::= ＜参数表＞
 ＜参数表＞    ::=  ＜类型标识符＞＜标识符＞{,＜类型标识符＞＜标识符＞}|＜空＞
 ＜主函数＞    ::= void main‘(’‘)’ ‘{’＜复合语句＞‘}’
 ＜表达式＞    ::= ［＋｜－］＜项＞{＜加法运算符＞＜项＞}
 ＜项＞     ::= ＜因子＞{＜乘法运算符＞＜因子＞}
 ＜因子＞    ::= ＜标识符＞｜＜标识符＞‘[’＜表达式＞‘]’｜＜整数＞|＜字符＞｜＜有返回值函数调用语句＞|‘(’＜表达式＞‘)’
 ＜语句＞    ::= ＜条件语句＞｜＜循环语句＞| ‘{’＜语句列＞‘}’｜＜有返回值函数调用语句＞;
 | ＜无返回值函数调用语句＞;｜＜赋值语句＞;｜＜读语句＞;｜＜写语句＞;｜＜空＞;|＜情况语句＞｜＜返回语句＞;
 ＜赋值语句＞   ::=  ＜标识符＞＝＜表达式＞|＜标识符＞‘[’＜表达式＞‘]’=＜表达式＞
 ＜条件语句＞  ::=  if ‘(’＜条件＞‘)’＜语句＞［else＜语句＞］
 ＜条件＞    ::=  ＜表达式＞＜关系运算符＞＜表达式＞｜＜表达式＞ //表达式为0条件为假，否则为真
 ＜循环语句＞   ::=  for‘(’＜标识符＞＝＜表达式＞;＜条件＞;＜标识符＞＝＜标识符＞(+|-)＜步长＞‘)’＜语句＞
 ＜步长＞    ::=  ＜非零数字＞｛＜数字＞｝
 ＜情况语句＞  ::=  switch ‘(’＜表达式＞‘)’ ‘{’＜情况表＞＜缺省＞‘}’
 ＜情况表＞   ::=  ＜情况子语句＞{＜情况子语句＞}
 ＜情况子语句＞  ::=  case＜常量＞：＜语句＞
 ＜缺省＞   ::=  default : ＜语句＞
 ＜有返回值函数调用语句＞ ::= ＜标识符＞‘(’＜值参数表＞‘)’
 ＜无返回值函数调用语句＞ ::= ＜标识符＞‘(’＜值参数表＞‘)’
 ＜值参数表＞   ::= ＜表达式＞{,＜表达式＞}｜＜空＞
 ＜语句列＞   ::= ｛＜语句＞｝
 ＜读语句＞    ::=  scanf ‘(’＜标识符＞{,＜标识符＞}‘)’
 ＜写语句＞    ::= printf ‘(’ ＜字符串＞,＜表达式＞ ‘)’| printf ‘(’＜字符串＞ ‘)’| printf ‘(’＜表达式＞‘)’
 ＜返回语句＞   ::=  return[‘(’＜表达式＞‘)’]
 附加说明：
 （1）char类型的表达式，用字符的ASCII码对应的整数参加运算，在写语句中输出字符
 （2）标识符区分大小写字母
 （3）写语句中的字符串原样输出
 （4）情况语句中，switch后面的表达式和case后面的常量只允许出现int和char类型；每个情况子语句执行完毕后，不继续执行后面的情况子语句
 （5）数组的下标从0开始
 （6）for语句先执行一次循环体中的语句再进行循环变量是否越界的测试
*/
 
