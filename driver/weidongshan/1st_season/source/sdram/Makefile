PREFIX:= arm-linux-

sdram.bin:head.S leds.c
	$PREFIX-gcc -c -o head.o head.S
	$PREFIX-gcc -c -o leds.o leds.c
	$PREFIX-ld -Ttext 0x30000000 head.o leds.o -o sdram_elf 
	$PREFIX-objcopy -o binary -S sdram_elf sdram.bin
	$PREFIX-objdump -D -m arm sdram_elf > sdram.dis 
clean:
	 rm -f sdram.dis sdram.bin sdram_elf *.o


	
