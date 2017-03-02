
#include<stdio.h>
#include <fcntl.h>

#include <stdlib.h>
#define DEVICE "/dev/GPIO_MDIO"

#define PHY_READ 0x1
#define PHY_WRITE 0x2 
#define MD_READ 0x3
#define MD_WRITE 0x4 
struct phy_reg_val{
unsigned long	int phy;
unsigned long	int reg;
unsigned long	int val;
};
int main(int argc,char *argv[])
{
	int i=0;long tmp,tmp1;
	int ret=0,fd=0;
	long option=0;
	struct phy_reg_val buf;	
	fd=open(DEVICE,O_RDWR);	
	if(fd<0)
	{
		printf("open error\n");
	}
	
	option=strtoul(argv[1],NULL,0);
	if(option==0&& argc==4)//./app 0 phy reg ,read 
	{
		buf.phy=strtoul(argv[2],NULL,0);
		/*printf("argv[1]=%x\n",argv[1]);*/
		buf.reg=strtoul(argv[3],NULL,0);
		/*printf("argv[2]=%x\n",argv[2]);*/
		ret=ioctl(fd,PHY_READ,&buf);
		if(ret<0)
		{
			printf("ioctl,read error,ret=%d\n",ret);
		}
		printf("phy=0x%lx,val=0x%lx\n",buf.phy,buf.val);
	}
	if(option==0&& argc==5) //./app 0 phy reg val,write
	{
		buf.phy=strtoul(argv[2],NULL,0);
		buf.reg=strtoul(argv[3],NULL,0);
		buf.val=strtoul(argv[4],NULL,0);
		ret=ioctl(fd,PHY_WRITE,&buf);
		if(ret<0)
		{
			printf("ioctl,write error,ret=%d\n",ret);
		}
		
	}
	if(option==1&&argc==3)//./app 1 reg,ip1818 read
	{
		buf.reg=strtoul(argv[2],NULL,0);	
		ret=ioctl(fd,MD_READ,&buf);
		if(ret<0)
		{
			printf("ioctl,read error,ret=%d\n",ret);
		}

		printf("ioctl,ip1818 reg:%lx,val=0x%lx\n",buf.reg,buf.val);
	}
	if(option==1&&argc==4)//.app 1 reg,ip1818 write
	{
		buf.reg=strtoul(argv[2],NULL,0);
		buf.val=strtoul(argv[3],NULL,0);
		ret=ioctl(fd,MD_WRITE,&buf);
		if(ret<0)
		{
			printf("ioctl,write error,ret=%d\n",ret);
		}

			printf("ioctl,write ip1818: %lx,val=%lx\n",buf.reg,buf.val);
	}

	if(option==2&&argc==3)
	{
		tmp=strtoul(argv[2],NULL,0);
		for(i=0;i<16;i++)
		{
			buf.reg=2*i+tmp;
			ret=ioctl(fd,MD_READ,&buf);
			printf("ioctl,read ip1818 reg:%lx,val=0x%lx\n",buf.reg,buf.val);
		}
	}
	if(option==2&&argc==4)
	{
		tmp=strtoul(argv[2],NULL,0);
		for(i=0;i<16;i++)
		{
			buf.reg=2*i+tmp;
			buf.val=strtoul(argv[3],NULL,0);
			ret=ioctl(fd,MD_WRITE,&buf);
			printf("ioctl,write ip1818: %lx,val=%lx\n",buf.reg,buf.val);
		}
	}

	if(option==3&&argc==2)
	{
		for(i=0;i<=0xff;i++)
		{
			buf.reg=i;
			ret=ioctl(fd,MD_READ,&buf);
			printf("ioctl,read ip1818 reg:%lx,val=0x%lx\n",buf.reg,buf.val);
		}
	}
	close(fd);
	return ret;
}
