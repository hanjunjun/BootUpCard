#include  "stc15f2k60s2.h"
#include  "Definition.h"	    		  //宏定义 常用函数
#include  "GUI.h"
#include  "../TFT/TFT.h"
#include  "hz16x16.h"
#include  "zifu8x16.h"
#include  "../SdCard/SdCard.h"

extern unsigned long CHINALIB_point;

//程序里 把没有用到的GUI函数 都给屏蔽掉了 

/************************************
清屏
入口参数： b_color是背景颜色。
出口参数: 无
说明：使用背景颜色清除TFT模块屏幕的全部显示内容。
*************************************/
void GUI_Clear(uint color)
{
	int i, j;
	Address_set(0, 0, 239, 319);
	for (i = 0; i < 320; i++)
	{
		for (j = 0; j < 240; j++)
		{
			Lcd_Write_Data16(color);
		}

	}
}

/*********************************************
画点
入口参数： (x，y)是点的坐标，color 是点的颜色。
出口参数: 无
说明：用指定的颜色在指定的坐标位置上画出一个点。
**********************************************/

void GUI_Point(uchar x, uint y, uint color)
{
	Address_setxy(x, y);
	Lcd_Write_Data16(color);
}





/**********************************************************
画横线
入口参数： (x，y)是横线起点坐标，length是横线长度，color 是横线颜色。
出口参数: 无
说明：用指定的颜色在指定位置上画出指定长度的一条横线。
************************************************************/
/*
  void GUI_HLine(uchar x, uint y, uchar length, uint color)
{
	Address_set(x,y,x+length-1,y);
	do
	{
		Lcd_Write_Data16(color);//逐点显示，描出水平线
		length--;
	}while(length);
}

	 */
	 /**********************************************************
	 画竖线
	 入口参数： (x，y)是竖线起点坐标，high 竖线高度，color 是竖线颜色。
	 出口参数: 无
	 说明：用指定的颜色在指定位置上画出指定长度的一条竖线。
	 ************************************************************/
	 /*
	 void GUI_SLine(uchar x, uint y, uint high, uint color)
	 {
		Address_set(x,y,x,y+high-1);
		do
		{
			 Lcd_Write_Data16(color);// 逐点显示，描出垂直线
			 high--;
		}while(high);
	 }
		  */


		  /********************************************************************
		  画直线（可以画任意方向直线，包括横线、竖线、斜线）。
		  入口参数： (x1,y1)起点, (x2,y2)终点, color颜色。
		  出口参数: 无
		  说明：用指定的颜色在指定的两点间画出一条直线。
		  ***********************************************************************/
void GUI_line(uint x1, uint y1, uint x2, uint y2, uint color)
{
	uint t;
	int xerr = 0, yerr = 0, delta_x, delta_y, distance;
	int incx, incy;
	uint row, col;
	delta_x = x2 - x1;//计算坐标增量  
	delta_y = y2 - y1;
	col = x1;
	row = y1;
	if (delta_x > 0) incx = 1;//设置单步方向  
	else
	{
		if (delta_x == 0) incx = 0;//垂直线  
		else { incx = -1; delta_x = -delta_x; }
	}
	if (delta_y > 0)incy = 1;
	else
	{
		if (delta_y == 0) incy = 0;//水平线  
		else { incy = -1; delta_y = -delta_y; }
	}
	if (delta_x > delta_y) distance = delta_x;//选取基本增量坐标轴  
	else distance = delta_y;

	for (t = 0; t <= distance + 1; t++)
	{                                     //画线输出  
		GUI_Point(col, row, color);
		xerr += delta_x;
		yerr += delta_y;
		if (xerr > distance)
		{
			xerr -= distance;
			col += incx;
		}
		if (yerr > distance)
		{
			yerr -= distance;
			row += incy;
		}
	}
}


/**********************************************************
显示图片(图标)
入口参数：(x，y)是开始点的坐标，length是图片长度，high是图片高度。//pic 图片数组的指针
出口参数: 无
说明：用指定位置上显示事先定义的图片。
要显示的图片事先定义在bmp.h中的pic[]数组中，
如果想修改图片大小、内容，请修改bmp.h中的pic[]数组，
建议用Image2Lcd软件将你要显示的图象自动转换为数组数据。
***********************************************************/

