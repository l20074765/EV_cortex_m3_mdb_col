/****************************************Copyright (c)*************************************************
**                      Fujian Junpeng Communicaiton Technology Co.,Ltd.
**                               http://www.easivend.com.cn
**--------------File Info------------------------------------------------------------------------------
** File name:           common.c
** Last modified Date:  2013-01-06
** Last Version:        No
** Descriptions:        系统参数定义及通用函数部分                     
**------------------------------------------------------------------------------------------------------
** Created by:          sunway
** Created date:        2013-01-06
** Version:             V0.1
** Descriptions:        The original version        
********************************************************************************************************/
#include "..\config.h"




/***********************************************************************************************
*  工程系统参数
*  add  by yoc 2014.2.18
************************************************************************************************************/

/***********************************************************************************************
*  定时器
*  add  by yoc 2014.2.18
************************************************************************************************************/


TIMER_ST Timer;




//任务通信邮箱的对发邮箱句柄
OS_EVENT *g_msg_main_to_dev;
OS_EVENT *g_msg_dev_to_main;
MAIN_DEV_TASK_MSG task_msg_main_to_dev;
MAIN_DEV_TASK_MSG task_msg_dev_to_main;



//纸币器通信邮箱
OS_EVENT *g_billIN;
BILL_RECV_MSG bill_recv_msg[G_BILL_IN_SIZE]; 
void *billInNum[G_BILL_IN_SIZE];

//硬币器通信消息队列
OS_EVENT *g_CoinIn;
unsigned char  CoinIn[G_COIN_IN_SIZE] = {0};
void *coinInNum[G_COIN_IN_SIZE];




//初始化任务信号量
OS_EVENT *g_InitSem;



void    TASK_Device(void *pdata); 




/*********************************************************************************************************
** Function name:     	XorCheck
** Descriptions:	    累加校验和
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
unsigned char XorCheck(unsigned char *pstr,unsigned short len)
{
	unsigned char xor_check = 0;
	unsigned short i;
	for(i=0;i<len;i++) 
	{
       	xor_check = xor_check^pstr[i];
   	}
	return xor_check;
}
/*********************************************************************************************************
** Function name:     	CrcCheck
** Descriptions:	    CRC校验和
** input parameters:    msg需要检验的数据;len数据长度
** output parameters:   无
** Returned value:      CRC检验结果
*********************************************************************************************************/
unsigned short CrcCheck(unsigned char *msg, unsigned short len) 
{
    unsigned short i, j;
    unsigned short crc = 0;
    unsigned short current = 0;
    for(i=0;i<len;i++) 
    {
        current = msg[i] << 8;
        for(j=0;j<8;j++) 
        {
            if((short)(crc^current)<0)
                crc = (crc<<1)^0x1021;
            else 
                crc <<= 1; 
            current <<= 1; 
        }
    }
    return crc;
}






/*********************************************************************************************************
** Function name:     	msleep
** Descriptions:	    毫秒睡眠函数  比较
** input parameters:    msg需要检验的数据;len数据长度
** output parameters:   无
** Returned value:      CRC检验结果
*********************************************************************************************************/

void msleep(unsigned int msec)
{
	unsigned int temp;
	
	temp = (msec > 10) ? msec/5 : 2;
	OSTimeDly(temp);
}



/*********************************************************************************************************
** Function name:       mbox_post_main_to_dev
** Descriptions:        主任务发送邮箱到设备任务
** input parameters:    无
** output parameters:   无
** Returned value:      
*********************************************************************************************************/
unsigned char mbox_post_main_to_dev(unsigned char type)
{
	unsigned char rst;
	
	task_msg_main_to_dev.type = type;
	Trace("task_msg_main_to_dev POST:%dr\n",type);
	rst = OSMboxPost(g_msg_main_to_dev,&task_msg_main_to_dev);
	OSTimeDly(5);
	return (rst == OS_NO_ERR) ? 1 : 0;
}




/*********************************************************************************************************
** Function name:       mbox_post_dev_to_main
** Descriptions:        设备任务到主任务的邮箱发送函数
** input parameters:    无
** output parameters:   无
** Returned value:      
*********************************************************************************************************/
unsigned char mbox_post_dev_to_main(unsigned char type)
{
	unsigned char rst;
	task_msg_dev_to_main.type = type;
	Trace("task_msg_dev_to_main POST:%dr\n",type);
	rst = OSMboxPost(g_msg_dev_to_main,&task_msg_dev_to_main);
	OSTimeDly(5);
	return (rst == OS_NO_ERR) ? 1 : 0;
}

/*********************************************************************************************************
** Function name:       CreateCommonMBox
** Descriptions:        穿件邮箱
** input parameters:    无
** output parameters:   无
** Returned value:      
*********************************************************************************************************/

void CreateCommonMBox(void)
{
	//创建主任务到设备任务的邮箱
	g_msg_main_to_dev = OSMboxCreate(NULL);
	//创建设备任务返回主任务的邮箱
	g_msg_dev_to_main = OSMboxCreate(NULL);
	
	g_billIN = OSQCreate(&billInNum[0],G_BILL_IN_SIZE);
	g_CoinIn = OSQCreate(&coinInNum[0],G_COIN_IN_SIZE);
	//创建初始化控制信号量
	g_InitSem = OSSemCreate(0);
}


/**************************************End Of File*******************************************************/
