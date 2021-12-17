

#include "Uart.h"



#define S2RI  0x01              //S2CON.0
#define S2TI  0x02              //S2CON.1


u8 xdata RX_buffer[tbuf];


//extern u8 RX_buffer[tbuf];

void UartInit(void)
{

	//PCON &= 0x7F;		//错误帧检测
	SCON = 0x50;		//8位数据，可变波特率
	AUXR |= 0x40;		//定时器1时钟1为FOSC,即1T
	AUXR &= 0xFE;		//串口1选择定时器1为波特率发生器
	TMOD &= 0x0F;		//设定定时器1工作模式0 为16位自动重装方式

	TL1 = (65536 - (11059200 / 4 / 115200));    //设置定时初值
	TH1 = (65536 - (11059200 / 4 / 115200)) >> 8; //设置定时初值
	//TL1 = (65536 - (11059200 / 4 / 9600));    //设置定时初值
	//TH1 = (65536 - (11059200 / 4 / 9600)) >> 8; //设置定时初值

	//ET1 = 0;		//禁止定时器1中断
	TR1 = 1;		//启动定时器1

	S2CON = 0x50;		//8位数据，可变波特率
	AUXR |= 0x04;		//定时器2时钟1为FOSC,即1T

	T2L = (65536 - (11059200 / 4 / 115200));    //设置定时初值
	T2H = (65536 - (11059200 / 4 / 115200)) >> 8; //设置定时初值

	AUXR |= 0x10;		//启动定时器2	   

}

/////////////////////////////////
/////////////////////////////////

//串口1发送串口数据(字节)
void Uart1SendByte(u8 dat)
{
	SBUF = dat;
	while (TI == 0);
	TI = 0;
}

/*
	测试eeprom
*/
u8 SendData(u8 dat)
{
	SBUF = dat;				 //发送数据
	while (TI == 0);			 //判断是否发送完成 置位TI
	TI = 0;					 //清标志位
	return dat;			 //返回发送的数据 为调用做准备
}

/*
	串口1打印调试信息到PC端
*/
void PrintLog(u8* str)
{
	while (*str)
	{
		Uart1SendByte(*str);
		str++;
	}
}

//串口1发送字符串
void Uart1SendStr(u8* str)
{
	while (*str)
	{
		Uart1SendByte(*str);
		str++;
	}
}

//串口1发送指定长度字符串
void Uart1Send(u8* str, u8 len)
{
	while (len--)
	{
		Uart1SendByte(*str);
		str++;
	}
}

/////////////////////////////////
/////////////////////////////////

//串口2发送串口数据(字节)
void Uart2SendByte(u8 ch)
{
	S2BUF = ch;                 //写数据到UART2数据寄存器
	while (!(S2CON & S2TI));
	S2CON &= ~S2TI;
}

//串口2发送字符串
void Uart2SendStr(u8* s)
{
	IE2 = 0x00;
	while (*s)                  //检测字符串结束标志
	{
		Uart2SendByte(*s++);     //发送当前字符
	}
	IE2 = 0x01;
}



/////////////////////////////////
/////////////////////////////////

u8 RX_num = 0;   //接收计数变量

//串口1中断
void Uart1() interrupt 4 using 1
{
	ES = 0;		   //关闭串口1中断
	if (RI)
	{
		RI = 0;                 //清除RI位
		RX_buffer[RX_num] = SBUF;
		RX_num++;
		if (RX_num > tbuf) RX_num = 0;


	}
	if (TI)
	{
		TI = 0;                 //清除TI位

	}
	ES = 1;	  //开启串口1中断

}


//串口2中断
void Uart2() interrupt 8 using 1
{

	IE2 = 0x00;		  //关闭串口2中断
	if (S2CON & S2RI)
	{
		S2CON &= ~S2RI;
		RX_buffer[RX_num] = S2BUF;
		RX_num++;
		if (RX_num > tbuf) RX_num = 0;
	}
	if (S2CON & S2TI)
	{
		S2CON &= ~S2TI;
	}
	IE2 = 0x01;		//开启串口2中断

}







