#include "deviceTask.h"
#include "..\config.h"

#define DEV_DEBUG
#ifdef DEV_DEBUG
#define print_dev(...)	Trace(__VA_ARGS__)
#else
#define print_dev(...)
#endif

//按键邮箱句柄
OS_EVENT *g_KeyMsg;
void *KeyMsgArray[2]; // 按键队列数组

extern OS_EVENT *g_mdb_event; //声明MDb事件




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
	MDB_createMbox();

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




static void DEV_mdbSwitch(G_MDB_ST *g_mdb_st)
{
	//出货 
	uint8 res;
	res = BT_open(1,g_mdb_st->column);
	if(res == 1){
		MDB_setColStatus(MDB_COL_SUCCESS);
	}
	else{
		MDB_setColStatus(MDB_COL_FAILED);
	}
	
	
}

static void DEV_mdbCtrl(G_MDB_ST *g_mdb_st)
{
	msleep(200);
	MDB_setColStatus(MDB_COL_SUCCESS);	
}


static void DEV_mdbReset(G_MDB_ST *g_mdb_st)
{
	uint8 res;
	res = EV_bento_check(1,g_mdb_st->bin);
	if(res == 1){
		MDB_setColStatus(MDB_COL_JUSTRESET);
	}
	else{
		MDB_setColStatus(MDB_COL_ERROR);
	}
	
}

void DEV_taskPoll(void)
{
	G_MDB_ST *g_mdb_st;
	INT8U err;
	g_mdb_st = OSQPend (g_mdb_event, 20, &err);
	if(err == OS_NO_ERR){
		print_dev("OSQPend:type= %d\r\n",g_mdb_st->type);
		switch(g_mdb_st->type){
			case G_MDB_RESET:
				DEV_mdbReset(g_mdb_st);
				break;
			case G_MDB_SWITCH:
				DEV_mdbSwitch(g_mdb_st);
				break;
			case G_MDB_CTRL:
				DEV_mdbCtrl(g_mdb_st);
				break;
			default:break;
		}
	}

}


void DEV_task(void *pdata)
{	
	//系统基本接口初始化
	SystemInit();
	//建立邮箱、信号量	
	CreateMBox();
	while(1){
		DEV_taskPoll();
		msleep(10);
	}
}



