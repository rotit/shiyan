#include <stdio.h>
#include <fcntl.h>
#include "gpio_i2c.h"


int main(int argc,char **argv)
{
	int fd,val,ret;
	int buf[5]={0};
	fd=open("/dev/gpioi2c",O_RDWR);
	if(fd<0)
	{
		printf("open error\n");
	}

	if(argc==3)
	{
		buf[0]=strtoul(argv[1],NULL,0);
		buf[1]=strtoul(argv[2],NULL,0);
		printf("buf0=%x,buf1=%x\n",buf[0],buf[1]);
		
		ret=ioctl(fd,GPIO_I2C_READ,buf);
		if(ret<0)
		{
			printf("ioctl error\n");
			close(fd);
			return -1;
		}
		printf("val=%x\n",buf[0]);
		//printf("add=%x,reg=%x,val=%x\n",buf.port,buf.currentval,buf.voltageval);
	}

	return 0;

}
