/*
	BootUpCard开机卡主程序
*/

#include  "stc15f2k60s2.h"		  //STC15头文件
#include  "Definition.h"	    		  //宏定义 常用函数
#include  "../Common/Sleep.h"				  //延时函数
#include  "../HardWare/EEPROM24C02/EEPROM24C02.h"
#include  "../HardWare/SerialPort/Uart.h"
#include  "string.h"
#include  "stdio.h"
#include  "stdlib.h"
#include  "../HardWare/GUI/GUI.h"
#include  "../HardWare/SPI/SPI.h"
#include  "../HardWare/LCD/LCD.h"
#include  "../HardWare/User/Test.h"

//是否配网指示灯
sbit ledInit = P4 ^ 1;
//网络指示灯
sbit ledNet = P5 ^ 5;
//蜂鸣器
sbit beep = P3 ^ 4;
//重置按钮
sbit initKey = P3 ^ 3;
//开机
sbit bootUpComputer = P1 ^ 5;
//重启
sbit restartComputer = P1 ^ 4;
extern u8 xdata RX_buffer[tbuf];
extern u8 RX_num;				 //接收计数变量
//esp8266工作模式  1:station 2:ap 3:ap+station
//u8 code esp_at[] = "AT\r\n";                  // 握手连接指令，返回"OK"
//u8 code esp_cwmode[] = "AT+CWMODE=2\r\n";     // 设置ESP8266的工作模式3 AP+Station，返回"OK"或者"no change" 
u8 code esp_cifsr[] = "AT+CIPAPMAC?\r\n";// "AT+CWLIF\r\n";//"AT+CIFSR\r\n";         // 本机IP地址查询指令
//u8 code esp_cipsend[] = "AT+CIPSEND=3\r\n";   // 设置发送数据长度
//u8 code esp_test[] = "hjj\r\n";   			//  数据内容
//u8 code esp_cipserver[] = "AT+CIPSERVER=1,9999\r\n";  //建立TCP服务器					
//u8 code esp_cipmux[] = "AT+CIPMUX=1\r\n";   			//打开多连接	
//u8 code esp_rst[] = "AT+RST\r\n"; 					// 软件复位
//u8 code esp_cwsap[] = "AT+CWSAP=\"BootUpCard-HanJunJun\",\"1234567890\",11,3\r\n";//设置ap参数
//u8 code esp_cipsta[] = "AT+CIPSTART=\"TCP\",\"112.74.51.95\",8888\r\n"; //连接到TCP服务器，返回“Linked” 
//u8 code esp_cwjap[] = "AT+CWJAP=\"app-a\",\"1234567890\"\r\n";  //连接到WiFi热点或无线路由上，MMA为无线路由名称，86081573为密码；连接成功返回“OK”   
//u8 code esp_cwjap[] = "AT+CWJAP=\"app\",\"88888888\"\r\n";  //连接到WiFi热点或无线路由上，MMA为无线路由名称，86081573为密码；连接成功返回“OK” 
//u8 code esp_cwmode_staion[] = "AT+CWMODE=1\r\n";             // 设置ESP8266的工作模式1 Station，返回"OK"或者"no change"
//u8 code esp_send_idNumber[] = "AT+CIPSEND=0,17\r\n";   // ap模式发送给手机端
u8 esp_macAddress[17];   			//ap模式发送mac地址给手机端
//u8 eeprom_connectAP[50];//存储在eeprom中的连接wifi at 指令
//u8 eeprom_connectAP[50] = { 0 };//存储在eeprom中的连接wifi at 指令
u8 eeprom_connectAP[50] = "AT+CWJAP=\"app-a\",\"1234567890\"\r\n";//存储在eeprom中的连接wifi at 指令
unsigned int x = 10;
unsigned int y = 5;

/*
	真对 IAP15W4K61S4   STC15W4K56S4 系列 IO口初始化
	io口初始化 P0 P1 P2 P3 P4 为准双向IO口
	注意: STC15W4K32S4系列的芯片,上电后所有与PWM相关的IO口均为
	高阻态,需将这些口设置为准双向口或强推挽模式方可正常使用
	相关IO: P0.6/P0.7/P1.6/P1.7/P2.1/P2.2
	P2.3/P2.7/P3.7/P4.2/P4.4/P4.5
*/
void IO_init(void)
{
	P0M0 = 0X00;
	P0M1 = 0X00;

	P1M0 = 0X00;
	P1M1 = 0X00;

	P2M0 = 0X00;
	P2M1 = 0X00;

	P3M0 = 0X00;
	P3M1 = 0X00;

	P4M0 = 0X00;
	P4M1 = 0X00;
}

