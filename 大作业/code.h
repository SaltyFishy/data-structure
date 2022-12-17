#ifndef code.h
#include<limits.h>
/*
* �ṹ��:�շ������ṹ
* ǿ��:��
*/
typedef struct HTNode {
	int weight;
	int parent, lchild, rchild;
}*HuffmanTree;
/*
* ȫ�ֱ���
*/
string* HuffmanCodeMap;//�շ�����������ĸ��
string alphabet = "abcdefghijklmnopqrstuvwxyz";//����ֻ���ַ�����
/*
* ��������:�շ�����������ѡ����������min1<=min2
* ��������:HT�Ǻշ�����,end�ǽ���λ��������s1Ϊ��һ������������s2Ϊ�ڶ�����������
* ǿ��:��һ������ʹ�������޸���ֵ
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
* ��������:�����շ�����
* ��������:HT�Ǻշ�������sizeΪ��Ҫ��������ַ��ĸ�����treeSizeΪ���Ĵ�С
* ǿ��:��һ������ʹ�������޸���ֵ
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
* ��������:�����շ�������
* ��������:HT�Ǻշ�������wΪȨֵ���飬sizeΪ��Ҫ��������ַ��ĸ���
* ǿ��:��һ������ʹ�������޸���ֵ
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
* ��������:ѹ���㷨��ʵ�ֶ��ַ��ĺշ�������
* ��������:strToBeCoded��Ҫ������Ϊ0-1���е��ַ����У�strCoded�����ձ���ɵ�0-1����
* ǿ��:��һ������ʹ�����ô���ֻ��Ϊ�������ٶȣ�������Ϊ���ǵ�Ҫ��������ַ������ܻ�ܳ�
* ǿ��:�ڶ�������ʹ�����ô�����Ϊ���޸���ֵ
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
* ��������:�շ�������
* ��������:HT�Ǻշ�������strToBeDeCoded��Ҫ����ѹ����0-1���У�message�ǽ�ѹ�����ַ�
* ǿ��:��һ��������ʹ�����ô���ֻ��Ϊ�������ٶ�
* ǿ��:����������ʹ��������Ϊ��Ϊ���޸���ֵ
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
* ��������:չʾ��Ӧ�ַ��ĺշ�������
* ��������:��
* ǿ��:��
*/
void showCodeMap() {
	for (int i = 1; i <= 26; i++) {
		cout << alphabet[i - 1] << " : " << HuffmanCodeMap[i] << endl;
	}
}
#endif
