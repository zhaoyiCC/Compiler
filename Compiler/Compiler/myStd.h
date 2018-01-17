#ifndef myStd_h
#define myStd_h
bool isChar(const std::string& s){
    if (s.size()!=3)
        return false;
    if (s[0]!='\''||s[2]!='\'')
        return false;
    return true;//检测合法!!!
}
bool isString(const std::string& s){
    if (s[0]!='\"'||s[s.size()-1]!='\"')
        return false;
    return true;
}
bool isNumber(const std::string& s){
    if (s=="-" || s=="+") return false;
    std::string::const_iterator it = s.begin();
    if (*it=='-'||*it=='+') it++;
    while (it != s.end() && std::isdigit(*it)) ++it;
    return !s.empty() && it == s.end();
}
int mystoi(string s){
    if (isChar(s))//为了统一处理addMips里的字符，加上对字符'a'的处理
        return ((int)s[1]);
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
bool checkChar(int x){ //'＜加法运算符＞'｜'＜乘法运算符＞'｜'＜字母＞'｜'＜数字＞'
    if (x == '+' || x == '-' || x == '*' || x == '/')
        return true;
    if ((x>='a'&&x<='z')||(x>='A'&&x<='Z')||(x>='0'&&x<='9'))
        return true;
    return false;
}
int str_replace(string &str, const string &src, const string &dest){ //把字符串中的src转换成dest，返回转换的个数
    int counter = 0;
    string::size_type pos = 0;
    while ((pos = str.find(src, pos)) != string::npos) {
        str.replace(pos, src.size(), dest);
        ++counter;
        pos += dest.size();
    }
    return counter;
}
void splitArrayName(string s, string& array_name, string& subscript){
    auto pos1 = s.find("[", 0);
    auto pos2 = s.find("]", 0);
    if (pos1 == s.npos || pos2 == s.npos){
        cout << "!!!ERROR_splitArrayName::: " << s << "!!!" << endl;
        return ;
    }
    array_name = s.substr(0, pos1);
    subscript = s.substr(pos1+1, pos2-pos1-1);
}
int calcLabel(string s){
    if (s.size() < 6) //LABEL_
        return -9999;
    return mystoi(s.substr(6, s.size()));
}
void calcDifference(vector<string>& v1, vector<string>& v2, vector<string>& v_ans){
    v_ans.clear();
    for (auto i: v1){
        if (find(v2.begin(), v2.end(), i) == v2.end())
            v_ans.push_back(i);
    }
}
void uniqueVector(vector<int>& v){
    sort(v.begin(),v.end());
    v.erase(unique(v.begin(), v.end()), v.end());
}
void uniqueVector(vector<string>& v){
    sort(v.begin(),v.end());
    v.erase(unique(v.begin(), v.end()), v.end());
    //unique()函数将重复的元素放到vector的尾部 然后返回指向第一个重复元素的迭代器 再用erase函数擦除从这个元素到最后元素的所有的元素
}
#endif /* myStd_h */