/*
	蜂鸣器响一下
*/
void Beep() {
	beep = 0;
	delay1ms(50);
	beep = 1;
	delay1ms(50);
	beep = 0;
	delay1ms(50);
	beep = 1;
	delay1ms(50);
}

u8* my_strcat(u8* str1, u8* str2)
{
	u8* pt = str1;
	while (*str1 != '\0') str1++;
	while (*str2 != '\0') *str1++ = *str2++;
	*str1 = '\0';
	return pt;
}

void Reset(void) {
	((void (code*) (void)) 0x0000) ();
}



/*
	打印日志到TF屏幕和pc串口1

*/
void ConsoleLog(unsigned int x1, unsigned int* y1, unsigned char* str, unsigned int dcolor, unsigned int bgcolor)
{
	POINT_COLOR = dcolor;
	//这段代码有点问题
	//把str复制到dest
	/*unsigned char* dest;
	dest = (unsigned char*)malloc(strlen(str) / 2 + 1);
	memcpy(dest, str, strlen(str) + 1);
	GUI_sprintf_hzstr16s(x1, y1, dest, dcolor, bgcolor);*/
	//*y1 = 1;
	//x1 = 1;
	LCD_ShowString(x1, y1, 16, str, 1);
	dcolor = 1;
	bgcolor = 1;
	PrintLog("\r\n");
	PrintLog(str);
	PrintLog("\r\n");

	/*unsigned char* dest;
	unsigned char* dest1;
	memcpy(dest, str, strlen(str) + 1);
	memcpy(dest1, dest, strlen(dest) + 1);
	GUI_sprintf_hzstr16s(x1, y1, dest, dcolor, bgcolor);
	PrintLog(dest1);
	PrintLog("\r\n");*/
}

u8* Split(u8* str, const char* s, u16 i)
{
	u16 tick = 0;
	//原样输出str
	ConsoleLog(x, &y, "Original string:", BLUE, BLACK);
	ConsoleLog(x, &y, str, BLUE, BLACK);
	//第一个值
	str = strtok(str, s);
	if (i == 0)
	{
		ConsoleLog(x, &y, str, BLUE, BLACK);
		return  (unsigned char*)str;
	}
	//第二个值
	str = strtok(NULL, s);
	while (str != NULL) {
		tick++;
		if (tick == i) {
			//索引为i的值
			ConsoleLog(x, &y, "-------Final result-------", BLUE, BLACK);
			ConsoleLog(x, &y, str, BLUE, BLACK);
			ConsoleLog(x, &y, "-------Final result-------", BLUE, BLACK);
			return /* (unsigned char*)*/str;
		}
		else
		{
			ConsoleLog(x, &y, str, BLUE, BLACK);
		}
		str = strtok(NULL, s);
	}
}

/*
	指定字符串与缓存数组数据进行数据比较
	*p 要比较的指定字符串指针数据
	返回：1 数据一致  0 数据不一致
*/
//u8 DataCompare(u8* p)
//{
//	if (strstr(RX_buffer, p) != NULL)
//		return 1;
//	else
//		return 0;
//}

/*
	判断a字符串中是否包含b
	返回：1 数据一致  0 数据不一致
*/
u8 StrContains(u8* a, u8* b)
{
	if (strstr(a, b) != NULL)
		return 1;
	else
		return 0;
}

/*
	清理串口1的缓存数据
*/
void ClearRxBuffer()
{
	memset(RX_buffer, 0, tbuf);//清缓存数据
	RX_num = 0;				   //接收计数变量清0
}

#define u8 unsigned char
#define u16 unsigned int

#define CMD_IDLE    0		 //空闲模式
#define CMD_READ    1		 //IAP字节读命令
#define CMD_PROGRAM 2		 //IAP字节编程命令
#define CMD_ERASE   3		 //IAP字节擦除命令

#define ENABLE_IAP  0X82     //if SYSCLK<20MHz

//测试地址
#define IAP_ADDRESS 0X0000	 //从手册上可以看出 STC15F2K60S2的EEPROM地址是0x0000- 0x03ff	正好是1k的地址空间  


//关闭IAP
void IapIdle()
{
	IAP_CONTR = 0;		 //关闭IAP功能
	IAP_CMD = 0;		 //清除命令寄存器
	IAP_TRIG = 0;		 //清楚触发寄存器
	IAP_ADDRH = 0X80;	 //将地址设置到非IAP区域
	IAP_ADDRL = 0;
}

