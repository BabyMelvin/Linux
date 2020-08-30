GXX=arm-linux

# 链接的顺序 start要放前面，可以从反汇编看出来
all: start.o led.o init.o uart.o main.o interrupt.o exception.o  timer.o
	#${GXX}-ld -Ttext 0 start.o main.o led.o init.o uart.o -o sdram.elf
	${GXX}-ld -T  sdram.lds $^ -o sdram_timer.elf
	${GXX}-objcopy -O binary -S sdram_timer.elf sdram_timer.bin
	${GXX}-objdump -D sdram_timer.elf > sdram_timer.dis

%.o : %.c
	${GXX}-gcc -c -o $@ $<

%.o : %.S
	${GXX}-gcc -c -o $@ $<

clean:
	rm -f *.bin *.o *.elf *.dis
