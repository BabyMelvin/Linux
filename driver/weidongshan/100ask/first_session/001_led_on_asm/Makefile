GXX=arm-linux
all:
	${GXX}-gcc -c -o led_on.o led_on.S
	${GXX}-ld -Ttext 0 led_on.o -o led_on.elf
	${GXX}-objcopy -O binary -S led_on.elf led_on.bin
	${GXX}-objdump -D led_on.elf > led_on.dis

clean:
	rm -f *.bin *.o *.elf *.dis