//从ISP/IAP/EEPROM区域读取一个字节
u8 IapReadByte(u16 addr)
{
	u8 dat;				 //数据缓冲区
	IAP_CONTR = ENABLE_IAP;	 //使能IAP	   同时设置等待时间
	IAP_CMD = CMD_READ;		 //设置IAP命令
	IAP_ADDRL = addr;		 //设置IAP低地址
	IAP_ADDRH = addr >> 8;		 //设置IAP高地址
	IAP_TRIG = 0X5a;			 //写触发命令（0x5a）	 写触摸命令以后  命令才会生效 手册713页
	IAP_TRIG = 0Xa5;			 //写触发命令（0xa5）
	delay1ms(10);				 //等待ISP/IAP/EEPROM操作完成
	dat = IAP_DATA;			 //读ISP/IAP/EEPROM数据
	IapIdle();				 //关闭IAP功能

	return dat;			 //返回
}

//写一个字节数据到ISP/IAP/EEPROM区域
void IapProgramByte(u16 addr, u8 dat)
{
	IAP_CONTR = ENABLE_IAP;	 //使能IAP	   同时设置等待时间
	IAP_CMD = CMD_PROGRAM;	 //设置IAP命令
	IAP_ADDRL = addr;		 //设置IAP低地址
	IAP_ADDRH = addr >> 8;		 //设置IAP高地址
	IAP_DATA = dat;			 //写ISP/IAP/EEPROM数据
	IAP_TRIG = 0X5a;			 //写触发命令（0x5a）    写触摸命令以后  命令才会生效 手册713页
	IAP_TRIG = 0Xa5;			 //写触发命令（0xa5）
	delay1ms(10);				 //等待ISP/IAP/EEPROM操作完成
	IapIdle();				 //关闭IAP功能  
}

//扇区擦除
void IapEraseSector(u16 addr)
{
	IAP_CONTR = ENABLE_IAP;	 //使能IAP	 同时设置等待时间
	IAP_CMD = CMD_ERASE;	     //设置IAP命令
	IAP_ADDRL = addr;		 //设置IAP低地址
	IAP_ADDRH = addr >> 8;		 //设置IAP高地址
	IAP_TRIG = 0X5a;			 //写触发命令（0x5a）	 写触摸命令以后  命令才会生效 手册713页
	IAP_TRIG = 0Xa5;			 //写触发命令（0xa5）
	delay1ms(10);				 //等待ISP/IAP/EEPROM操作完成
	IapIdle();				 //关闭IAP功能 
}

/*
	重置ESP8266
*/
void ResetEsp8266()
{
	//重置esp8266
	ConsoleLog(x, &y, "Start Reset esp8266!", BLUE, BLACK);
	while (1)
	{
		ClearRxBuffer();
		delay1ms(300);
		Uart2SendStr("AT+RST\r\n");	   //串口2对wifi模块发送握手指令 即AT
		delay1ms(300);
		ConsoleLog(x, &y, "Reset esp8266 return:", BLUE, BLACK);
		ConsoleLog(x, &y, RX_buffer, BLUE, BLACK);
		if (StrContains(RX_buffer, "ready"))
		{
			
			ConsoleLog(x, &y, "ESP8266 Reset successfully!", GREEN, BLACK);
			ClearRxBuffer();
			break;
		}
		else
		{
			
			ConsoleLog(x, &y, "ESP8266 Reset failed!", RED, BLACK);
			ClearRxBuffer();
		}
		delay1ms(600);
	}
}

