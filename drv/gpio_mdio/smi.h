
extern void write_bit(unsigned char data);
extern void smi_reset(void);
extern void smi_start_write(void);
extern void smi_send_byte(unsigned char id, unsigned char c);
extern void smi_write_double_byte(unsigned short int  c);
extern void smi_start_read(void);
extern void time_delay_us(unsigned int usec);
extern int stmmac_gpio_mdio_write(struct mii_bus *bus, int phyaddr, int phyreg, unsigned short phydata);
extern unsigned int stmmac_gpio_mdio_read(struct mii_bus *bus, int phyaddr, int phyreg);
extern int stmmac_gpio_mdio_reset(struct mii_bus *bus);
extern void  IP1818_SMI_WRITE(unsigned char phyaddr, unsigned char regaddr, unsigned short regval);
extern unsigned short  IP1818_SMI_READ( unsigned char phyaddr, unsigned char regaddr);
