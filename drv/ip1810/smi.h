//smi
#define MDIO_RD		0
#define MDIO_WR		1
//#define MDIO_DELAY  10

//mux
#define MUXCTRL_REG_MDIO			IO_ADDRESS(0xF8A21000+0x70)//132 GPIO, default:130
#define MUXCTRL_REG_MDCK			IO_ADDRESS(0xF8A21000+0x74)//131 GPIO

//ip1810 base gpio3
#define GPIO_SMI_BASE	0xF8B23000
#define MDIO_SHIFT_NUM 4
#define MDC_SHIFT_NUM  5
#define GPIO_MDIO_OFFSET 0
#define GPIO_MDC_OFFSET 1


//gpio_data_reg,address
#define GPIO_SMI_MDIO_REG 	IO_ADDRESS(GPIO_SMI_BASE+(1<<(MDIO_SHIFT_NUM+2)))
#define GPIO_SMI_MDC_REG 	IO_ADDRESS(GPIO_SMI_BASE+(1<<(MDC_SHIFT_NUM+2)))

//gpio dir
#define GPIO_BASE_MDIO_DIR IO_ADDRESS(GPIO_SMI_BASE+0x400)

//gio val
#define MDIO 1<<4
#define MDC 1<<5


//cal
#define HW_REG(a) (*(unsigned long *)(a))

//ip1810  reset  gpio4_2 //rgmii0
#define IP1810_GPIO_RESET_MUX 		IO_ADDRESS(0xF8A21000+0x80)
#define IP1810_GPIO_RESET_BASE 		0xF8B24000
#define IP1810_GPIO_RESET_DIR		IO_ADDRESS(IP1810_GPIO_RESET_BASE+0x400)
#define IP1810_GPIO_RESET_SHIFT      2

#define IP1810_GPIO_RESET_REG 			IO_ADDRESS(0xF8B24000+(1<<(2+IP1810_GPIO_RESET_SHIFT)))



//ex phy reset gpio10_0 //rgmii1
#define EX_PHY_RESET_MUX 		IO_ADDRESS(0xF8A21000+0x120)
#define EX_PHY_RESET 			IO_ADDRESS(0xF8B2A000)

//phy
#define GPIO_MII_WRITE_ALL 0





extern unsigned int me_mdio_read(struct mii_bus *bus,u8 page, u16 regaddr);
extern unsigned int me_mdio_wrtie(struct mii_bus *bus, u8 page, u8 regaddr,u16 data);

extern unsigned int stmmac_gpio_mdio_read(struct mii_bus *bus, int phyaddr, int phyreg);
extern void me_mdio_write(struct mii_bus*bus,u8 page,u16 regaddr,u16 value);
extern unsigned int ip1829_phy_reg_read(u8 phyaddr, u8 phyreg);
