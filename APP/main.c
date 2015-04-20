/****************************************Copyright (c)*************************************************
**                      Fujian Junpeng Communicaiton Technology Co.,Ltd.
**                               http://www.easivend.com.cn
**--------------File Info------------------------------------------------------------------------------
** File name:           main.c
** Last modified Date:  2013-01-06
** Last Version:        No
** Descriptions:        主程序文件                     
**------------------------------------------------------------------------------------------------------
** Created by:          sunway
** Created date:        2013-01-06
** Version:             V0.1
** Descriptions:        The original version        
********************************************************************************************************/
#include "..\config.h"
#include "task_device.h"
#include "Global.h"



/*********************************************************************************************************
  定义全局变量
*********************************************************************************************************/


//MAIN
#define MAIN_START						(1)//交易开始
#define MAIN_FREE						(2)//交易空闲
#define MAIN_CHECK_GOODS				(4)//出货前检查
#define MAIN_OUTGOODS					(5)//出货
#define MAIN_OVER						(6)//交易结束
#define MAIN_WEIHU						(7)//维护模式
#define MAIN_FAULT						(0x16)//故障模式
#define MAIN_PAYOUT						(9)//找零


//按键邮箱句柄
OS_EVENT *g_KeyMsg;






void *KeyMsgArray[2]; // 按键队列数组



/*********************************************************************************************************
  交易主 任务ID、优先级、堆栈设置及函数声明                                
*********************************************************************************************************/
#define TASK_Trade_ID                          7                             /*   交易主任务ID                       */
#define TASK_Trade_PRIO                        TASK_Trade_ID                      /* 任务优先级                   */
#define TASK_Trade_STACK_SIZE                  1024                           /* 定义用户堆栈长度             */
OS_STK  TASK_Trade_STACK[TASK_Trade_STACK_SIZE];                                  /* 定义任务B堆栈                */
extern void    TASK_Trade(void *pdata);                                      /* 声明任务B                    */


/*********************************************************************************************************
  设备 任务ID、优先级、堆栈设置及函数声明                                
*********************************************************************************************************/   
#define TASK_Device_ID                          5                             /* 任务ID                       */
#define TASK_Device_PRIO                        TASK_Device_ID                      /* 任务优先级                   */
#define TASK_Device_STACK_SIZE                  512                           /* 定义用户堆栈长度             */
OS_STK  TASK_Device_STACK[TASK_Device_STACK_SIZE];                                  /* 定义任务C 堆栈               */



/*********************************************************************************************************
** Function name:       CreateMBox
** Descriptions:        为任务之间通信创建邮箱和信号量
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void CreateMBox(void)
{
	//创建按键邮箱
	g_KeyMsg = OSQCreate(&KeyMsgArray[0],2);
	CreateCommonMBox();
}

/*********************************************************************************************************
** Function name:       SystemInit
** Descriptions:        系统初始化
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void SystemInit()
{
	//注意复位后，Uart0使能，Uart2/3被禁能
	InitUart0();
	Trace("InitUart0 init over.....\r\n");
	InitUart1();
	InitUart2();
	InitUart3();
    InitBuzzer();
	InitI2C0();
	InitTimer(0,240000);
	
}


/*********************************************************************************************************
** Function name:       TASK_Trade
** Descriptions:       交易主任务
** input parameters:   
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/

void TASK_Trade(void *pvData)
{

	pvData = pvData;
	//系统基本接口初始化
	SystemInit();
	//建立邮箱、信号量	
	CreateMBox();
	//建立设备维护任务
	OSTaskCreateExt(TASK_Device, 
				(void *)0,
				&TASK_Device_STACK[TASK_Device_STACK_SIZE-1],
				TASK_Device_PRIO, 
				TASK_Device_ID, 
				&TASK_Device_STACK[0], 
				TASK_Device_STACK_SIZE, 
				(void *)0,
				OS_TASK_OPT_STK_CHK);
	while(1)
	{
		OSTimeDly(200);
	}

}






/*********************************************************************************************************
** Function name:       main
** Descriptions:          用户程序入口函数
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
int main(void)
{
    targetInit();																//初始化目标板
    pinInit();      															//IO初始化                                                                     
    OSInit();                                                                                                       
	//创建 交易主任务	
	OSTaskCreateExt(TASK_Trade, 
					(void *)0,
					&TASK_Trade_STACK[TASK_Trade_STACK_SIZE-1],
					TASK_Trade_PRIO, 
					TASK_Trade_ID, 
					&TASK_Trade_STACK[0], 
					TASK_Trade_STACK_SIZE, 
					(void *)0,
					OS_TASK_OPT_STK_CHK);

	OSStart();																	//启动多任务
}
/**************************************End Of File*******************************************************/
