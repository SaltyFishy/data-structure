#ifndef FileHandle.h
#include<fstream>
#include<iostream>
#include<vector>
#include<windows.h>
#include<stack>
#include<algorithm>
#include"page.h"
#include"code.h"
#define asciiMap 26//26个字母
using namespace std;
string Path = "..\\fruits\\ascii-fruits\\";//全局变量Path，表示所有txt文件路径
string txtName[10] = { "apple","cherry","grape","mangosteen","orange","peach","pear","pineapple","pomelo","watermelon" };//文本名称数组
int asciiCount[asciiMap] = { 0 };//统计字符数目用于编码
HuffmanTree HT;//赫夫曼树
/*
* 结构体:字母表框架集合
* 强调:无
*/
typedef struct DocNode {//文档链表
    int DocId;//文档ID
    int P;//频次
    vector<int> pos;//单词在文档中的位置
    DocNode* next;//下一个文档
}*DocList;
typedef struct WordList {//字母表框架
    string word;//单词内容
    DocList list;//文档链表
    int size;//与一个词条相关联的文档数目
};
typedef struct Collection {//集合框架
    vector<WordList> collection;//选用vector而不是指针是考虑到读入的时候不知道有多少个单词，方便操作
    int size;//词条总个数
};
Collection c;
/*
* 结构体:返回结果集合
* 强调:无
*/
typedef struct Result {
    vector<int> wordListIndex;//单词的索引
    vector<bool> txtCollection;//表示10个文档是否存在的布尔逻辑
};
/*
* 结构体:临时结果集合
* 强调:无
*/
typedef struct tempResult {
    vector<bool> txtCollection;//表示10个文档是否存在的布尔逻辑
};
/*
* 结构体:缓存所有单词
* 强调:无
*/
typedef struct Buffer {
    vector<string> word;//单词本身
};
Buffer* buffer;
/*
* 字符数组:运算符优先级
* 强调:无
*/
char Prior[5][5] = {
'>','>','<','>','>',
'>','>','<','>','>',
'<','<','<','=','>',
'>','>',' ','>','>',
'<','<','<','<','=',
};
/*
* 字符数组:运算符
* 强调:无
*/
string OP[5] = { "&","|","(",")","#" };
/*
* 函数；判断是否是操作符
* 强调:无
*/
bool In(string s) {
    if (s == "(" || s == "and" || s == "or" || s == ")" || s == "\n") {
        return true;
    }
    return false;
}
/*
* 函数:比较字符串的优先级
* 强调:无
*/
char Precede(string a, string b) {
    int x = 0, y = 0;
    while (1) {
        if (a == OP[x]) {
            break;
        }
        x++;
    }
    while (1) {
        if (b == OP[y]) {
            break;
        }
        y++;
    }
    return Prior[x][y];
}
/*
* 函数作用:初始化集合
* 参数解释:无
* 强调:无
*/
void initCollection() {
    c.size = 0;
}
/*
* 函数作用:初始化结果集合
* 参数解释:无
* 强调:无
*/
void initResult(Result& R) {
    R.wordListIndex.clear();
    R.txtCollection.assign(10, false);
}
/*
* 函数作用:初始化临时结果集合
* 参数解释:无
* 强调:无
*/
void initTempResult(tempResult& tR,WordList w) {
    DocList temp = w.list;
    while (temp) {
        tR.txtCollection[temp->DocId] = true;
        temp = temp->next;
    }
}
/*
* 函数作用:创建集合时查找集合中是否存在word字段
* 参数解释:WordList的向量，使用引用提升速度
* 强调:无
*/
int FindWord(vector<WordList>& w, string word) {
    for (int i = 0; i < w.size(); i++) {
        if (w[i].word == word) {
            return i;
        }
    }
    return -1;
}
/*
* 函数作用:执行逻辑运算时查找集合中是否存在word字段
* 参数解释:WordList的向量，使用引用提升速度
* 强调:无
*/
int Binary_Search(vector<WordList>& w, string word) {
    int left = 0, right = w.size() - 1, mid = (left + right) / 2;
    while (left <= right) {
        mid = (left + right) / 2;
        if (w[mid].word == word) {
            return mid;
        }
        else if (w[mid].word > word) {
            right = mid - 1;
        }
        else if (w[mid].word < word) {
            left = mid + 1;
        }
    }
    return -1;
}
/*
* 函数作用:尾插法
* 参数解释:词汇索引表跟结点
* 强调:无
*/
void TailInsert(WordList& w, DocNode& d) {
    if (w.list == NULL) {
        w.list = &d;
        d.next = NULL;
    }
    else {
        DocList temp = w.list;
        while (temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = &d;
        d.next = NULL;
    }
}
/*
* 函数作用:读取文件
* 参数解释:文件名
* 强调:无
*/
void ReadFile(int fileIndex,string file) {
    ifstream in(file);
    string s;
    while (getline(in, s));
    for (int i = 0; i < (int)s.length(); i++) {
        if (s[i] >= 'a' && s[i] <= 'z') {
            asciiCount[s[i] - 'a']++;
        }
    }
    int pos = 0;
    for (int i = 0; i < (int)s.length();) {
        string word = "";
        while (s[i] != ' ') {
            word += s[i];
            i++;
        }
        buffer[fileIndex].word.push_back(word);
        int tempWordIndex = FindWord(c.collection, word);
        //如果集合不存在该word
        if (tempWordIndex == -1){
            vector<int>* dvector = new vector<int>;
            (*dvector).push_back(pos++);
            DocNode* d = new DocNode{ fileIndex, 1, *dvector, NULL };
            WordList w = { word,d,1 };
            c.collection.push_back(w);
            c.size++;
        }
        //如果集合存在该word，首先要查看文档Id是否在倒排索引中已经有记录
        //因为倒排索引很短，只需要顺序遍历即可
        else {
            bool ok = false;
            //当倒排索引列表不为空
            //查找是否已经有了该文档记录，如果有，则频次+1,添加位置信息
            DocList temp = c.collection[tempWordIndex].list;
            while (temp != NULL) {
                if (temp->DocId == fileIndex) {
                    temp->pos.push_back(pos++);
                    temp->P++;
                    ok = true;
                    break;
                }
                temp = temp->next;
            }
            //如果没有该记录，则插入倒排索引链表尾部
            if (!ok) {
                vector<int>* dvector = new vector<int>;
                (*dvector).push_back(pos++);
                DocNode* d = new DocNode{ fileIndex, 1, *dvector, NULL };
                TailInsert(c.collection[tempWordIndex],*d);
                c.collection[tempWordIndex].size++;
            }
        }
        i++;
    }
}
/*
* 函数作用:读取全部文件
* 参数解释:无
* 强调:无
*/
void ReadAll() {
    buffer = new Buffer[10];
    for (int i = 0; i < 10; i++) {
        ReadFile(i,Path + txtName[i] + ".txt");
    }
}
/*
* 函数作用:排序
* 参数解释:无
* 强调:无
*/
bool cmp(WordList w1, WordList w2) {
    if (w1.word < w2.word) {
        return 1;
    }
    else if (w1.word == w2.word) {
        if (w1.list->P >= w2.list->P) {
            return 1;
        }
        else {
            return 0;
        }
    }
    return 0;
}
/*
* 函数作用:实现与逻辑计算
* 参数解释:tR与tR2必须引用
* 强调:无
*/
void AND(tempResult& tR, tempResult& tR2) {
    if (tR.txtCollection.size() == 1) {
        tR = tR2;
        return;
    }
    else if (tR2.txtCollection.size() == 1) {
        return;
    }
    for (int i = 0; i < 10; i++) {
        if (tR.txtCollection[i] && tR2.txtCollection[i]) {
            tR.txtCollection[i] = true;
        }
        else {
            tR.txtCollection[i] = false;
        }
    }
}
/*
* 函数作用:实现或逻辑计算
* 参数解释:tR与tR2必须引用
* 强调:无
*/
void OR(tempResult& tR, tempResult& tR2) {
    if (tR.txtCollection.size() == 1) {
        tR = tR2;
        return;
    }
    else if (tR2.txtCollection.size() == 1) {
        return;
    }
    for (int i = 0; i < 10; i++) {
        if (tR.txtCollection[i] || tR2.txtCollection[i]) {
            tR.txtCollection[i] = true;
        }
        else {
            tR.txtCollection[i] = false;
        }
    }
}
/*
* 函数作用:展示整个数据库
* 参数解释:无
* 强调:无
*/
void showDatabase(Collection c) {
    cout << "集合中有" << c.size << "个单词" << endl;
    cout << "--------------------------------------------------------" << endl;
    for (int x = 0; x < c.collection.size(); x++) {
        cout << "当前存储的单词为" << c.collection[x].word << "     它有" << c.collection[x].size << "个关联文档" << endl;
        DocList temp = c.collection[x].list;
        for (int i = 0; i < c.collection[x].size; i++) {
            cout << "当前的文档id为" << temp->DocId << "   文档名称为" << txtName[temp->DocId];
            cout << "   其频率为" << temp->P << "   其在当前文档的位置为<";
            for (int y = 0; y < temp->pos.size() - 1; y++) {
                cout << temp->pos[y] << ',';
            }
            cout << temp->pos[temp->pos.size() - 1] << ">\n";
            temp = temp->next;
        }
        cout << "--------------------------------------------------------" << endl;
    }
}
/*
* 函数作用:导出整个数据库
* 参数解释:无
* 强调:无
*/
void OutputTxt(Collection c) {
    ofstream on("test.txt");
    on << "集合中有" << c.size << "个单词" << endl;
    on << "--------------------------------------------------------" << endl;
    for (int x = 0; x < c.collection.size(); x++) {
        on << "当前存储的单词为" << c.collection[x].word << "     它有" << c.collection[x].size << "个关联文档" << endl;
        DocList temp = c.collection[x].list;
        for (int i = 0; i < c.collection[x].size; i++) {
            on << "当前的文档id为" << temp->DocId << "   文档名称为" << txtName[temp->DocId];
            on << "   其频率为" << temp->P << "   其在当前文档的位置为<";
            for (int y = 0; y < temp->pos.size() - 1; y++) {
                on << temp->pos[y] << ',';
            }
            on << temp->pos[temp->pos.size() - 1] << ">\n";
            temp = temp->next;
        }
        on << "--------------------------------------------------------" << endl;
    }
}
/*
* 函数作用:计算该搜索字符的逻辑，并返回搜索结果
* 参数解释:用户输入
* 强调:无
*/
bool Search(Result& R) {
    string input;
    string op;
    int index;
    stack<tempResult> inputString;//操作文件栈
    stack<string> logicalOp;//操作符栈
    logicalOp.push("#");
    getline(cin, input, '\n');
    transform(input.begin(), input.end(), input.begin(), ::tolower);
    vector<string> word;
    for (int i = 0; i < (int)input.length(); i++) {
        string temp = "";
        if (input[i] == '(' || input[i] == ')') {
            temp += input[i];
            word.push_back(temp);
            i++;
            temp = "";
        }
        while (i < (int)input.length() && input[i] != ' ' && input[i] !=  ')') {
            temp += input[i];
            i++;
        }
        if (temp != "") {
            word.push_back(temp);
            if (input[i] == ')') {
                word.push_back(")");
            }
        }
    }
    word.push_back("\n");
    for (int i = 0; i < word.size();) {
        if (word[i] == "exit") {
            return true;
        }
        if (!In(word[i])) {
            index = Binary_Search(c.collection, word[i]);
            tempResult tR;
            if (index == -1) {//数据库中不存在该记录
                tR.txtCollection.assign(1,true);
            }
            else {
                R.wordListIndex.push_back(index);
                tR.txtCollection.assign(10, false);
                initTempResult(tR, c.collection[index]);
            }
            inputString.push(tR);
            i++;
        }
        else {
            string temp = word[i];
            if (word[i] == "and") {
                temp = "&";
            }
            else if (word[i] == "or") {
                temp = "|";
            }
            else if (word[i] == "\n") {
                temp = "#";
            }
            switch (Precede(logicalOp.top(), temp)) {
            case '<':
                logicalOp.push(temp);
                i++;
                break;
            case '>': {
                tempResult tR = inputString.top();
                inputString.pop();
                op = logicalOp.top();
                logicalOp.pop();
                if (op == "&") {
                    AND(inputString.top(), tR);
                }
                else {
                    OR(inputString.top(), tR);
                }
                break;
            }
            case '=':
                logicalOp.pop();
                i++;
            }
        }
    }
    //合并
    R.txtCollection = inputString.top().txtCollection;
    //输出部分
    bool hasInfo = false;
    for (int i = 0; i < R.wordListIndex.size(); i++) {
        WordList temp = c.collection[R.wordListIndex[i]];
        DocList tempDocList = temp.list;
        while (tempDocList) {
            if (R.txtCollection[tempDocList->DocId]) {
                hasInfo = true;
                for (int x = 0; x < tempDocList->pos.size(); x++) {
                    cout << "文档编号为" << tempDocList->DocId << ":";
                    if (tempDocList->pos[x] <= 3) {
                        for (int y = 0; y <= tempDocList->pos[x]; y++) {
                            cout << buffer[tempDocList->DocId].word[y] << ' ';
                        }
                        for (int y = tempDocList->pos[x]; y <= tempDocList->pos[x] + 4; y++) {
                            cout << buffer[tempDocList->DocId].word[y] << ' ';
                        }
                        cout << endl;
                    }
                    else if (tempDocList->pos[x] >= buffer[tempDocList->DocId].word.size() - 4) {
                        for (int y = tempDocList->pos[x] - 4; y < tempDocList->pos[x]; y++) {
                            cout << buffer[tempDocList->DocId].word[y] << ' ';
                        }
                        for (int y = tempDocList->pos[x]; y <= tempDocList->pos[x] - 1; y++) {
                            cout << buffer[tempDocList->DocId].word[y] << ' ';
                        }
                        cout << endl;
                    }
                    else {
                        for (int y = tempDocList->pos[x] - 4; y < tempDocList->pos[x]; y++) {
                            cout << buffer[tempDocList->DocId].word[y] << ' ';
                        }
                        for (int y = tempDocList->pos[x]; y <= tempDocList->pos[x] + 4; y++) {
                            cout << buffer[tempDocList->DocId].word[y] << ' ';
                        }
                        cout << endl;
                    }
                }
            }
            tempDocList = tempDocList->next;
        }
    }
    if (!hasInfo) {
        cout << "No Info!\n";
    }
    initResult(R);
    cout << "--------------------------------------------------------" << endl;
    cout << "Welcome to search world!\n";
    cout << "Type what you want to search\n";
    cout << "Type \"exit\" to quit\n";
    return false;
}
/*
* 函数作用:程序执行主方法
* 参数解释:无
* 强调:无
*/
void start() {
    initCollection();
    ReadAll();
    sort(c.collection.begin(), c.collection.end(), cmp);
    HuffmanCoding(HT,asciiCount,asciiMap);
    //系统运行
    string select;
    Result R;
    cout << "Welcome to OffLine Search\nPlease type what you want to search\n";
    while (1) {
        cout << "--------------------------------------------------------" << endl;
        cout << "type \"database\" to get databse\n";
        cout << "type \"HuffmanMap\" to get HuffmanCode\n";
        cout << "type \"distance\" to get HuffmanCode\n";
        cout << "type \"export\" to get database (txt form)\n";
        cout << "type \"search\" to start your search world!\n";
        getline(cin, select,'\n');
        transform(select.begin(), select.end(), select.begin(), ::tolower);
        system("cls");
        if (select == "database") {
            showDatabase(c);
        }
        else if (select == "huffmanmap") {
            showCodeMap();
        }
        else if (select == "distance") {
            calMinDis();
        }
        else if (select == "clear") {
            system("cls");
        }
        else if (select == "export") {
            OutputTxt(c);
        }
        else if (select == "search"){
            cout << "Welcome to search world!\n";
            cout << "Type what you want to search\n";
            cout << "Type \"exit\" to quit\n";
            initResult(R);
            while (!Search(R));
        }
        else {
            cout << "Please type correct order!\n";
            cout << "Wait 5 seconds and System will restart\n";
            Sleep(5000);
        }
    }
}
#endif
