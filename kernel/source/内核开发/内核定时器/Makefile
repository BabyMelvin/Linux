ifneq($(KERNELRELEASE),)
obj-m:=timer.o
else
KDIR:=/home/4-3-1/linux-2.6.29
all:
	make -C $(KDIR) M=$(PWD) modules ARCH=arm CROSS_COMPILE=arm-linux-2

clean:
	rm -r *.ko *.o *.mod *.mod.c *.symvers modul*
endif