
#include<stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

struct valnportinfo
{
	char macaddr[8];
	unsigned int ipaddr;
	unsigned int  port;
}port[128];
int main(int argc,char **argv)
{
	int link=0;
	int fd=0;
	int ret=0;
	int i=0;
	fd=open("/dev/ip1810_link",O_RDONLY);
	if(fd<0)
	{
		printf("open error\n");
		return -1;
	}

	if(argc==1)
	{	
		ret=ioctl(fd,0x1,&link);
		if(ret<0)
		{
			printf("ioctl error\n");
			close(fd);
			return -1;
		}
		printf("link=%x\n",link);
	}

	if(argc==2)
	{
		(struct valnportinfo *) port;
		printf("jinmin\n");
		ret=ioctl(fd,0x11,port);
		if(ret<0)
		{
			printf("ioctl error\n");
			close(fd);
			return -1;
		}
		for(i=0;i<128;i++)
		{
			/*if(port[i].port!=0)*/
			printf("mac=%s,ip=%x,port=%d\n",(char *)(port[i].macaddr),port[i].ipaddr,port[i].port);
		}
		for(i=0;i<8;i++)
			printf("mac=%x\n",port[0].macaddr[i]);
	}
	close(fd);
	return 0;
}
