ARCH=arm
CROSS_COMPILE?=arm-hisiv300-linux-
export  ARCH CROSS_COMPILE 

ifneq ($(KERNELRELEASE), )
mdio-objs=main.o smi.o
obj-m:=mdio.o
else
	PWD :=$(shell pwd)
	KDIR :=/work/xvr/3536v/linux-3.10.y_1818
	TST=1
all:
	make -C $(KDIR) M=$(PWD)  modules

clean:
	make -C $(KDIR) M=$(PWD) clean
endif
