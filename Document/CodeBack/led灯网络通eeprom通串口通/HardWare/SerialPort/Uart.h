#ifndef  __UART_H__
#define  __UART_H__

#include <string.h>
#include <stdio.h>
#include  "STC15F2K60S2.h"
#include  "Definition.h"	    		  //宏定义 常用函数

#define T_1  1
#define T_12 12
#define Double_Baudrate	 1
#define Single_Baudrate	 2

#define tbuf 50


void UartInit(void);
u8 SendData(u8 dat);
void PrintLog(u8* str);
void Uart1SendStr(u8* str);
void Uart1SendByte(u8 dat);
void Uart1Send(u8* str, u8 len);
void Uart2SendStr(u8* str);
void Uart2SendByte(u8 dat);

#endif