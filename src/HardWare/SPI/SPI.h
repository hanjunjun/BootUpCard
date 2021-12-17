
#ifndef  SPI_H
#define  SPI_H

#include  "Definition.h"	    		  //宏定义 常用函数

#define  SPIF  0x80		        //SPI传输完成标志
#define  WCOL  0x40				//SPI写冲突标志


#define  SSIG 0X80	           	//1为主机	可用IO口控制
#define  SPEN 0x40				//SPI使能位
#define  DORD 0x20				//设定SPI数据 接收发送的位顺序
#define  MSTR 0x10				//主从模式选择位
#define  CPOL 0x08				//SPI时钟极性
#define  CPHA 0x04				// SPI时钟	相位选择

#define  SPDHH 0x00				//速度现在位
#define  SPDH  0x01
#define  SPDL  0x02
#define  SPDLL 0x03


 void Init_SPI();		     //SPI初始化
uchar SPI_SendByte(unsigned char SPI_SendData);	 //SPI为全双工通讯
void SPI_Speed(unsigned char speed);
#endif




















