#ifndef _MEMDEV_H_
#define _MEMDEV_H_
#include <linux/ioctl.h>

#define MEMDEV_IOC_MAGIC 'k'

#define MEMDEV_IOCON _IO(MEMDEV_IOC_MAGIC,1)
#define MEMDEV_IOCOFF _IO(MEMDEV_IOC_MAGIC,2)
#define MEMDEV_IOC_MAXNR 2
#endif