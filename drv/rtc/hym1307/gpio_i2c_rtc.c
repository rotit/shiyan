#include <linux/module.h>
#include <linux/errno.h>
#include <linux/miscdevice.h>
#include <linux/fcntl.h>

#include <linux/init.h>
#include <linux/delay.h>
#include <linux/proc_fs.h>
#include <linux/workqueue.h>

#include <asm/uaccess.h>
//#include <asm/system.h>
#include <mach/io.h>
/*#include <arch/arm/mach-hi3798cv2x/include/mach/io.h>*/
/*#include <mach-hi3798cv2x/include/mach/io.h>*/

#include "gpio_i2c_rtc.h" 
spinlock_t  gpioi2c_rtc_lock;

/*
change log :
1. change the base address 
2. change time_delay_us dly amplify 155/25

hi3531 skt :  
I2C_SCL  -- GPIO12_5 
I2C_SDA  -- GPIO12_4
GPIO12 base addr : 0x20210000

hi398C 
HDMI I2C_SCL  -- GPIO7_3	//0xF8A21000+0x0C4
HDMI I2C_SDA  -- GPIO7_2   //0xF8A2_1000+0x0C0

		GPIO7 0xF8B2_7000
*/

//I2C_CPU_RTC_SCLK		IN	GPIO5_3
//I2C_CPU_RTC_SDA		BI	GPIO5_4


#define GPIO_SCL_BASE 			0xF8004000
#define GPIO_SDA_BASE 			0xF8004000

#define SCL_SHIFT_NUM   			3
#define SDA_SHIFT_NUM   			4
#define SCL                 				(0x1 << SCL_SHIFT_NUM)   
#define SDA                 				(0x1 << SDA_SHIFT_NUM)    

#define GPIO_I2C_SCL_REG    		IO_ADDRESS(GPIO_SCL_BASE + (0x1<<(SCL_SHIFT_NUM+2)))  
#define GPIO_I2C_SDA_REG    		IO_ADDRESS(GPIO_SDA_BASE + (0x1<<(SDA_SHIFT_NUM+2)))  

#define GPIO_SCL_DIR 			IO_ADDRESS( GPIO_SCL_BASE + 0x400)
#define GPIO_SDA_DIR 			IO_ADDRESS( GPIO_SDA_BASE + 0x400)


#define HW_REG(reg)         			*((volatile unsigned int *)(reg))
#define DELAY(us)           			time_delay_us(us)

/* 
 * I2C by GPIO simulated  clear 0 routine.
 *
 * @param whichline: GPIO control line
 *
 */
static void i2c_clr(unsigned char whichline)
{
	unsigned char regvalue;
	
	if(whichline == SCL)
	{
		regvalue = HW_REG(GPIO_SCL_DIR);
		regvalue |= SCL;
		HW_REG(GPIO_SCL_DIR) = regvalue;
		
		HW_REG(GPIO_I2C_SCL_REG) = 0;
		return;
	}
	else if(whichline == SDA)
	{
		regvalue = HW_REG(GPIO_SDA_DIR);
		regvalue |= SDA;
		HW_REG(GPIO_SDA_DIR) = regvalue;
		HW_REG(GPIO_I2C_SDA_REG) = 0;
		return;
	}
	else if(whichline == (SDA|SCL))
	{
		regvalue = HW_REG(GPIO_SCL_DIR);
		regvalue |= SCL;
		HW_REG(GPIO_SCL_DIR) = regvalue;		
		HW_REG(GPIO_I2C_SCL_REG) = 0;
		
		regvalue = HW_REG(GPIO_SDA_DIR);
		regvalue |= SDA;
		HW_REG(GPIO_SDA_DIR) = regvalue;		
		HW_REG(GPIO_I2C_SDA_REG) = 0;
		return;
	}
	else
	{
		printk("Error input.\n");
		return;
	}
	
}

/* 
 * I2C by GPIO simulated  set 1 routine.
 *
 * @param whichline: GPIO control line
 *
 */
static void  i2c_set(unsigned char whichline)
{
	unsigned char regvalue;
	
	if(whichline == SCL)
	{
		regvalue = HW_REG(GPIO_SCL_DIR);
		regvalue |= SCL;
		HW_REG(GPIO_SCL_DIR) = regvalue;
		
		HW_REG(GPIO_I2C_SCL_REG) = SCL;
		return;
	}
	else if(whichline == SDA)
	{
		regvalue = HW_REG(GPIO_SDA_DIR);
		regvalue |= SDA;
		HW_REG(GPIO_SDA_DIR) = regvalue;
		
		HW_REG(GPIO_I2C_SDA_REG) = SDA;
		return;
	}
	else if(whichline == (SDA|SCL))
	{
		regvalue = HW_REG(GPIO_SCL_DIR);
		regvalue |= SCL;
		HW_REG(GPIO_SCL_DIR) = regvalue;
		
		HW_REG(GPIO_I2C_SCL_REG) = SCL;
		
		regvalue = HW_REG(GPIO_SDA_DIR);
		regvalue |= SDA;
		HW_REG(GPIO_SDA_DIR) = regvalue;
		
		HW_REG(GPIO_I2C_SDA_REG) = SDA;
		return;
	}
	else
	{
		printk("Error input.\n");
		return;
	}
}