/*
	配置为AP模式-开放热点供其他人连接
*/
void InitESP8266ToAPMode()
{
	//发送AT 进行握手
	while (1)
	{
		Uart2SendStr("AT\r\n");	   //串口2对wifi模块发送握手指令 即AT
		delay1ms(2500);
		
		if (StrContains(RX_buffer, "OK"))
		{
			
			ConsoleLog(x, &y, "MCU connect success with BootUpCard!", GREEN, BLACK);
			ClearRxBuffer();
			break;
		}
		else
		{
			
			ConsoleLog(x, &y, "MCU connect fail with BootUpCard!", RED, BLACK);
		}
		delay1ms(600);
	}

	//配置wifi工作模式为ap+sta模式
	while (1)
	{
		Uart2SendStr("AT+CWMODE=2\r\n");	   //串口2对wifi模块工作模式进行设置  
		delay1ms(1000);
		if (StrContains(RX_buffer, "OK") || StrContains(RX_buffer, "no change"))
		{
			
			ConsoleLog(x, &y, "Set mode success as AP+Station with BootUpCard!", GREEN, BLACK);
			ClearRxBuffer();
			break;
		}
		else
		{
			
			ConsoleLog(x, &y, "Set mode fail as AP+Station with BootUpCard!", RED, BLACK);
		}
		delay1ms(600);
	}

	//配置ap参数
	while (1)
	{
		//Uart2SendStr(esp_cwsap);
		Uart2SendStr("AT+CWSAP=\"BootUpCard-HanJunJun\",\"1234567890\",11,3\r\n");
		delay1ms(1000);
		if (StrContains(RX_buffer, "OK") || StrContains(RX_buffer, "no change"))
		{
			
			ConsoleLog(x, &y, "Set AP params success in BootUpCard!", GREEN, BLACK);
			ClearRxBuffer();
			break;
		}
		else
		{
			
			ConsoleLog(x, &y, "Set AP params fail in BootUpCard!", RED, BLACK);
		}
		delay1ms(600);
	}

	//配置wifi为多路可连接模式
	while (1)
	{
		Uart2SendStr("AT+CIPMUX=1\r\n");	   //串口2对wifi模块 设置多连接 （多路连接模式）
		delay1ms(1000);
		if (StrContains(RX_buffer, "OK"))
		{
			
			ConsoleLog(x, &y, "Multiplex connection mode set successfully!", GREEN, BLACK);
			ClearRxBuffer();
			break;
		}
		else
		{
			
			ConsoleLog(x, &y, "Multiplex connection mode set failed!", RED, BLACK);
		}
		delay1ms(600);
	}

	//建立TCP服务器监听端口
	while (1)
	{
		Uart2SendStr("AT+CIPSERVER=1,9999\r\n");	   //串口2设置wifi模块 为TCP服务器模式，并配置端口为5000 
		delay1ms(1000);
		if (StrContains(RX_buffer, "OK"))
		{
			
			ConsoleLog(x, &y, "Create TCP sever is successfully!", GREEN, BLACK);
			ClearRxBuffer();
			break;
		}
		else
		{
			
			ConsoleLog(x, &y, "Create TCP sever is failed!", RED, BLACK);
		}
		delay1ms(600);
	}

	//查询模块ip
	while (1)
	{
		const char ab[2] = "\"";//3
		u8* macAddress = 1;
		//char src[100]="";
		ClearRxBuffer();
		delay1ms(1000);
		Uart2SendStr(esp_cifsr);	   //串口2查询wifi模块 当前ip地址 
		delay1ms(1000);
		if (StrContains(RX_buffer, "OK"))
		{
			ConsoleLog(x, &y, "BootUpCard current IP is:", BLUE, BLACK);
			ConsoleLog(x, &y, RX_buffer, BLUE, BLACK);
			delay1ms(1000);
			//获取硬件序列号
			macAddress = Split(RX_buffer, ab, 1);
			//原始值
			ConsoleLog(x, &y, "-------Original Value-------", BLUE, BLACK);
			ConsoleLog(x, &y, RX_buffer, BLUE, BLACK);
			ConsoleLog(x, &y, "-------Original Value-------", BLUE, BLACK);
			//数组索引1的值
			ConsoleLog(x, &y, "-------Split Value-------", BLUE, BLACK);
			ConsoleLog(x, &y, macAddress, BLUE, BLACK);
			ConsoleLog(x, &y, "-------Split Value-------", BLUE, BLACK);
			//赋值给新的指针
			memcpy(esp_macAddress, macAddress, strlen(macAddress) + 1);
			ConsoleLog(x, &y, "-------New Value-------", BLUE, BLACK);
			ConsoleLog(x, &y, esp_macAddress, BLUE, BLACK);
			ConsoleLog(x, &y, "-------New Value-------", BLUE, BLACK);
			//老的值
			ConsoleLog(x, &y, "-------Old Value-------", BLUE, BLACK);
			ConsoleLog(x, &y, macAddress, BLUE, BLACK);
			ConsoleLog(x, &y, "-------Old Value-------", BLUE, BLACK);
			ClearRxBuffer();
			break;
		}
		else
		{
			ConsoleLog(x, &y, "Query IP Error!", RED, BLACK);
			ClearRxBuffer();
		}
		delay1ms(1000);
	}
}

