#include<stdio.h>
#include <fcntl.h>
#include <unistd.h>
/*#include <stdlib.h>*/
#include <string.h>
#include "mdio.h"

#define DEVICE "/dev/GPIO_MDIO"


//ip1810 phy reset  gpio4_2 //rgmii0
#define IP1810_GPIO_RESET_MUX 		IO_ADDRESS(0xF8A21000+0x80)
#define IP1810_GPIO_RESET_BASE 		0xF8B24000
#define IP1810_GPIO_RESET_DIR		IO_ADDRESS(IP1810_GPIO_RESET_BASE+0x400)
#define IP1810_GPIO_RESET_SHIFT      2

#define IP1810_GPIO_RESET_REG 			IO_ADDRESS(0xF8B24000+(1<<(2+IP1810_GPIO_RESET_SHIFT)))


struct page_reg_val buf;
struct phy_reg_val phy;
unsigned  int  val=0;
struct cmd_val cmd_val;
int fd=0;

unsigned int me_mdio_write( char *bus, u8 page, u8 regaddr,u16 data)
{
	struct page_reg_val buf;
	int ret=0;
	buf.page=page;
	buf.reg=regaddr;
	buf.val=data;

	ret=ioctl(fd,MDIO_WRITE,&buf);
	if(ret<0)
	{
		printf("ioctl,write error,ret=%d\n",ret);
		close(fd);
		return -1;
	}
	/*printf("jinmin write-- page=%d reg=%x,val=%x\n",buf.page,buf.reg,buf.val);*/
	return 0;
}


unsigned int me_mdio_read(char *bus,u8 page, u16 regaddr)
{
	struct page_reg_val buf;
	int ret=0;
	buf.page=page;
	buf.reg=regaddr;

	ret=ioctl(fd,MDIO_READ,&buf);
	if(ret<0)
	{
		printf("ioctl,read error,ret=%d\n",ret);
		close(fd);
		return -1;
	}
	return buf.val;
	/*printf("jinmin app, buf.val=%x\n",buf.val);*/


}

