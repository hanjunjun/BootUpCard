
#include  "stc15f2k60s2.h"

#include "SdCard.h"
#include "../SPI/SPI.h"
#include  "../../Common/Delay.h"







//============================================================
//检测SD卡的响应
unsigned char SdResponse()
{
unsigned char i=0,response;

while(i<=8)
{

response =SPI_SendByte(0xff); //从SD卡中读取数据

if(response==0x00) break;	  //如果返回值中有以下值 即为得到SD卡相应 可以返回
if(response==0x01) break;				 
i++;
}

return response; 
} 



//================================================================
//发命令到SD卡
void SdCommand(unsigned char command, unsigned long argument, unsigned char CRC)
{
						

SD_CS=1;		                                //片选拉高
SPI_SendByte(0xff);
SPI_SendByte(0xff);	 
SPI_SendByte(0xff);							    
SD_CS=0;  										//启动SPI总线


SPI_SendByte(command|0x40);                     //写一字节到SD
SPI_SendByte(((unsigned char *)&argument)[0]);
SPI_SendByte(((unsigned char *)&argument)[1]);
SPI_SendByte(((unsigned char *)&argument)[2]);
SPI_SendByte(((unsigned char *)&argument)[3]);
SPI_SendByte(CRC);
}




/*********************************************************************
名称:SD卡写命令
描述:
功能:
返回值:
输入值:CMD命令号,address地址,CRC,response此命令的响应值
*********************************************************************/

unsigned char Write_Command_SD(unsigned char CMD,unsigned long address,unsigned char CRC,unsigned char response) 
{  
    unsigned char tmp; 
    unsigned char Timeout=0; 

    /* 提高兼容性,这里相当重要 */
SD_CS=1;		                                //片选拉高
SPI_SendByte(0xff);
SPI_SendByte(0xff);	 
SPI_SendByte(0xff);							    
SD_CS=0;  										//启动SPI总线 


    /* 写命令号 */
    SPI_SendByte(CMD|0x40);

    /* 写4字节参数 */
    SPI_SendByte(address>>24);
    SPI_SendByte(address>>16);
    SPI_SendByte(address>>8);
    SPI_SendByte(address);

    /* 写CRC */
    SPI_SendByte(CRC); 

    do
    {

    /* 一直读，直到读到的不是响应值或超时 */
    tmp= SPI_SendByte(0xff);
    Timeout++;
    }while((tmp!=response)&&(Timeout<TRY_TIME));

    return(tmp);
}




 /*********************************************************************
名称:SD卡复位
描述:
功能:
返回值:0表示成功
输入值:
*********************************************************************/

unsigned char SD_Reset(void)//SD卡复位，进入SPI模式，使用CMD0（命令0）
{
    unsigned char i,temp,time;

    SD_CS=0; 
    for(i=0;i<0x0f;i++)//复位时，首先要发送最少74个时钟信号，这是必须的！！！
    	SPI_SendByte(0xff);//120个时钟

    SD_CS=1;//拉高片选

    time = 0;
    do
     { 
      temp=Write_Command_SD(0x00,0x00,0x95,0x01);//写入命令0,响应值为0x01
	  delay1ms(10);
      time++;
     }while( (temp!=0x01) && (time < TRY_TIME) );//CMD0的响应为0x01


    if(time >= TRY_TIME) 
    { 
    SD_CS=0; //使能片选
    SPI_SendByte(0xff);
    return(INIT_CMD0_ERROR);//CMD0写入失败
    }
         

    SD_CS=0;  //使能片选
    SPI_SendByte(0xff); //按照SD卡的操作时序在这里补8个时钟 


    /* 设置块长度512 */
    Write_Command_SD(16,512,0xff,0x00);

    
    return 0;//返回0,说明复位操作成功
}







//初始化SD卡
unsigned char SdInit(void)
{
int delay=0, trials=0;
unsigned char i;
unsigned char response=0x01;
	   
SD_CS=1;
	
for(i=0;i<=9;i++)	
SPI_SendByte(0xff);	
 
SD_CS=0;
	
//Send Command 0 to put MMC in SPI mode
SdCommand(0x00,0,0x95);	  

response=SdResponse();	 

if(response!=0x01)			     //判断是否有应答 如果没有 初始化失败
{
return 0;	 
} 

while(response==0x01)	
{
SD_CS=1;	
SPI_SendByte(0xff);

SD_CS=0;
SdCommand(0x01,0x00ffc000,0xff);
response=SdResponse();
} 
SD_CS=1;
	
SPI_SendByte(0xff);
return 1; 
}





