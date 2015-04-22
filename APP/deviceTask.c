#include "deviceTask.h"
#include "..\config.h"

//#define DEV_DEBUG
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



//出货 
static void DEV_mdbSwitch(ST_MDB *mdb)
{
	uint8 res;
	res = BT_open(1,mdb->column);
	if(res == 1){
		stMdb.result = MDB_COL_SUCCESS;
	}
	else{
		stMdb.result = MDB_COL_FAILED;
	}
	MDB_setRequest(MDB_REQ_FINISH);
}

static void DEV_mdbCtrl(ST_MDB *mdb)
{
	uint8 res;
	res = EV_bento_light(1,mdb->lightCtrl);
	if(res == 1){
		stMdb.result = MDB_COL_IDLE;
	}
	else{
		stMdb.result = MDB_COL_ERROR;
	}
	MDB_setRequest(MDB_REQ_FINISH);
}


static void DEV_mdbReset(ST_MDB *mdb)
{
	uint8 res;
	res = EV_bento_check(1,mdb->bin);
	if(res == 1){
		stMdb.result = MDB_COL_JUSTRESET;
	}
	else{
		stMdb.result = MDB_COL_ERROR;
	}
	MDB_setRequest(MDB_REQ_FINISH);
}

void DEV_taskPoll(void)
{
	ST_MDB *mdb = &stMdb;
	if(MDB_getRequest() == MDB_REQ_HANDLE){
		print_dev("MDB-Request:type= %d\r\n",mdb->type);
		switch(mdb->type){
			case G_MDB_RESET:
				DEV_mdbReset(mdb);
				break;
			case G_MDB_SWITCH:
				DEV_mdbSwitch(mdb);
				break;
			case G_MDB_CTRL:
				DEV_mdbCtrl(mdb);
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
		msleep(20);
	}
}



