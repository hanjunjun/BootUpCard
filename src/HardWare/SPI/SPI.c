
#include  "stc15f2k60s2.h"
#include  "SPI.h"
#include  "Definition.h"	    		  //宏定义 常用函数


//#define  ESPI  0x02			    //IE2.1为spi中断开关




 void Init_SPI()		     //SPI初始化

{
AUXR1=0X20;				 //将 SPI 调整到 P4.1 P4.2 P4.3
SPCTL = 0xd0;            //  SSIG 1 开启主机模式 SPEN 1 SPI使能  DORD 0 先发最高位   MSTR 1 主机模式
                         //  CPOL 0  SPICLK空闲时为低  CPHA  0 数据在SPICLK的前时钟沿驱动  时钟CPU_CLK/4 

SPSTAT=0xc0;             //SPDAT.7和SPDAT.6写11，可以将中断标志清零。注意是写1才清零

//IE2 |= 0x02;           //允许SPI中断
//EA =1;                 //开总中断

}


//SPI为全双工通讯  所以在发送的同时可以接收到数据

uchar SPI_SendByte(unsigned char SPI_SendData)

{

SPDAT= SPI_SendData;     // 将数据 写入 
		
while((SPSTAT&0x80)==0); //等待写入完成

SPSTAT = 0xc0;           //清除中断标志，和写冲突标志，注意是对应位写1才能清零
						 
return  SPDAT;           //返回得到的数据
	
}



 //SPI时钟速率设置
void SPI_Speed(unsigned char speed)
{

    switch(speed)					//每一次降速 都要先清为最高 在进行降速
    {
        case 0:	SPCTL&=0xFC;break;	              //SysClk/4,SPR1=0,SPR0=0 
        case 1:	SPCTL&=0xFC; SPCTL|=0x01;break;   //SysClk/16,SPR1=0,SPRO=1
        case 2:	SPCTL&=0xFC; SPCTL|=0x02;break;	  //SysClk/64,SPR1=1,SPR0=0
        case 3:	SPCTL&=0xFC; SPCTL|=0x03;break;	  //SysClk/128,SPR1=1,SPR0=1
    }
    
}




