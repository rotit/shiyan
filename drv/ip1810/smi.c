
#include <linux/mii.h>
#include <linux/phy.h>
#include <linux/delay.h>
#include "smi.h"
#include "mdio.h"
#define IP1829_GPIO_MDIO

DEFINE_SPINLOCK(lock);
void time_delay_us(unsigned int usec)
{
	volatile int i,j;     
        for(i = 0 ; i< usec * 100 ;i++)
    	{
            //for(j = 0 ; j < 50 * 6 ; j++)
            for(j = 0 ; j < 6 ; j++)
    		{;}
    	}
}

#define DELAY(us)           				time_delay_us(us)


static  void mdio_1(void)
{	
	unsigned char regvalue;
	unsigned data=1;
	regvalue = HW_REG(GPIO_BASE_MDIO_DIR);
	regvalue |= 1<<MDIO_SHIFT_NUM;
	HW_REG(GPIO_BASE_MDIO_DIR) = regvalue;
	int i=0;

	if(data)
	{
		HW_REG(GPIO_SMI_MDIO_REG) = MDIO;	
		HW_REG(GPIO_SMI_MDC_REG) = 0;
		/*DELAY(1);*/
	DELAY(1);
		//udelay(5);
		HW_REG(GPIO_SMI_MDIO_REG) = MDIO;
		HW_REG(GPIO_SMI_MDC_REG) = MDC;
	DELAY(1);
	}
	else
	{
		HW_REG(GPIO_SMI_MDIO_REG) = 0;	
		HW_REG(GPIO_SMI_MDC_REG) = 0;
		/*DELAY(1);*/
	//DELAY(2);
		//udelay(5);
		HW_REG(GPIO_SMI_MDIO_REG) = 0;			
		HW_REG(GPIO_SMI_MDC_REG) = MDC;
	DELAY(1);

	}

}


static  void  mdio_0(void)
{	
	unsigned char regvalue;
	unsigned data=0;
	regvalue = HW_REG(GPIO_BASE_MDIO_DIR);
	regvalue |= 1<<MDIO_SHIFT_NUM;
	HW_REG(GPIO_BASE_MDIO_DIR) = regvalue;
	int i=0;
	if(data)
	{
		HW_REG(GPIO_SMI_MDIO_REG) = MDIO;	
		HW_REG(GPIO_SMI_MDC_REG) = 0;
		/*DELAY(1);*/
	//DELAY(2);
		//udelay(5);

		HW_REG(GPIO_SMI_MDIO_REG) = MDIO;
		HW_REG(GPIO_SMI_MDC_REG) = MDC;
	DELAY(1);
	}
	else
	{
		HW_REG(GPIO_SMI_MDIO_REG) = 0;	
		HW_REG(GPIO_SMI_MDC_REG) = 0;
		/*DELAY(1);*/
	DELAY(1);
			//udelay(5);
		HW_REG(GPIO_SMI_MDIO_REG) = 0;			
		HW_REG(GPIO_SMI_MDC_REG) = MDC;
	DELAY(1);
	}

}

void mdio_start(void){
	mdio_0();
	mdio_1();
}


void mdio_rw(int rw){
	if(rw==MDIO_RD){
		mdio_1();
		mdio_0();
	}else{
		mdio_0();
		mdio_1();
	}
}

void mdio_set_MDC_MDIO_direction(unsigned char mdc, unsigned char mdio)//0:input, 1:output for mdc/mdio values
{
	unsigned char regvalue;

	if(mdio==0)
	{
		regvalue = HW_REG(GPIO_BASE_MDIO_DIR);
		regvalue &= ~(1<<MDIO_SHIFT_NUM);
		HW_REG(GPIO_BASE_MDIO_DIR) = regvalue;

	}
	else
	{
		regvalue = HW_REG(GPIO_BASE_MDIO_DIR);
		regvalue |= 1<<MDIO_SHIFT_NUM;
		HW_REG(GPIO_BASE_MDIO_DIR) = regvalue;

	}
}

void ic_mdio_wr(unsigned short pa, unsigned short ra, unsigned short va){
	int i=0;
	unsigned short data=0;

	//set MDC/MDIO pins to GPIO mode
	//ip1829_gpio_direction_out();
	//ip1829_gpio_direction_out(void);

	//set MDC direction to output
	//set MDIO direction to output
	//mdio_set_MDC_MDIO_direction(1,1);

	for(i=0;i<32;i++)
	mdio_1();
	mdio_start();
	mdio_rw(MDIO_WR);
	for(i=0;i<5;i++){
		if((pa>>(5-1-i))%2)
			mdio_1();
		else
			mdio_0();
	}
	for(i=0;i<5;i++){
		if((ra>>(5-1-i))%2)
			mdio_1();
		else
			mdio_0();
	}
	mdio_1();
	mdio_0();
	for(i=0;i<16;i++){
		data=va<<i;
		data=data>>15;
		if(data==1)
			mdio_1();
		else
			mdio_0();
	}
}


