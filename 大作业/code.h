#ifndef code.h
#include<limits.h>
/*
* 结构体:赫夫曼树结构
* 强调:无
*/
typedef struct HTNode {
	int weight;
	int parent, lchild, rchild;
}*HuffmanTree;
/*
* 全局变量
*/
string* HuffmanCodeMap;//赫夫曼树各个字母表
string alphabet = "abcdefghijklmnopqrstuvwxyz";//设置只对字符编码
/*
* 函数作用:赫夫曼树构建的选择函数，满足min1<=min2
* 参数解释:HT是赫夫曼树,end是结束位置索引，s1为第一个结点的索引，s2为第二个结点的索引
* 强调:第一个参数使用引用修改其值
*/
void Select(HuffmanTree& HT,int end,int& s1,int& s2) {//
	int min1 = INT_MAX, min2 = INT_MAX;
	for (int x = 1; x <= end; x++) {
		if (min1 > HT[x].weight && HT[x].parent == 0) {
			min2 = min1;
			s2 = s1;
			min1 = HT[x].weight;
			s1 = x;
		}
		else if (min2 > HT[x].weight && HT[x].parent == 0) {
			min2 = HT[x].weight;
			s2 = x;
		}
	}
}
/*
* 函数作用:构建赫夫曼树
* 参数解释:HT是赫夫曼树，size为需要被编码的字符的个数，treeSize为树的大小
* 强调:第一个参数使用引用修改其值
*/
void CreateHuffmanTree(HuffmanTree& HT, int size,int treeSize) {
	for (int x = size + 1; x <= treeSize; x++) {
		int s1, s2;
		Select(HT,x - 1,s1,s2);
		HT[s1].parent = HT[s2].parent = x;
		HT[x].lchild = s1;
		HT[x].rchild = s2;
		HT[x].weight = HT[s1].weight + HT[s2].weight;
	}
}
/*
* 函数作用:构建赫夫曼编码
* 参数解释:HT是赫夫曼树，w为权值数组，size为需要被编码的字符的个数
* 强调:第一个参数使用引用修改其值
*/
void HuffmanCoding(HuffmanTree& HT,int* w,int size) {
	if (size <= 1) {
		return;
	}
	int treeSize = 2 * size - 1;
	HT = new HTNode[treeSize + 1];
	for (int i = 1; i <= size; i++) {
		HT[i] = { w[i - 1],0,0,0 };
	}
	for (int i = size + 1; i <= treeSize; i++) {
		HT[i] = { 0,0,0,0 };
	}
	CreateHuffmanTree(HT,size,treeSize);
	HuffmanCodeMap = new string[size + 1];
	for (int i = 1; i <= size; i++) {
		int c, parent;
		string ElementCode = "";
		for (c = i, parent = HT[i].parent; parent != 0; c = parent, parent = HT[parent].parent) {
			if (HT[parent].lchild == c) {
				ElementCode = "0" + ElementCode;
			}
			else {
				ElementCode = "1" + ElementCode;
			}
		}
		HuffmanCodeMap[i] = ElementCode;
	}
}
/*
* 函数作用:压缩算法，实现对字符的赫夫曼编码
* 参数解释:strToBeCoded是要被编码为0-1序列的字符序列，strCoded是最终编码成的0-1序列
* 强调:第一个参数使用引用传递只是为了提升速度，这是因为考虑到要被编码的字符串可能会很长
* 强调:第二个参数使用引用传递是为了修改其值
*/
void toBeCoded(string& strToBeCoded,string& strCoded) {
	strCoded = "";
	for (int i = 0; i < (int)strToBeCoded.length(); i++) {
		for (int x = 0; x < (int)alphabet.length(); x++) {
			if (strToBeCoded[i] == alphabet[x]) {
				strCoded += HuffmanCodeMap[x + 1];
				break;
			}
		}
	}
}
/*
* 函数作用:赫夫曼解码
* 参数解释:HT是赫夫曼树，strToBeDeCoded是要被解压缩的0-1序列，message是解压缩的字符
* 强调:第一二个参数使用引用传递只是为了提升速度
* 强调:第三个参数使用引用因为是为了修改其值
*/
bool HuffmanDeCoding(HuffmanTree HT, string strToBeDeCoded,string& message,int size) {
	message = "";
	int index = 0;
	int HuffmanRoot = 2 * size - 1;
	int root = HuffmanRoot;
	string val = "";
	for (; index <= (int)strToBeDeCoded.length();) {
		if (index == (int)strToBeDeCoded.length()) {
			bool deCode = false;
			for (int x = 1; x <= size; x++) {
				if (val == HuffmanCodeMap[x]) {
					message += alphabet[x - 1];
					deCode = true;
					val = "";
					break;
				}
			}
			if (!deCode) {
				return false;
			}
			index++;
		}
		else {
			if (strToBeDeCoded[index] == '0' && HT[root].lchild != 0) {
				val += strToBeDeCoded[index];
				index++;
				root = HT[root].lchild;
			}
			else if (strToBeDeCoded[index] == '1' && HT[root].rchild != 0) {
				val += strToBeDeCoded[index];
				index++;
				root = HT[root].rchild;
			}
			else {
				bool deCode = false;
				for (int x = 1; x <= size; x++) {
					if (val == HuffmanCodeMap[x]) {
						message += alphabet[x - 1];
						deCode = true;
						val = "";
						break;
					}
				}
				if (!deCode) {
					return false;
				}
				root = HuffmanRoot;
			}
		}
	}
	return true;
}
/*
* 函数作用:展示对应字符的赫夫曼编码
* 参数解释:无
* 强调:无
*/
void showCodeMap() {
	for (int i = 1; i <= 26; i++) {
		cout << alphabet[i - 1] << " : " << HuffmanCodeMap[i] << endl;
	}
}
#endif
