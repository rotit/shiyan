#include <linux/mii.h>
#include <linux/phy.h>
#include <linux/slab.h>

#define IP1818_PHY_CMD									0xda
#define IP1818_PHY_DATA									0xdb

#define HW_REG(a) (*(unsigned long *)(a))
/*#define HW_REG(reg)         				*((volatile unsigned int *)(reg))*/
#define DELAY(us)           				time_delay_us(us)
#define GPIO_BASE_MDC 				0x12190000	//4_6
#define GPIO_BASE_MDIO 				0x12160000	//1_6



#define MDC_SHIFT_NUM   				6
#define MDIO_SHIFT_NUM   			6

#define MDC                 					(0x1 << MDC_SHIFT_NUM)    /* GPIO 4_6 */
#define MDIO                 					(0x1 << MDIO_SHIFT_NUM)    /* GPIO 1_6 */

#define MUXCTRL_REG_MDCK			IO_ADDRESS(0x120F0000+0x0c4)//0x3
#define MUXCTRL_REG_MDIO			IO_ADDRESS(0x120F0000+0x0d4)//0x3


#define GPIO_SMI_MDC_REG   			IO_ADDRESS(GPIO_BASE_MDC + (0x1<<(MDC_SHIFT_NUM+2)))  /* 0x200 */
#define GPIO_SMI_MDIO_REG    			IO_ADDRESS(GPIO_BASE_MDIO + (0x1<<(MDIO_SHIFT_NUM+2)))  /* 0x100 */

#define GPIO_BASE_MDC_DIR 			IO_ADDRESS(GPIO_BASE_MDC + 0x400)
#define GPIO_BASE_MDIO_DIR 			IO_ADDRESS(GPIO_BASE_MDIO + 0x400)

void time_delay_us(unsigned int usec)
{
	volatile int i,j;     
        for(i = 0 ; i< usec * 5 ;i++)
    	{
            //for(j = 0 ; j < 50 * 6 ; j++)
            for(j = 0 ; j < 6 ; j++)
    		{;}
    	}
}

//1out,0in
unsigned char  read_bit(void)
{	
	unsigned char regvalue;
	unsigned int data;
	
	regvalue = HW_REG(GPIO_BASE_MDIO_DIR);
	regvalue &= ~(1<<MDIO_SHIFT_NUM);
	HW_REG(GPIO_BASE_MDIO_DIR) = regvalue;

	HW_REG(GPIO_SMI_MDC_REG) = 0;
	DELAY(1);
	HW_REG(GPIO_SMI_MDC_REG) = MDC;
	data = HW_REG(GPIO_SMI_MDIO_REG);
	if((data&MDIO) != 0)
	{
		return 1;
	}
	return 0 ;
}

void  write_bit(unsigned char data)
{	
	unsigned char regvalue;
	
	regvalue = HW_REG(GPIO_BASE_MDIO_DIR);
	regvalue |= 1<<MDIO_SHIFT_NUM;
	HW_REG(GPIO_BASE_MDIO_DIR) = regvalue;

	if(data)
	{
		HW_REG(GPIO_SMI_MDIO_REG) = MDIO;	
		HW_REG(GPIO_SMI_MDC_REG) = 0;
		DELAY(1);
		HW_REG(GPIO_SMI_MDC_REG) = MDC;
	}
	else
	{
		HW_REG(GPIO_SMI_MDIO_REG) = 0;	
		HW_REG(GPIO_SMI_MDC_REG) = 0;
		DELAY(1);
		HW_REG(GPIO_SMI_MDC_REG) = MDC;
	}

}

void smi_send_byte(unsigned char id, unsigned char c)
{
	int i;
	local_irq_disable();	
	if(id&2)
		write_bit(1);
	else
		write_bit(0);	
	if(id & 1)
		write_bit(1);
	else
		write_bit(0);	 
	for (i=0; i<8; i++)
	{
	    if (c & (1<<(7-i)))
	       write_bit(1);
	    else
	       write_bit(0);	   
	}	
    local_irq_enable();
}

void smi_write_double_byte(unsigned short int  c)
{
	int i;
	local_irq_disable();
	
	for (i=0; i<16; i++)
	{
	    if (c & (1<<(15-i)))
	       write_bit(1);
	    else
	       write_bit(0);	    
	}	
    local_irq_enable();
}

unsigned  int smi_receive_byte(void)
{
	unsigned int j=0;
	int i;
	local_irq_disable();
	//等0
	//while (read_bit()){};
	
	for (i=0; i<16; i++)
	{		
		if (read_bit())
		    j+=(1<<(15-i));	
	}
	local_irq_enable();
	
	return j;
}

void smi_start_read(void)
{
        write_bit(0);
       	write_bit(1);
 	write_bit(1);
	write_bit(0);
}


