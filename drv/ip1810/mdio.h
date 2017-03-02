
#define MDIO_READ 1
#define MDIO_WRITE 33
#define MDIO_COUNT 22
#define MDIO_PHY_READ 44
#define SWITCH_RESET	55
#define u8 unsigned char
#define u16 unsigned short

#define prk(x,args...) printk("jinmin,kernel[%s,%d]--- " x,__FUNCTION__,__LINE__,##args)
struct page_reg_val {
	u8 page;
	u16 reg;
	u16 val;
};

struct phy_reg_val {
	u8 phy;
	u16 reg;
	u16 val;
};
struct dx_val{
	u8 dx;
	u16 val;
};

struct cmd_val {
	u16 cmd;
	unsigned int val;
};
