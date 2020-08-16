GXX=arm-linux
all:
	${GXX}-gcc -c -o led.o led.c
	${GXX}-gcc -c -o start.o start.S
	${GXX}-ld -Ttext 0 start.o led.o -o led_key.elf
	${GXX}-objcopy -O binary -S led_key.elf led_key.bin
	${GXX}-objdump -D led_key.elf > led_key.dis

clean:
	rm -f *.bin *.o *.elf *.dis
