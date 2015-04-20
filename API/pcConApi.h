
#ifndef _PCCONAPI_H_
#define _PCCONAPI_H_

//PC --> DB ºê
#define HEAD_PC     0xE5



//DB --> PC ºê
#define     SF          0
#define     LEN         1
#define     ADDR        2
#define     SN          3
#define     MT          4
#define     DATA        5
#define     HEAD_LEN    5
#define     HEAD_DB     0xE6


#define		DB_MT_CHECK_REQ		0x21
#define		DB_MT_CHECK_RPT		0x91

#define 	DB_MT_ACTION_REQ	0x22
#define 	DB_MT_ACTION_RPT	0x92

#define 	MDB_CHECK_REQ 		0x01
#define 	MDB_CHECK_RPT 		0x01

#define 	MDB_BILL_INFO_REQ 	0x02
#define 	MDB_BILL_INFO_RPT	0x02

#define 	MDB_COIN_INFO_REQ 	0x03
#define 	MDB_COIN_INFO_RPT	0x03


#define 	MDB_INIT_REQ 		0x01
#define 	MDB_INIT_RPT		0x01


#define 	MDB_CONTROL_REQ 	0x02
#define 	MDB_CONTROL_RPT		0x02

#define 	MDB_PAYOUT_REQ 		0x03
#define 	MDB_PAYOUT_RPT		0x03

#define 	MDB_PAYBACK_REQ 	0x04
#define 	MDB_PAYBACK_RPT		0x04

#define 	MDB_COST_REQ 		0x05
#define 	MDB_COST_RPT		0x05










typedef unsigned char uchar;
typedef unsigned short ushort;


#define print_err(...)


void DB_task(void);





#endif
