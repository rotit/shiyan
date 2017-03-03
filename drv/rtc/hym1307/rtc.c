#include <linux/module.h>
#include <linux/errno.h>
#include <linux/miscdevice.h>
#include <linux/fcntl.h>

#include <linux/init.h>
#include <linux/delay.h>
#include <linux/proc_fs.h>
#include <linux/workqueue.h>
#include <linux/irq.h>
#include <linux/interrupt.h>
#include <asm/uaccess.h>
//#include <asm/system.h>
#include <asm/io.h>
#include <linux/ioctl.h>

unsigned char gpio_i2c_rtc_read(unsigned char devaddress, unsigned char address);
void gpio_i2c_rtc_write(unsigned char devaddress, unsigned char address, unsigned char value);

typedef struct 
{
        unsigned int  year;
        unsigned int  month;
        unsigned int  date;
        unsigned int  hour;
        unsigned int  minute;
        unsigned int  second;
        unsigned int  weekday;
} rtc_time_t;

typedef	struct 
{
	unsigned char reg;
	unsigned char val;
} reg_data_t;

typedef enum temp_sel_mode 
{
	TEMP_SEL_FIXMODE  = 0,
	TEMP_SEL_OUTSIDE,
	TEMP_SEL_INTERNAL,
} temp_sel_mode;

typedef struct 
{
	temp_sel_mode mode;
	int value;
} reg_temp_mode_t; 

typedef struct 
{
	unsigned int freq_l;
} rtc_freq_t;

#define HI_RTC_AIE_ON		_IO('p', 0x01)
#define HI_RTC_AIE_OFF		_IO('p', 0x02)

#define HI_RTC_COMP_ON		_IO('p', 0x03)
#define HI_RTC_COMP_OFF		_IO('p', 0x04)

#define HI_RTC_GET_FREQ		_IOR('P', 0x05, rtc_freq_t)
#define HI_RTC_SET_FREQ		_IOW('p', 0x06, rtc_freq_t)

#define HI_RTC_ALM_SET		_IOW('p', 0x07,  rtc_time_t)
#define HI_RTC_ALM_READ		_IOR('p', 0x08,  rtc_time_t)
#define HI_RTC_RD_TIME		_IOR('p', 0x09,  rtc_time_t)
#define HI_RTC_SET_TIME		_IOW('p', 0x0a,  rtc_time_t)
#define HI_RTC_RESET		_IOW('p', 0x0b,  rtc_time_t)
#define HI_RTC_REG_SET		_IOW('p', 0x0c,  reg_data_t)
#define HI_RTC_REG_READ		_IOR('p', 0x0d,  reg_data_t)
#define HI_RTC_SET_TEMP_MODE	_IOW('p', 0x0e,  reg_temp_mode_t)

#define RTC_REG_SECOND	0x00
#define RTC_REG_MINUTE	0x01
#define RTC_REG_HOUR	0x02
#define RTC_REG_WEEK	0x03
#define RTC_REG_DAY		0x04
#define RTC_REG_MON		0x05
#define RTC_REG_YEAR	0x06

#define RTC_DEV "hi_rtc"

//[0-99] 有效
//4bits 来表示一位10进制数
#define	BCD(a)		((((a) / 10) << 4) | ((a) % 10))
//用4bits 来转换成一位10进制数
#define	IBCD(a)		(((((a) >> 4) & 0x0F) * 10) + ((a) & 0x0F))