void smi_start_write(void)
{
        write_bit(0);
       	write_bit(1);
	write_bit(0);
	write_bit(1);
}

void smi_reset(void)
{
	unsigned char regvalue;
	int i;
	regvalue = HW_REG(GPIO_BASE_MDIO_DIR);
	regvalue |= 1<<MDIO_SHIFT_NUM;
	HW_REG(GPIO_BASE_MDIO_DIR) = regvalue;
	
	HW_REG(GPIO_SMI_MDIO_REG) = 0x0;
	DELAY(20);
	//HW_REG(GPIO_SMI_MDC_REG) = MDC;
	HW_REG(GPIO_SMI_MDIO_REG) = MDIO;
	for(i=0;i<5;i++)
	{
		HW_REG(GPIO_SMI_MDC_REG) = 0;
		DELAY(1);
		HW_REG(GPIO_SMI_MDC_REG) = MDC;
		DELAY(1);
	}
	HW_REG(GPIO_SMI_MDC_REG) = 0;
	DELAY(1);
	HW_REG(GPIO_SMI_MDC_REG) = MDC;
}

 int stmmac_gpio_mdio_write(struct mii_bus *bus, int phyaddr, int phyreg, unsigned short phydata)
{
	unsigned char id, c;	
	id = 0x00&0x3;
	c = phyreg&0xff;
	//id = 0x00;
	//c = ((phyaddr&0x7)<<5)|(phyreg&0x1f);
	smi_reset();
	smi_start_write();
	smi_send_byte(id, c);	
	write_bit(1);		
	write_bit(0);
	smi_write_double_byte(phydata);
	return 0;
}



//static unsigned int stmmac_gpio_mdio_read(struct mii_bus *bus, int phyaddr, int phyreg)
 unsigned int stmmac_gpio_mdio_read(struct mii_bus *bus, int phyaddr, int phyreg)
{
	unsigned int rxdata;
	unsigned char regvalue,id,c;
	
	//id = 0x00;
	//c = ((phyaddr&0x7)<<5)|(phyreg&0x1f);
	id = 0x00&0x3;
	c = phyreg&0xff;
	smi_reset();
	smi_start_read();
	smi_send_byte(id, c);
	DELAY(1);
	HW_REG(GPIO_SMI_MDIO_REG) = 0;
	HW_REG(GPIO_SMI_MDC_REG) = 0;
	DELAY(1);
	HW_REG(GPIO_SMI_MDC_REG) = MDC;
	
	regvalue = HW_REG(GPIO_BASE_MDIO_DIR);
	regvalue &= ~(1<<MDIO_SHIFT_NUM);
	HW_REG(GPIO_BASE_MDIO_DIR) = regvalue;
	
	rxdata = smi_receive_byte();
	//printk("***gpio_smi_read,id=0x%x,c=0x%x,reg=0x%x*****\n",id,c,rxdata);
	return rxdata;
}

 int stmmac_gpio_mdio_reset(struct mii_bus *bus)
{
	unsigned int regvalue;
	regvalue = HW_REG(GPIO_BASE_MDC_DIR);
	regvalue |= 1<<MDC_SHIFT_NUM;
	HW_REG(GPIO_BASE_MDC_DIR) = regvalue;
	
	HW_REG(MUXCTRL_REG_MDCK) = 0x3;
	HW_REG(MUXCTRL_REG_MDIO) = 0x3;
	return 0;
}


void  IP1818_SMI_WRITE(unsigned char phyaddr, unsigned char regaddr, unsigned short regval)
{
	unsigned short reg_val=0,reg_data=0;
	stmmac_gpio_mdio_write(NULL, 0, IP1818_PHY_DATA, regval); 
	reg_val = phyaddr & 0x1f;
	reg_val |= (regaddr &0x1f)<<5;
	reg_val |= 0x1<<14;
	reg_val |= 0x1<<15;
	stmmac_gpio_mdio_write(NULL,0, IP1818_PHY_CMD, reg_val); 
	do{reg_data=stmmac_gpio_mdio_read(NULL, 0, IP1818_PHY_CMD);}while((reg_data&(0x1<<13))==0);
}


unsigned short  IP1818_SMI_READ( unsigned char phyaddr, unsigned char regaddr)
{
	unsigned short reg_val=0,reg_data=0;
	reg_val = phyaddr & 0x1f;
	reg_val |= (regaddr &0x1f)<<5;
	reg_val |= 0x1<<15;
	stmmac_gpio_mdio_write(NULL, 0, IP1818_PHY_CMD, reg_val); 
	do{reg_data=stmmac_gpio_mdio_read(NULL, 0, IP1818_PHY_CMD);}while((reg_data&(0x1<<13))==0);
reg_val=stmmac_gpio_mdio_read(NULL, 0, IP1818_PHY_DATA);
return reg_val;
}
