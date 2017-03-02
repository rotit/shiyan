ARCH=arm
CROSS_COMPILE?=arm-histbv310-linux-
export  ARCH CROSS_COMPILE 

ifneq ($(KERNELRELEASE), )
obj-m:=poll_select2.o
else
	PWD :=$(shell pwd)
	KDIR :=/work/xvr/3798c/kernel/linux-3.18.y
	TST=1
all:
	make -C $(KDIR) M=$(PWD)  modules
	cp poll_select2.ko /tftpboot/ -rf

clean:
	make -C $(KDIR) M=$(PWD) clean
endif
