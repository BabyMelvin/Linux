GXX=arm-linux

# 链接的顺序 start要放前面，可以从反汇编看出来
all: start.o init.o main.o led.o uart.o exception.o
	#${GXX}-ld -Ttext 0 start.o main.o led.o init.o uart.o -o sdram.elf
	${GXX}-ld -T  sdram.lds $^ -o sdram_und.elf
	${GXX}-objcopy -O binary -S sdram_und.elf sdram_und.bin
	${GXX}-objdump -D sdram_und.elf > sdram_und.dis

%.o : %.c
	${GXX}-gcc -c -o $@ $<

%.o : %.S
	${GXX}-gcc -c -o $@ $<

clean:
	rm -f *.bin *.o *.elf *.dis