/*
void GUI_DisPicture(unsigned int x,unsigned int y,unsigned int width,unsigned int height,unsigned char pic[])
{
 unsigned int i,n;
 Address_set(x,y,x+width-1,y+height-1);
 n=width*height;
 for(i=0;i<n;i++)
 {
  Lcd_Write_Data(pic[i*2+1],pic[i*2]);
 }
}

  */


  /****************************************************
  横屏显示 ——采集SD卡的16x16 点阵字库
  通过该函数调整 进行横屏显示
  *****************************************************/

void PutGB1616_h(unsigned int x, unsigned int  y, unsigned char Disp_char[2], unsigned int fColor, unsigned int bColor)
{

	unsigned char tmp;

	unsigned char tmp_Char_Model[32];
	unsigned int y2, i = 0, j, i2 = 0;
	unsigned char bz;

	y2 = 239 - y;

	// 读字模 
	SD_read_Byte((CHINALIB_point << 9) + (((unsigned long)Disp_char[0] - 0xa0 - 1) * 94 + ((unsigned long)Disp_char[1] - 0xa0 - 1)) * 32, tmp_Char_Model, 32);


	// 显示出来 
	for (bz = 0; bz < 16; bz++)
	{
		Address_set(y2 - bz, x, y2 - bz, x + 15);     //选择坐标位置
		for (j = 0; j < 16; j++)
		{
			tmp = tmp_Char_Model[i];
			if ((tmp & (1 << (7 - i2))) != 0)
			{
				Lcd_Write_Data(fColor >> 8, fColor);
			}
			else {
				Lcd_Write_Data(bColor >> 8, bColor);
			}
			i2++;
			if (i2 == 8) { i2 = 0; i++; }
		}
	}



}


//字符横向显示
//不改变屏的设置基础上 通过函数调整 将字母横向显示

void GUI_sprintf_char_h(unsigned int x, unsigned int y, unsigned char value, unsigned int dcolor, unsigned int bgcolor)
{
	unsigned int y2, i = 0, j, i2 = 0;
	unsigned char bz;
	unsigned char* temp = zifu816;    //temp是*temp的地址  这里temp就是zifu的首地址 

	temp += (value - 32) * 16;   //确定要显示的值
				   //这里用的是ascii表  前32个ascii没有存入zifu库里 所以要减32
				   //并且 每个字符用12个字节显示 所以在乘以12  就是对应的显示位的首地址	

	y2 = 239 - y;								//调整竖向坐标

	for (bz = 0; bz < 16; bz++)
	{
		Address_set(y2 - bz, x, y2 - bz, x + 7);     //选择坐标位置
		for (j = 0; j < 8; j++)
		{

			if ((*temp & (1 << (7 - j))) != 0)
			{
				Lcd_Write_Data(dcolor >> 8, dcolor);
			}
			else {
				Lcd_Write_Data(bgcolor >> 8, bgcolor);
			}
		}
		temp++;
	}

}

//显示横向汉字及字符
void GUI_sprintf_zmhz16(unsigned int x1, unsigned int y1, unsigned char* str, unsigned int dcolor, unsigned int bgcolor)
{
	unsigned char l = 0;
	while (*str)
	{
		if (*str < 0x80)		  //小于128   ascii 都在数组内
		{
			GUI_sprintf_char_h(x1 + l * 8, y1, *str, dcolor, bgcolor);
			l++;
			str++;
		}
		else
		{
			PutGB1616_h(x1 + l * 8, y1, (unsigned char*)str, dcolor, bgcolor);
			str += 2; l += 2;
		}

	}
}
/*********************************************
显示英文或数字字符
入口参数：(x，y) 是显示内容的左上角坐标，c：显示的英文字符，
		  color:颜色,b_color:背景颜色。
出口参数: 无
说明：用指定位置上显示单个英文字符或数字字符。
**********************************************/
void GUI_sprintf_char(unsigned int x, unsigned int y, unsigned char value, unsigned int dcolor, unsigned int bgcolor)
{
	unsigned char i, j;
	unsigned char* temp = zifu816;    //temp是*temp的地址  这里temp就是zifu的首地址 
	//if (x > 240 - 16 || y > 320 - 16)return;
	Address_set(x, y, x + 7, y + 15); //设置区域      

	temp += (value - 32) * 16;   //确定要显示的值
				   //这里用的是ascii表  前32个ascii没有存入zifu库里 所以要减32
				   //并且 每个字符用12个字节显示 所以在乘以12  就是对应的显示位的首地址
	for (j = 0; j < 16; j++)
	{
		for (i = 0; i < 8; i++)		    //先横扫
		{
			if ((*temp & (1 << (7 - i))) != 0)					//将1 左移 然后对应位进行相与 
			{
				Lcd_Write_Data(dcolor >> 8, dcolor);
			}
			else
			{
				Lcd_Write_Data(bgcolor >> 8, bgcolor);
			}

		}
		temp++;
	}
}

