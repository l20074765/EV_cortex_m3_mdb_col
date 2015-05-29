#ifndef _BENTO_API_H_
#define _BENTO_API_H_
#include "..\Drivers\board.h"
#include "..\Drivers\mdb_uart.h"

#define BT_HEAD   				0xC7
#define BT_TYPE_OPEN 			0x52
#define BT_TYPE_CHECK 			0x51
#define BT_TYPE_LIGHT			0x56
#define BT_TYPE_HOT 			0x53
#define BT_TYPE_COOL 			0x55

#define BT_TYPE_OPEN_ACK 			0x62
#define BT_TYPE_CHECK_ACK 			0x61
#define BT_TYPE_LIGHT_ACK			0x66
#define BT_TYPE_HOT_ACK 			0x63
#define BT_TYPE_COOL_ACK 			0x65



uint8 EV_bento_hot(uint8 cabinet,uint8 flag);
uint8 EV_bento_col(uint8 cabinet,uint8 flag);
uint8 EV_bento_light(uint8 cabinet,uint8 flag);
uint8  EV_bento_check(uint8 cabinet,ST_BIN *st_bento);
uint8 BT_open(uint8 cabinet,uint8 box);

#endif

/**************************************End Of File*******************************************************/
