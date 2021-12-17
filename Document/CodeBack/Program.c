/*
	BootUpCard开机卡主程序
*/

#include  "stc15f2k60s2.h"		  //STC15头文件
#include  "Definition.h"	    		  //宏定义 常用函数
#include  "../Common/Delay.h"				  //延时函数
#include  "../HardWare/EEPROM24C02/EEPROM24C02.h"
#include  "../HardWare/SerialPort/Uart.h"
#include  "string.h"
#include  "stdio.h"
#include  "stdlib.h"

sbit  led = P5 ^ 5;			 //定义P5.5口 LED指示灯
extern u8 xdata RX_buffer[tbuf];
extern u8 RX_num;				 //接收计数变量
//1:station 2:ap 3:ap+station
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

u8* Split(u8* str, const char* s, u16 i)
{
	u16 tick = 0;
	//原样输出str
	PrintLog("\r\nOriginal string:\r\n");
	PrintLog(str);
	PrintLog("\r\n");
	//第一个值
	str = strtok(str, s);
	if (i == 0)
	{
		PrintLog("\r\n");
		PrintLog(str);
		PrintLog("\r\n");
		return  (unsigned char*)str;
	}
	//第二个值
	str = strtok(NULL, s);
	while (str != NULL) {
		tick++;
		if (tick == i) {
			//索引为i的值
			PrintLog("\r\n");
			PrintLog(str);
			PrintLog("\r\n");
			return  (unsigned char*)str;
		}
		else
		{
			PrintLog("\r\n");
			PrintLog(str);
			PrintLog("\r\n");
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
u8 DataCompare(u8* a, u8* b)
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



//sbit led = P3 ^ 5;		//P3.5口LED灯定义


//延时函数
//void delay1ms(u8 n)
//{
//	while (n--);
//}


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
//
//
//
//void uart_init()
//{
//	SCON = 0x50;        //REN=1允许串行接受状态，串口工作模式2     	   
//	TMOD = 0x00;       //定时器1为模式0（16位自动重载）                
//	AUXR = 0X40;		 //开启1T模式
//	TL1 = (65535 - (11059200 / 4 / 9600));    //设置波特率重装值
//	TH1 = (65535 - (11059200 / 4 / 9600)) >> 8;
//
//	TR1 = 1;        //开启定时器1                                                                      
//}
//
//
//u8 SendData(u8 dat)
//{
//	SBUF = dat;				 //发送数据
//	while (!TI);			 //判断是否发送完成 置位TI
//	TI = 0;					 //清标志位
//	return dat;			 //返回发送的数据 为调用做准备
//}


/*
	重置ESP8266
*/
void ResetEsp8266()
{
	//重置esp8266
	while (1)
	{
		Uart2SendStr("AT+RST\r\n");	   //串口2对wifi模块发送握手指令 即AT
		PrintLog(RX_buffer);
		if (DataCompare(RX_buffer, "OK"))
		{
			PrintLog("\r\nESP8266 Reset successfully!\r\n");
			ClearRxBuffer();
			break;
		}
		else
		{
			PrintLog("ESP8266 Reset failed!\r\n");
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
		if (DataCompare(RX_buffer, "OK"))
		{
			PrintLog("MCU connect success with BootUpCard!\r\n");
			ClearRxBuffer();
			break;
		}
		else
		{
			PrintLog("MCU connect fail with BootUpCard!\r\n");
		}
		delay1ms(600);
	}

	//配置wifi工作模式为ap+sta模式
	while (1)
	{
		Uart2SendStr("AT+CWMODE=2\r\n");	   //串口2对wifi模块工作模式进行设置  
		if (DataCompare(RX_buffer, "OK") || DataCompare(RX_buffer, "no change"))
		{
			PrintLog("Set mode success as AP+Station with BootUpCard!\r\n");
			ClearRxBuffer();
			break;
		}
		else
		{
			PrintLog("Set mode fail as AP+Station with BootUpCard!\r\n");
		}
		delay1ms(600);
	}

	//配置ap参数
	while (1)
	{
		//Uart2SendStr(esp_cwsap);
		Uart2SendStr("AT+CWSAP=\"BootUpCard-HanJunJun\",\"1234567890\",11,3\r\n");
		if (DataCompare(RX_buffer, "OK") || DataCompare(RX_buffer, "no change"))
		{
			PrintLog("Set AP params success in BootUpCard!\r\n");
			ClearRxBuffer();
			break;
		}
		else
		{
			PrintLog("Set AP params fail in BootUpCard!\r\n");
		}
		delay1ms(600);
	}

	//配置wifi为多路可连接模式
	while (1)
	{
		Uart2SendStr("AT+CIPMUX=1\r\n");	   //串口2对wifi模块 设置多连接 （多路连接模式）
		if (DataCompare(RX_buffer, "OK"))
		{
			PrintLog("Multiplex connection mode set successfully!\r\n");
			ClearRxBuffer();
			break;
		}
		else
		{
			PrintLog("Multiplex connection mode set failed!\r\n");
		}
		delay1ms(600);
	}

	//建立TCP服务器监听端口
	while (1)
	{
		Uart2SendStr("AT+CIPSERVER=1,9999\r\n");	   //串口2设置wifi模块 为TCP服务器模式，并配置端口为5000 
		if (DataCompare(RX_buffer, "OK"))
		{
			PrintLog("Create TCP sever is successfully!\r\n");
			ClearRxBuffer();
			break;
		}
		else
		{
			PrintLog("Create TCP sever is failed!\r\n");
		}
		delay1ms(600);
	}

	//查询模块ip
	//查询模块ip
	while (1)
	{
		delay1ms(2000);
		Uart2SendStr(esp_cifsr);	   //串口2查询wifi模块 当前ip地址 
		delay1ms(500);
		if (DataCompare(RX_buffer, "OK"))
		{
			PrintLog("BootUpCard current IP is:\r\n");

			PrintLog(RX_buffer);
			PrintLog("\r\n");
			ClearRxBuffer();
			delay1ms(3000);
			break;
		}
		else
		{
			PrintLog("Query IP Error!\r\n");
		}
		delay1ms(600);
	}
	while (1)
	{
		const char ab[3] = "\"";//3
		u8* macAddress = 1;
		//char src[100]="";
		delay1ms(200);
		Uart2SendStr(esp_cifsr);	   //串口2查询wifi模块 当前ip地址 
		delay1ms(1888);
		if (DataCompare(RX_buffer, "OK"))
		{
			PrintLog("BootUpCard current IP is:\r\n");
			PrintLog(RX_buffer);
			PrintLog("\r\n");
			delay1ms(3000);
			//获取硬件序列号
			macAddress = Split(RX_buffer, ab, 1);
			PrintLog(RX_buffer);
			PrintLog("\r\n");
			PrintLog(macAddress);
			PrintLog("\r\n");
			memcpy(esp_macAddress, macAddress, 17);
			delay1ms(3000);
			//memcpy(esp_macAddress, macAddress, sizeof(macAddress));
			/*memset(esp_macAddress, '\0', sizeof(esp_macAddress));
			strcpy(src, macAddress);
			strcpy(esp_macAddress, src);*/

			/*memset(esp_macAddress, '\0', sizeof(esp_macAddress));
			strcpy(esp_macAddress, macAddress);*/
			//esp_macAddress = macAddress;
			PrintLog("\r\nmacAddress:\r\n");
			PrintLog(esp_macAddress);
			PrintLog("\r\n");
			PrintLog("\r\nold address:");
			PrintLog(macAddress);
			PrintLog("\r\n");
			delay1ms(3000);
			//ClearRxBuffer();
			break;
		}
		else
		{
			PrintLog("Query IP Error!\r\n");
		}
		delay1ms(600);
	}
}

/*
	重连阿里云服务器-连接wifi和远程阿里云服务器
*/
void RetryConnectServer()
{
	//连接热点wifi  wifi名 密码  如果失败 延时继续连接
	while (1)
	{
		Uart2SendStr("AT+CWJAP=\"app-a\",\"1234567890\"\r\n");	   //串口2发送 指点wifi名 密码 等待模块连接 
		PrintLog("AT+CWJAP=\"app-a\",\"1234567890\"\r\n");
		if (DataCompare(RX_buffer, "OK"))
		{
			PrintLog("Connection specified WIFI successful!\r\n");
			ClearRxBuffer();
			break;
		}
		else
		{
			PrintLog("Connection specified WIFI failed!\r\n");
		}
		delay1ms(3000);
	}

	//配置连接阿里云服务器
	while (1)
	{
		Uart2SendStr("AT+CIPSTART=\"TCP\",\"112.74.51.95\",8888\r\n");	   //wifi模块配置 服务器 IP 和端口
		if (DataCompare(RX_buffer, "OK"))
		{
			PrintLog("Connection remote TCP sever is successfully!\r\n");
			ClearRxBuffer();
			break;
		}
		else
		{
			PrintLog("Connection remote TCP sever is failed!\r\n");
		}
		delay1ms(2500);
	}
}

/*
	配置为Station模式-连接其他人的热点，然后连接阿里云服务器
*/
void InitESP8266ToStationMode()
{
	//重置esp8266
	ResetEsp8266();
	delay1ms(2000);
	//发送AT 进行握手
	while (1)
	{
		Uart2SendStr("AT\r\n");	   //串口2对wifi模块发送握手指令 即AT
		if (DataCompare(RX_buffer, "OK"))
		{
			PrintLog("MCU connect success with BootUpCard!\r\n");
			ClearRxBuffer();
			break;
		}
		else
		{
			PrintLog("MCU connect fail with BootUpCard!\r\n");
		}
		delay1ms(600);
	}

	//配置wifi工作模式为Stationi模式
	while (1)
	{
		Uart2SendStr("AT+CWMODE=1\r\n");
		if (DataCompare(RX_buffer, "OK") || DataCompare(RX_buffer, "no change"))
		{
			PrintLog("Set mode success as Station with BootUpCard!\r\n");
			ClearRxBuffer();
			break;
		}
		else
		{
			PrintLog("Set mode fail as Station with BootUpCard!\r\n");
		}
		delay1ms(600);
	}
	//连接wifi和远程阿里云服务器
	RetryConnectServer();
	//查询模块ip
	while (1)
	{
		Uart2SendStr(esp_cifsr);	   //串口2查询wifi模块 当前ip地址 
		if (DataCompare(RX_buffer, "OK"))
		{
			PrintLog("BootUpCard current IP is:\r\n");
			
			PrintLog(RX_buffer);
			PrintLog("\r\n");
			ClearRxBuffer();
			delay1ms(3000);
			break;
		}
		else
		{
			PrintLog("Query IP Error!\r\n");
		}
		delay1ms(600);
	}
}

//const char s[2] = ",";



/*
	主程序
*/
void main()
{
	
	u8 datalist[50];
	//u8* datalist;
	u8 eeprom_testdata[] = "AT+CWJAP=\"app-a\",\"1234567890\"\r\n";
	u16  i;									//临时变量
	u16  t=0;
	//心跳包发送的间隔时间
	unsigned int lens = 15;
	u8 num = 3;
	//针对 IAP15W4K61S4 IO口初始化
	IO_init();
	//串口初始化
	UartInit();
	//串口1中断打开
	ES = 1;
	//串口2中断打开
	IE2 = 0x01;
	//总中断打开
	EA = 1;
	//等待ESP8266启动
	delay1ms(1888);
	//eeprom测试
	//uart_init();								//串口初始化  串口1 工作模式2 定时器工作模式0  波特率9600
	PrintLog("\r\nStart EEPROM test...\r\n");
	//led = 0;	 //出现问题指示灯亮
	//IapEraseSector(IAP_ADDRESS);				//擦除地址对应的扇区  一个扇区为512个字节
	
	//判断EEPROM是否存在wifi配置
	unsigned char tempdata = IapReadByte(IAP_ADDRESS + i);
	if (tempdata == 0xff)		//判断是否都等于0xff  如果等于说明上面擦除成功了
	{
		//不存在wifi配置
		//初始化esp8266ap为AP模式
		InitESP8266ToAPMode();

		//发送硬件序列号
		while (1)
		{
			PrintLog("send params:\r\n");
			PrintLog("AT+CIPSEND=0,17\r\n");
			PrintLog("\r\n");
			PrintLog("send content:\r\n");
			PrintLog(esp_macAddress);
			//ClearRxBuffer();
			//发送硬件序列号
			Uart2SendStr("AT+CIPSEND=0,17\r\n");	   //配置测试数据字节长度
			delay1ms(200);
			Uart2SendStr(esp_macAddress);	   //发送测试数据
			delay1ms(300);
			if (DataCompare(RX_buffer, "SEND OK"))
			{
				PrintLog("\r\nsend hardware idNumber successfully!\r\n");
				break;
			}
			else
			{
				PrintLog("send hardware idNumber failed!\r\n");
			}
			delay1ms(1000);
		}

		//配置开机卡连接的无线网络
		while (1)
		{
			//接收手机端数据
			if (DataCompare(RX_buffer, "+IPD"))
			{
				const char a[2] = ":";
				u8* receiveData = Split(RX_buffer, a, 1);
				PrintLog("\r\nnreceiveData:\r\n");
				PrintLog(receiveData);
				PrintLog("\r\n");
				PrintLog("\r\nMy Wifi Information:\r\n");
				PrintLog(receiveData);
				PrintLog("\r\n");
				//获取APP端设置的WIFI名称和密码
				if (DataCompare(receiveData, "AT+CWJAP="))
				{
					//EEPROM_Write()
					PrintLog("Save ESP8266 AP WifiName and WifiPassword in Configuration is successfluy!\r\n");

					break;
				}
				//const char a[2] = ",";
				//u8* receiveData = Split(RX_buffer, a, 2);
				////PrintLog("\r\nreceiveData:\r\n");
				////PrintLog(receiveData);
				////PrintLog("\r\n");
				//const char b[2] = ":";
				//u8* wifiATInfo = Split(receiveData, b, 1);//存放wifi数据的变量
				/*PrintLog("\r\nreceiveData:\r\n");
				PrintLog(tempData);
				PrintLog("\r\n");*/

				/*u8* newTempData1;
				memcpy(newTempData1, tempData, sizeof(tempData));
				u8* newTempData2;
				memcpy(newTempData2, tempData, sizeof(tempData));*/
				//const char c[2] = "&";
				//u8* ssid = Split(newTempData1, c, 0);//wifi名称
				//u8* pwd = Split(newTempData2, c, 1);//wifi密码
				//u8 code esp_cwjap[] = "AT+CWJAP=\"app-a\",\"1234567890\"\r\n";
			}
			else
			{
				//没有收到数据
				PrintLog("\r\nServer is not Receive data!\r\n");
				PrintLog("\r\n");
			}
			//清空缓存
			ClearRxBuffer();
			delay1ms(1000);
		}
	}
	else
	{
		//存在wifi配置
	}
	for (i = 0; i < 1024; i++)						//读取地址对应的数据 512个字节  
	{
		unsigned char tempdata = IapReadByte(IAP_ADDRESS + i);
		if (tempdata != 0xff)		//判断是否都等于0xff  如果等于说明上面擦除成功了
		{
			PrintLog("clear failed!\r\n");
		}
	}
	//for (i = 0; i < 512; i++)						//指定地址写入512个字节 写入的数据随着i的递增而递增
	//{
	//	IapProgramByte(IAP_ADDRESS + i, (u8)i);		//（u8）i  这里用到了强制转换 因为存入的是字节 不能超过FF  而i是16位 
	//}
	PrintLog("data array list:\r\n");
	//PrintLog(sizeof(eeprom_testdata));
	//写配置
	for (i = 0; i < sizeof(eeprom_testdata); i++)						//指定地址写入512个字节 写入的数据随着i的递增而递增
	{
		Uart1SendByte(eeprom_testdata[i]);
		PrintLog("\r\n");
		delay1ms(10);
		IapProgramByte(IAP_ADDRESS + i, eeprom_testdata[i]);		//（u8）i  这里用到了强制转换 因为存入的是字节 不能超过FF  而i是16位 
	}
	//读取配置
	//u8 eeprom_readdata[sizeof(eeprom_testdata)];
	//datalist[1]='1';
	
	//datalist = (u8 *)malloc(strlen(eeprom_testdata)* sizeof(u8));
	/*PrintLog("malloc size:\r\n");
	PrintLog("\r\n");
	PrintLog(sizeof(datalist));*/
	
	/*PrintLog("\r\n");
	PrintLog((u8*)(strlen(eeprom_testdata)));*/
	while (1) {
		if (t < 1024) {
			u8 tempdata = IapReadByte(IAP_ADDRESS + t);
			if (tempdata == 0xff) {
				PrintLog("datalist is over!\r\n");
				break;
			}
			else {
				datalist[t] = tempdata;
				PrintLog("datalist add one!\r\n");
			}
			t++;
		}
		else
		{
			break;
		}
	}
	PrintLog("read byte ok,the data is:\r\n");
	PrintLog(datalist);

	
	//PrintLog(datalist);
	//delay1ms(10);
	//for (i = 0; i < 512; i++)						//指定地址多去512个字节 判断是否和上面写入的一样
	//{
	//	unsigned char tempdata = IapReadByte(IAP_ADDRESS + i);
	//	unsigned char si = (u8)i;
	//	PrintLog(&tempdata);
	//	if (tempdata == (u8)i)	//（u8）i  这里用到了强制转换 因为存入的是字节 不能超过FF  而i是16位 
	//	{
	//		PrintLog(&si);
	//		//PrintLog("read byte ok\r\n");
	//	}
	//	else 
	//	{
	//		//PrintLog("read byte error\r\n");
	//	}
	//}

	PrintLog("My BootUpCard is running...\r\nThis project was developed by hjj!\r\n");
	ClearRxBuffer();

	
	//初始化esp8266ap为Station模式
	InitESP8266ToStationMode();
	PrintLog("Start receiving data from remote server...\r\n");
	ClearRxBuffer();
	//根据从阿里云服务器接收的数据指令执行特定的操作
	while (1)
	{
		//接收服务端发来的数据
		PrintLog("\r\nReceive data:\r\n");
		PrintLog(RX_buffer);
		PrintLog("\r\n");

		if (DataCompare(RX_buffer, "+IPD,1:A"))
		{
			PrintLog("Open LED!\r\n");
			led = 0;
		}
		else if (DataCompare(RX_buffer, "+IPD,1:B"))
		{
			PrintLog("Close LED!\r\n");
			led = 1;
		}
		else
		{
			PrintLog("No matching commands were found!\r\n");
		}
		//清空缓存
		ClearRxBuffer();
		delay1ms(788);
		if (lens <= 0) {
			lens = 15;
			//发送心跳数据包
			Uart2SendStr("AT+CIPSEND=3\r\n");	   //配置测试数据字节长度
			delay1ms(200);
			Uart2SendStr("hjj\r\n");	   //发送测试数据
			delay1ms(300);
			if (DataCompare(RX_buffer, "SEND OK"))
			{
				PrintLog("Successful to send of TCP heartbeat packet!\r\n");
			}
			else
			{
				PrintLog("Failed to send TCP heartbeat packet!\r\n");
				RetryConnectServer();
			}
			//清空缓存
			ClearRxBuffer();
		}
		lens--;
	}
}