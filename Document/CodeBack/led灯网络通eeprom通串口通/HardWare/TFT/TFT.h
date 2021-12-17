#ifndef R61505V_H
#define R61505V_H
#include  "Definition.h"	    		  //宏定义 常用函数



#define Bus_16                  //16位数据模式,如果使用8位模式，请注释此语句，如果使用16位模式，请打开此句
#define  LCD_DataPortH P2       //高8位数据口,8位模式下只使用高8位 
#define  LCD_DataPortL P0       //低8位数据口	,8位模式下低8位可以不接线
sbit LCD_RS = P3^7;  		    //数据/命令切换
sbit LCD_WR = P4^2;		        //写控制
sbit LCD_RD =P4^4;		        //读控制
sbit LCD_CS=P3^6;		        //片选	
sbit LCD_REST = P4^1;	        //复位   



void Lcd_Init(void);   //初始化
void LCD_Writ_Bus(char VH,char VL);		//并行数据写入函数
void Lcd_Write_Com(char VH,char VL);   //发送命令-8位参数
void Lcd_Write_Data(char VH,char VL); //发送数据-8位参数
void Lcd_Write_Data16(uint val);
void Lcd_Write_Com_Data(int com,int val); //发送数据命令
void Address_set(unsigned int x1,unsigned int y1,unsigned int x2,unsigned int y2);//设置坐标范围

void Address_setxy(uchar x,uint y);

#endif
