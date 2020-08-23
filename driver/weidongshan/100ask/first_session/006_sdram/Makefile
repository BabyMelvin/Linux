GXX=arm-linux
all:
	${GXX}-gcc -c -o led.o led.c
	${GXX}-gcc -c -o start.o start.S
	${GXX}-gcc -c -o init.o init.c 
	${GXX}-gcc -c -o main.o main.c 
	${GXX}-gcc -c -o uart.o uart.c 
	${GXX}-ld -Ttext 0 start.o main.o led.o init.o uart.o -o sdram.elf
	${GXX}-objcopy -O binary -S sdram.elf sdram.bin
	${GXX}-objdump -D sdram.elf > sdram.dis

clean:
	rm -f *.bin *.o *.elf *.dis
