#CFLAGS += -march=arm4
# 链接的顺序 start要放前面，可以从反汇编看出来
all: start.o uart.o init.o main.o led.o exception.o timer.o interrupt.o #my_printf.o lib1funcs.o
	#arm-linux-ld -Ttext 0 start.o main.o led.o init.o uart.o -o sdram.elf
	arm-linux-ld -T  sdram.lds $^ -o nand_flash.elf
	arm-linux-objcopy -O binary -S nand_flash.elf nand_flash.bin
	arm-linux-objdump -D nand_flash.elf > nand_flash.dis

%.o : %.c
	arm-linux-gcc -c -march=armv4 -o $@ $<

%.o : %.S
	arm-linux-gcc -c -march=armv4 -o $@ $<

.PHONY:
clean:
	rm -f *.bin *.o *.elf *.dis
