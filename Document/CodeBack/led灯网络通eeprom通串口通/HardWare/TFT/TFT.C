


#include  "stc15f2k60s2.h"
#include  "Definition.h"	    		  //宏定义 常用函数
#include  "TFT.h"
#include  "../../Common/Delay.h"





 //TFT数据及命令函数

#ifdef  Bus_16    //条件编译-16位数据模式   
void LCD_Writ_Bus(char VH,char VL)   //并行数据写入函数
{
    LCD_DataPortH=VH;	
	LCD_DataPortL=VL;		
	LCD_WR=0;
	LCD_WR=1; 
}
#else			//条件编译-8位数据模式 
void LCD_Writ_Bus(char VH,char VL)   //并行数据写入函数
{	
    LCD_DataPortH=VH;	
   	LCD_WR=0;
	LCD_WR=1;
	LCD_DataPortH=VL;		
	LCD_WR=0;
	LCD_WR=1;
}
#endif



void Lcd_Write_Com(char VH,char VL)	 //发送命令-8位参数
{	
    LCD_RS=0;
	LCD_Writ_Bus(VH,VL);
}
void Lcd_Write_Data(char VH,char VL) //发送数据-8位参数
{
    LCD_RS=1;
	LCD_Writ_Bus(VH,VL);
}

void Lcd_Write_Data16(uint val)
{
Lcd_Write_Data(val>>8,val);
}

void Lcd_Write_Com_Data(int com,int val)		   //发送数据命令
{
	Lcd_Write_Com(com>>8,com);
    Lcd_Write_Data(val>>8,val);
}

//设置坐标范围		与tft驱动IC有关
void Address_set(unsigned int x1,unsigned int y1,unsigned int x2,unsigned int y2)
{
	Lcd_Write_Com(0x00,0x50);Lcd_Write_Data(x1>>8,x1);
	Lcd_Write_Com(0x00,0x51);Lcd_Write_Data(x2>>8,x2);
	Lcd_Write_Com(0x00,0x52);Lcd_Write_Data(y1>>8,y1);
	Lcd_Write_Com(0x00,0x53);Lcd_Write_Data(y2>>8,y2);  
  	Lcd_Write_Com(0x00,0x20);Lcd_Write_Data(x1>>8,x1);	  
    Lcd_Write_Com(0x00,0x21);Lcd_Write_Data(y1>>8,y1); 
    Lcd_Write_Com(0x00,0x22);							 
}



//===============================================================
//设定当前显示坐标
//入口参数: x,y为当前显示坐标。
//出口参数: 无
//说明：本函数实际上是LCD_setwindow函数的一个特例，显示窗口为最小，仅是一个像素点。
void Address_setxy(uchar x,uint y)
{
	Address_set(x,y,x,y);	
}




//液晶初始化
void Lcd_Init(void)
{

    LCD_REST=1;
	delay1ms(5);
	LCD_REST=0;
	delay1ms(5);
	LCD_REST=1;
	LCD_CS=1;
	LCD_RD=1;
	LCD_WR=1;
	delay1ms(5);

	LCD_CS =0;  //打开片选使能

    Lcd_Write_Com_Data(0x0001,0x0100);	//Driver Output Contral.	
    Lcd_Write_Com_Data(0x0002,0x0700);	//LCD Driver Waveform Contral.
	Lcd_Write_Com_Data(0x0003,0x1030);	//Entry Mode Set.
	Lcd_Write_Com_Data(0x0004,0x0000);	//Scalling Contral.			
	Lcd_Write_Com_Data(0x0008,0x0202);	//Display Contral 2.(0x0207)		
	Lcd_Write_Com_Data(0x0009,0x0000);	//Display Contral 3.(0x0000)
	Lcd_Write_Com_Data(0x000A,0x0000);	//Frame Cycle Contal.(0x0000)
	Lcd_Write_Com_Data(0x000C,0x0000);	
	Lcd_Write_Com_Data(0x000D,0x0000);	//Frame Maker Position.
	Lcd_Write_Com_Data(0x000F,0x0000);	//Extern Display Interface Contral 2.

	Lcd_Write_Com_Data(0x0010,0x0000);	
	Lcd_Write_Com_Data(0x0011,0x0007);								//Power Control 2.(0x0001)				    //Power Control 3.(0x0138)
	Lcd_Write_Com_Data(0x0012,0x0000);	
	Lcd_Write_Com_Data(0x0013,0x0000);								//Power Control 4.
	Lcd_Write_Com_Data(0x0007,0x0001);								//Power Control 7.
	delay1ms(50);

    Lcd_Write_Com_Data(0x0010,0x1690);
	Lcd_Write_Com_Data(0x0011,0x0227);
	delay1ms(50);
	Lcd_Write_Com_Data(0x0012,0x009D);
	delay1ms(50);
	Lcd_Write_Com_Data(0x0013,0x1900);	
	delay1ms(50);
	Lcd_Write_Com_Data(0x0029,0x0025);	
	Lcd_Write_Com_Data(0x002B,0x000D);
	delay1ms(50);
	Lcd_Write_Com_Data(0x0020,0x0000);	
	Lcd_Write_Com_Data(0x0021,0x0000);
	delay1ms(50);
	Lcd_Write_Com_Data(0x0030,0x0007);	
	Lcd_Write_Com_Data(0x0031,0x0303);	
	Lcd_Write_Com_Data(0x0032,0x0003);	
	Lcd_Write_Com_Data(0x0035,0x0206);	
	Lcd_Write_Com_Data(0x0036,0x0008);	
	Lcd_Write_Com_Data(0x0037,0x0406);	
	Lcd_Write_Com_Data(0x0038,0x0304);
	Lcd_Write_Com_Data(0x0039,0x0007);	
	Lcd_Write_Com_Data(0x003C,0x0601);	
	Lcd_Write_Com_Data(0x003D,0x0008);			
				

	Lcd_Write_Com_Data(0x0050,0x0000);	
	Lcd_Write_Com_Data(0x0051,0x00EF);	
	Lcd_Write_Com_Data(0x0052,0x0000);	
	Lcd_Write_Com_Data(0x0053,0x013F);	
	Lcd_Write_Com_Data(0x0060,0xA700);	
	Lcd_Write_Com_Data(0x0061,0x0001);	
	Lcd_Write_Com_Data(0x006A,0x0000);	


	Lcd_Write_Com_Data(0x0080,0x0000);	//Display Position? Partial Display 1.
	Lcd_Write_Com_Data(0x0081,0x0000);	//RAM Address Start? Partial Display 1.
	Lcd_Write_Com_Data(0x0082,0x0000);	//RAM Address End-Partial Display 1.
	Lcd_Write_Com_Data(0x0083,0x0000);	//Displsy Position? Partial Display 2.
	Lcd_Write_Com_Data(0x0084,0x0000);	//RAM Address Start? Partial Display 2.
	Lcd_Write_Com_Data(0x0085,0x0000);	//RAM Address End? Partial Display 2.

	Lcd_Write_Com_Data(0x0090,0x0010);	
	Lcd_Write_Com_Data(0x0092,0x0600);	//Panel Interface Contral 2.(0x0000)

	Lcd_Write_Com_Data(0x0007,0x0133);	//(0x0173)
	Lcd_Write_Com(0x00,0x22);
	LCD_CS =1;  //关闭片选使能

}	  
