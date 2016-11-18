#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/delay.h>
#include <mach/io.h>
#include "mdio.h"
#include "smi.h"


#define u16 unsigned short


static void ip1810_reset(void)
{	
	u32 regvalue=0;
	u16 data=0;
	regvalue = HW_REG(IP1810_GPIO_RESET_MUX);
	regvalue &= ~(7<<0);
	HW_REG(IP1810_GPIO_RESET_MUX) = regvalue;

//reset
	regvalue = HW_REG(IP1810_GPIO_RESET_DIR);
	regvalue |= 1<<2;
	HW_REG(IP1810_GPIO_RESET_DIR)=regvalue;

//hlh
	
	regvalue = HW_REG(IP1810_GPIO_RESET_REG);
	regvalue |= 1<<2;
	HW_REG(IP1810_GPIO_RESET_REG)=regvalue;
	udelay(400);

	regvalue = HW_REG(IP1810_GPIO_RESET_REG);
	regvalue &= ~(1<<2);
	HW_REG(IP1810_GPIO_RESET_REG)=regvalue;
	udelay(500);

	
	regvalue = HW_REG(IP1810_GPIO_RESET_REG);
	regvalue |= 1<<2;	
	HW_REG(IP1810_GPIO_RESET_REG)=regvalue;
	udelay(500);
}



static int misc_open (struct inode *inode, struct file *file)
{

	return 0;
}

static long misc_ioctl (struct file *file, unsigned int cmd, unsigned long args)
{
	unsigned long int val=0;
	int ret=0;
	struct page_reg_val buf;
	/*prk("in ioctl,cmd=%x\n",cmd);*/
	switch (cmd)
	{
		case MDIO_READ:
			if(copy_from_user(&buf,(unsigned int __user*)args,sizeof(buf)))
			{
				prk("cpy from user error\n");
			}

			val=me_mdio_read(NULL,buf.page,buf.reg);
			buf.val=val;
			prk("read page=%x,reg=%x,val=%x\n",buf.page,buf.reg,buf.val);
			/*val=stmmac_gpio_mdio_read(NULL,0x0,0x0);*/
			/*prk("read page=%x,reg=%x,val=%x\n",buf.page,buf.reg,val);*/
			if(copy_to_user((unsigned int __user*)args,&buf,sizeof(buf)))
			{
				prk("cpy to user error\n");
				return -1;
			}
			return 0;

		case MDIO_WRITE:
			prk("xxxxxxxxxxxxxxxxxxxxxxxxxx\n");
			if(copy_from_user(&buf,(unsigned int __user*)args,sizeof(buf)))
			{
				prk("cpy from user error\n");
				return -1;
			}
			prk("write,page=%x,reg=%x,val=%x\n",buf.page,buf.reg,buf.val);
			me_mdio_write(NULL,buf.page,buf.reg,buf.val);	
			val=me_mdio_read(NULL,buf.page,buf.reg);
			buf.val=val;
			prk("write,read page=%x,reg=%x,val=%x\n",buf.page,buf.reg,buf.val);
			return 0;
		default:
			prk("error ioctl mdio\n");
			return -1;
	}

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
	 ip1810_reset();
	 prk("init 4");
	misc_register(&gpio_mdio);
	return 0;
}


static void __exit demo_exit(void)
{
	misc_deregister(&gpio_mdio);
	prk("exit\n");
}


module_init(demo_init);
module_exit(demo_exit);
MODULE_LICENSE("GPL");