void ip1829_mdio_wr(unsigned short pa, unsigned short ra, unsigned short va)
{
#ifdef IP1829_GPIO_MDIO
	ic_mdio_wr(pa,ra,va);
#else
	mdio_write_function(pa,ra,va);
#endif
}


void IP2Page(u8 page)		// Change page to pagenum
{

	ip1829_mdio_wr((((u16)0xff)>>5)&0x1f,((u16)0xff)&0x1f,page);
#ifdef IP1829DEBUG
	printk(KERN_ALERT "set to  page[%02u]\n", page);
#endif
}





void mdio_z(void){

	unsigned char regvalue;
	int i;
	regvalue = HW_REG(GPIO_BASE_MDIO_DIR);
	regvalue &= ~(1<<MDIO_SHIFT_NUM);
	HW_REG(GPIO_BASE_MDIO_DIR) = regvalue;
	
	//set MDC to 0
	HW_REG(GPIO_SMI_MDC_REG) = 0;
	DELAY(1);
	HW_REG(GPIO_SMI_MDC_REG) = MDC;
	DELAY(1);


	HW_REG(GPIO_SMI_MDC_REG) = 0;
	DELAY(1);
	HW_REG(GPIO_SMI_MDC_REG) = MDC;
	DELAY(1);
	
}

static void ip1829_gpio_set(unsigned offset,int value)
{

	if(offset==1)
	{
		if(value==1)	
		HW_REG(GPIO_SMI_MDC_REG) = MDC;	
		else	
		HW_REG(GPIO_SMI_MDC_REG) = 0;
	}
	else if(offset==0)
	{
		if(value==1)	
		HW_REG(GPIO_SMI_MDIO_REG) = MDIO;	
		else	
		HW_REG(GPIO_SMI_MDIO_REG) = 0;
	}
	
}

static unsigned int ip1829_gpio_get(unsigned int offset)
{	
	unsigned int data=0;
	if(offset==0)
	{
		data=HW_REG(GPIO_SMI_MDIO_REG);
		/*prk("reg bit=%x\n",data);		*/
		if((data&MDIO)!=0)
			return 1;
		else
			return 0;
	}
	return -1;
}



//1out,0in
static unsigned char  read_bit(void)
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
	//DELAY(1);
	if((data&MDIO) != 0)
	{
		return 1;
	}
	return 0 ;
}

static unsigned  int smi_receive_byte(void)
{
	unsigned int j=0;
	int i;
	//local_irq_disable();
	//等0
	//while (read_bit()){};
	
	for (i=0; i<16; i++)
	{		
		if (read_bit())
		    j+=(1<<(15-i));	
	}
	//local_irq_enable();
	
	return j;
}
unsigned short ic_mdio_rd(unsigned short pa, unsigned short ra){
	int i=0,j=0;
	unsigned short data=0;	
	unsigned short data2=0;
	unsigned int regBit;
	unsigned int bit;
	unsigned char debug[16];

	//set MDC/MDIO pins to GPIO mode
	//ip1829_gpio_direction_out();
	//ip1829_gpio_direction_out(void);

	//set MDC/MDIO PIN direction
	//mdio_set_MDC_MDIO_dir();
	//MDC direction set to output
	//MDIO direction set to output
	//mdio_set_MDC_MDIO_direction(1,1);

	for(i=0;i<64;i++)
		mdio_1();
	mdio_start();
	mdio_rw(MDIO_RD);
	for(i=0;i<5;i++){
		if((pa>>(5-1-i))%2)
			mdio_1();
		else
			mdio_0();
	}
	for(i=0;i<5;i++){
		if((ra>>(5-1-i))%2)
			mdio_1();
		else
			mdio_0();
	}
	//regBit=ip1829_gpio_get(GPIO_MDIO_OFFSET);
	mdio_z();
	//mdio_0();

	//set MDC to 1

	//ssleep(10);

	//MDIO DIR set to input
	//mdio_set_MDC_MDIO_direction(1,0);
	//udelay(1);
	//data=smi_receive_byte();
#if 1
	for(j=0;j<16;j++){
		//MDC set to 0
		ip1829_gpio_set(GPIO_MDC_OFFSET,0);

		DELAY(1);
		//get MDIO value
		regBit=ip1829_gpio_get(GPIO_MDIO_OFFSET);
		/*printk("jinmin7-- bit=%d\n",regBit);*/
		udelay(1);
		/*DELAY(1);*/
		/* bit=read_bit();
		if(bit)
			data2+=(1<<(15-j));*/
		if(regBit==0)
		{
			data|=0;
			debug[15-j]=0;
		}
		else
		{
			data|=1;
			debug[15-j]=1;
		}
		if(j<15)
			data=data<<1;
		//MDC set to 1
		ip1829_gpio_set(GPIO_MDC_OFFSET,1);

		DELAY(1);
	}
#endif
	/*mdio_set_MDC_MDIO_direction(1,1);DELAY(1);*/
	//MDC set to 0
	ip1829_gpio_set(GPIO_MDC_OFFSET,0);

	DELAY(1);
	//MDC set to 1
	ip1829_gpio_set(GPIO_MDC_OFFSET,1);

	DELAY(1);
/*printk("11111111111111 data=0x%x\n",data);*/
	//while(1);
	//ssleep(10);
	/*printk("jinmin test data=%d,data2=%d\n",data,data2);*/
	return data;
}

