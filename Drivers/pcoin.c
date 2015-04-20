/****************************************Copyright (c)*************************************************
**                      Fujian Junpeng Communicaiton Technology Co.,Ltd.
**                               http://www.easivend.com.cn
**--------------File Info------------------------------------------------------------------------------
** File name:           pcoin.c
** Last modified Date:  2013-01-12
** Last Version:         
** Descriptions:        脉冲硬币器接口(并行、串行)                     
**------------------------------------------------------------------------------------------------------
** Created by:          sunway
** Created date:        2013-01-12
** Version:             V0.1
** Descriptions:        The original version       
********************************************************************************************************/
#include "..\config.h"

#define	PARALLCOIN_CTL		(1ul<<29)				//P1.20并行脉冲硬币器使能线,低电平使能、高电平禁能
#define SERIALCOIN_CTL 		(1ul<<30)				//P1.21串行脉冲硬币器使能线,低电平使能、高电平禁能

#define PARALLCHL_IN		0x0000000000070103		//并行硬币器通道
#define GETPPCOIN_CHL1()	((FIO1PIN >> 10)& 0x01)	//通道1
#define GETPPCOIN_CHL2()	((FIO1PIN >> 9) & 0x01)	//通道2
#define GETPPCOIN_CHL3()	((FIO1PIN >> 8) & 0x01)	//通道3
#define GETPPCOIN_CHL4()	((FIO1PIN >> 4) & 0x01)	//通道4
#define GETPPCOIN_CHL5()	((FIO1PIN >> 1) & 0x01)	//通道5
#define GETPPCOIN_CHL6()	((FIO1PIN >> 0) & 0x01)	//通道6

#define SERIALCOIN_IN		(1ul<<11)				//串行硬币器通道P2.11
#define GETSERIALCON_CHL()	((FIO2PIN >>11) & 0x01)	//串行通道

#define PARALLELCOIN_CHANNEL_QUEUE_SIZE 8
#define SERIALCOIN_CHANNEL_QUEUE_SIZE 	20

void *PCoinChannel[PARALLELCOIN_CHANNEL_QUEUE_SIZE];






OS_EVENT *QPChannel;
volatile unsigned char PostParallelCoinChannel;


OS_EVENT *QSChannel;//定义串行脉冲硬币器消息事件
void *SCoinChannel[SERIALCOIN_CHANNEL_QUEUE_SIZE];
volatile unsigned char PostSerialCoinChannel[SERIALCOIN_CHANNEL_QUEUE_SIZE];
static unsigned char  serialCoinIndex = 0; 



volatile unsigned char PARALLELPULSECOINACCEPTORDEVICESTATUS = 0x00;//默认关闭并行脉冲硬币器
volatile unsigned char SERIALPULSECOINACCEPTORDEVICESTATUS   = 0x00;//默认关闭串行行脉冲硬币器

/*****************************************并行硬币器API**************************************************/