//================================================================
//往SD卡指定地址写数据,一次最多512字节
/*unsigned char SdWriteBlock(unsigned char *Block, unsigned long address,int len)
{
unsigned int count;
unsigned char dataResp;
//Block size is 512 bytes exactly
//First Lower SS

SD_CS=0;
//Then send write command
SdCommand(0x18,address,0xff);

if(SdResponse()==00)
{
SdWrite(0xff);
SdWrite(0xff);
SdWrite(0xff);
//command was a success - now send data
//start with DATA TOKEN = 0xFE
SdWrite(0xfe);
//now send data
for(count=0;count<len;count++) SdWrite(*Block++);

for(;count<512;count++) SdWrite(0);
//data block sent - now send checksum
SdWrite(0xff); //两字节CRC校验, 为0XFFFF 表示不考虑CRC
SdWrite(0xff);
dataResp=SdRead();

while(SdRead()==0);

dataResp=dataResp&0x0f; //mask the high byte of the DATA RESPONSE token
SD_CS=1;
SdWrite(0xff);
if(dataResp==0x0b)
{
return 0;
}
if(dataResp==0x05)
return 1;

return 0;
}
return 0;
}  */

//=======================================================================
//从SD卡指定地址读取数据,一次最多512字节
/*
unsigned char SdReadBlock(unsigned char *Block, unsigned long address,int len)
{
unsigned int count;
//Block size is 512 bytes exactly
//First Lower SS

 //printf("MMC_read_block\n");

SD_CS=0;
//Then send write command
SdCommand(0x11,address,0xff);

if(SdResponse()==00)
{
//command was a success - now send data
//start with DATA TOKEN = 0xFE
//while(SdRead()!=0xfe);  SPI_SendByte(0xff);
while(SPI_SendByte(0xff)!=0xfe);
for(count=0;count<len;count++) *Block++=SPI_SendByte(0xff);//SdRead(); 
                                    

for(;count<512;count++) SPI_SendByte(0xff);//SdRead();

//data block sent - now send checksum
SPI_SendByte(0xff);
SPI_SendByte(0xff);

//Now read in the DATA RESPONSE token
SD_CS=1;
SPI_SendByte(0xff);

return 1;
}
 //printf("Command 0x11 (Read) was not received by the MMC.\n");
return 0;
}	   
*/


/*********************************************************************
名称:SD卡读扇区
描述:
功能:
返回值:0表示成功
输入值:Sector_NUM要读的扇区号, *Tmp_Buffer读出存放的地址
*********************************************************************/


   
unsigned char SD_read_sector(unsigned long Sector_NUM,unsigned char *Tmp_Buffer) 
{ 
    unsigned int i;
    unsigned char temp=0,retry;

    
	SD_CS=1; 

    retry = 0;
    do
    {
    /* 写入CMD17 */

    temp=Write_Command_SD(17,Sector_NUM<<9,0xff,0x00);
    retry++;
    }while((temp!=0x00) && (retry < TRY_TIME)); 


    if (retry >= TRY_TIME) 
    {
    SD_CS=0;
    return READ_BLOCK_ERROR; 
    }


    /* 读到0xfe报头  */
    while(SPI_SendByte(0xff)!=0xfe);

     for(i=0;i<512;i++) 
    {
    /* 连续读出一扇区数据 */
    Tmp_Buffer[i]=SPI_SendByte(0xff);  
    }

    /* 读2个CRC校验字节 */
    SPI_SendByte(0xff); 
    SPI_SendByte(0xff);  

    SD_CS=0;
    
    /* 补一个字节 */
    SPI_SendByte(0xff); 
    return 0; 
} 
					

/*********************************************************************
名称:SD卡读字节
描述:
功能:
返回值:
输入值:addr为地址，*Tmp_Buffer为读出数据所存放地址，Bytes为要读的个数
*********************************************************************/
unsigned char SD_read_Byte(unsigned long addr,unsigned char *Tmp_Buffer,unsigned long Bytes) 
{ 
    unsigned int i;
    unsigned char temp=0,retry;

    /* 命令16设置块长度 */
    Write_Command_SD(16,Bytes,0xff,0x00);

    SD_CS=1; 

    retry = 0;

    do
    {
    /* 写入CMD17 */
    temp=Write_Command_SD(17,addr,0xff,0x00);
    retry++;
    }
	
	while((temp!=0x00) && (retry < TRY_TIME)); 

	
    if (retry >= TRY_TIME) 
    {
    SD_CS=0; 
    return READ_BLOCK_ERROR; 
    }

    /* 读到0xfe报头  */
    while(SPI_SendByte(0xff)!=0xfe);

     for(i=0;i<Bytes;i++) 
    {

        /* 连续读出一扇区数据 */
        Tmp_Buffer[i] = SPI_SendByte(0xff);  

    }

    /* 读2个CRC校验字节 */
    SPI_SendByte(0xff); 
    SPI_SendByte(0xff);  

    SD_CS=0; 
    
    /* 补一个字节 */
   SPI_SendByte(0xff); 

    /* 命令16恢复块长度 */
    Write_Command_SD(16,512,0xff,0x00);

    
    return 0; 

} 


