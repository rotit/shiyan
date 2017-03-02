ARCH=arm
CROSS_COMPILE?=arm-histbv310-linux-
export  ARCH CROSS_COMPILE 

$(warning $(TST))
ifneq ($(KERNELRELEASE), )
#a:
	#echo $TST jinnnnnnnnnnn
#$(warning $(TST))
#$(warning $(MM))
obj-m+=test.o
obj-m+=test2.o
else
	PWD :=$(shell pwd)
	KDIR :=/work/xvr/3798c/kernel/linux-3.18.y
	TST=1
all:
	make -C $(KDIR) M=$(PWD) modules
	echo $(TST) 111111
clean:
	make -C $(KDIR) M=$(PWD) clean
endif
