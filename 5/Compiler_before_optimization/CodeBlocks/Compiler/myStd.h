#ifndef myStd_h
#define myStd_h
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
#endif /* myStd_h */
