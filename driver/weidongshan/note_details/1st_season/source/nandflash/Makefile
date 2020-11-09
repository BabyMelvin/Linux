PREFIX:= arm-linux-

nand.bin:head.S init.c nand.c main.o
	$PREFIX-ld -Tnand.lds -o nand_elf $^
	$PREFIX-objcopy -o binary -S nand_elf nand.bin
	$PREFIX-objdump -D -m arm nand_elf > nand.dis 

%.o:%.c
	$PREFIX-gcc -Wall -c -O2 -o $@ $<

%.o:%.S
	$PREFIX-gcc -Wall -c -O2 -o $@ $<

clean:
	 rm -f sdram.dis sdram.bin sdram_elf *.o


	
