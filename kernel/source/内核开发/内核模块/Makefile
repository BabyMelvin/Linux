ifneq ($(KERNELRELEASE),)
obj-m:=hello.o
else
KDIR:=/lib/modules/2.6.18-53.e15/build

all:
	make -C $(KDIR) M=$(PWD) modules
	
clean:
	rm -f *.ko *.o *.mod *.mod.c *.sysmvers
endif