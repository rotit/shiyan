#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#define RANDOM(i) (rand()%i)

#define N 50
#define me 1
int quicksort(int *num,int low,int high)
{
	int i=low;
	int j=high;
	int temp=num[i];
	if(i<j)
	{
		while(i<j)
		{
			while((num[j]>=temp) && (i< j))
				j--;
			num[i]=num[j];	
#if me
			num[j]=temp;
#endif
			while((num[i]<=temp) && (i< j))
				i++;
			num[j]=num[i];
#if me
			num[i]=temp;
#endif 
		}
#if !me
			num[i]=temp;
#endif
			quicksort(num,low,i-1);
			quicksort(num,j+1,high);
			return 0;
	}
	else
	return -1;
}


int quicksort2(int *num,int low,int high)
{
	if(low > high)
		return -1;

	int i=low;
	int j=high;
	int temp=num[i];

	while(i<j)
	{
		while( (i<j)&& num[j]>temp )
			j--;
		num[i]=num[j];
		num[j]=temp;
	
		while((num[i]<temp)&& (i<j))
			i++;
		num[i]=temp;
		num[j]=num[i];
	}
	quicksort2(num,low,i-1);
	quicksort2(num,j+1,high);
}





int main()
{
	int num[N];
	int i=0;
	srand((int)time(0));  //设置随机数种子
	for (i=0; i<N; i++)  //排序前
	{
		num[i] = RANDOM(100);
		printf("%d\t", num[i]);
	}
	quicksort2(num,0,N-1);
	for(i=0;i<N;i++)
		printf("num=%d\n",num[i]);
	
}
