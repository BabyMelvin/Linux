objs :=start.o init.o nand_flash.o mmu.o led.o uart.o  main.o exception.o interrupt.o timer.o nor_flash.o my_printf.o string_utils.o lib1funcs.o

objs += lcd/lcd_test.o
objs += lcd/font.o
objs += lcd/font_8x16.o
objs += lcd/framebuffer.o
objs += lcd/gemetory.o
objs += lcd/lcd.o
objs += lcd/lcd_4.3.o
objs += lcd/lcd_controller.o
objs += lcd/s3c2440_lcd_controller.o
objs += adc_touchscreen/adc.o
objs += adc_touchscreen/adc_test.o
objs += adc_touchscreen/touchscreen.o
objs += adc_touchscreen/touchscreen_test.o
objs += adc_touchscreen/tslib.o

all: $(objs)
	#arm-linux-ld -Ttext 0 -Tdata 0x30000000  start.o led.o uart.o init.o main.o -o sdram.elf
	arm-linux-ld -T sdram.lds $^ libgcc.a -o sdram.elf
	arm-linux-objcopy -O binary -S sdram.elf mmu_test.bin
	arm-linux-objdump -D sdram.elf > sdram.dis
clean:
	rm *.bin *.o *.elf *.dis lcd/*.o adc_touchscreen/*.o
	
%.o : %.c
	arm-linux-gcc -march=armv4 -c -o $@ $<

%.o : %.S
	arm-linux-gcc -march=armv4 -c -o $@ $<
	
