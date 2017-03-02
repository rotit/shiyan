
#define PHY_READ 0X1
#define PHY_WRITE 0X2
#define MD_READ 0x3
#define MD_WRITE 0x4 

struct phy_reg_val {
unsigned long	int phy;
unsigned long   int reg;
unsigned long	int val;
};
