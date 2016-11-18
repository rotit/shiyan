
#define MDIO_READ 1
#define MDIO_WRITE 33

#define u8 unsigned char
#define u16 unsigned short

#define prk(x,args...) printk("jinmin,kernel[%s,%d]--- " x,__FUNCTION__,__LINE__,##args)
struct page_reg_val {
	u8 page;
	u16 reg;
	u16 val;
};



