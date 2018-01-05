#ifndef headers_h
#define headers_h
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

#define debug
using namespace std;
string now_proc_type; //保存当前处理的过程的类型 void/int/main
bool is_char = false; //全局变量判断当前处理完的表达式是什么类型
/********************错误处理*********************/
void error(int errorId, int line_pos);
/********************语义分析处理*******************/
int lexicalAnalysis(string& str, string& s);
/********************语法分析处理*******************/
void expression(string& str, string& res), statExecution(string& str, bool is_multi_statement, bool is_main), statement(string& str);

int id, cnt = 999, now = 0, last;
size_t siz;
map<int,int> mp_line;
int addr = 0, cnt_quat, cnt_tab = 0, cnt_proc = 0, index_proc[1010], cnt_tmp = 0, cnt_label, para_i, para_now_cnt; //index_proc为分程序索引表，里面存的是每个程序的第一个定义的变量在符号表的位置(就是过程/函数自己，因为自己也会存在这个里面的)
string route, sym, str; //sym: 获取到的接下来的一个字符串
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
    int block_id = -1; //默认不是基本块的起点，遇到一个过程的定义后的第一句话就是block_id=0，之后可能会编上标号。总之不是-1就代表不是基本块
    vector<int> label;
}quat[MAX_QUAT+10];

//struct Temp{
//    int addr, program_id;
//};
//vector<int> case_v;
map<string, int> mp_s;
int cnt_mp_s = 0, sp = 0, main_pos;
bool is_global = true; //是否是全局变量，在全局变量定义完之后置为false
bool reg[41]; //寄存器使用状态，true代表正在被使用
map<string, pair<int,int>> mp_tmp; //存放四元式产生的临时变量对应的在第几个程序块和在程序块的addr
map<int, int> mp_quat_para_num, mp_quat_para_num_with_local, mp_quat_cnt_temp, mp_proc_variable; //这几个map的键都对应的是第几个程序块
//mp_proc_variable存储的是第key号程序块的局部变量共占多少个单位，其中一般的变量算一个，数组算n个
//mp_quat_cnt_temp存储的是第key号程序块的四元式产生了多少中间变量
//mp_quat_para_num_with_local存储的是第key号程序块的四元式的变量个数(包括参数，局部常变量，其中数组算n个，不包括中间变量)
//mp_quat_para_num存储的是第key号程序块的参数的个数，这个就对应的是符号表里函数过程的para_num
struct Func{ //存放函数的结构体，其中type有三种类型，int, char, "" 其中第三个代表是过程
    int tab_id; //在符号表的登录位置
    string type;
    int para_num;
    vector<bool> is_char;
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

    {8, "variable define missing a ;"},
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
    {45, "missing main )"},

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

map<string,int> mp_quat = {
    {"const_int", 1}, {"const_char", 1},
    {"variable_int", 3}, {"variable_char", 3},
    {"function_int", 5}, {"function_char", 5},
    {"parameter_int", 7}, {"parameter_char", 7},
    {"void_", 9},
    {"+", 10}, {"-", 10}, {"*", 10}, {"/", 10},
    {"=",20}, {">=", 20}, {"==", 20}, {"!=", 20}, {"<=", 20}, {">", 20}, {"<", 20},
    {"variable_int[]", 40}, {"variable_char[]", 40},
    {"BEGIN", 50}, //这个是我人为加上的一个标记，代表的是函数中变量定义结束的位置，在这个时候我进行了ra和sp压到运行栈的步骤
    {"PRINT", 60}, //从100搬到了60，把输出的类型打印出来了，例如RINT_int 方便debug
    {"GOTO", 100}, {"BZ", 100},  {"READ", 100}, {"PUSH", 100}, {"ret", 100}, {"call", 100}, {"SWITCH", 100}, {"nop", 100},
    {"PRINTLN", 1000},
};

map<string,int> mp_mips = {
    {"const_int", 1}, {"const_char", 1},
    {"variable_int", 3}, {"variable_char", 3},
    {"function_int", 5}, {"function_char", 5},
    {"parameter_int", 7}, {"parameter_char", 7},
    {"void_", 5},
    {"+", 10}, {"-", 11}, {"*", 12}, {"/", 13},
    {"=",20}, {">=", 21}, {"==", 21}, {"!=", 21}, {"<=", 21}, {">", 21}, {"<", 21},

    {"variable_int[]", 40}, {"variable_char[]", 40},
    {"BEGIN", 50},
    {"PUSH", 101}, {"BZ", 102}, {"PRINT", 103}, {"READ", 104}, {"GOTO", 105}, {"ret", 106}, {"call", 105}, {"SWITCH", 107}, {"nop", 108},
    {"PRINTLN", 1000},
};

map<string,int> mp_dag = {
    {"const_int", 1}, {"const_char", 1},
    {"variable_int", 3}, {"variable_char", 3},
    {"function_int", 5}, {"function_char", 5},
    {"parameter_int", 7}, {"parameter_char", 7},
    {"void_", 9},
    {"+", 10}, {"-", 10}, {"*", 10}, {"/", 10},
    {"=",20}, {">=", 21}, {"==", 21}, {"!=", 21}, {"<=", 21}, {">", 21}, {"<", 21},
    {"variable_int[]", 40}, {"variable_char[]", 40},
    {"BEGIN", 50}, //这个是我人为加上的一个标记，代表的是函数中变量定义结束的位置，在这个时候我进行了ra和sp压到运行栈的步骤
    {"PRINT", 60}, //从100搬到了60，把输出的类型打印出来了，例如RINT_int 方便debug
    {"GOTO", 100}, {"BZ", 100},  {"READ", 100}, {"PUSH", 100}, {"ret", 100}, {"call", 100}, {"SWITCH", 100}, {"nop", 100},
    {"PRINTLN", 1000},
};
#endif /* headers_h */

