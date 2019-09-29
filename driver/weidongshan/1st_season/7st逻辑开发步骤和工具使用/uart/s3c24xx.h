/* WATCH DOG REGISTER*/
#define WTCON           (*(volatile unsigned long *)0x53000000)

/* SDRAM registers*/
#define MEM_CTL_BASE    0x48000000 
#define SDRAM_BASE      0x30000000

/* GPIO registers */
#define GPBCON              (*(volatile unsigned long *)0x56000010)
#define GPBDAT              (*(volatile unsigned long *)0x56000014)

#define GPFCON              (*(volatile unsigned long *)0x56000050)
#define GPFDAT              (*(volatile unsigned long *)0x56000054)
#define GPFUP               (*(volatile unsigned long *)0x56000058)

#define GPGCON              (*(volatile unsigned long *)0x56000060)
#define GPGDAT              (*(volatile unsigned long *)0x56000064)
#define GPGUP               (*(volatile unsigned long *)0x56000068)

#define GPHCON              (*(volatile unsigned long *)0x56000070)
#define GPHDAT              (*(volatile unsigned long *)0x56000074)
#define GPHUP               (*(volatile unsigned long *)0x56000078)

/* clock registers */
#define LOCKTIME        (*(volatile unsigned long *)0x4c000000)
#define MPLLCON         (*(volatile unsigned long *)0x4c000004)
#define UPLLCON         (*(volatile unsigned long *)0x4c000008)
#define CLKCON          (*(volatile unsigned long *)0x4c00000c)
#define CLKSLOW         (*(volatile unsigned long *)0x4c000010)
#define CLKDIVN         (*(volatile unsigned long *)0x4c000014)

/* UART register*/
#define ULCON0          (*(volatile unsigned long *)0x50000000)
#define UCON0           (*(volatile unsigned long *)0x50000004)
#define UFCON0          (*(volatile unsigned long *)0x50000008)
#define UMCON0          (*(volatile unsigned long *)0x5000000c)
#define UTRSTAT0       (*(volatile unsigned long *)0x50000010)
#define UTXH0           (*(volatile unsigned long *)0x50000020)
#define URXH0           (*(volatile unsigned long *)0x50000024)
#define UBRDIV0         (*(volatile unsigned long *)0x50000028)

#define GSTATUS1        (*(volatile unsigned long *)0x560000B0)
