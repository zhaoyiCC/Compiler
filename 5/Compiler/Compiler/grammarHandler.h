#ifndef grammarHandler_h
#define grammarHandler_h
#include "headers.h"
#include "error.h"
#include "myStd.h"
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
    int pos_line_header = last, offset, pos;
    //string res = sym;
    if (id == 98 || id == 99){ //代表是字符或者(无符号)整数
        is_char = (id == 98); //如果是字符就是char
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
        is_char = false;
    }else if (id < 1000){ //代表不是标识符或者函数名
        error(14);
        id = lexicalAnalysis(str, sym);
        //return ;
    }else{//现在是最后的情况即是一个标识符，之前已经读过标识符了
        res = op2 = factor_name = sym; //三连等赋值
        id = lexicalAnalysis(str, sym);
        
        if (sym == "["){
            pos = locateVariable(factor_name, cnt_proc, offset, false);
            
            id = lexicalAnalysis(str, sym);
            expression(str, op3);
            test({"]"},15);
            id = lexicalAnalysis(str, sym);
            newTmp(op1);
            //cout << "@@@@@@@" << endl;
            addQuat("=", op1, op2+"["+op3+"]", "");//!!!addQuat("load", op1, op2, op3); // t1 = op2[op3]
            res = op1;
            is_char = (tab[pos].type == "char[]"); //顺序很重要，因为[]中间的表达式不能改变is_char的值
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
            else{ //******
                //!!!!res = "RET_"+mp_func[factor_name].type; //!!! //这样写会带来i=work(1)+work(1) 编程i=RET_int+RET_int的麻烦 即都按成了后面那个来计算
                newTmp(op1);
                op2 = "RET_"+mp_func[factor_name].type;
                addQuat("=", op1, op2, "");
                res = op1;
                is_char = (mp_func[factor_name].type == "char");
            }
            //return ;
        }else{
            //否则就是一个单纯的标识符，也就是变量名，不需要做任何事
            pos = locateVariable(factor_name, cnt_proc, offset, false);
            is_char = (tab[pos].type == "char");
        }
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
        is_char = false;
    }
    cout << "This is a term statemnt::: " << str.substr(pos_line_header, last-pos_line_header) << endl << endl;
    res = op1;
}
void expression(string& str, string& res){ //＜表达式＞    ::= ［＋｜－］＜项＞{＜加法运算符＞＜项＞}
    string op1, op2, op3, expression_sign;
    int pos_line_header = last;
    if (sym == "+" || sym == "-"){ //遇到前导的正号或者负号，先提取出来
        expression_sign = sym;
        id = lexicalAnalysis(str, sym);
        is_char = false; //有正号负号就一定不会是char类型的了
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
void condStatement(string& str, bool is_loop, Quat& q){ //＜条件＞::=＜表达式＞＜关系运算符＞＜表达式＞｜＜表达式＞//表达式为0条件为假，否则为真
    string op1, op2, op3, res, cond;
    int pos_line_header = last;
    expression(str, res);
    cond = sym;
    op1 = res;
    if (id >= 1 && id <= 6) { //代表是关系运算符
        id = lexicalAnalysis(str, sym);
        expression(str, res);
        op2 = res;
    }else if ((!is_loop&&sym == ")")||(is_loop&&sym==";")){ //代表是一个单独的表达式
        cond = "!=";
        op2 = "0";
    }else
        error(103);
    
    //if (!is_loop)
    addQuat(cond, op1, op2, ""); //"==" ">="
    //else{ q.type = cond; q.op1 = op1; q.op2 = op2; q.op3 = ""; q.program_id = cnt_proc;}
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
    condStatement(str, false, ifel_quat); //false代表不是for循环 因为f语句的条件必须是由()包住区分，而for循环的条件不需要()
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
//void loopStatement(string &str){ //＜循环语句＞::=for‘(’＜标识符＞＝＜表达式＞;＜条件＞;＜标识符＞＝＜标识符＞(+|-)＜步长＞‘)’＜语句＞
//    string op1, op2, op3, res, loop_variment, step_num;
//    Quat loop_cond_quat, loop_step;
//    int pos_line_header = last, loop_pos, label_first, label_second;
//    id = lexicalAnalysis(str, sym); //之前已经读进来过for了
//    test({"("},18);
//    id = lexicalAnalysis(str, sym);
//    if (id < 1000) //必须是标识符
//        error(19);
//    loop_variment = sym;
//    id = lexicalAnalysis(str, sym);
//    test({"="},20);
//    id = lexicalAnalysis(str, sym);
//    expression(str, res);
//    addQuat("=", loop_variment, res, ""); //i = 1
//    
//    loop_pos = cnt_quat;
//    newLabel(label_first);
//    quat[loop_pos+1].label.push_back(label_first);//= label_first; //在i=1的下一句加上标记
//    
//    test({";"},21);
//    id = lexicalAnalysis(str, sym);
//    condStatement(str, true, loop_cond_quat);
//    
//    test({";"},21);
//    id = lexicalAnalysis(str, sym); //处理到=前面的标识符
//    if (id < 1000) //必须是标识符
//        error(19);
//    id = lexicalAnalysis(str, sym); //处理到=
//    test({"="},20);
//    id = lexicalAnalysis(str, sym); //处理到=后面的标识符
//    if (id < 1000) //必须是标识符
//        error(19);
//    id = lexicalAnalysis(str, sym); //处理到+ -
//    test({"+", "-"},20);
//    loop_step.type = sym; //+ -
//    id = lexicalAnalysis(str, sym);
//    step(str, step_num);
//    test({")"}, 25);
//    loop_step.op1 = loop_variment;
//    loop_step.op2 = loop_variment;
//    loop_step.op3 = step_num;
//    loop_step.program_id = cnt_proc;
//    //loop_step.label = quat[cnt_quat+1].label; //!!! 重要 //!!!时间错误
//    id = lexicalAnalysis(str, sym);
//    statement(str);
//    loop_step.label = quat[cnt_quat+1].label;
//    quat[++cnt_quat] = loop_step; //步长 i=i+1
//    loop_cond_quat.label = quat[cnt_quat+1].label; //i<=tmp1
//    quat[++cnt_quat] = loop_cond_quat;
//    newLabel(label_second);
//    addQuat("BZ", "LABEL_"+int2string(label_second),"","");
//    addQuat("GOTO", "LABEL_"+int2string(label_first),"","");
//    quat[cnt_quat+1].label.push_back(label_second);//= label_second;
//    cout << "This is a loop statement::: " << str.substr(pos_line_header, last-pos_line_header) << endl << endl;
//}

void loopStatement(string &str){ //＜循环语句＞::=for‘(’＜标识符＞＝＜表达式＞;＜条件＞;＜标识符＞＝＜标识符＞(+|-)＜步长＞‘)’＜语句＞
    cout << mp["x"] << "~~~~~~~~~~~~~~~~~~~~~~~~~" << endl;
    string op1, op2, op3, res, loop_variment, step_num, sym_end_loop;
    Quat loop_cond_quat, loop_step;
    int pos_line_header = last, loop_pos, loop_pos_program,label_first, label_second, pos_end_loop, pos_before_step, last_end_loop, id_end_loop;
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
    loop_pos_program = now;
    newLabel(label_first);
    quat[loop_pos+1].label.push_back(label_first);//= label_first; //在i=1的下一句加上标记
    
    test({";"},21);
    
    while (str[now]!=';')// id = lexicalAnalysis(str, sym); //    condStatement(str, true, loop_cond_quat);
        now++;
    now++;
    pos_before_step = now;
    sym=";";
    test({";"},21);
    
    while (str[now]!=')')
        now++;
    now++;
    sym=")"; //Emit!!
    test({")"}, 25);

    id = lexicalAnalysis(str, sym);
    statement(str);
    pos_end_loop = now;
    last_end_loop = last;
    sym_end_loop = sym;
    id_end_loop = id;
    
    now = pos_before_step;
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
    loop_step.label = quat[cnt_quat+1].label;
    quat[++cnt_quat] = loop_step; //步长 i=i+1
    
    now = loop_pos_program;
    id = lexicalAnalysis(str, sym);
    condStatement(str, true, loop_cond_quat); //true代表是for循环 因为条件后的是分号，以便和if语句的条件必须和()包住区分
    test({";"},21);

    now = pos_end_loop;
    last = last_end_loop;
    sym = sym_end_loop;
    id = id_end_loop;
    
    newLabel(label_second);
    addQuat("BZ", "LABEL_"+int2string(label_second),"","");
    addQuat("GOTO", "LABEL_"+int2string(label_first),"","");
    quat[cnt_quat+1].label.push_back(label_second);//= label_second;
    cout << "This is a loop statement::: " << str.substr(pos_line_header, last-pos_line_header) << endl << endl;
    cout << mp["x"] << "~~~~~~~~~~~~~~~~~~~~~~~~~" << endl;
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
    string op1, op2, op3, res, prin_type = "int";
    int pos_line_header = last;
    id = lexicalAnalysis(str, sym);
    test({"("}, 29);
    id = lexicalAnalysis(str, sym);
    if (id == 97) { //代表是字符串
        addQuat("PRINT", sym, "string", ""); //***op2是我后来加的，这个是可有可无的，因为我后来输出判断了是不是字符串
        id = lexicalAnalysis(str, sym);
        if (sym == ","){ //＜字符串＞,＜表达式＞
            id = lexicalAnalysis(str, sym);
            expression(str, res);
            if (is_char)
                prin_type = "char";
            addQuat("PRINT", res, prin_type, "");
        }
    }else{
        expression(str, res);
        if (is_char)
            prin_type = "char";
        addQuat("PRINT", res, prin_type, "");
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
                cout << sym << endl;
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
#endif /* grammarHandler_h */
