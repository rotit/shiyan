#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#define RANDOM(i) (rand()%i)
#define N 100


int num[N];
int main()
{
	srand((unsigned)time(NULL));
	int i,j,temp=0;
	
	for(i=0;i<N;i++)
	{
		num[i]=RANDOM(N);
		printf("num=%d\t",num[i]);
	}

	printf("\n");
	for(i=0;i<N-1;i++)
	{
		for(j=0;j<N-1-i;j++)
		{
			if(num[j]>num[j+1])
			{
				temp=num[j];
				num[j]=num[j+1];
				num[j+1]=temp;
			}
		}
	}

	for(i=0;i<N;i++)
	{
		printf("num=%d\t",num[i]);
	}
}
