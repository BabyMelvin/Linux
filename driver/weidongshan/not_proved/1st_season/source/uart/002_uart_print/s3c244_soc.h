#ifndef __S3C2440_SOC_H
#define __S3C2440_SOC_H

#define __REG(x)    (*(volatile unsigned int *)(x))
#define __REG_BYTE(x)  (*(volatile unsigned char *)(x));

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


/*UART*/
#define     ULCON0                   __REG(0x50000000)  //UART 0 line control      
#define     UCON0                    __REG(0x50000004)  //UART 0 control           
#define     UFCON0                   __REG(0x50000008)  //UART 0 FIFO control      
#define     UMCON0                   __REG(0x5000000C)  //UART 0 modem control     
#define     UTRSTAT0                 __REG(0x50000010)  //UART 0 Tx/Rx status      
#define     UERSTAT0                 __REG(0x50000014)  //UART 0 Rx error status   
#define     UFSTAT0                  __REG(0x50000018)  //UART 0 FIFO status       
#define     UMSTAT0                  __REG(0x5000001C)  //UART 0 modem status    
#define     UTXH0                    __REG_BYTE(0x50000020)  //UART 0 transmission hold 
#define     URXH0                    __REG_BYTE(0x50000024)  //UART 0 receive buffer    
#define     UBRDIV0                  __REG(0x50000028)  //UART 0 baud rate divisor 
#define     ULCON1                   __REG(0x50004000)  //UART 1 line control      
#define     UCON1                    __REG(0x50004004)  //UART 1 control           
#define     UFCON1                   __REG(0x50004008)  //UART 1 FIFO control      
#define     UMCON1                   __REG(0x5000400C)  //UART 1 modem control     
#define     UTRSTAT1                 __REG(0x50004010)  //UART 1 Tx/Rx status      
#define     UERSTAT1                 __REG(0x50004014)  //UART 1 Rx error status   
#define     UFSTAT1                  __REG(0x50004018)  //UART 1 FIFO status       
#define     UMSTAT1                  __REG(0x5000401C)  //UART 1 modem status        
#define     UTXH1                    __REG(0x50004020)  //UART 1 transmission hold 
#define     URXH1                    __REG(0x50004024)  //UART 1 receive buffer   
#define     UBRDIV1                  __REG(0x50004028)  //UART 1 baud rate divisor 
#define     ULCON2                   __REG(0x50008000)  //UART 2 line control      
#define     UCON2                    __REG(0x50008004)  //UART 2 control           
#define     UFCON2                   __REG(0x50008008)  //UART 2 FIFO control       
#define     UTRSTAT2                 __REG(0x50008010)  //UART 2 Tx/Rx status      
#define     UERSTAT2                 __REG(0x50008014)  //UART 2 Rx error status   
#define     UFSTAT2                  __REG(0x50008018)  //UART 2 FIFO status       
#define     UTXH2                    __REG(0x50008020)  //UART 2 transmission hold 
#define     URXH2                    __REG(0x50008024)  //UART 2 receive buffer     
#define     UBRDIV2                  __REG(0x50008028)  //UART 2 baud rate divisor 
#endif /* __S3C2440_SOC_H */
