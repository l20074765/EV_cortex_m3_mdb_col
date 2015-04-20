#ifndef _GLOBAL_H
#define _GLOBAL_H


#define VMC_DISP_OFF					0
#define VMC_DISP_LED					1
#define VMC_DISP_LCD					2


#define VMC_DISP_SELECT					VMC_DISP_LED  //lcd 1   0 led


#define VMC_TYPE_CHANGER               (0x51) //定义兑零机通用机型
#define VMC_TYPE_TRADE                (0x81) //定义售货机通用机型
#define VMC_TYPE					   VMC_TYPE_CHANGER  //实现机型转换



//=============================================================================
//机型配置
//目前计划将售货机和兑零机整合成一套软件自动识别
//=============================================================================

#define PINTU							(3)

#define VMC_TRACE_ON					1
#define VMC_TRACE_OFF					0
#define VMC_TRACE_FLAG					1 //调试开关



#define VMC_OPEN_COINFG					0 //是否开启配置文件1开启  0关闭



//兑零机配置
//硬币器串行脉冲
#define  VMC_COIN_SERIAL	1
#define  VMC_COIN_MDB		2
#define  VMC_COIN_PARALLEL  3
#define  VMC_COIN_NO		0//关硬币器
#define  VMC_COIN_TYPE     VMC_COIN_MDB


//找零器通道面值
#define VMC_COIN_CHANNEL_1	100
#define VMC_COIN_CHANNEL_2	0
#define VMC_COIN_CHANNEL_3	0
#define VMC_COIN_CHANNEL_4	0
#define VMC_COIN_CHANNEL_5	0
#define VMC_COIN_CHANNEL_6	0
#define VMC_COIN_CHANNEL_7	0
#define VMC_COIN_CHANNEL_8	0

//纸币器MDB

#define VMC_BILL_NO			0//关纸币器
#define VMC_BILL_RS232		1 //GBA
#define VMC_BILL_MDB		2//MDB
#define VMC_BILL_MDB_ITL	3// MDB ITL

#define VMC_BILL_TYPE     VMC_BILL_NO

//找零器

#define VMC_CHANGER_HOPPER		1
#define VMC_CHANGER_NO			0//关找零器
#define VMC_CHANGER_TYPE     VMC_CHANGER_NO

//定义hopper找零 比例
#define HOPPER_LEVEL_RATIO			 {2,1,0} //与hopper数要对应按照hopper面值斗 从大到小 
#define HOPPER_BASE_CHANGED			  500   //找零基数以分为单位

//找零器通道面值
#define VMC_CHANGER_CHANNEL_1	100
#define VMC_CHANGER_CHANNEL_2	200
#define VMC_CHANGER_CHANNEL_3	0




//小数点位置
#define VMC_PIONT_0					0
#define VMC_PIONT_1					1
#define VMC_PIONT_2					2


#define VMC_PIONT_TYPE      VMC_PIONT_1

//定义货道配置文件
#define VMC_HUODAO_OPEN_CONFIG     1 //是否开启货道文件配置
#define VMC_HUODAO_LEVEL_OPEN     {1,0} //定义层开关

//定义货道开关
#define VMC_HUODAO_COLUMN_OPEN    {{1,0,0,1,1,1,0,0},   \
								   {0,0,0,0,0,0,0,0} }



//======================机型配置结束=======================================================


//=============================================================================
//宏函数及表达式
//=============================================================================

#define HUINT16(n)				(unsigned char)(((n) >> 8) & 0xff)//16位数值高8位
#define LUINT16(n)				(unsigned char)((n) & 0xff)//16位数值低8位	
#define H0UINT32(n)				(unsigned char)(((n) >> 24) & 0xff)//32位数值第一个高8位
#define H1UINT32(n)				(unsigned char)(((n) >> 16) & 0xff)//32位数值第二个高8位
#define L0UINT32(n)				(unsigned char)(((n) >> 8) & 0xff)//32位数值第一个低8位
#define L1UINT32(n)				(unsigned char)((n) & 0xff)//32位数值第二个低8位
#define INTEG16(h,l)			(unsigned short)(((unsigned short)(h) << 8) | ((l) & 0xff))
	//四个unsigned char 数据整合成unsigned int型数据
#define INTEG32(h0,l0,h1,l1)	(unsigned int)(((unsigned int)(h0) << 24) | \
									((unsigned int)(l0) << 16) | \
									((unsigned int)(h1) << 8) | \
									((l1) & 0xff))



//是否开启Hopper找零功能
#define HOPPER_OPEN						(3)
//是否开启纸币器收币功能
#define BILL_OPEN						(4)


extern OS_EVENT *g_KeyMsg;









#endif
