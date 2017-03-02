#include<stdio.h>
int main()
{
	int a[3][4]={{11,12,13,14},{21,222,23,24},{31,32,33,34}};
	int b[3]={1,2,3};
	printf("address b=%x\n",b);
	printf("address b=%x\n",&b);
	printf("address b=%x\n",*b);//qu suzu diyige zhi

	/*printf("a[1][1]=%d\n",**a);*/
	/*printf("a[1][1]=%x\n",(a+1));*/
	/*printf("a[1][1]=%x\n",*(a+1));*/
	/*printf("address a=%x\n",a);*/
	/*printf("address a=%x\n",&a);*/
	/*printf("address a=%x\n",*a);*/
	/*printf("address a=%d\n",**a);*/
	/*printf("a[1][1]=%d\n",*(a+5));*/
	/*printf("a[1][1]=%d\n",*(a[1]+1));*/
	printf("a[1][1]=%d\n",*(a+1)[1]);
	printf("a[1][1]=%d\n",(*(a+1))[1]); //*(a+1) kancheng shi di er hang de suzu ,suzu[1] ze shi zhi
	printf("a[1][1]=%d\n",((**(a+1))+1)    );
	printf("a[1][1]=%d\n",(*(*(a+1)+1))    );
	printf("a[1][1]=%d\n", *(a)[1]   );// []youxianjigao, a suzu zhizheng zhixiang dierhang zuzhu,* qu suzhu diyige zhi
}
