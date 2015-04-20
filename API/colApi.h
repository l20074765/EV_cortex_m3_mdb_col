
#ifndef _COL_API_H_
#define _COL_API_H_



#define COL_POLL_IDLE		0x00
#define COL_POLL_BUSY		0x01
#define COL_POLL_FAIL		0x02
#define COL_POLL_SUC		0x04
#define COL_POLL_ERR		0x05
#define COL_POLL_JUSTRESET	0x06


#define COL_BIT_COOL	(0x01U << 0)
#define COL_BIT_LIGHT	(0x01U << 1)
#define COL_BIT_HOT		(0x01U << 3)


typedef struct _col_ctrl_{
	unsigned char cool; //制冷开关 0关 1开
	unsigned char light;//照明开关 0关 1开
	unsigned char hot;//加热开关 0关 1开
	unsigned char coolTemp; //制冷温度
	unsigned char hotTemp;//加热温度

}COL_CTRL;


typedef struct _col_column_{

	unsigned char empty;

}COL_COLUMN;


typedef struct _col_bin_{
	unsigned char sum;
	unsigned char sensorFault;
	unsigned char coolTemp;
	unsigned char hotTemp;
	COL_COLUMN col[80];

}COL_BIN;



typedef struct _col_info_{
	unsigned char mcb_ver;
	unsigned char svc_ver;
	unsigned char columnNums;
	unsigned char coolCtrl;
	unsigned char hotCtrl;
	unsigned char emptyCtrl;	

}COL_INFO;



unsigned char col_status(unsigned char dev,COL_INFO *info);
void colTaskPoll(void);





#endif

