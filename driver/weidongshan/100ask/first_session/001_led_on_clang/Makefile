GXX=arm-linux
all:
	${GXX}-gcc -c -o led.o led.c
	${GXX}-gcc -c -o start.o start.S
	${GXX}-ld -Ttext 0 start.o led.o -o led_clang.elf
	${GXX}-objcopy -O binary -S led_clang.elf led_clang.bin
	${GXX}-objdump -D led_clang.elf > led_clang.dis

clean:
	rm -f *.bin *.o *.elf *.dis
