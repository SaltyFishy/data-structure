#ifndef page.h
#include<string>
using namespace std;
/*
* ȫ������
* p:���ʾ���
* dis:�������
*/
double p[10][10] = {
	1.0000, 0.8530, 0.8636, 0.1771, 0.2546, 0.1262, 0.2162, 0.0488, 0.2061,0.1165,
	0.8530, 1.0000, 0.2819, 0.4287, 0.3330, 0.3598, 0.8237, 0.7063, 0.9565,0.4772,
	0.8636, 0.2819, 1.0000, 0.0429, 0.8887, 0.0297, 0.5160, 0.7141, 0.7820,0.9492,
	0.1771, 0.4287, 0.0429, 1.0000, 0.4003, 0.0611, 0.7951, 0.2877, 0.2106,0.5884,
	0.2546, 0.3330, 0.8887, 0.4003, 1.0000, 0.9987, 0.0640, 0.5897, 0.8763,0.6273,
	0.1262, 0.3598, 0.0297, 0.0611, 0.9987, 1.0000, 0.9034, 0.9996, 0.2579,0.2076,
	0.2162, 0.8237, 0.5160, 0.7951, 0.0640, 0.9034, 1.0000, 0.3385, 0.2499,0.6091,
	0.0488, 0.7063, 0.7141, 0.2877, 0.5897, 0.9996, 0.3385, 1.0000, 0.4253,0.8842,
	0.2061, 0.9565, 0.7820, 0.2106, 0.8763, 0.2579, 0.2499, 0.4253, 1.0000,0.0953,
	0.1165, 0.4772, 0.9492, 0.5884, 0.6273, 0.2076, 0.6091, 0.8842, 0.0953,1.0000 };
double** dis;
/*
* ��������:��ʼ��dis����
* ��������:��
* ǿ��:��
*/
void initMartix() {
	dis = new double* [10];
	for (int i = 0; i < 10; i++) {
		dis[i] = new double[10];
	}
	for (int x = 0; x < 10; x++) {
		for (int y = 0; y < 10; y++) {
			dis[x][y] = 1 / p[x][y];
		}
	}
}
/*
* ��������:�Ͻ�˹�����㷨�������ҳ֮��ľ���
* ��������:��
* ǿ��:��
*/
void Dijkstra(int start) {
	double* PathPower = new double[10];
	int* visited = new int[10];
	for (int i = 0; i < 10; i++) {
		visited[i] = 0;
		PathPower[i] = dis[start][i];
	}
	visited[start] = 1;
	PathPower[start] = 0;
	int k;
	double min;
	for (int i = 1; i < 10; i++) {
		min = 1000000.0;
		for (int j = 0; j < 10; j++) {
			if (!visited[j] && PathPower[j] < min) {
				min = PathPower[j];
				k = j;
			}
		}
		visited[k] = 1;
		for (int j = 0; j < 10; j++) {
			if (!visited[j] && (min + dis[k][j]) < PathPower[j]) {
				PathPower[j] = min + dis[k][j];
			}
		}
	}
	for (int x = 0; x < 10; x++) {
		cout << "��" + to_string(start + 1) + "��" + to_string(x + 1) + "����̾���Ϊ:" << PathPower[x] << endl;
	}
	delete[] PathPower;//�ǵ��ͷſռ�
	delete[] visited;//�ǵ��ͷſռ�
}
/*
* ��������:���������㷨��������ҳ֮��ľ���
* ��������:��
* ǿ��:��
*/
void Floyd() {
	double** D = new double*[10];
	for (int i = 0; i < 10; i++) {
		D[i] = new double[10];
	}
	for (int x = 0; x < 10; x++) {
		for (int y = 0; y < 10; y++) {
			D[x][y] = dis[x][y];
		}
	}
	for (int i = 0; i < 10; i++) {
		for (int j = 0; j < 10; j++) {
			for (int k = 0; k < 10; k++) {
				if (D[j][i] + D[i][k] < D[j][k]) {
					D[j][k] = D[j][i] + D[i][k];
				}
			}
		}
	}
	for (int x = 0; x < 10; x++) {
		for (int y = 0; y < 10; y++) {
			if (x != y) {
				cout << "��" + to_string(x + 1) + "��" + to_string(y + 1) + "����̾���Ϊ:" << D[x][y] << endl;
			}
			else {
				cout << "��" + to_string(x + 1) + "��" + to_string(y + 1) + "����̾���Ϊ:" << 0 << endl;
			}
		}
	}
	for (int i = 0; i < 10; i++) {
		delete[] D[i];
	}
	delete[] D;
}
/*
* ��������:��ʾ������ҳ֮�����
* ��������:��
* ǿ��:��
*/
void calMinDis() {
	initMartix();
	//Floyd();
	for (int i = 0; i < 10; i++) {//10�εϽ�˹�����㷨�󳤶�
		Dijkstra(i);
	}
	for (int i = 0; i < 10; i++) {
		delete[] dis[i];
	}
	delete[] dis;
}
#endif
