# 编译好内核目录
KERNEL_DIR = /work/syste/linux-2.9

# -C转到后面目录执行make
# M 是指当前目录是什么
# modules是目标

all:
	make -C $(KERNEL_DIR) M=`pwd` modules

clean:
	make -C $(KERN_DIR) M=`pwd` modules clean
	rm -rf modules.order

obj-m += first_drv.o
