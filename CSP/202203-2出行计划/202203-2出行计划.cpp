/*
 * �� д �ߣ�162150107�͹���
 * ��дʱ�䣺2022��10��9�� 
*/ 
#include <iostream>
#include <algorithm>
using namespace std;
const int N = 200010;
int a[N];

int main()
{
	int n, m, k;							//�ƻ���Ŀn����ѯ����m���ȴ�������������ʱ��k��
	scanf("%d%d%d", &n, &m, &k);
	for (int i = 0; i < n; i++)				//�����������
	{
		int t, c;
		scanf("%d%d", &t, &c);
		a[max(1, t - k - c + 1)]++;
		a[max(1, t - k + 1)]--;
	}
	for (int i = 1; i < N; i++)				//��ǰ׺��
	{
		a[i] += a[i - 1];
	}
	int* ans = new int[m];
	for (int i = 0; i < m; i++)
	{
		int q;
		scanf("%d", &q);
		ans[i] = a[q];
	}
	for(int i=0;i<m;i++) printf("%d\n", ans[i]);
	return 0;
}
