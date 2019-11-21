/*
 * FILE: i2c.c
 * 用于主机发送/接收
 * 
 * 
 * 数据传输：发送到SDA每个字节是8位，每个字节后必须跟一个响应位
 * (响应信号)ACK(接收模式有效):接收器在接收到8位数据后，在第9个时钟周期，拉低SDA电平
 *      发送器：发送数据到总线的器件
 *      接收器：从总线接收数据的器件
 *      主机  ：发起/停止数据传输、提供时钟信号的器件
 *      从机  : 被主机寻址的器件
 */

/*
 * 每传输8位之后，都会有ACK信号,以下三种特例:
 *  1.当从机不能响应从机地址时,没有ACK信号。主机发出一个P信号终止或者重新发出一个S信号开始新的传输
 *  2.从机接收器在传输过程中不能接收更多数据，不会发出ACK信号.(主机要防呆，发出一个P信号或者重新发出S信号)
 *  3.主机接收器接收到最后一个字节后，也不会发出ACK信号。从机发送器释放SDA线，以允许主机发出P信号结束传输
 * */

#include <stdio.h>
#include "s3c24xx.h"
#include "i2c.h"

void Delay(int time);

#define WRDATA      (1)
#define RDDATA      (2)

typedef struct tI2C {
    unsigned char *pData;   /* 数据缓冲区 */
    volatile int DataCount; /* 等待传输的数据长度 */
    volatile int Status;    /* 状态 */
    volatile int Mode;      /* 模式：读/写 */
    volatile int Pt;        /* pData中待传输数据的位置 */
}tS3C24xx_I2C, *ptS3C24xx_I2C;
static tS3C24xx_I2C g_tS3C24xx_I2C;

/*
 * I2C中断服务程序
 * 根据剩余的数据长度选择继续传输或者结束
 * 
 * IICCON:
 *  [7]:ACK信号使能，发送无意义
 *      接收模式，发送ACK信号
 *  [5]:中断使能
 *  [4]:中断标记
 *      标识是否发生中断，读1：发生中断，I2C传输停止
 *      写0清楚中断，继续传输
 * IICSTAT:
 *  [7:6]:工作模式 
 *  [5]:读看总线是否忙或空闲
 *      写0：发出P信号
 *      写1：发出S信号，S发出后IICDS寄存器中数据将被自动发送
 *  [4]:串行输出使能位
 *      写0：禁止传输
 *      写1：使能接收/发送功能
 *  [2]:从机地址状态
 *      作为从机时，检测到S/P信号自动清0，接收到地址与IICADD之相等时，此为被置1
 *  [0]:最后一位状态
 *      0:接收到ACK信号
 *      1:没有接收到ACK信号
 */

