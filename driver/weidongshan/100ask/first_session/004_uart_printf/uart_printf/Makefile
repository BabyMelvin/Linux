GXX=arm-linux
all:
	${GXX}-gcc -c -o led.o led.c
	${GXX}-gcc -c -o uart.o uart.c
	${GXX}-gcc -c -o main.o main.c
	${GXX}-gcc -c -o my_printf.o my_printf.c
	${GXX}-gcc -c -o start.o start.S
	${GXX}-gcc -c -o lib1funcs.o lib1funcs.S
	${GXX}-ld -Ttext 0 -Tdata 0xe80 start.o led.o uart.o main.o my_printf.o lib1funcs.o -o uart_print.elf
	${GXX}-objcopy -O binary -S uart_print.elf uart_print.bin
	${GXX}-objdump -D uart_print.elf > uart_print.dis

clean:
	rm -f *.bin *.o *.elf *.dis
