#include "deviceTask.h"
#include "..\config.h"



//按键邮箱句柄
OS_EVENT *g_KeyMsg;
void *KeyMsgArray[2]; // 按键队列数组






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
	InitUart0();
	InitUart1();
	uart2Init();
	InitUart3();
    InitBuzzer();
	InitI2C0();
	InitTimer(0,240000);
	
}


void DEV_task(void *pdata)
{	
	Trace("DEV_task\r\n");
	//系统基本接口初始化
	SystemInit();
	//建立邮箱、信号量	
	CreateMBox();
	
	
	while(1){
		msleep(100);
	}
}



