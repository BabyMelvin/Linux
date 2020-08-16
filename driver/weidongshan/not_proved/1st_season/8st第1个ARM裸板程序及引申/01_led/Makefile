GCC := arm-linux-gnueabi

all:
	$(GCC)-gcc -c -o led_on.o led_on.S
	$(GCC)-ld -Ttext 0 led_on.o -o led_on.elf
	$(GCC)-objcopy -O binary -S led_on.elf led_on.bin
	$(GCC)-objdump -D led_on.elf > led_on.dis

clean:
	rm *.bin *.o *.elf
