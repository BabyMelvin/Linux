GXX=arm-linux
all:
	${GXX}-gcc -c -o led.o led.c
	${GXX}-gcc -c -o uart.o uart.c
	${GXX}-gcc -c -o main.o main.c
	${GXX}-gcc -c -o start.o start.S
	${GXX}-ld -Ttext 0 start.o led.o uart.o main.o -o uart.elf
	${GXX}-objcopy -O binary -S uart.elf uart.bin
	${GXX}-objdump -D uart.elf > uart.dis

clean:
	rm -f *.bin *.o *.elf *.dis
