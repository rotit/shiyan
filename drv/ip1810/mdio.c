#include <linux/module.h>
/*#include <linux/kernel.h>*/
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
	unsigned  int val=0;
	int ret=0;
	unsigned int count_cmd=0;
	struct cmd_val cmd_val;
	unsigned int l_val,h_val;
	struct page_reg_val buf;
	struct phy_reg_val phy_reg_val;
	unsigned int regvalue=0;
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
			
		case MDIO_COUNT:
			if(copy_from_user(&cmd_val,(unsigned int __user*)args,sizeof(cmd_val)))
			{
				prk("cpy from user error\n");
				return -1;
			}
			val=me_mdio_read(NULL,0x0,0x1);
			val|=1<<9;
			printk("val=0x%x\n",val);
			me_mdio_write(NULL,0x0,0x1,val);
			me_mdio_write(NULL,0x0,0x1,val);

			/*prk("jjjj--- cmd_val.cmd=%x\n",cmd_val.cmd);*/
			me_mdio_write(NULL,0x0,0x64,cmd_val.cmd);
			l_val=me_mdio_read(NULL,0x0,0x65);
			h_val=me_mdio_read(NULL,0X0,0X66);
			cmd_val.val=l_val|(h_val<<16);
			prk("jjjjj--- val=%d\n",cmd_val.val);			

			if(copy_to_user((unsigned int __user*)args,&cmd_val,sizeof(cmd_val)))
			{
				prk("cpy to user error\n");
				return -1;
			}
			return 0;
		case MDIO_PHY_READ:
			if(copy_from_user(&phy_reg_val,(unsigned int __user*)args,sizeof(phy_reg_val)))
			{
				prk("cpy from user error\n");
				return -1;
			}
			
			phy_reg_val.val=ip1829_phy_reg_read(phy_reg_val.phy,phy_reg_val.reg);
			if(phy_reg_val.val==-1)
			{
				return -1;
			}
			prk("phy=%d,reg=%x,val=%x\n",phy_reg_val.phy,phy_reg_val.reg,phy_reg_val.val);

			if(copy_to_user((struct __user phy_reg_val *)args,&phy_reg_val,sizeof(phy_reg_val)))
			{
				prk("cpy to user error\n");
				return -1;
			}
			return 0;
			
		case SWITCH_RESET:
#if 1	
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
			mdelay(100);

			regvalue = HW_REG(IP1810_GPIO_RESET_REG);
			regvalue &= ~(1<<2);
			HW_REG(IP1810_GPIO_RESET_REG)=regvalue;
			mdelay(200);


			regvalue = HW_REG(IP1810_GPIO_RESET_REG);
			regvalue |= 1<<2;	
			HW_REG(IP1810_GPIO_RESET_REG)=regvalue;
			mdelay(100);
#endif
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
	 //ip1810_reset();
	 prk("init 4");
	misc_register(&gpio_mdio);


#if 1
	//page 2 0x6,bit1,1:tag base vlan enable
	//page 2 0x6,bit3,1:based on vid

	int i=0;
	unsigned short data=0;
	data=me_mdio_read(NULL,2,0x6);
	data|= (1<<1 | 1<<3);
	me_mdio_write(NULL,0x2,0x6,data);

	//tag based vlan
	for(i=0;i<16;i++)
	{
		data=i+1;
		data|=1<<15;
		me_mdio_write(NULL,0x2,0x13+i,data);
	}

	//set member tag
	for(i=1;i<=8;i++)
	{
		/* read data from VLAN table */
		data=0x8000|i;
		me_mdio_write(NULL,0x2,0xd2,data);
		while(me_mdio_read(NULL,0x2,0xd2)&0x8000);

		data = i&0xFFFF;
		me_mdio_write(NULL,0x2,0xd3,data);

		/* set entry memeber */
		data=me_mdio_read(NULL,0x2,0xd4);
		data|=1<<1;
		me_mdio_write(NULL,0x2,0xd4,data);

		/* set valid bit to 1 */
		data=me_mdio_read(NULL,0x2,0xd8);
		data|=1<<15;
		me_mdio_write(NULL,0x2,0xd8,data);


		/* set entry addtag */
		data=me_mdio_read(NULL,0x2,0xd4);
		data|=7<<13;
		me_mdio_write(NULL,0x2,0xd4,data);

		data=me_mdio_read(NULL,0x2,0xd5);
		data|=0x1f;
		me_mdio_write(NULL,0x2,0xd5,data);


		/* Write data to VLAN table */
		data = 0xC000 | i;
		me_mdio_write(NULL,0x2,0xd2,data);
		while(me_mdio_read(NULL,0x2,0xd2)&0x8000);
	}

#endif
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