/*********************************************
显示16x16单个汉字
入口参数：(x，y) 是显示内容的左上角坐标，*s：英文字符串指针，
		  color:颜色,b_color:背景颜色。
出口参数: 无
说明：在指定位置上显示英文字符串。
**********************************************/

void GUI_sprintf_hz1616(unsigned short x, unsigned short  y, unsigned char c[2], unsigned int dcolor, unsigned int bgcolor) {
	unsigned int i, j, k;
	//c[2] 把要显示的字 以两个字节的形式放入 数组c中
	Address_set(x, y, x + 16 - 1, y + 16 - 1);       //选择坐标位置

	for (k = 0; k < 64; k++) { //64标示自建汉字库中的个数，循环查询内码		  一个汉字需要两个字节
		if ((codeHZ_16[k].Index[0] == c[0]) && (codeHZ_16[k].Index[1] == c[1]))	    //寻找对应汉字

		{
			for (i = 0; i < 32; i++) {										  //32个字节 每个字节都要一个点一个点处理 所以是处理了32X8次
				unsigned short m = codeHZ_16[k].Msk[i];
				for (j = 0; j < 8; j++) {
					if ((m & 0x80) == 0x80) {							//判断是否是要写入点 	 如果是 给字体颜色
						Lcd_Write_Data(dcolor >> 8, dcolor);
					}
					else {											 //如果不是 为颜色点  给颜色
						Lcd_Write_Data(bgcolor >> 8, bgcolor);
					}
					m <<= 1;												 //左移一位  判断下一点
				}
			}
		}
	}
}





/*********************************************
显示英文字符串或16X16汉字
入口参数：(x，y) 是显示内容的左上角坐标，*s：英文字符串指针，
		  color:颜色,b_color:背景颜色。
出口参数: 无
说明：在指定位置上显示英文字符串。
**********************************************/
void GUI_sprintf_hzstr16s(unsigned int x1, unsigned int* y1, unsigned char* str, unsigned int dcolor, unsigned int bgcolor)
{
	unsigned char leng = 0;
	*y1 += 16;
	while (*str)
	{

		if (*str < 0x80)		  //小于128   ascii 都在数组内
		{
			if (x1 > 232) {
				x1 = 0; *y1 += 16;
			}
			if (*y1 > 312)
			{
				*y1 = 0;
				x1 = 0;
				GUI_Clear(0x0000);
			}
			GUI_sprintf_char(x1, *y1, *str, dcolor, bgcolor);//8
			x1 += 8;
			str++;
		}
		else
		{
			GUI_sprintf_hz1616(x1 + leng * 8, y1, (unsigned char*)str, dcolor, bgcolor);
			str += 2; leng += 2;
		}
	}
}



