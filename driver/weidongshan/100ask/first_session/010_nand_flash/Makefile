#CFLAGS += -march=arm4
# 链接的顺序 start要放前面，可以从反汇编看出来
all: start.o init.o interrupt.o nand_flash.o led.o uart.o main.o exception.o timer.o my_printf.o string_utils.o lib1funcs.o
	#arm-linux-ld -Ttext 0 start.o main.o led.o init.o uart.o -o sdram.elf
	arm-linux-ld -T  sdram.lds $^ -o nand_flash.elf
	arm-linux-objcopy -O binary -S nand_flash.elf nand_flash.bin
	arm-linux-objdump -D nand_flash.elf > nand_flash.dis

%.o : %.c
	arm-linux-gcc -march=armv4 -c -o $@ $<

%.o : %.S
	arm-linux-gcc -march=armv4 -c -o $@ $<

.PHONY:
clean:
	rm -f *.bin *.o *.elf *.dis
