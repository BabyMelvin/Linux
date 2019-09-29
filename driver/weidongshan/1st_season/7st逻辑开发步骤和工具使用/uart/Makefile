objs := head.o init.o serial.o main.o
GCC :=arm-linux-gnueabi-

uart.bin: $(objs)
	$(GCC)ld -Tuart.lds -o uart_elf $^
	$(GCC)objcopy -O binary -S uart_elf $@
	$(GCC)objdump -D -m arm uart_elf > uart.dis

%.o : %.c
	$(GCC)gcc -Wall -nostdlib -O2 -c -o $@ $<

%.o : %.S
	$(GCC)gcc -Wall -nostdlib -O2 -c -o $@ $<

.PHONY:
clean:
	rm -f uart.bin uart_elf uart.dis

