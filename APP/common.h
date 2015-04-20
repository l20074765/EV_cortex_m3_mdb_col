/****************************************Copyright (c)*************************************************
**                      Fujian Junpeng Communicaiton Technology Co.,Ltd.
**                               http://www.easivend.com.cn
**--------------File Info------------------------------------------------------------------------------
** File name:           common.h
** Last modified Date:  2013-01-06
** Last Version:        No
** Descriptions:        系统参数定义及通用函数部分                     
**------------------------------------------------------------------------------------------------------
** Created by:          sunway
** Created date:        2013-01-06
** Version:             V0.1
** Descriptions:        The original version        
********************************************************************************************************/
#ifndef __COMMON_H 
#define __COMMON_H

#define print_log(...)	




//==============================定时器计数变量结构体========================================================

typedef struct _timer_st_{

	volatile unsigned char secTimer;//定义秒级 定时

//==============================10毫秒级定时器变量=====================================================================
	volatile unsigned short printTimer;
	volatile unsigned short pcm_handle_timeout;
	volatile unsigned short checkDeviceTimeout;
	volatile unsigned short HpHandleTimer;
	volatile unsigned int bill_comunication_timeout;//MDB通讯超时时间
	volatile unsigned short led_paoma_timer;
	volatile unsigned char  user_led_green;

//==============================秒级定时器变量=====================================================================
	volatile unsigned short sec_usr_op_timer;//用户操作超时	
	volatile unsigned short sec_changer_timer;
	volatile unsigned short sec_hopper_state_timer;//hopper  定时复位
	volatile unsigned short sec_pccon_timer;//处理PC命令超时
	volatile unsigned char	PayoutTimer;//出币等待定时器
	volatile unsigned char  sec_coin_reset_timer;


}TIMER_ST;
extern TIMER_ST Timer;





/************************************************************************************
*邮箱结构体
***************************************************************************************/
//纸币器任务通讯结构体
typedef struct _task_msg_bill_{

	 unsigned char type;
	 unsigned char status;
	 unsigned int  amount;
	 unsigned int  change;

}TASK_MSG_BILL;

//硬币器任务通讯结构体
typedef struct _task_msg_coin_{

	 unsigned char type;
	 unsigned char status;
	 unsigned int  amount;
	 unsigned int  change;


}TASK_MSG_COIN;


//hopper 任务通讯结构体
typedef struct _task_msg_hopper_{

   	 unsigned char value8;
	 unsigned short value16;
	 unsigned int  value32;

}TASK_MSG_HOPPER;
//定义主任务与设备任务之间通信的结构体
typedef struct
{
    unsigned char   type;				//命令类型
	unsigned char   subType;		   //子集命令类型
	TASK_MSG_BILL 	bill;
	TASK_MSG_COIN 	coin;
	TASK_MSG_HOPPER hopper;

	
} MAIN_DEV_TASK_MSG;




//任务通信邮箱的对发邮箱句柄
extern OS_EVENT *g_msg_main_to_dev;
extern OS_EVENT *g_msg_dev_to_main;
extern MAIN_DEV_TASK_MSG task_msg_main_to_dev;
extern MAIN_DEV_TASK_MSG task_msg_dev_to_main;


//纸币器邮箱
extern OS_EVENT *g_billIN;
typedef struct _bill_recv_msg_{
	unsigned char channel;
	unsigned int  value;
	
}BILL_RECV_MSG;
#define G_BILL_IN_SIZE    20  
extern BILL_RECV_MSG bill_recv_msg[G_BILL_IN_SIZE];


//硬币器通信消息队列
#define G_COIN_IN_SIZE    100
extern OS_EVENT *g_CoinIn;
extern unsigned char  CoinIn[G_COIN_IN_SIZE];


//初始化任务信号量
extern OS_EVENT *g_InitSem;


/*---------------------------------------------------------
任务间通信设备任务接受的操作指令集合
-----------------------------------------------------------*/
#define TASK_DISABLE           	1 //主任务发送禁能 请求

#define TASK_ENABLE            	3 //主任务发送使能 请求
 	 
#define TASK_HOPPER         	5 //主任务发送hopper 请求

#define TASK_INIT		    	7//主任务发送初始化请求

#define TASK_CHANGER			9 //找零请求

#define TASK_OVER				0xAA
#define TASK_NOT_ACK			0x0A
//请求对象
#define OBJ_BILL					0x0001
#define OBJ_COIN					0x0002
#define OBJ_HOPPER					0x0004

#define OBJ_ALL						0xFFFF



//======================================================================================
unsigned char XorCheck(unsigned char *pstr,unsigned short len);
unsigned short CrcCheck(unsigned char *msg, unsigned short len);
void msleep(unsigned int msec);
unsigned char mbox_post_main_to_dev(unsigned char type);
unsigned char mbox_post_dev_to_main(unsigned char type);
void CreateCommonMBox(void);



#endif
/**************************************End Of File*******************************************************/