/*
 *  delays for a specified number of micro seconds rountine.
 *
 *  @param usec: number of micro seconds to pause for
 *
 */
 // FPGA  APB :  25M
 // ASIC  APB : 155M
 //  ·­×ª5±¶
 void time_delay_us(unsigned int usec)
{
	volatile int i,j;
	/*
	//FPGA: 25MHZ
	for(i=0;i<usec * 5;i++)
	{
		for(j=0;j<47;j++)
		{;}
	}

    */
    //ASIC: 155MHZ   
    //AP = 155/25 = 6.2
    for(i=0;i<usec * 2;i++)
	{
		for(j=0;j<50*6;j++)
		{;}
	}
}


/* 
 * I2C by GPIO simulated  read data routine.
 *
 * @return value: a bit for read 
 *
 */
 
static unsigned char i2c_data_read(void)
{
	unsigned char regvalue;
	
	regvalue = HW_REG(GPIO_SDA_DIR);
	regvalue &= (~SDA);
	HW_REG(GPIO_SDA_DIR) = regvalue;
	DELAY(1);
		
	regvalue = HW_REG(GPIO_I2C_SDA_REG);
	if((regvalue&SDA) != 0)
		return 1;
	else
		return 0;
}



/*
 * sends a start bit via I2C rountine.
 *
 */
static void i2c_start_bit(void)
{
        DELAY(1);
       	i2c_set(SDA | SCL);
       	DELAY(1);
        i2c_clr(SDA);
        DELAY(1);
}

/*
 * sends a stop bit via I2C rountine.
 *
 */
static void i2c_stop_bit(void)
{
        /* clock the ack */
        DELAY(1);
        i2c_set(SCL);
        DELAY(1); 
        i2c_clr(SCL);  

        /* actual stop bit */
        DELAY(1);
        i2c_clr(SDA);
        DELAY(1);
        i2c_set(SCL);
        DELAY(1);
        i2c_set(SDA);
        DELAY(1);
}

/*
 * sends a character over I2C rountine.
 *
 * @param  c: character to send
 *
 */
static void i2c_send_byte(unsigned char c)
{
    int i;
    local_irq_disable();
    for (i=0; i<8; i++)
    {
        DELAY(1);
        i2c_clr(SCL);
        DELAY(1);

        if (c & (1<<(7-i)))
            i2c_set(SDA);
        else
            i2c_clr(SDA);

        DELAY(1);
        i2c_set(SCL);
        DELAY(1);
        i2c_clr(SCL);
    }
    DELAY(1);
   // i2c_set(SDA);
    local_irq_enable();
}

/*  receives a character from I2C rountine.
 *
 *  @return value: character received
 *
 */
static unsigned char i2c_receive_byte(void)
{
    int j=0;
    int i;
    unsigned char regvalue;

    local_irq_disable();
    for (i=0; i<8; i++)
    {
        DELAY(1);
        i2c_clr(SCL);
        DELAY(1);
        i2c_set(SCL);
        
        regvalue = HW_REG(GPIO_SDA_DIR);
        regvalue &= (~SDA);
        HW_REG(GPIO_SDA_DIR) = regvalue;
        DELAY(1);
        
        if (i2c_data_read())
            j+=(1<<(7-i));

        DELAY(1);
        i2c_clr(SCL);
    }
    local_irq_enable();
    DELAY(1);
   // i2c_clr(SDA);
   // DELAY(1);

    return j;
}

/*  receives an acknowledge from I2C rountine.
 *
 *  @return value: 0--Ack received; 1--Nack received
 *          
 */
static int i2c_receive_ack(void)
{
    int nack;
    unsigned char regvalue;
    
    DELAY(1);
    
    regvalue = HW_REG(GPIO_SDA_DIR);
    regvalue &= (~SDA);
    HW_REG(GPIO_SDA_DIR) = regvalue;
    
    DELAY(1);
    i2c_clr(SCL);
    DELAY(1);
    i2c_set(SCL);
    DELAY(1);
    
    

    nack = i2c_data_read();

    DELAY(1);
    i2c_clr(SCL);
    DELAY(1);
  //  i2c_set(SDA);
  //  DELAY(1);

    if (nack == 0)
        return 1; 

    return 0;
}

#if 0
static void i2c_send_ack(void)
{
    DELAY(1);
    i2c_clr(SCL);
    DELAY(1);
    i2c_set(SDA);
    DELAY(1);
    i2c_set(SCL);
    DELAY(1);
    i2c_clr(SCL);
    DELAY(1);
    i2c_clr(SDA);
    DELAY(1);
}
#endif