/*********************************************
显示32x32单个汉字
入口参数：(x，y) 是显示内容的左上角坐标，*s：英文字符串指针，
		  color:颜色,b_color:背景颜色。
出口参数: 无
说明：在指定位置上显示英文字符串。
**********************************************/
/*
void GUI_sprintf_hz3232(unsigned short x, unsigned short  y, unsigned char c[2], unsigned int dcolor,unsigned int bgcolor)
{
	unsigned int i,j,k;

	Address_set(x,y,x+32-1, y+32-1);     //选择坐标位置

	for (k=0;k<40;k++) { //15标示自建汉字库中的个数，循环查询内码
	  if ((codeHZ_32[k].Index[0]==c[0])&&(codeHZ_32[k].Index[1]==c[1])){
		for(i=0;i<128;i++) {
		  unsigned short m=codeHZ_32[k].Msk[i];
		  for(j=0;j<8;j++) {
			if((m&0x80)==0x80) {
				Lcd_Write_Data(dcolor>>8,dcolor);
				}
			else {
				Lcd_Write_Data(bgcolor>>8,bgcolor);
				}
			m<<=1;
			}
		  }
		}
	  }
	}

	*/
	/*********************************************
	显示16x32字母
	入口参数：(x，y) 是显示内容的左上角坐标，*s：英文字符串指针，
			  color:颜色,b_color:背景颜色。
	出口参数: 无
	说明：在指定位置上显示英文字符串。
	**********************************************/

	/*
	void GUI_sprintf_ZM3216(unsigned short x, unsigned short  y, unsigned char c[2], unsigned int dcolor,unsigned int bgcolor){
		unsigned int i,j,k;

		Address_set(x,y,x+16-1, y+32-1);     //选择坐标位置

		for (k=0;k<19;k++) { //15标示自建汉字库中的个数，循环查询内码
		  if (codeZM_32[k].Index[0]==c[0]){
			for(i=0;i<64;i++) {
			  unsigned short m=codeZM_32[k].Msk[i];
			  for(j=0;j<8;j++) {
				if((m&0x80)==0x80) {
					Lcd_Write_Data(dcolor>>8,dcolor);
					}
				else {
					Lcd_Write_Data(bgcolor>>8,bgcolor);
					}
				m<<=1;
				}
			  }
			}
		  }
		}
							*/
							/*********************************************
							显示32x32汉字串
							入口参数：(x，y) 是显示内容的左上角坐标，*s：英文字符串指针，
									  color:颜色,b_color:背景颜色。
							出口参数: 无
							说明：在指定位置上显示英文字符串。
							**********************************************/
							/*
							void GUI_sprintf_hz3232s(unsigned int x1,unsigned int y1,unsigned char *str,unsigned int dcolor,unsigned int bgcolor)
								 {
									unsigned char l=0;
									while(*str)
									 {
										if(*str<0x80)		  //小于128   ascii 都在数组内
									   {
										  GUI_sprintf_ZM3216(x1+l*8,y1,(unsigned char*)str,dcolor, bgcolor);
										  str+=1;l+=2;

										}
										else
										{
										 GUI_sprintf_hz3232(x1+l*8,y1,(unsigned char*)str,dcolor, bgcolor);
										 str+=2;l+=4;
										 }
									 }
								}


							 */








							 /*********************************************
							 画实心矩形
							 入口参数： (sx,sy)左上角顶点坐标, (ex,ey)右下角顶点坐标, color颜色
							 出口参数: 无
							 说明：在指定位置上画出实心矩形。
							 **********************************************/


void GUI_box(uint sx, uint sy, uint ex, uint ey, uint color)
{
	int temp, temp1, m, n;
	Address_set(sx, sy, ex, ey);
	n = ex - sx + 1;
	m = ey - sy + 1;

	for (temp = 0; temp < m; temp++)
	{

		for (temp1 = 0; temp1 < n; temp1++)
		{
			Lcd_Write_Data16(color);
		}
	}
}


/*********************************************
画空心矩形
入口参数： (sx,sy)左上角顶点坐标, (ex,ey)右下角顶点坐标, color颜色
出口参数: 无
说明：在指定位置上画出空心矩形。
**********************************************/
/*
void GUI_rectangle( uchar sx,uint sy,uchar ex,uint ey,uint color)
{
	GUI_line(sx,sy,ex,sy,color);
	GUI_line(sx,sy,sx,ey,color);
	GUI_line(ex,sy,ex,ey,color);
	GUI_line(sx,ey,ex,ey,color);
}
  */




  /*********************************************
  读SD卡 字库函数
  入口参数： (x,y)左上角顶点坐标, Disp_char[2]要是显示的汉字数据, color颜色
  出口参数: 无
  说明：CHINALIB_point  此数据 为SD卡里汉字字库的首扇区
		Disp_char[2]    此数据 为SD卡 HZ.TXT 里采集出来的 一个汉字（一个汉字用两个字节表示）

		此程序 主要目的  是将采集出来的汉字GBK码 Disp_char[2]  转换为汉字字库的区位码 并将其显示出来

		汉字字库里存的是 汉字16X16的点阵  16*16=256 一个字节8位 256/8=32 也就是32个字节表示一个字

  **********************************************/
