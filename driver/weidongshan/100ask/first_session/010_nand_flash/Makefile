GXX=arm-linux

#CFLAGS += -march=arm4
# 链接的顺序 start要放前面，可以从反汇编看出来
all: start.o led.o init.o uart.o main.o interrupt.o exception.o  timer.o my_printf.o lib1funcs.o nor_flash.o string_utils.o

	#${GXX}-ld -Ttext 0 start.o main.o led.o init.o uart.o -o sdram.elf
	${GXX}-ld -T  sdram.lds $^ -o nor_flash.elf
	${GXX}-objcopy -O binary -S nor_flash.elf nor_flash.bin
	${GXX}-objdump -D nor_flash.elf > nor_flash.dis

%.o : %.c
	${GXX}-gcc -c -march=armv4 -o $@ $<

%.o : %.S
	${GXX}-gcc -c -march=armv4 -o $@ $<

clean:
	rm -f *.bin *.o *.elf *.dis