long rtc_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	unsigned char addr= 0xD0;
	unsigned char reg, data;
    rtc_time_t rtc_time;
	int pm = 0;
	int ret = 0;
	memset(&rtc_time,0,sizeof(rtc_time));
	printk("jinmin-- rtc,ioctl,cmd=%d\n",_IOC_NR(cmd));
	switch(cmd)
	{
	case HI_RTC_RD_TIME:
		reg = RTC_REG_SECOND;
		data = gpio_i2c_rtc_read(addr, reg);
		if (data & (1 << 7))//bit[7]==0 for enable RTC, ==1 disable RTC
		{
			data = data&0x7F;//bit[7]==0 for enable RTC		
			gpio_i2c_rtc_write(addr, reg, data);
		}
		rtc_time.second = IBCD(data&0x7F);		

		reg = RTC_REG_MINUTE;
		data = gpio_i2c_rtc_read(addr, reg);
		rtc_time.minute = IBCD(data&0x7F);		
		
		reg = RTC_REG_HOUR;
		data = gpio_i2c_rtc_read(addr, reg);
		if (data & (1 << 7))//bit[7]==1 is 12 mode
		{
			if (data & (1 << 5))//bit[5]==1 is pm
			{
				pm = 1;
			}
			
			rtc_time.hour = IBCD(data&0x1F);
			if (pm)
			{
				rtc_time.hour +=12;
			}
			
		}
		else
		{
			rtc_time.hour = IBCD(data&0x3F);

		}
		
		reg = RTC_REG_DAY;
		data = gpio_i2c_rtc_read(addr, reg);
		rtc_time.date = IBCD(data&0x3F);	
		
		reg = RTC_REG_MON;
		data = gpio_i2c_rtc_read(addr, reg);
		rtc_time.month = IBCD(data&0x1F);	
		
		reg = RTC_REG_YEAR;
		data = gpio_i2c_rtc_read(addr, reg);
		rtc_time.year = IBCD(data&0xFF);	
		rtc_time.year += 2000;
		
		reg = RTC_REG_WEEK;
		data = gpio_i2c_rtc_read(addr, reg);
		rtc_time.weekday = IBCD(data&0x0F);	
		
		printk("read y-m-d:%d-%d-%d, h:m:s %d:%d:%d, week:%d\n", rtc_time.year, rtc_time.month, rtc_time.date, rtc_time.hour, rtc_time.minute, rtc_time.second, rtc_time.weekday);
				
		if (copy_to_user((rtc_time_t __user *)arg, &rtc_time, sizeof(rtc_time)))
		{
			printk("REG_READ_CMD read  copy to user error!\n");
			return -1;
		}
		break;
		
	case HI_RTC_SET_TIME://默认按照24小时制
		if (copy_from_user(&rtc_time, (rtc_time_t __user *)arg, sizeof(rtc_time)))
		{
			printk("AS_WR error!\n");
			return -1;
		}
		
		printk("set y-m-d:%d-%d-%d, h:m:s %d:%d:%d, week:%d\n", rtc_time.year, rtc_time.month, rtc_time.date, rtc_time.hour, rtc_time.minute, rtc_time.second, rtc_time.weekday);
		reg = RTC_REG_SECOND;
		data = BCD((rtc_time.second%60))&0x7F;//bit[7]==0 for enable RTC		
		gpio_i2c_rtc_write(addr, reg, data);

		reg = RTC_REG_MINUTE;
		data = BCD((rtc_time.minute%60))&0x7F;	
		printk("jinmin-- write minute=%d\n",data);
		gpio_i2c_rtc_write(addr, reg, data);
		
		reg = RTC_REG_MINUTE;
		data = gpio_i2c_rtc_read(addr, reg);
		printk("jinmin-- read minute=%d\n",data);

		reg = RTC_REG_HOUR;
		data = BCD((rtc_time.hour%24))&0x3F;	//bit[7]==0 for enable 24 mode
		gpio_i2c_rtc_write(addr, reg, data);

		reg = RTC_REG_DAY;
		data = BCD((rtc_time.date))&0x3F;	
		gpio_i2c_rtc_write(addr, reg, data);
		
		reg = RTC_REG_MON;
		data = BCD((rtc_time.month))&0x1F;	
		gpio_i2c_rtc_write(addr, reg, data);

		reg = RTC_REG_YEAR;
		data = BCD((rtc_time.year%100))&0xFF;//max 99year	
		gpio_i2c_rtc_write(addr, reg, data);
		
		reg = RTC_REG_WEEK;
		data = BCD((rtc_time.weekday))&0x0F;//[1,7]	
		gpio_i2c_rtc_write(addr, reg, data);
		break;
			
	default:
		printk("%s() not support this cmd.\n", __FUNCTION__);
		return -1;
	}
	
    return ret;
}

int rtc_open(struct inode * inode, struct file * file)
{
    return 0;
}
int rtc_close(struct inode * inode, struct file * file)
{
    return 0;
}

static struct file_operations rtc_fops = 
{
    .owner      = THIS_MODULE,
    .unlocked_ioctl = rtc_ioctl,
    .open       = rtc_open,
    .release    = rtc_close
};

static struct miscdevice rtc_dev = 
{
   .minor		= 255,
   .name		= RTC_DEV,
   .fops  = &rtc_fops,
};

static int __init rtc_init(void)
{
    int ret;
    
    ret = misc_register(&rtc_dev);
    if(0 != ret)
    {
    	printk("misc_register error \n");
    	return -1;
    }
	
	printk("rtc init is OK!\n");

    return 0;    
}

static void __exit rtc_exit(void)
{
    misc_deregister(&rtc_dev);
	
	printk("rtc exit ok!\n");
}

module_init(rtc_init);
module_exit(rtc_exit);

MODULE_LICENSE("GPL");


