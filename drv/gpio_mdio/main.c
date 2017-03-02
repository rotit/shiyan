#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/miscdevice.h>
#include <linux/phy.h>
#include <linux/fs.h>  
#include "bus.h"
#include "smi.h"

#define prk(x,args...) printk("jinmin,kernel[%s,%d]--- " x,__FUNCTION__,__LINE__,##args)

#define u16 unsigned short


static int list[2]={0};
static int n_list;

struct mii_bus bus;
int phyaddr=0;

static long misc_ioctl (struct file *file, unsigned int cmd, unsigned long args)
{
	unsigned long int val=0;
	int ret=0;
	struct phy_reg_val buf;
	/*prk("in ioctl,cmd=%x\n",cmd);*/
	switch (cmd)
	{
		case PHY_READ:
			if( copy_from_user(&buf,(unsigned int __user *)args,sizeof(buf)) )
			{
				prk("cpy from user error\n");
			}
			val=IP1818_SMI_READ(buf.phy,buf.reg);
			prk("read phy=0x%lx,reg=0x%lx,val=0x%lx\n",buf.phy,buf.reg,val);
			buf.val=val;
			ret=copy_to_user((unsigned int __user *)args,&buf,sizeof(buf));
			if(ret)
			{
				prk("cpy to usr error\n");
			}

			return ret;
		case PHY_WRITE:
			if( copy_from_user(&buf,(unsigned int __user *)args,sizeof(buf)) )
			{
				prk("cpy user error\n");
			}
			IP1818_SMI_WRITE(buf.phy,buf.reg,buf.val);
			prk("write phy=0x%lx,reg=0x%lx,val=0x%lx\n",buf.phy,buf.reg,val);
			return 0;

		case MD_READ:
			if( copy_from_user(&buf,(unsigned int __user *)args,sizeof(buf)) )
			{
				prk("cpy user error\n");
			}
			val=stmmac_gpio_mdio_read(NULL,0,buf.reg);
			buf.val=val;
			prk("read ip1818:%lx,val=%lx\n",buf.reg,val);
			ret=copy_to_user((unsigned int __user *)args,&buf,sizeof(buf));
			if(ret)
			{
				prk("cpy to usr error\n");
			}

			return ret;
		case MD_WRITE:
			if( copy_from_user(&buf,(unsigned int __user *)args,sizeof(buf)) )
			{
				prk("cpy user error\n");
			}
				stmmac_gpio_mdio_write(NULL,0,buf.reg,buf.val);	
				prk("write ip1818: %lx,val=%lx\n",buf.reg,buf.val);
				return 0;
		default:
			prk("error ioctl mdio\n");
			return -1;
	}

}


/*ssize_t misc_read (struct file *, char __user *, size_t, loff_t *)*/
/*{*/

/*}*/
/*ssize_t misc_write (struct file *, const char __user *, size_t, loff_t *)*/
/*{*/

/*}*/


static int misc_open (struct inode *inode, struct file *file)
{
	return 0;
}

static struct file_operations misc_gpio_mdio={
	.owner=THIS_MODULE,
	.open=misc_open,
	/*.read=misc_read,*/
	/*.write=misc_write,*/
	.unlocked_ioctl=misc_ioctl,
	/*.compat_ioctl=misc_ioctl,*/
};

static struct miscdevice gpio_mdio={
	.minor = MISC_DYNAMIC_MINOR,
	.name ="GPIO_MDIO",
	.fops =&misc_gpio_mdio
};


static int __init demo_init(void)
{
	int ret=0,val=0;
	int i=0;
	ret=misc_register(&gpio_mdio);
	if(ret<0)
	{
		prk("misc registre error\n");
	}
	prk("mdio init ok\n");

	//for(i=0;i<16;i++)
	//{	
		//val=IP1818_SMI_READ(i,0x03);
		//prk("i=%d,val=%x\n",i,val);
	//}

	//prk("#####\n");
	//for(i=0;i<16;i++)
	//{	
		//val=IP1818_SMI_READ(i,0x04);
		//prk("i=%d,val=%x\n",i,val);
	//}
	//for(i=0;i<16;i++)
	//{	
		//val=IP1818_SMI_READ(i,0x01);
		//prk("i=%d,val=%x\n",i,val);
	//}

	//for(i=0;i<16;i++)
	//{	
		//val=IP1818_SMI_READ(i,0xfa);
		//prk("i=%d,val=%x\n",i,val);
	//}

	//if(list[0]!=0||list[1]!=0)
	//{
		//val=IP1818_SMI_READ(i,0x04);
		//prk("phy=%d,val=%x\n",list[0],list[1]);
	//}

		val=stmmac_gpio_mdio_read(NULL,0,0x03);
		prk("0x03, val=%x\n",val);
		val=stmmac_gpio_mdio_read(NULL,0,0x04);
		prk("0x04, val=%x\n",val);
		val=stmmac_gpio_mdio_read(NULL,0,0xfa);
		prk("0xfa, val=%x\n",val);


		/*val=stmmac_gpio_mdio_read(NULL,0,0xfa);*/
		/*prk("0xfa, val=%x\n",val);*/
		/*stmmac_gpio_mdio_write(NULL,0,0xfa,0x4122);*/
		/*val=stmmac_gpio_mdio_read(NULL,0,0xfa);*/
		/*prk("0xfa, val=%x\n",val);*/
		
	return 0;
}


static void __exit demo_exit(void)
{
	misc_deregister(&gpio_mdio);
	prk("exit\n");
}

module_param_array(list,int,&n_list,S_IRUGO|S_IWUSR);

module_init(demo_init);
module_exit(demo_exit);
MODULE_LICENSE("GPL");
