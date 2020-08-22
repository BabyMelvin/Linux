GXX=arm-linux
all:
	${GXX}-gcc -c -o led.o led.c
	${GXX}-gcc -c -o start.o start.S
	${GXX}-ld -Ttext 0 start.o led.o -o led_clk.elf
	${GXX}-objcopy -O binary -S led_clk.elf led_clk.bin
	${GXX}-objdump -D led_clk.elf > led_clk.dis

clean:
	rm -f *.bin *.o *.elf *.dis
