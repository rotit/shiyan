ARCH=arm
CROSS_COMPILE?=arm-histbv310-linux-
export  ARCH CROSS_COMPILE 

ifneq ($(KERNELRELEASE), )
demo-objs=mdio.o smi.o
obj-m:=demo.o
else
	PWD :=$(shell pwd)
	KDIR :=/work/xvr/3798c/kernel/linux-3.18.y
	TST=1
all:
	make -C $(KDIR) M=$(PWD)  modules
	cp demo.ko /tftpboot/ -rf
	cp app /tftpboot/ -rf

clean:
	make -C $(KDIR) M=$(PWD) clean
endif