EXPORT_SYMBOL(gpio_i2c_rtc_read);
unsigned char gpio_i2c_rtc_read(unsigned char devaddress, unsigned char address)
{

	int rxdata;

	spin_lock(&gpioi2c_rtc_lock);
	i2c_start_bit();
	i2c_send_byte((unsigned char)(devaddress));
	i2c_receive_ack();
	i2c_send_byte(address);
	i2c_receive_ack(); 
	i2c_start_bit();
	i2c_send_byte((unsigned char)(devaddress) | 0x1);
	i2c_receive_ack();
	rxdata = i2c_receive_byte();
	//i2c_send_ack();
	i2c_stop_bit();

	spin_unlock(&gpioi2c_rtc_lock);
	return rxdata;

}


EXPORT_SYMBOL(gpio_i2c_rtc_write);
void gpio_i2c_rtc_write(unsigned char devaddress, unsigned char address, unsigned char data)
{
    spin_lock(&gpioi2c_rtc_lock);
    
    i2c_start_bit();
    i2c_send_byte((unsigned char)(devaddress));
    i2c_receive_ack();
    i2c_send_byte(address);
    i2c_receive_ack();
    i2c_send_byte(data); 
    // i2c_receive_ack();//add by hyping for tw2815
    i2c_stop_bit();

   spin_unlock(&gpioi2c_rtc_lock);
}

//int gpioi2c_rtc_ioctl(struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg)
int gpioi2c_rtc_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
   	unsigned int val;
	char device_addr, reg_addr;
	unsigned int buf[5];
	switch(cmd)
	{
		case GPIO_I2C_READ:
			if (copy_from_user(&buf[0], (unsigned int __user *)arg, sizeof(buf)))
			{
				printk("I2C read  copy from user error!\n");
				return -1;
			}
			
			device_addr	= (unsigned char)(buf[0] & 0xFF);
			reg_addr 		= (unsigned char)(buf[1] & 0xFF);
			
			buf[0] = gpio_i2c_rtc_read(device_addr, reg_addr);
			printk("device_addr=  0x%x reg_addr= 0x%x,buf[0]=0x%0x\n", device_addr, reg_addr,buf[0]);
			if (copy_to_user((unsigned int __user *)arg, buf, sizeof(buf)))
			{
				printk("I2C read  copy to user error!\n");
				return -1;
			}
			break;
		
		case GPIO_I2C_WRITE:			
			if (copy_from_user(&buf[0], (unsigned int __user *)arg, sizeof(buf)))
			{
				printk("I2C write  copy from user error!\n");
				return -1;
			}
			
			device_addr	= (unsigned char)(buf[0] & 0xFF);
			reg_addr 		= (unsigned char)(buf[1] & 0xFF);
			val 			= (unsigned char)(buf[2] & 0xFF);	
			
			gpio_i2c_rtc_write(device_addr, reg_addr, val);
			break;		
			
	
		default:
			return -1;
	}
    return 0;
}

int gpioi2c_rtc_open(struct inode * inode, struct file * file)
{
    return 0;
}
int gpioi2c_rtc_close(struct inode * inode, struct file * file)
{
    return 0;
}


static struct file_operations gpioi2c_rtc_fops = {
    .owner      = THIS_MODULE,
    //.ioctl      = gpioi2c_rtc_ioctl,
    .unlocked_ioctl = gpioi2c_rtc_ioctl,
    .open       = gpioi2c_rtc_open,
    .release    = gpioi2c_rtc_close
};


static struct miscdevice gpioi2c_rtc_dev = {
   .minor		= MISC_DYNAMIC_MINOR,
   .name		= "gpioi2c_rtc",
   .fops  		= &gpioi2c_rtc_fops,
};

#define MUXCTRL_REG_BASE 			IO_ADDRESS(0xF8000000)
									
static int __init gpio_i2c_rtc_init(void)
{
	int ret ;
	unsigned int reg;
	ret = misc_register(&gpioi2c_rtc_dev);
	if(0 != ret)
		return -1;

	reg = HW_REG(MUXCTRL_REG_BASE+0x44)= 0;
	HW_REG(MUXCTRL_REG_BASE+0x44)= reg | (1<<8) |(1<<7);

	i2c_set(SCL | SDA); 

	spin_lock_init(&gpioi2c_rtc_lock);
	return 0;    
}

static void __exit gpio_i2c_rtc_exit(void)
{
    misc_deregister(&gpioi2c_rtc_dev);
}


module_init(gpio_i2c_rtc_init);
module_exit(gpio_i2c_rtc_exit);

#ifdef MODULE
//#include <linux/compile.h>
#endif
//MODULE_INFO(build, UTS_VERSION);
MODULE_LICENSE("GPL");