/*********************************************************************************************************
** Function name:       InitParallelPluseCoinAcceptor
** Descriptions:        初始化并行硬币器
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void InitParallelPluseCoinAcceptor(void)
{
	QPChannel = OSQCreate(&PCoinChannel[0],PARALLELCOIN_CHANNEL_QUEUE_SIZE);
	FIO1DIR &= (~PARALLCHL_IN);				//设置通道为输入模式
	//EnableParallelPluseCoinAcceptor();		//使能硬币器
	InitTimer(1,480000);					//20ms扫描一次并行硬币器			
}
/*********************************************************************************************************
** Function name:       EnableParallelPluseCoinAcceptor
** Descriptions:        使能并行硬币器
** input parameters:    设置全局变量PARALLELPULSECOINACCEPTORDEVICESTATUS
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void EnableParallelPluseCoinAcceptor(void)
{
	PARALLELPULSECOINACCEPTORDEVICESTATUS =0x01;
	FIO0DIR |= (SERIALCOIN_CTL|PARALLCOIN_CTL);//P0.30 PLS_MASK
	FIO0SET |= (SERIALCOIN_CTL|PARALLCOIN_CTL);
	//Trace("ParalleEnable+FIO0.30--%x %x\r\n",FIO0PIN>>30,FIO0PIN>>29);
}
/*********************************************************************************************************
** Function name:       DisableParallelPluseCoinAcceptor
** Descriptions:        禁能并行硬币器
** input parameters:    无
** output parameters:   设置全局变量PARALLELPULSECOINACCEPTORDEVICESTATUS
** Returned value:      无
*********************************************************************************************************/
void DisableParallelPluseCoinAcceptor(void)
{
	PARALLELPULSECOINACCEPTORDEVICESTATUS =0x00;
	FIO0DIR |= (SERIALCOIN_CTL|PARALLCOIN_CTL);//P0.30 PLS_MASK
	FIO0CLR |= (SERIALCOIN_CTL|PARALLCOIN_CTL);
	//Trace("ParalleDisable+FIO0.30--%x %x\r\n",FIO0PIN>>30,FIO0PIN>>29);
	//FIO0DIR &= ~(SERIALCOIN_CTL|PARALLCOIN_CTL);//P0.30 PLS_MASK
	//FIO0SET |= (SERIALCOIN_CTL|PARALLCOIN_CTL);
}
/*********************************************************************************************************
** Function name:       GetParallelCoinAcceptorStatus
** Descriptions:        获取并行硬币器状态：禁能或使能
** input parameters:    无
** output parameters:   无
** Returned value:      硬币器状态：0x00-关闭；0x01-开启
*********************************************************************************************************/
unsigned char GetParallelCoinAcceptorStatus(void)
{
	return PARALLELPULSECOINACCEPTORDEVICESTATUS;	
}
/*********************************************************************************************************
** Function name:       ReadParallelCoinAcceptor
** Descriptions:        读取并行硬币器通道值
** input parameters:    无
** output parameters:   无
** Returned value:      硬币器通道：0x00-未投币；0x01~0x06相应的通道有投币
*********************************************************************************************************/
unsigned char ReadParallelCoinAcceptor(void)
{
	unsigned char err;
	unsigned char *Pchannel;
	Pchannel = (unsigned char *)OSQPend(QPChannel,5,&err);
	if(err == OS_NO_ERR)
	{
		return *Pchannel;	
	}
	else
		return 0x00;	
}
/*********************************************************************************************************
** Function name:       ScanPPCoinChannel
** Descriptions:        扫描并行硬币器通道，注意只能在Time1定时中断中扫描
** input parameters:    无
** output parameters:   将扫描到的值送入并行脉冲硬币器专用消息队列
** Returned value:      无
*********************************************************************************************************/
void ScanPPCoinChannel(void)
{
	unsigned int Rchannel;
	unsigned char CurChannel,Temp;
	static unsigned char PreChannel;
	static unsigned char PreStatus ;
	Rchannel  = GETPPCOIN_CHL1()<<0;
	Rchannel |= GETPPCOIN_CHL2()<<1;
	Rchannel |= GETPPCOIN_CHL3()<<2;
	Rchannel |= GETPPCOIN_CHL4()<<3;
	Rchannel |= GETPPCOIN_CHL5()<<4;
	Rchannel |= GETPPCOIN_CHL6()<<5;
	Temp = (unsigned char)(~Rchannel);
	Temp &= 0x3F;
	switch(Temp)
	{
		case 0x00 : if(PreStatus == 0x01)		
					{
						PreStatus = 0x00;
						PostParallelCoinChannel = PreChannel;
						OSQPost(QPChannel,(void *)&PostParallelCoinChannel);
					}
					PreChannel = 0x00;
					CurChannel = 0x00;
					break;
		case 0x01 : CurChannel = 0x06;
					break;
		case 0x02 : CurChannel = 0x05;
					break;
		case 0x04 : CurChannel = 0x04;
					break;
		case 0x08 : CurChannel = 0x03;
					break;
		case 0x10 : CurChannel = 0x02;
					break;
		case 0x20 : CurChannel = 0x01;
					break;
		default  :  CurChannel = 0x00;
					break;					
	}
	if(PreStatus == 0x00)
	{
		if((PreChannel > 0x00)&(PreChannel == CurChannel))
			PreStatus = 0x01;
		else
			PreChannel = CurChannel;
	}
}

/*****************************************串行硬币器API**************************************************/

