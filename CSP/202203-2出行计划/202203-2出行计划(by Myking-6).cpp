/*
 * 编 写 者：162150107纪冠州
 * 编写时间：2022年10月9日 
*/ 
#include <iostream>
#include <algorithm>
using namespace std;
const int N = 200010;
int a[N];

int main()
{
	int n, m, k;							//计划数目n，查询个数m，等待核酸检测结果所需时间k。
	scanf("%d%d%d", &n, &m, &k);
	for (int i = 0; i < n; i++)				//构建差分数组
	{
		int t, c;
		scanf("%d%d", &t, &c);
		a[max(1, t - k - c + 1)]++;
		a[max(1, t - k + 1)]--;
	}
	for (int i = 1; i < N; i++)				//求前缀和
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