void PutGB1616(unsigned int x, unsigned int  y, unsigned char Disp_char[2], unsigned int fColor, unsigned int bColor)
{

	unsigned char tmp, i, j;

	unsigned char tmp_Char_Model[32];


	/* 设置1个汉字的显示范围 */
	Address_set(x, y, x + 16 - 1, y + 16 - 1);


	/* 读字模 */						  //  得到区值									   得到位数值
	SD_read_Byte((CHINALIB_point << 9) + (((unsigned long)Disp_char[0] - 0xa1) * 94 + ((unsigned long)Disp_char[1] - 0xa1)) * 32, tmp_Char_Model, 32);
	/*
   CHINALIB_point 为采集的 SD字库扇区值  一个扇区512个字节
   CHINALIB_point<<9相当于 CHINALIB_point*2的9次方=CHINALIB_point*512
   到CHINALIB_point的所在的开始字节

   Disp_char[2]	这个为GBK码  一个GBK表示的汉字要用两个字节 所以用数组

   存在SD卡里的 汉字字库 是以区位码形式存入的
   区 是所有汉字由多个区组成  位 就是每一个区有94个字	每个字为一位

   这里是将GBK码 转换为 区位码   GBK码的高低字节分别-0xa1
   即 高字节Disp_char[0]-0xa1代表区位码的区号   低字节Disp_char[1]-0xa1代表区位码的位数

	(unsigned long)Disp_char[0]-0xa1)*94 指定到对应字所在的区
	(unsigned long)Disp_char[0]-0xa1)*94 + ((unsigned long)Disp_char[1]-0xa1) 指定到对应汉字所在区的具体位 即指定汉字位置

	(((unsigned long)Disp_char[0]-0xa1)*94 + ((unsigned long)Disp_char[1]-0xa1))*32
	转换后*32  汉字是16X16=256点 一点代表一位	 256/8=32 所以一个汉字占32个字节
	转换后的区位码*32 指 转到对应字节

	所以
	(CHINALIB_point<<9) + (((unsigned long)Disp_char[0]-0xa1)*94 + ((unsigned long)Disp_char[1]-0xa1))*32
	是将 要转换的GBK码Disp_char[2]转换为区位码 并指向对应的点阵字节位置

   tmp_Char_Model,32); 最后提取指定位置的32个字节 到指定数组 tmp_Char_Model[32] 为以后显示做准备

	*/

	/* 显示出来 */
	for (i = 0; i < 32; i++)
	{
		tmp = tmp_Char_Model[i];

		for (j = 0; j < 8; j++)
		{
			if (tmp & 0x80)
			{
				Lcd_Write_Data16(fColor);
			}
			else
			{
				Lcd_Write_Data16(bColor);
			}
			tmp <<= 1;
		}
	}

}






/*********************************************
显示英文字符串或16X16汉字	 显示的汉字 来自SD卡 汉字库

说明：在指定位置上显示英文字符串。
**********************************************/
void GUI_sprintf_hzstr16x(unsigned int x1, unsigned int y1, unsigned char* str, unsigned int dcolor, unsigned int bgcolor)
{
	unsigned char l = 0;
	while (*str)
	{
		if (*str < 0x80)		  //小于128   ascii 都在数组内
		{
			GUI_sprintf_char(x1 + l * 8, y1, *str, dcolor, bgcolor);
			l += 1;
			str++;
		}
		else
		{
			PutGB1616(x1 + l * 8, y1, (unsigned char*)str, dcolor, bgcolor);
			str += 2; l += 2;
		}
	}
}









/**********************************************
说明  坦克图案
应用  主要应用于游戏显示  红外操作等等
原理  坦克的结构主要由单颜色巨型所组成
	  以一个点为中心点 来画整个坦克
*********************************************/


void tks(uchar x, uchar y)	//x y 为坦克的位置坐标 这个位置 在坦克的中心点
{

	GUI_box(x - 13, y - 15, x - 6, y + 15, Red);	    //左边履带	  宽7 长31
	GUI_box(x - 6, y - 5, x + 6, y + 5, Red);			//中间		  宽11 长13
	GUI_box(x + 6, y - 15, x + 13, y + 15, Red);		//右边履带	  款7  长31
	GUI_box(x - 1, y - 25, x + 1, y - 5, Red);			//炮筒		  宽3  长20


}

void tkx(uchar x, uchar y)
{

	GUI_box(x - 13, y - 15, x - 6, y + 15, Red);
	GUI_box(x - 6, y - 5, x + 6, y + 5, Red);
	GUI_box(x + 6, y - 15, x + 13, y + 15, Red);
	GUI_box(x - 1, y + 5, x + 1, y + 25, Red);
}


void tkz(uchar x, uchar y)
{

	GUI_box(x - 15, y + 6, x + 15, y + 13, Red);
	GUI_box(x - 5, y - 6, x + 5, y + 6, Red);
	GUI_box(x - 15, y - 13, x + 15, y - 6, Red);
	GUI_box(x - 25, y - 1, x - 5, y + 1, Red);
}

void tky(uchar x, uchar y)
{

	GUI_box(x - 15, y + 6, x + 15, y + 13, Red);
	GUI_box(x - 5, y - 6, x + 5, y + 6, Red);
	GUI_box(x - 15, y - 13, x + 15, y - 6, Red);
	GUI_box(x + 5, y - 1, x + 25, y + 1, Red);
}



