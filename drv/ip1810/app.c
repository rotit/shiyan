#include<stdio.h>
#include <fcntl.h>

#include <stdlib.h>
#include "mdio.h"
#define DEVICE "/dev/GPIO_MDIO"

struct page_reg_val buf;

int main(int argc,char *argv[])
{
	int fd=0;
	int ret=0;
	printf("argc=%d\n",argc);
	int i=0;
	fd=open(DEVICE,O_RDWR);	
	if(fd<0)
	{
		printf("open error\n");
	}

	buf.page=strtoul(argv[1],NULL,0);
	buf.reg=strtoul(argv[2],NULL,0);
	if(argc==3)
	{
		//for(i=0;i<10;i++)
		//{	
			ret=ioctl(fd,MDIO_READ,&buf);
			if(ret<0)
			{
				printf("ioctl,read error,ret=%d\n",ret);
			}
			printf("jinmin app, buf.val=%x\n",buf.val);
		//}	
	}
	else if(argc==4)
	{
		/*for(i=0;i<10;i++)*/
		/*{			*/
			buf.val=strtoul(argv[3],NULL,0);
			ret=ioctl(fd,MDIO_WRITE,&buf);
			if(ret<0)
			{
				printf("ioctl,write error,ret=%d\n",ret);
			}
			printf("jinmin app,write buf.val=%x\n",buf.val);
		/*}	*/
	}

		close(fd);

}
