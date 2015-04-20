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
#ifndef __PCOIN_H 
#define __PCOIN_H

//并行硬币器
void InitParallelPluseCoinAcceptor(void);			//初始化并行硬币器
void EnableParallelPluseCoinAcceptor(void);			//使能并行硬币器
void DisableParallelPluseCoinAcceptor(void);		//禁能并行硬币器
unsigned char GetParallelCoinAcceptorStatus(void);	//获取并行硬币器状态
unsigned char ReadParallelCoinAcceptor(void);		//获取并行硬币器通道值
void ScanPPCoinChannel(void);						//Time1定时扫描并行硬币器，应用层不得调用

//串行硬币器
void InitSerialPluseCoinAcdeptor(void);				//初始化串行硬币器
void EnableSerialPluseCoinAcceptor(void);			//使能串行硬币器
void DisableSerialPluseCoinAcceptor(void);			//禁能串行硬币器
unsigned char GetSerialCoinAcceptorStatus(void);	//获取串行硬币器状态
unsigned char ReadSerialPluseCoinAcceptor(void);	//获取串行硬币器通道值
void ScanSPCoinChannel(void);						//Time1定时扫描串行硬币器，应用层不得调用

#endif
/**************************************End Of File*******************************************************/