/*
	重连阿里云服务器-连接wifi和远程阿里云服务器
*/
void RetryConnectServer()
{
	u16 ii = 0;
	u16 tt = 0;
	//连接热点wifi  wifi名 密码  如果失败 延时继续连接
	u8 newLine[] = "\r\n";
	u8* eeprom_config = my_strcat(eeprom_connectAP, newLine);//之前这段代码写到while循环里，导致每次都拼接\r\n，at指令错误，所以失败卡了很久。
retryConnect:
	while (1)
	{
		ConsoleLog(x, &y, "Read the wifi AT command in the current EEPROM:", BLUE, BLACK);
		//delay1ms(2000);//测试用的，方便在pc端查看十六进制的数据
		ConsoleLog(x, &y, eeprom_config, BLUE, BLACK);
		//delay1ms(3000);//测试用的，方便在pc端查看十六进制的数据
		Uart2SendStr(eeprom_config);	   //串口2发送 指点wifi名 密码 等待模块连接 
		delay1ms(3000);
		ConsoleLog(x, &y, "connect wifi result value:", BLUE, BLACK);
		//s延迟一段时间等待esp8266执行完成
	//retry1:
		ConsoleLog(x, &y, RX_buffer, BLUE, BLACK);
		if (StrContains(RX_buffer, "OK"))
		{
			
			ConsoleLog(x, &y, "Connection specified WIFI successful!", GREEN, BLACK);
			ClearRxBuffer();
			break;
		}
		/*else if (StrContains(RX_buffer, "busy"))
		{
			ConsoleLog(x, &y, "Retry get connect information...", GREEN, BLACK);
			delay1ms(1000);
			goto retry1;
		}*/
		else
		{
			if (ii > 10) {
				goto retryConnect;
			}
			
			ConsoleLog(x, &y, "Connection specified WIFI failed!", RED, BLACK);
			ClearRxBuffer();
			ii++;
		}
		delay1ms(600);
	}

	//配置连接阿里云服务器
	while (1)
	{
		Uart2SendStr("AT+CIPSTART=\"TCP\",\"112.74.51.95\",8800\r\n");	   //wifi模块配置 服务器 IP 和端口
		delay1ms(2000);
		if (StrContains(RX_buffer, "OK"))
		{
			
			ConsoleLog(x, &y, "Connection remote TCP sever is successfully!", GREEN, BLACK);
			ClearRxBuffer();
			break;
		}
		else
		{
			if (tt > 10) {
				goto retryConnect;
			}
			
			ConsoleLog(x, &y, "Connection remote TCP sever is failed!", RED, BLACK);
			tt++;
		}
		delay1ms(1000);
	}
}

/*
	配置为Station模式-连接其他人的热点，然后连接阿里云服务器
*/
void InitESP8266ToStationMode()
{
	ConsoleLog(x, &y, "Init esp8266 to STATION mode!", BLUE, BLACK);
	//重置esp8266
	ResetEsp8266();
	delay1ms(1000);
	//发送AT 进行握手
	while (1)
	{
		Uart2SendStr("AT\r\n");	   //串口2对wifi模块发送握手指令 即AT
		delay1ms(2000);
		if (StrContains(RX_buffer, "OK"))
		{
			
			ConsoleLog(x, &y, "MCU connect success with BootUpCard!", GREEN, BLACK);
			ClearRxBuffer();
			break;
		}
		else
		{
			
			ConsoleLog(x, &y, "MCU connect fail with BootUpCard!", RED, BLACK);
		}
		delay1ms(600);
	}

	//配置wifi工作模式为Stationi模式
	while (1)
	{
		Uart2SendStr("AT+CWMODE=1\r\n");
		delay1ms(2000);
		if (StrContains(RX_buffer, "OK") || StrContains(RX_buffer, "no change"))
		{
			
			ConsoleLog(x, &y, "Set mode success as Station with BootUpCard!", GREEN, BLACK);
			ClearRxBuffer();
			break;
		}
		else
		{
			
			ConsoleLog(x, &y, "Set mode fail as Station with BootUpCard!", RED, BLACK);
		}
		delay1ms(600);
	}
	//连接wifi和远程阿里云服务器
	RetryConnectServer();
	//查询模块ip
	while (1)
	{
		delay1ms(200);
		Uart2SendStr(esp_cifsr);	   //串口2查询wifi模块 当前ip地址 
		delay1ms(2000);
		if (StrContains(RX_buffer, "OK"))
		{
			ConsoleLog(x, &y, "BootUpCard current IP is:", BLUE, BLACK);
			ConsoleLog(x, &y, RX_buffer, GREEN, BLACK);
			ClearRxBuffer();
			delay1ms(1000);
			break;
		}
		else
		{
			ConsoleLog(x, &y, "Query IP Error!", RED, BLACK);
		}
		delay1ms(1000);
	}
}

