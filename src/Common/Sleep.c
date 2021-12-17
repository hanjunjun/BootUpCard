#include "../Common/Sleep.h"
#include "Definition.h"
/*
24MHz主频下
在示波器下看
一个语句的时间大概是 0.8us
*/


//4微秒	延时
//此函数是以4us的倍数增加
//t  4us的倍数
void delay4us(unsigned int us)
{
	u16 i;
	u8 m;
	for (i = 0; i < us; i++)
		for (m = 0; m < 5; m++);
}



//1毫秒 延时
//ms 1ms的倍数
void delay1ms(unsigned int ms)
{
	u16 m;
	for (m = 0; m < ms; m++)
		delay4us(250);
}


