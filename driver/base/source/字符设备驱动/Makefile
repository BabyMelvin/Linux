ifneq (($(KERNELRELEASE),)
obj-m:=memdev.o
else
KDIR:=/home/linux-2.6.29
all:
	make -C $(KDIR) M=$(PWD) modules ARCH=arm CROSS_COMPRILE=arm-linx-
clean:
	rm -r *.ko *.o *.mod.o *mode.c *.symvers modul*
endif