/*********************************************************************************************************
** Function name:       InitSerialPluseCoinAcdeptor
** Descriptions:        初始化串行行硬币器
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void InitSerialPluseCoinAcdeptor(void)
{
	QSChannel = OSQCreate(&SCoinChannel[0],SERIALCOIN_CHANNEL_QUEUE_SIZE);
	FIO2DIR &= (~SERIALCOIN_IN);			//设置通道为输入模式
	//EnableSerialPluseCoinAcceptor();		//使能硬币器
	InitTimer(1,480000);					//20ms扫描一次并行硬币器
}
/*********************************************************************************************************
** Function name:       EnableSerialPluseCoinAcceptor
** Descriptions:        使能串行硬币器
** input parameters:    设置全局变量SERIALPULSECOINACCEPTORDEVICESTATUS
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void EnableSerialPluseCoinAcceptor(void)
{
	SERIALPULSECOINACCEPTORDEVICESTATUS =0x01;
	FIO0DIR |= (SERIALCOIN_CTL|PARALLCOIN_CTL);//P0.30 PLS_MASK
	FIO0CLR |= (SERIALCOIN_CTL|PARALLCOIN_CTL);
	//FIO0SET |= (SERIALCOIN_CTL|PARALLCOIN_CTL);
	OSTimeDly(5);	
	//Trace("SerialDisable+FIO0.30--%x %x\r\n",FIO0PIN>>30,FIO0PIN>>29);
}
/*********************************************************************************************************
** Function name:       DisableSerialPluseCoinAcceptor
** Descriptions:        使能串行硬币器
** input parameters:    设置全局变量SERIALPULSECOINACCEPTORDEVICESTATUS
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void DisableSerialPluseCoinAcceptor(void)
{
	SERIALPULSECOINACCEPTORDEVICESTATUS =0x00;
	FIO0DIR |= (SERIALCOIN_CTL|PARALLCOIN_CTL);//P0.30 PLS_MASK
	FIO0SET |= (SERIALCOIN_CTL|PARALLCOIN_CTL);
	//FIO0CLR |= (SERIALCOIN_CTL|PARALLCOIN_CTL);
	OSTimeDly(5);
	//Trace("SerialDisable+FIO0.30--%x %x\r\n",FIO0PIN>>30,FIO0PIN>>29);
}
/*********************************************************************************************************
** Function name:       GetSerialCoinAcceptorStatus
** Descriptions:        获取串行硬币器状态
** input parameters:    设置全局变量SERIALPULSECOINACCEPTORDEVICESTATUS
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
unsigned char GetSerialCoinAcceptorStatus(void)
{
	return SERIALPULSECOINACCEPTORDEVICESTATUS;
}
/*********************************************************************************************************
** Function name:       ReadSerialPluseCoinAcceptor
** Descriptions:        读取串行硬币器通道值
** input parameters:    无
** output parameters:   无
** Returned value:      硬币器通道：0x00-未投币；0x01收到一个脉冲，
*********************************************************************************************************/
unsigned char ReadSerialPluseCoinAcceptor(void)
{
	unsigned char err;
	unsigned char *Schannel;
	Schannel = (unsigned char *)OSQPend(QSChannel,5,&err);
	if(err == OS_NO_ERR)
	{
		//Trace("\r\nSchannel...%d\r\n",*Schannel);
		return *Schannel;	
	}
	else
		return 0x00;
}
/*********************************************************************************************************
** Function name:       ScanSPCoinChannel
** Descriptions:        扫描串行硬币器通道，注意只能在Time1定时中断中扫描
** input parameters:    无
** output parameters:   将扫描到的值送入串行脉冲硬币器专用消息队列
** Returned value:      无
*********************************************************************************************************/
void ScanSPCoinChannel(void)
{
	unsigned char CurStatus;
	static unsigned char PreStatus ;

	FIO2DIR &= (~SERIALCOIN_IN);
	CurStatus  = (unsigned char)GETSERIALCON_CHL();
	switch(CurStatus)
	{					
		case 0x00 :	if(PreStatus == 0x01)
					{
						PostSerialCoinChannel[serialCoinIndex] = 0x01;
						OSQPost(QSChannel,(void *)&PostSerialCoinChannel[serialCoinIndex]);
						serialCoinIndex = (++serialCoinIndex) % SERIALCOIN_CHANNEL_QUEUE_SIZE;
					}
					PreStatus = CurStatus;
					break;
		case 0x01 : PreStatus = CurStatus;
					break;	
		default   : break;
	}
}

/**************************************End Of File*******************************************************/
