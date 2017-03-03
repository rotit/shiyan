ARCH=arm
CROSS_COMPILE?=arm-histbv310-linux-
export  ARCH CROSS_COMPILE 

ifneq ($(KERNELRELEASE), )
obj-m:=time.o
obj-m:=time_workqueue.o
else
	PWD :=$(shell pwd)
	KDIR :=/work/xvr/3798c/kernel/linux-3.18.y
	TST=1
all:
	make -C $(KDIR) M=$(PWD)  modules
	#arm-histbv310-linux-gcc -o app app.c
	#cp time.ko /tftpboot/ -rf
	cp time_workqueue.ko /tftpboot/ -rf
	#cp app /tftpboot/ -rf

clean:
	make -C $(KDIR) M=$(PWD) clean
endif