void I2CIntHandle(void)
{
	unsigned int iicSt,i;

	// 清中断
    SRCPND = BIT_IIC;
    INTPND = BIT_IIC;
    
    iicSt  = IICSTAT;
	if(iicSt & 0x8){ printf("Bus arbitration failed\n\r"); }

	switch (g_tS3C24xx_I2C.Mode)
    {    
        case WRDATA:
			if((g_tS3C24xx_I2C.DataCount--) == 0)
            {
                // 下面两行用来恢复I2C操作，发出P信号
                IICSTAT = 0xd0;
                IICCON  = 0xaf;
                Delay(10000);  // 等待一段时间以便P信号已经发出
                break;    
            }

            IICDS = g_tS3C24xx_I2C.pData[g_tS3C24xx_I2C.Pt++];
            
            // 将数据写入IICDS后，需要一段时间才能出现在SDA线上
            for (i = 0; i < 10; i++);   

            IICCON = 0xaf;      // 恢复I2C传输
            break;
		 case RDDATA:
			if (g_tS3C24xx_I2C.Pt == -1)
            {
                // 这次中断是发送I2C设备地址后发生的，没有数据
                // 只接收一个数据时，不要发出ACK信号
                g_tS3C24xx_I2C.Pt = 0;
                if(g_tS3C24xx_I2C.DataCount == 1)
                   IICCON = 0x2f;   // 恢复I2C传输，开始接收数据，接收到数据时不发出ACK
                else 
                   IICCON = 0xaf;   // 恢复I2C传输，开始接收数据
                break;
            }
			if ((g_tS3C24xx_I2C.DataCount--) == 0)
            {
                g_tS3C24xx_I2C.pData[g_tS3C24xx_I2C.Pt++] = IICDS;

                // 下面两行恢复I2C操作，发出P信号
                IICSTAT = 0x90;
                IICCON  = 0xaf;
                Delay(10000);  // 等待一段时间以便P信号已经发出
                break;    
            }
			g_tS3C24xx_I2C.pData[g_tS3C24xx_I2C.Pt++] = IICDS;

            // 接收最后一个数据时，不要发出ACK信号
            if(g_tS3C24xx_I2C.DataCount == 0)
                IICCON = 0x2f;   // 恢复I2C传输，接收到下一数据时无ACK
            else 
                IICCON = 0xaf;   // 恢复I2C传输，接收到下一数据时发出ACK
            break;
		default:
			break;
}

/*
 * I2C初始化
 *  初始化s3c24xx控制器
 */
void i2c_init(void)
{
    GPEUP  |= 0xc000;       // 禁止内部上拉
    GPECON |= 0xa0000000;   // 选择引脚功能：GPE15:IICSDA, GPE14:IICSCL

    INTMSK &= ~(BIT_IIC);

    /* bit[7] = 1, 使能ACK
     * bit[6] = 0, IICCLK = PCLK/16
     * bit[5] = 1, 使能中断
     * bit[3:0] = 0xf, Tx clock = IICCLK/16
     * PCLK = 50MHz, IICCLK = 3.125MHz, Tx Clock = 0.195MHz
     */
    IICCON = (1<<7) | (0<<6) | (1<<5) | (0xf);  // 0xaf

    IICADD  = 0x10;     // S3C24xx slave address = [7:1]
    IICSTAT = 0x10;     // I2C串行输出使能(Rx/Tx)
}

/*
 * 主机发送
 * slvAddr : 从机地址，buf : 数据存放的缓冲区，len : 数据长度 
 */
void i2c_write(unsigned int slvAddr, unsigned char *buf, int len)
{
    g_tS3C24xx_I2C.Mode = WRDATA;   // 写操作
    g_tS3C24xx_I2C.Pt   = 0;        // 索引值初始为0
    g_tS3C24xx_I2C.pData = buf;     // 保存缓冲区地址
    g_tS3C24xx_I2C.DataCount = len; // 传输长度
    

    /*
     * 主机发送
     *  1.待发出为slvAddr地址
     *  2.设置为主机发送器模式,[7:6]=11
     *  3.发送S信号，IICDS内容自动被发送,[5]=1
     *  4.使能接收/发送功能,[4] = 1
     * */
    IICDS   = slvAddr;
    IICSTAT = 0xf0;         // 主机发送，启动
    
    /* 等待直至数据传输完毕 */
    while (g_tS3C24xx_I2C.DataCount != -1);
}

/*
 * 主机接收
 * slvAddr : 从机地址，buf : 数据存放的缓冲区，len : 数据长度 
 */
void i2c_read(unsigned int slvAddr, unsigned char *buf, int len)
{
    g_tS3C24xx_I2C.Mode = RDDATA;   // 读操作
    g_tS3C24xx_I2C.Pt   = -1;       // 索引值初始化为-1，表示第1个中断时不接收数据(地址中断)
    g_tS3C24xx_I2C.pData = buf;     // 保存缓冲区地址
    g_tS3C24xx_I2C.DataCount = len; // 传输长度
    
    IICDS        = slvAddr;
    IICSTAT      = 0xb0;    // 主机接收，启动
    
    /* 等待直至数据传输完毕 */    
    while (g_tS3C24xx_I2C.DataCount != -1);
}

/*
 * 延时函数
 */
void Delay(int time)
{
    for (; time > 0; time--);
}

