
/*************************此部分为AT2402的驱动程序使用I2C总线连接*************************************/
#include  "stc15f2k60s2.h"
#include  "../../Common/Sleep.h"
#include  "../../Common/I2C.h"
#include  "EEPROM24C02.h"





//24c 默认的硬件地址都是 0xa0  即 1010   然后A2 A1 A0  最后一位 为读写位  1读 0写
//24c02  256x8 bit    
//24c04  512x8 bit
//而子地址 只是8位的 即最大也就256个字节  那么24c04 就要占用2个256个字节 
//这里24c04硬件上采用的是 分页方法  即有24c02里有A0 A1 A2 当是24c04时 A0做为分页位
//当A0为0 表示第一页 写入第一页256字节
//当A0为1 表示第二页 写入第二页256字节









//在AT24CXX指定地址读出一个数据
//Addr:开始读数的地址  
//返回值  :读到的数据
u8 AT24C_Rcvone(u8 Addr)
{
	u8 temp = 0;
	Start_I2c();                //启动总线
	SendByte(0xa0);             //发送写命令
	I2c_wait_ack();			   //等待应答
	SendByte(Addr);             //发送地址
	I2c_wait_ack();			   //等待应答

	Start_I2c();                //重新启动总线
	SendByte(0xa1);		       //设置为读操作
	I2c_wait_ack();			   //等待应答;

	temp = RcvByte(0);			   //读字节	非应答


	Stop_I2c();                 //结束总线 
	return temp;
}



//在AT24CXX指定地址写入一个数据	 此函数只限于 c02-c16
//Addr:写入数据的目的地址    
//Data:要写入的数据
void AT24C_Sendone(u8 Addr, u8 Data)
{
	Start_I2c();               //启动总线

	SendByte(0xa0);            //发送写命令
	I2c_wait_ack();			  //等待应答
	SendByte(Addr);            //发送地址
	I2c_wait_ack();			  //等待应答
	SendByte(Data);			  //发送字节数据
	I2c_wait_ack();			  //等待应答
	Stop_I2c();                //结束总线 
	delay1ms(10);			  //如果是连续发送字节的时候这个延时很重要 否则将回传错
}


//在AT24CXX里面的指定地址开始写入长度为Len的数据
//该函数用于写入16bit或者32bit的数据.
//Addr  :开始写入的地址  
//Data  :数据数组首地址
//Len   :要写入数据的长度2,4
void AT24C_SendLenByte(u8 Addr, u8* Data, u8 Len)
{
	while (Len--)
	{
		AT24C_Sendone(Addr, *Data);
		Addr++;
		Data++;
	}
}



//在AT24CXX里面的指定地址开始读出长度为Len的数据
//该函数用于读出16bit或者32bit的数据.
//Addr   :开始读出的地址 
//返回值     :数据
//Len        :要读出数据的长度2,4
void AT24C_RcvLenByte(u8 Addr, u8* temp, u8 Len)
{

	while (Len)
	{
		*temp++ = AT24C_Rcvone(Addr++);
		Len--;
	}

}







