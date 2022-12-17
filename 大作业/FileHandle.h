#ifndef FileHandle.h
#include<fstream>
#include<iostream>
#include<vector>
#include<windows.h>
#include<stack>
#include<algorithm>
#include"page.h"
#include"code.h"
#define asciiMap 26//26����ĸ
using namespace std;
string Path = "..\\fruits\\ascii-fruits\\";//ȫ�ֱ���Path����ʾ����txt�ļ�·��
string txtName[10] = { "apple","cherry","grape","mangosteen","orange","peach","pear","pineapple","pomelo","watermelon" };//�ı���������
int asciiCount[asciiMap] = { 0 };//ͳ���ַ���Ŀ���ڱ���
HuffmanTree HT;//�շ�����
/*
* �ṹ��:��ĸ���ܼ���
* ǿ��:��
*/
typedef struct DocNode {//�ĵ�����
    int DocId;//�ĵ�ID
    int P;//Ƶ��
    vector<int> pos;//�������ĵ��е�λ��
    DocNode* next;//��һ���ĵ�
}*DocList;
typedef struct WordList {//��ĸ����
    string word;//��������
    DocList list;//�ĵ�����
    int size;//��һ��������������ĵ���Ŀ
};
typedef struct Collection {//���Ͽ��
    vector<WordList> collection;//ѡ��vector������ָ���ǿ��ǵ������ʱ��֪���ж��ٸ����ʣ��������
    int size;//�����ܸ���
};
Collection c;
/*
* �ṹ��:���ؽ������
* ǿ��:��
*/
typedef struct Result {
    vector<int> wordListIndex;//���ʵ�����
    vector<bool> txtCollection;//��ʾ10���ĵ��Ƿ���ڵĲ����߼�
};
/*
* �ṹ��:��ʱ�������
* ǿ��:��
*/
typedef struct tempResult {
    vector<bool> txtCollection;//��ʾ10���ĵ��Ƿ���ڵĲ����߼�
};
/*
* �ṹ��:�������е���
* ǿ��:��
*/
typedef struct Buffer {
    vector<string> word;//���ʱ���
};
Buffer* buffer;
/*
* �ַ�����:��������ȼ�
* ǿ��:��
*/
char Prior[5][5] = {
'>','>','<','>','>',
'>','>','<','>','>',
'<','<','<','=','>',
'>','>',' ','>','>',
'<','<','<','<','=',
};
/*
* �ַ�����:�����
* ǿ��:��
*/
string OP[5] = { "&","|","(",")","#" };
/*
* �������ж��Ƿ��ǲ�����
* ǿ��:��
*/
bool In(string s) {
    if (s == "(" || s == "and" || s == "or" || s == ")" || s == "\n") {
        return true;
    }
    return false;
}
/*
* ����:�Ƚ��ַ��������ȼ�
* ǿ��:��
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
* ��������:��ʼ������
* ��������:��
* ǿ��:��
*/
void initCollection() {
    c.size = 0;
}
/*
* ��������:��ʼ���������
* ��������:��
* ǿ��:��
*/
void initResult(Result& R) {
    R.wordListIndex.clear();
    R.txtCollection.assign(10, false);
}
/*
* ��������:��ʼ����ʱ�������
* ��������:��
* ǿ��:��
*/
void initTempResult(tempResult& tR,WordList w) {
    DocList temp = w.list;
    while (temp) {
        tR.txtCollection[temp->DocId] = true;
        temp = temp->next;
    }
}
/*
* ��������:��������ʱ���Ҽ������Ƿ����word�ֶ�
* ��������:WordList��������ʹ�����������ٶ�
* ǿ��:��
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
* ��������:ִ���߼�����ʱ���Ҽ������Ƿ����word�ֶ�
* ��������:WordList��������ʹ�����������ٶ�
* ǿ��:��
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
* ��������:β�巨
* ��������:�ʻ�����������
* ǿ��:��
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
* ��������:��ȡ�ļ�
* ��������:�ļ���
* ǿ��:��
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
        //������ϲ����ڸ�word
        if (tempWordIndex == -1){
            vector<int>* dvector = new vector<int>;
            (*dvector).push_back(pos++);
            DocNode* d = new DocNode{ fileIndex, 1, *dvector, NULL };
            WordList w = { word,d,1 };
            c.collection.push_back(w);
            c.size++;
        }
        //������ϴ��ڸ�word������Ҫ�鿴�ĵ�Id�Ƿ��ڵ����������Ѿ��м�¼
        //��Ϊ���������̣ܶ�ֻ��Ҫ˳���������
        else {
            bool ok = false;
            //�����������б�Ϊ��
            //�����Ƿ��Ѿ����˸��ĵ���¼������У���Ƶ��+1,���λ����Ϣ
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
            //���û�иü�¼������뵹����������β��
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
* ��������:��ȡȫ���ļ�
* ��������:��
* ǿ��:��
*/
void ReadAll() {
    buffer = new Buffer[10];
    for (int i = 0; i < 10; i++) {
        ReadFile(i,Path + txtName[i] + ".txt");
    }
}
/*
* ��������:����
* ��������:��
* ǿ��:��
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
* ��������:ʵ�����߼�����
* ��������:tR��tR2��������
* ǿ��:��
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
* ��������:ʵ�ֻ��߼�����
* ��������:tR��tR2��������
* ǿ��:��
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
* ��������:չʾ�������ݿ�
* ��������:��
* ǿ��:��
*/
void showDatabase(Collection c) {
    cout << "��������" << c.size << "������" << endl;
    cout << "--------------------------------------------------------" << endl;
    for (int x = 0; x < c.collection.size(); x++) {
        cout << "��ǰ�洢�ĵ���Ϊ" << c.collection[x].word << "     ����" << c.collection[x].size << "�������ĵ�" << endl;
        DocList temp = c.collection[x].list;
        for (int i = 0; i < c.collection[x].size; i++) {
            cout << "��ǰ���ĵ�idΪ" << temp->DocId << "   �ĵ�����Ϊ" << txtName[temp->DocId];
            cout << "   ��Ƶ��Ϊ" << temp->P << "   ���ڵ�ǰ�ĵ���λ��Ϊ<";
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
* ��������:�����������ݿ�
* ��������:��
* ǿ��:��
*/
void OutputTxt(Collection c) {
    ofstream on("test.txt");
    on << "��������" << c.size << "������" << endl;
    on << "--------------------------------------------------------" << endl;
    for (int x = 0; x < c.collection.size(); x++) {
        on << "��ǰ�洢�ĵ���Ϊ" << c.collection[x].word << "     ����" << c.collection[x].size << "�������ĵ�" << endl;
        DocList temp = c.collection[x].list;
        for (int i = 0; i < c.collection[x].size; i++) {
            on << "��ǰ���ĵ�idΪ" << temp->DocId << "   �ĵ�����Ϊ" << txtName[temp->DocId];
            on << "   ��Ƶ��Ϊ" << temp->P << "   ���ڵ�ǰ�ĵ���λ��Ϊ<";
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
* ��������:����������ַ����߼����������������
* ��������:�û�����
* ǿ��:��
*/
bool Search(Result& R) {
    string input;
    string op;
    int index;
    stack<tempResult> inputString;//�����ļ�ջ
    stack<string> logicalOp;//������ջ
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
            if (index == -1) {//���ݿ��в����ڸü�¼
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
    //�ϲ�
    R.txtCollection = inputString.top().txtCollection;
    //�������
    bool hasInfo = false;
    for (int i = 0; i < R.wordListIndex.size(); i++) {
        WordList temp = c.collection[R.wordListIndex[i]];
        DocList tempDocList = temp.list;
        while (tempDocList) {
            if (R.txtCollection[tempDocList->DocId]) {
                hasInfo = true;
                for (int x = 0; x < tempDocList->pos.size(); x++) {
                    cout << "�ĵ����Ϊ" << tempDocList->DocId << ":";
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
* ��������:����ִ��������
* ��������:��
* ǿ��:��
*/
void start() {
    initCollection();
    ReadAll();
    sort(c.collection.begin(), c.collection.end(), cmp);
    HuffmanCoding(HT,asciiCount,asciiMap);
    //ϵͳ����
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
