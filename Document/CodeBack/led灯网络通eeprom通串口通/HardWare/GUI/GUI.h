#ifndef GUI_H
#define GUI_H
#include  "Definition.h"	    		  //宏定义 常用函数



/*GUI函数*/
void GUI_Clear(uint color);	//清屏
void GUI_Point(uchar x, uint y, uint color);//画点
void GUI_HLine(uchar x, uint y, uchar length, uint color);//画横线 
void GUI_SLine(uchar x, uint y, uint high, uint color);//画竖线
void GUI_line(uint x1,uint y1,uint x2,uint y2,uint color);//画直线
//void GUI_DisPicture(uchar x, uint y, uchar length, uint high);//显示图片(图标)
void GUI_DisPicture(unsigned int x,unsigned int y,unsigned int width,unsigned int height,unsigned char pic[]); //显示图片(图标)
void GUI_sprintf_char(unsigned int x,unsigned int y,unsigned char value,unsigned int dcolor,unsigned int bgcolor); //显示英文或数字字符
void GUI_sprintf_hz1616(unsigned short x, unsigned short  y, unsigned char c[2], unsigned int dcolor,unsigned int bgcolor);//显示16x16单个汉字
void GUI_sprintf_hzstr16s(unsigned int x1,unsigned int* y1,unsigned char *str,unsigned int dcolor,unsigned int bgcolor);//显示英文字符串或16X16汉字
																													   //这里的汉字是存在单片机程序里的

void GUI_sprintf_hz3232(unsigned short x, unsigned short  y, unsigned char c[2], unsigned int dcolor,unsigned int bgcolor);//显示32x32单个汉字
void GUI_sprintf_hz3232s(unsigned int x1,unsigned int y1,unsigned char *str,unsigned int dcolor,unsigned int bgcolor);//显示32x32汉字串
void GUI_sprintf_ZM3216(unsigned short x, unsigned short  y, unsigned char c[2], unsigned int dcolor,unsigned int bgcolor);//显示16x32字母

void GUI_box(uint sx,uint sy,uint ex,uint ey,uint color);//画实心矩形
void GUI_rectangle( uchar sx,uint sy,uchar ex,uint ey,uint color);//画空心矩形


void PutGB1616(unsigned int x, unsigned int  y, unsigned char Disp_char[2], unsigned int fColor,unsigned int bColor);  //采集SD卡汉字库 显示16X16点阵汉字

void GUI_sprintf_hzstr16x(unsigned int x1,unsigned int y1,unsigned char *str,unsigned int dcolor,unsigned int bgcolor);	//


void GUI_sprintf_char_h(unsigned int x,unsigned int y,unsigned char value,unsigned int dcolor,unsigned int bgcolor);   //显示横向单个字符
void GUI_sprintf_zmhz16(unsigned int x1,unsigned int y1,unsigned char *str,unsigned int dcolor,unsigned int bgcolor); //显示横向字符串函数

void tks(uchar x,uchar y);
void tkx(uchar x,uchar y);
void tkz(uchar x,uchar y);
void tky(uchar x,uchar y);

/*定义常用颜色的颜色码*/
#define Red      0xf800	//红
#define Yellow   0xffe0	//黄
#define Green    0x07e0	//绿
#define Cyan     0x07ff	//青
#define Blue     0x001f	//蓝
#define Purple   0xf81f	//紫
#define Black    0x0000	//黑
#define White    0xffff	//白
#define Gray     0x7bef	//灰
#define Blue1    0xa5ff //淡蓝
#define Purple1  0x8a9e //淡紫
#define Green1   0x0410 //墨绿
#endif
