//const char s[2] = ",";

/*
	主程序
*/
void main()
{
	/*while (1) {
		if (initKey == 0) {
			delay1ms(200);
			if (initKey == 0) {
				bootUpComputer = 0;
				delay1ms(1000);
				bootUpComputer = 1;
				Beep();
			}
		}
		delay1ms(500);
	}*/
	//成员和属性定义
	u16  i = 0;
	u16  t = 0;
	u16 sp = 0;
	//心跳包发送的间隔时间
	unsigned int timeCount = 0;
	unsigned char tempdata;
	//针对 IAP15W4K61S4 IO口初始化
	IO_init();
	//串口初始化
	UartInit();
	//串口1中断打开
	ES = 1;
	//串口2中断打开
	IE2 = 0x01;

	//中断1
	//EX1 = 1;   //开启外部中断0  即P3.2
	//IT1 = 0;	//低电平触发
	//总中断打开
	EA = 1;

	//液晶屏初始化
	LCD_Init();
	//Init_SPI();
	//SPI_Speed(3);
	//测试屏幕
	DrawTestPage("Remote Boot Card");
	POINT_COLOR = MAGENTA;
	BACK_COLOR = BLUE;
	LCD_ShowString(x, &y, 16, "My remote boot card is running...", 1);
	LCD_ShowString(x, &y, 16, "This project was developed by hjj!", 1);
	
	LCD_ShowString(x, &y, 16, "Author's email:1454055505@qq.com", 1);
	POINT_COLOR = BLUE;

	//重置配置
	/*LCD_ShowString(x, &y, 16, "Please press and hold for 5 seconds to reset the system!", 1);
	LCD_ShowString(x, &y, 16, "Start to detect key events...", 1);
	while (1) {
		if (sp > 6) {
			break;
		}
		if (initKey == 0) {
			delay1ms(5000);
			if (initKey == 0) {
				ConsoleLog(x, &y, "Reset EEPROM configuration!", BLUE, BLACK);
				IapEraseSector(IAP_ADDRESS);
				Beep();
				break;
			}
			else {
				ConsoleLog(x, &y, "Long press for less than 5 seconds!", BLUE, BLACK);
			}
		}
		delay1ms(1000);
		sp++;
	}
	LCD_ShowString(x, &y, 16, "End detection of key events...", 1);*/

	
	//delay1ms(5200);
	//DrawTestPage("全动电子综合测试程序");
	//Gui_StrCenter(0, 30, RED, BLUE, "全动电子", 16, 1);//居中显示
	//Gui_StrCenter(0, 60, RED, BLUE, "综合测试程序", 16, 1);//居中显示	
	//Gui_StrCenter(0, 90, YELLOW, BLUE, "3.2' ILI9341 240X320", 16, 1);//居中显示
	//Gui_StrCenter(0, 120, BLUE, BLUE, "xiaoFeng@QDtech 2014-02-25", 16, 1);//居中显示
	//delay_ms(1500);
	//delay_ms(1500);
	//main_test();
	//delay1ms(5200);
	//English_Font_test();

	//delay1ms(5200);
	//main_test();
	//English_Font_test();
	//ConsoleLog(x, &y, "My remote boot card is running...", YELLOW, BLACK);
	//ConsoleLog(x, &y, "This project was developed by hjj!", RED, BLACK);
	//ConsoleLog(x, &y, "Author's email:1454055505@qq.com", GREEN, BLACK);
	//等待ESP8266启动
	delay1ms(1888);
	//开始执行主程序
	//测试代码-清除EEPROM
	//IapEraseSector(IAP_ADDRESS);
	//ConsoleLog(x, &y, "Test Erase EEPROM is successfully!", GRAY, BLACK);

	//1.判断EEPROM是否存在wifi配置
	//tempdata = IapReadByte(IAP_ADDRESS + i);
	//if (tempdata == 0xff)		//判断是否都等于0xff  如果等于说明上面擦除成功了
	//{
	//	//不存在wifi配置
	//	//初始化esp8266ap为AP模式
	//	InitESP8266ToAPMode();
	//	ClearRxBuffer();
	//	//发送硬件序列号给APP
	//	while (1)
	//	{
	//		//发送硬件序列号
	//		ConsoleLog(x, &y, "Send MAC Address...", BLUE, BLACK);
	//		ConsoleLog(x, &y, esp_macAddress, YELLOW, BLACK);
	//		Uart2SendStr("AT+CIPSEND=0,17\r\n");
	//		delay1ms(200);
	//		Uart2SendStr(esp_macAddress);
	//		delay1ms(800);
	//		if (StrContains(RX_buffer, "SEND OK"))
	//		{
	//			
	//			ConsoleLog(x, &y, "Send hardware idNumber successfully!", GREEN, BLACK);
	//			ClearRxBuffer();
	//			break;
	//		}
	//		else
	//		{
	//			
	//			ConsoleLog(x, &y, "Send hardware idnumber failed!", RED, BLACK);
	//		}
	//		delay1ms(1000);
	//	}

	//	//接收来自APP的数据-配置开机卡连接的无线网络--app会将连接WIFI的AT指令发过来
	//	while (1)
	//	{
	//		//测试代码-接收数据存入EEPROM
	//		//u8 testdata[50] = "+IPD,1:AT+CWJAP=\"app-a\",\"1234567890\"\r\n";
	//		//接收手机端数据
	//		if (StrContains(RX_buffer, "+IPD"))//if (DataCompare(testdata, "+IPD"))
	//		{
	//			const char a[2] = ":";
	//			u8* receiveData = Split(RX_buffer, a, 1);//u8* receiveData = Split(testdata, a, 1);
	//			ConsoleLog(x, &y, "Receive data from android app:", BLUE, BLACK);
	//			ConsoleLog(x, &y, receiveData, BLUE, BLACK);
	//			ConsoleLog(x, &y, "Configuration wifi Information:", BLUE, BLACK);
	//			ConsoleLog(x, &y, receiveData, BLUE, BLACK);
	//			//获取APP端设置的WIFI名称和密码
	//			if (StrContains(receiveData, "AT+CWJAP="))
	//			{
	//			RetryWriteEEPROM:
	//				
	//				//擦除地址对应的扇区  一个扇区为512个字节
	//				IapEraseSector(IAP_ADDRESS);
	//				ConsoleLog(x, &y, "Erase EEPROM is successfully!", GREEN, BLACK);
	//				POINT_COLOR = BLUE;
	//				//写wifi at指令到EEPROM
	//				ConsoleLog(x, &y, "-------Write AT Command into EEPROM-------", BLUE, BLACK);
	//				for (i = 0; i < strlen(receiveData); i++)
	//				{
	//					Uart1SendByte(receiveData[i]);
	//					delay1ms(50);
	//					IapProgramByte(IAP_ADDRESS + i, receiveData[i]);
	//					ConsoleLog(x, &y, "EEPROM config set one!", BLUE, BLACK);
	//				}
	//				ConsoleLog(x, &y, "-------Write AT Command into EEPROM-------", BLUE, BLACK);

	//				//判断是否写成功-没写成功跳回到擦出继续重新写入
	//				ConsoleLog(x, &y, "-------Check AT Command from EEPROM-------", BLUE, BLACK);
	//				for (i = 0; i < strlen(receiveData); i++)
	//				{
	//					if (IapReadByte(IAP_ADDRESS + i) != receiveData[i])
	//					{
	//						
	//						ConsoleLog(x, &y, "The EEPROM config data is check failed!", RED, BLACK);
	//						goto RetryWriteEEPROM;
	//					}
	//				}
	//				ConsoleLog(x, &y, "-------Check AT Command from EEPROM-------", BLUE, BLACK);

	//				ConsoleLog(x, &y, "Save ESP8266 AP WifiName and WifiPassword in Configuration is successfluy!", GREEN, BLACK);
	//				//测试代码-延迟一会方便查看日志
	//				delay1ms(1000);
	//				break;
	//			}
	//		}
	//		else
	//		{
	//			
	//			//没有收到app端发来的数据
	//			ConsoleLog(x, &y, "-------Receive data from app-------", BLUE, BLACK);
	//			ConsoleLog(x, &y, RX_buffer, BLUE, BLACK);
	//			ConsoleLog(x, &y, "-------Receive data from app-------", BLUE, BLACK);
	//			ConsoleLog(x, &y, "Receive data is not an valid command!", BLUE, BLACK);
	//		}
	//		delay1ms(1000);
	//	}
	//}

	////2.读取wifi配置
	//POINT_COLOR = BLUE;
	//ConsoleLog(x, &y, "BootUpCard already initialized,start reading wifi configuration...", BLUE, BLACK);
	//while (1) {
	//	if (t < 1024) {
	//		u8 tempdata = IapReadByte(IAP_ADDRESS + t);
	//		if (tempdata == 0xff) {
	//			//ConsoleLog(x, &y, "EEPROM config is over!", GREEN, BLACK);
	//			break;
	//		}
	//		else {
	//			/*eeprom_connectAP[t] = tempdata;
	//			Uart1SendByte(eeprom_connectAP[t]);*/
	//			/**eeprom_connectAP = &tempdata;
	//			Uart1SendByte(eeprom_connectAP);
	//			eeprom_connectAP++;*/
	//			eeprom_connectAP[t] = tempdata;
	//			//ConsoleLog(x, &y, "EEPROM config get one!", GREEN, BLACK);
	//		}
	//		t++;
	//	}
	//	else
	//	{
	//		break;
	//	}
	//}
	POINT_COLOR = BLUE;
	ConsoleLog(x, &y, "Read EEPROM config is successfully!The data is:", GREEN, BLACK);
	delay1ms(1000);//延迟几秒方便pc端查看eeprom配置
	ConsoleLog(x, &y, eeprom_connectAP, YELLOW, BLACK);
	delay1ms(1000);
	//初始化esp8266ap为Station模式
	InitESP8266ToStationMode();

	//根据从阿里云服务器接收的数据指令执行特定的操作
	ConsoleLog(x, &y, "Start receiving data from Aliyun remote cloud server...", BLUE, BLACK);

	while (1)
	{
		if (initKey == 0) {
			delay1ms(20);
			if (initKey == 0) {
				ConsoleLog(x, &y, "keyup boot up your computer!", GREEN, BLACK);
				//关机或者开机
				bootUpComputer = 0;
				delay1ms(1000);
				bootUpComputer = 1;
				Beep();
			}
		}
		//接收服务端发来的数据
		ConsoleLog(x, &y, "Receive data:", GREEN, BLACK);
		ConsoleLog(x, &y, RX_buffer, GREEN, BLACK);
		if (StrContains(RX_buffer, "+IPD,1:A"))
		{
			
			ConsoleLog(x, &y, "Boot up your computer!", GREEN, BLACK);
			ledInit = !ledInit;
			ledNet = !ledNet;
			//开机或关机
			bootUpComputer = 0;
			delay1ms(1000);
			bootUpComputer = 1;
			Beep();
		}
		else if (StrContains(RX_buffer, "+IPD,1:B"))
		{
			
			ConsoleLog(x, &y, "Forced shutdown your computer!", GREEN, BLACK);
			ledInit = 0;
			ledNet = 0;
			//强关机
			bootUpComputer = 0;
			delay1ms(6000);
			bootUpComputer = 1;
			Beep();
		}
		else
		{
			ConsoleLog(x, &y, "No matching commands were found!", GREEN, BLACK);
		}
		POINT_COLOR = BLUE;
		//清空缓存
		ClearRxBuffer();
		if (timeCount == 60) {
			timeCount = 0;
			//发送心跳数据包
			Uart2SendStr("AT+CIPSEND=3\r\n");	   //配置测试数据字节长度
			delay1ms(200);
			Uart2SendStr("hjj");	   //发送测试数据
			delay1ms(800);
			if (StrContains(RX_buffer, "SEND OK"))
			{
				
				ConsoleLog(x, &y, "Successful to send of TCP heartbeat packet!", GREEN, BLACK);
			}
			else
			{
				
				ConsoleLog(x, &y, "Failed to send TCP heartbeat packet!", RED, BLACK);
				RetryConnectServer();
			}
			//清空缓存
			ClearRxBuffer();
		}
		delay1ms(1000);
		timeCount++;
	}
}


///*
//	重置按键检测
//*/
//void zhong() interrupt 0
//{
//	ledNet = !ledNet;
//}

//#include  "stc15f2k60s2.h"		  //STC15头文件
//sbit  led = P5 ^ 5;
//
//
//main()
//{
//	EX1 = 1;   //??????0  ?P3.2
//	IT1 = 0;	//?????
//	EA = 1;	//开启总中断
//	while (1);
//
//}
//
//
//void zhong() interrupt 0
//{
//	led = !led;
//}