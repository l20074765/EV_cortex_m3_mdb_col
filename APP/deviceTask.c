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
	res = BT_open(mdb->binNo,mdb->sw.col);
	if(res == 1){
		MDB_setStatus(mdb->mdbAddr,MDB_COL_SUCCESS);
	}
	else{
		MDB_setStatus(mdb->mdbAddr,MDB_COL_FAILED);
	}
	
}

static void DEV_mdbCtrl(ST_MDB *mdb)
{
	uint8 res;
	if(mdb->bin.islight == 1){
		res = EV_bento_light(mdb->binNo,mdb->ctrl.lightCtrl);
	}
	
	if(mdb->bin.iscool == 1){
		res = EV_bento_light(mdb->binNo,mdb->ctrl.coolCtrl);
	}
	if(mdb->bin.ishot == 1){
		res = EV_bento_light(mdb->binNo,mdb->ctrl.hotCtrl);
	}
	
	
	if(res == 1){
		MDB_setStatus(mdb->mdbAddr,MDB_COL_IDLE);
	}
	else{
		MDB_setStatus(mdb->mdbAddr,MDB_COL_ERROR);
	}
	
}


static void DEV_mdbReset(ST_MDB *mdb)
{
	uint8 res;
	print_dev("DEV_mdbReset:%d\r\n",mdb->binNo);
	res = 1;
	//res = EV_bento_check(mdb->binNo,&mdb->bin);
	if(res == 1){
		MDB_setStatus(mdb->mdbAddr,MDB_COL_JUSTRESET);
	}
	else{
		MDB_setStatus(mdb->mdbAddr,MDB_COL_ERROR);
	}
	//print_dev("MDB_getRequest() = %d\r\n",MDB_getStatus(mdb->mdbAddr));
	
}





void DEV_taskPoll(void)
{
	ST_MDB *mdb = NULL;
	if(MDB_getRequest(&mdb) == 1){
		switch(mdb->cmd){
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
	//uint32 i;
	//系统基本接口初始化
	SystemInit();
	//print_dev("Hello Booooo...\r\n");
	FIO2DIR &= ~(0x01UL << 2);
	//建立邮箱、信号量	
	CreateMBox();
	MDB_binInit();//初始化柜子
	while(1){
		DEV_taskPoll();
		msleep(20);
	}
}



