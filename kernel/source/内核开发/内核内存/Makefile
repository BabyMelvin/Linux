ifneq ($(KERNELRELEASE),)
obj-m:=alloc_mem.o
else
KDIR:=/home/4-3-1/linxu-2.6.29
all:
	make -C $(KDIR) M=$(PWD) modules ARCH=arm CROSS_COMPILE=arm-linux-

clean:
	rm -r *.ko *.o *.mod *mod.c *.symvers modul*
endif