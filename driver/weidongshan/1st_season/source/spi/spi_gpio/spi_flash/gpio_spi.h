#ifndef __SPI_GPIO_H__
#define __SPI_GPIO_H__

void SPI_init(void);
void SPI_send_byte(unsigned char val);
unsigned char SPI_recv_byte(void);

#endif /* __SPI_GPIO_H__ */
