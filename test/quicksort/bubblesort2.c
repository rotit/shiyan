
#include<stdio.h>
#include<stdlib.h>

#define N 100
int num[N]={0};

int bubblesort(int * num,int count)
{
	int i=0;
	int j=0;
	int temp=0;
	for(i=0;i<N;i++)
	{
		for(j=0;j<N-1-i;j++)
			if(num[j]<num[j+1])
			{
				temp=num[j];
				num[j]=num[j+1];
				num[j+1]=temp;

			}
	}
}
int main()
{
		srand((unsigned int)time(NULL));
		for(int i=0;i<N;i++)
		{
			num[i]=rand()%100;
		}

		/*for(int i=0;i<N;i++)*/
		/*{*/
			/*printf("num[%d]=%-7d \t",i,num[i]);*/
		/*}*/

		
		bubblesort(num,N);

		for(int i=0;i<N;i++)
		{
			printf("num[%d]=%-10d \t",i,num[i]);
		}
}