int main(int argc,char *argv[])
{
	int ret=0;
	/*printf("argc=%d\n",argc);*/
	/*printf("jinmintestccccccccct\n");*/
	int i=0;
	int oc=0;char *arg;
	unsigned short data=0;
	int reg_fd=0;
	fd=open(DEVICE,O_RDWR);	
	if(fd<0)
	{
		printf("open error\n");
	}

	while((oc=getopt(argc,argv,":abuiqwep:r:"))!=-1)	
	{
		/*printf("optind=%d\n",optind);*/
		int i,j=0;
		switch (oc)
		{
			case 'p':
				phy.phy=strtoul(optarg,NULL,0);		
				break;

			case 'r':
				phy.reg=strtoul(optarg,NULL,0);
				break;

			case 'a':
				for(i=0;i<=0xc;i++)
					for(j=0;j<=0xff;j++)
					{
						buf.page=i;
						buf.reg=j;
						ret=ioctl(fd,MDIO_READ,&buf);
						if(ret<0)
						{
							printf("ioctl,read error,ret=%d\n",ret);
							close(fd);
						}
						printf("jinmin app,page=%x,reg=%x,val=%x\n",buf.page,j,buf.val);
					}
			case 'b':	

					printf("/****phy control reg read*******/\n");
				for(i=8;i<16;i++)
				{
						phy.phy=i;
						phy.reg=0;
						ret=ioctl(fd,MDIO_PHY_READ,&phy);
						if(ret<0)
						{
							close(fd);
							printf("maybe phy address error,should be 8~15");
							return -1;
						}
						printf("jinmin app,phy=%d,reg=%d,val=%x\n",phy.phy,phy.reg,phy.val);

				}
					printf("/****phy status reg read*******/\n");
				for(i=8;i<16;i++)
				{
						phy.phy=i;
						phy.reg=1;
						ret=ioctl(fd,MDIO_PHY_READ,&phy);
						if(ret<0)
						{
							close(fd);
							printf("maybe phy address error,should be 8~15");
							return -1;
						}
						printf("jinmin app,phy=%d,reg=%d,val=%x\n",phy.phy,phy.reg,phy.val);

				}
				//full poll
				//printf("/****phy reg read*******/\n");
				/*for(i=8;i<16;i++)
				{
					for(j=0;j<19;j++)
					{
						phy.phy=i;
						phy.reg=j;
						ret=ioctl(fd,MDIO_PHY_READ,&phy);
						if(ret<0)
						{
							close(fd);
							printf("maybe phy address error,should be 8~15");
							return -1;
						}
						printf("jinmin app,phy=%d,reg=%d,val=%x\n",phy.phy,phy.reg,phy.val);
					}
					printf("\n");
				}*/
				break;
			case 'q':
					me_mdio_write(NULL,0xc,0x00,0x1);
					printf("reset the switch of IP1810, all parameters are maintained\n");
				break;
			case 'w':
					me_mdio_write(NULL,0xc,0x00,0x10);
					printf("reset the switch of IP1810, all parameters are reset to default\n");
					break;
			case 'u':
					ret=ioctl(fd,SWITCH_RESET,NULL);
					if(ret<0)
					{
						printf("ioctl,switch reset error,ret=%d\n",ret);
						close(fd);
						return -1;
					}
					printf("jinmin, hard reset ok\n");
					break;
			case 'i':

				//soft mac reset
					me_mdio_write(NULL,0x0,0x5a,0x0);
					me_mdio_write(NULL,0x0,0x5b,0x0);
					me_mdio_write(NULL,0x0,0x5b,0x0);
					printf("jinmin mac reset ok\n");
					break;
			case 'e':
				/*for(i=0;i<5;i++)*/
				/*{*/
					/*val=me_mdio_read(NULL,0,0);*/
					/*printf("ip1810 read page=%x,reg=%x,val=%x\n",0,0,val);*/
				/*}*/

				me_mdio_write(NULL,0x3,0x12,0x13b);

				me_mdio_write(NULL,0xc,0xa,0x5b2);

#if 0
				me_mdio_write(NULL,0x0,0x41,0x2000);
				me_mdio_write(NULL,0xc,0x3,0x400b);
#endif

				//set vlan tag
#if 1
				//page 2 0x6,bit1,1:tag base vlan enable
				//page 2 0x6,bit3,1:based on vid

				data=me_mdio_read(NULL,2,0x6);
				data|= (1<<1);
				me_mdio_write(NULL,0x2,0x6,data);

				//led clock bit[7:6]
				/*
				 *00 : 781 KHz
				 *01 : 2.5 MHz
				 *10 : 5.2 MHz
				 *11 : 10.4 MHz
				 */
				data=me_mdio_read(NULL,3,0x17);
				data&= ~(1<<6 | 1<<7);
				/*data|=1<<6;*/
				me_mdio_write(NULL,0x3,0x17,data);

				//tag based vlan
				for(i=0;i<8;i++)
				{
					data=0;
					data=i+1;
					me_mdio_write(NULL,0x2,0x13+i,data);
				}

				data=0xa;
				me_mdio_write(NULL,0x2,0x2f,data);


				me_mdio_write(NULL,0x2,0xcd,0x1100);
				me_mdio_write(NULL,0x2,0xce,0xff);

				//set member tag
				for(i=1;i<=10;i++)
				{
					data=0;
					data=0x8000|i;
					me_mdio_write(NULL,0x2,0xd2,data);
					while(me_mdio_read(NULL,0x2,0xd2)&0x8000);

					if(i==10)
						me_mdio_write(NULL,0x2,0xd3,0xff);
					else
						me_mdio_write(NULL,0x2,0xd3,1<<(i-1));

					me_mdio_write(NULL,0x2,0xd4,0x1100);
					me_mdio_write(NULL,0x2,0xd8,0x8000);

					data = 0xC000 | i;
					me_mdio_write(NULL,0x2,0xd2,data);
					while(me_mdio_read(NULL,0x2,0xd2)&0x8000);
				}
#endif
					printf("jinmin,flush vlan ok\n");

				break;
			case '?':
				printf("error option\n");
				close(fd);
				return -1;

			case ':':
				printf("error args\n");
				close(fd);
				return -1;
		}

	}

	if(strcmp("-p",argv[1])==0&& argc==3)
	{
		printf("need reg\n");
		close(fd);
		return -1;
	}


	if(argc==5) //./app -p 0xphy -r 0xreg 
	{
		/*printf("1phy=%d,reg=%x,val=%x\n",phy.phy,phy.reg,phy.val);*/
		ret=ioctl(fd,MDIO_PHY_READ,&phy);
		if(ret<0)
		{
			close(fd);
			printf("maybe phy address error,should be 8~15");
			return -1;
		}
		printf("phy=%d,reg=%x,val=%x\n",phy.phy,phy.reg,phy.val);
	}
	
	if( argc==1)//||argc==2 )
	{
		/*cmd_val.cmd=strtoul(argv[1],NULL,0);*/

		if(argc==1)
		{	
			cmd_val.cmd=0;
			cmd_val.cmd|=1<<15|10<<5;//
			cmd_val.cmd|=~(1<<10);//rx
			cmd_val.cmd|=1<<10;//
			printf("cmd=%x\n",cmd_val.cmd);
		}
		else
		cmd_val.cmd=strtoul(argv[1],NULL,0);

		/*printf("cmd=0x%x\n",cmd_val.cmd);*/

		for(i=0;i<17;i++)
		{
			/*cmd_val.cmd&=i<<0;*/
			ret=ioctl(fd,MDIO_COUNT,&cmd_val);
			printf("jinmin app,address=%d, count=%d\n",i,cmd_val.val);
		}
		if(ret<0)
		{
			printf("ioctl,read error,ret=%d\n",ret);
			close(fd);
		}

	}
	else if(argc==3) //./app 0xpage 0xreg 
	{
		buf.page=strtoul(argv[1],NULL,0);
		buf.reg=strtoul(argv[2],NULL,0);
			ret=ioctl(fd,MDIO_READ,&buf);
			if(ret<0)
			{
				printf("ioctl,read error,ret=%d\n",ret);
				close(fd);
			}
			printf("jinmin app, buf.val=%x\n",buf.val);
	}
	else if(argc==4) //.app 0xpage 0xreg 0xval
	{
		/*for(i=0;i<10;i++)*/
		/*{			*/
			buf.page=strtoul(argv[1],NULL,0);
			buf.reg=strtoul(argv[2],NULL,0);
			buf.val=strtoul(argv[3],NULL,0);
			ret=ioctl(fd,MDIO_WRITE,&buf);
			if(ret<0)
			{
				printf("ioctl,write error,ret=%d\n",ret);
				close(fd);
			}
			printf("jinmin write-- page=%d reg=%x,val=%x\n",buf.page,buf.reg,buf.val);
		/*}	*/
	}


		close(fd);
		return 0;
		
}
