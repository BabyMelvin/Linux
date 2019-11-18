GXX_PRE := arm-linux
all:
	$(GXX_PRE)-gcc -c -o led.o led.c
	$(GXX_PRE)-gcc -c -o start.o start.S
	$(GXX_PRE)-ld -Ttext 0 start.o led.o -o led.elf
	$(GXX_PRE)-objcopy -O binary -S led.elf led.bin
	$(GXX_PRE)-objdump -D led.elf > led.dis
clean:
	rm *.bin *.o *.elf *.dis

