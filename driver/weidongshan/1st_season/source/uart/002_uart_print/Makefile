# 注意ld连接的顺序start.o main.o 和  all:main.o test.o 的区别
all:
	arm-linux-gcc -c -o uart.o uart.c
	arm-linux-gcc -c -o main.o main.c
	arm-linux-gcc -c -o lib1funcs.o lib1funcs.S
	arm-linux-gcc -c -o my_printf.o my_printf.c
	arm-linux-gcc -c -o start.o start.S
	arm-linux-ld -Ttext 0 -Tdata 0xe80 start.o uart.o lib1funcs.o my_printf.o main.o -o uart.elf 
	arm-linux-objcopy -O binary -S uart.elf uart.bin
	arm-linux-objdump -D uart.elf > uart.dis
clean:
	rm *.bin *.o *.elf *.dis
