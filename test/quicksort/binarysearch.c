#include<stdio.h>

#define N 5



int binarysearch(int * num,int count,int val)
{
	int low=0;
	int high=count-1;

	while(low<=high)
	{
		int mid=(low+high)/2;
		int midval=num[mid];
		if(val>midval)
			low=mid+1;
		else if(val<midval)
			high=mid-1;
		else
			return mid;
	}
	return -1;	
}

int main()
{
	int num[N]={1,2,4,5,6};
	int a=0;
	int b=0;
	printf("input num\n");
	scanf("%d",&a);

	b=binarysearch(num,N,a);
	printf("b=%d\n",b);
}
