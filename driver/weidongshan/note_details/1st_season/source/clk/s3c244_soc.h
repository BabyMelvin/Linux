#ifndef __S3C2440_SOC_H
#define __S3C2440_SOC_H

#define __REG(x)    (*(volatile unsigned int *)(x))

/*I/O port*/
#define     GPACON     __REG(0x56000000)  //Port A control
#define     GPADAT     __REG(0x56000004)  //Port A data                                    
#define     GPBCON     __REG(0x56000010)  //Port B control
#define     GPBDAT     __REG(0x56000014)  //Port B data
#define     GPBUP      __REG(0x56000018)  //Pull-up control B
#define     GPCCON     __REG(0x56000020)  //Port C control
#define     GPCDAT     __REG(0x56000024)  //Port C data
#define     GPCUP      __REG(0x56000028)  //Pull-up control C
#define     GPDCON     __REG(0x56000030)  //Port D control
#define     GPDDA1T    __REG(0x56000034)  //Port D data
#define     GPDUP      __REG(0x56000038)  //Pull-up control D
#define     GPECON     __REG(0x56000040)  //Port E control
#define     GPEDAT     __REG(0x56000044)  //Port E data
#define     GPEUP      __REG(0x56000048)  //Pull-up control E
#define     GPFCON     __REG(0x56000050)  //Port F control
#define     GPFDAT     __REG(0x56000054)  //Port F data
#define     GPFUP      __REG(0x56000058)  //Pull-up control F
#define     GPGCON     __REG(0x56000060)  //Port G control
#define     GPGDAT     __REG(0x56000064)  //Port G data
#define     GPGUP      __REG(0x56000068)  //Pull-up control G
#define     GPHCON     __REG(0x56000070)  //Port H control
#define     GPHDAT     __REG(0x56000074)  //Port H data
#define     GPHUP      __REG(0x56000078)  //Pull-up control H
#define     GPJCON     __REG(0x560000D0)  //Port J control
#define     GPJDAT     __REG(0x560000D4)  //Port J data
#define     GPJUP      __REG(0x560000D8)  //Pull-up control J
#define     MISCCR     __REG(0x56000080)  //Miscellaneous control
#define     DCLKCON    __REG(0x56000084)  //DCLK0/1 control
#define     EXTINT0    __REG(0x56000088)  //External interrupt control register 0
#define     EXTINT1    __REG(0x5600008C)  //External interrupt control register 1
#define     EXTINT2    __REG(0x56000090)  //External interrupt control register 2

#endif /* __S3C2440_SOC_H */
