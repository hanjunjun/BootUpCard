



#ifndef SD_H
#define SD_H

//错误码定义
//-------------------------------------------------------------
#define INIT_CMD0_ERROR     0x01 //CMD0错误
#define INIT_CMD1_ERROR     0x02 //CMD1错误
#define WRITE_BLOCK_ERROR   0x03 //写块错误
#define READ_BLOCK_ERROR    0x04 //读块错误
//-------------------------------------------------------------

#define TRY_TIME 10    //向SD卡写入命令之后，读取SD卡的回应次数，即读TRY_TIME次，如果在TRY_TIME次中读不到回应，产生超时错误，命令写入失败
sbit SD_CS=P1^2;
void SdCommand(unsigned char command, unsigned long argument, unsigned char CRC);
unsigned char SdResponse();
unsigned char SdInit(void);
unsigned char SdReadBlock(unsigned char *Block, unsigned long address,int len);

unsigned char SD_read_sector(unsigned long Sector_NUM,unsigned char *Tmp_Buffer);
unsigned char SD_read_Byte(unsigned long addr,unsigned char *Tmp_Buffer,unsigned long Bytes);
unsigned char SD_Reset(void);//SD卡复位，进入SPI模式，使用CMD0（命令0）
#endif