unsigned short ip1829_mdio_rd(unsigned short pa, unsigned short ra)
{
#ifdef IP1829_GPIO_MDIO
	return ic_mdio_rd(pa,ra);
#else
	return mdio_read_function(pa,ra);
#endif
}

u16 Read_Reg(u8 regaddr)
{
	u16 u16d;
#ifdef TEST_REG
	u16d = RegList[pg][regaddr];
#else
	u16d=ip1829_mdio_rd(((regaddr)>>5)&0x1f,(regaddr)&0x1f);
#endif
#ifdef TEST_REG
	printk(KERN_ALERT "RegList[%x][0x%02X]=0x%04x\n", pg, regaddr, RegList[pg][regaddr]);
#endif
#ifdef IP1829DEBUG
	printk(KERN_ALERT "read [%02X]=%04x\n", regaddr, u16d);
#endif
	return u16d;
}


static void smi_reset(void)
{
	unsigned char regvalue;
	int i;
	regvalue = HW_REG(GPIO_BASE_MDIO_DIR);
	regvalue |= (1<<MDIO_SHIFT_NUM | 1<<MDC_SHIFT_NUM );
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
unsigned int me_mdio_read(struct mii_bus *bus,u8 page, u16 regaddr)
{
	spin_lock(&lock);
	u16 rxdata;
	local_irq_disable();
	smi_reset();
	IP2Page(page);
	rxdata=Read_Reg(regaddr);
	local_irq_enable();
	spin_unlock(&lock);
	return rxdata;
}

void me_mdio_write(struct mii_bus*bus,u8 page,u16 regaddr,u16 value)
{
	spin_lock(&lock);
	local_irq_disable();
	IP2Page(page);
	ip1829_mdio_wr(((regaddr)>>5)&0x1f,(regaddr)&0x1f,value);
	local_irq_enable();
	spin_unlock(&lock);
}


/////////////////////////////////////////////////////
//
//
//
//
////////////////////////////////////////////////////


static  void  write_bit(unsigned char data)
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

static void smi_send_byte(unsigned char id, unsigned char c)
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


static void smi_start_read(void)
{
        write_bit(0);
       	write_bit(1);
 	write_bit(1);
	write_bit(0);
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



 unsigned int ip1829_phy_reg_read(u8 phyaddr, u8 phyreg)
{

	u16 val,tmp=0;
	IP2Page(0x3);
	struct mii_bus * bus=NULL;
	//printk("jinmin13-1-phyaddr=%x,phyreg=%x\n",phyaddr,phyreg);
	if(phyaddr>=8)
	{
		//page3 0x13,0-4:phy address,9-5:reg,10:write all tp,13-11:reserve,14:0 read/1 write,15:1 start/0 idle
		//page3 0x14 read back data / data to write
		val = ( phyaddr<<0 | phyreg <<5 | 1<<15);
		if(GPIO_MII_WRITE_ALL)
		val |=(1<<10);
	
		me_mdio_write(bus,0x3,0x13,val);
		do
		{
		val=me_mdio_read( bus,0x3,0x14);
		val=me_mdio_read( bus,0x3,0x14);
		tmp=me_mdio_read( bus,0x3,0x13);
			}
		while( (tmp&(1<<15)) == 1 );
	//printk("jinmin13-2-phyaddr=%x,phyreg=%x, mii data =%x\n",phyaddr,phyreg,val);	
		return val;
	}
	else
	{
		prk("phyaddr error\n");
		return -1;
	}
}



