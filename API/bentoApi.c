#include "bentoApi.h"
#include "..\config.h"

typedef unsigned char (*FUN_uartIsNotEmpty)(void);
typedef unsigned char (*FUN_uartGetCh)(void);
typedef void (*FUN_uartClear)(void);
typedef void (*FUN_uartPutStr)(unsigned char const *Str, unsigned int Len);

#if 0
static FUN_uartIsNotEmpty uartIsNotEmpty = Uart3BuffIsNotEmpty;
static FUN_uartGetCh uartGetCh = Uart3GetCh;
static FUN_uartClear uartClear = uart3_clr_buf;
static FUN_uartPutStr uartPutStr = Uart3PutStr;
#else
static FUN_uartIsNotEmpty uartIsNotEmpty = Uart0BuffIsNotEmpty;
static FUN_uartGetCh uartGetCh = Uart0GetCh;
static FUN_uartClear uartClear = ClrUart0Buff;
static FUN_uartPutStr uartPutStr = Uart0PutStr;
#endif
uint8 BT_recv(uint8 *rdata,uint8 *rlen)
{
    uint8 index = 0,len = 0,ch;
    uint16 crc;
	*rlen = 0;
    if(rdata == NULL){
		return 0;
	}   
	Timer.bentoTimeout = 100;
    while(Timer.bentoTimeout){ 
		if(uartIsNotEmpty()){
			ch = uartGetCh();	
			rdata[index++] = ch;
			if(index == 1){
                if(ch != BT_HEAD + 1){
					index = 0;
				}    
            }
            else if(index == 2){
                len = ch;
            }
            else if(index >= (len + 2)){
                crc = CrcCheck(rdata,len);
                if(crc == INTEG16(rdata[len],rdata[len + 1])){
                    *rlen = len + 2;
                     return 1;
                }
                else
                    return 0;
            }
		}
	}
	return 0;
	
}




uint8 BT_send(uint8 cmd,uint8 cabinet,uint8 arg,uint8 *rdata)
{
    uint8 buf[24] = {0},len = 0,ret,rbuf[64] = {0};
    uint16 crc;
	buf[len++] = BT_HEAD;
	buf[len++] = 0x07;
	buf[len++] = cabinet - 1;
	buf[len++] = cmd;
	buf[len++] = cabinet - 1;
	buf[len++] = cabinet - 1;//0x08;
	buf[len++] = arg;//0x00;	
	crc = CrcCheck(buf,len);
	buf[len++] = HUINT16(crc);
	buf[len++] = LUINT16(crc);
	uartClear();
	uartPutStr(buf,len);
	ret = BT_recv(rbuf,&len);
	if(ret == 1){
		if(cmd == BT_TYPE_OPEN){
			if(rbuf[3] == BT_TYPE_OPEN_ACK){
				return 1;
			}
		}
		else if(cmd == BT_TYPE_CHECK){
			if(rbuf[3] == BT_TYPE_CHECK_ACK){
				if(rdata != NULL){
					memcpy(rdata,rbuf,rbuf[1]);
				}
				return 1;
			}
		}
		else if(cmd == BT_TYPE_LIGHT){
			if(rbuf[3] == BT_TYPE_LIGHT_ACK)
				return 1;
			else
				return 0;
		}
	}
	return 0;
}



uint8 BT_open(uint8 cabinet,uint8 box)
{
    uint8 ret;
	if(cabinet <= 0 || box <= 0)
		return 0;	
    ret = BT_send(BT_TYPE_OPEN,cabinet,box,NULL);
    return ret;
}


uint8 EV_bento_light(uint8 cabinet,uint8 flag)
{
	int ret = 0;
	if(cabinet <= 0)
		return 0;
    ret = BT_send(BT_TYPE_LIGHT,cabinet,flag,NULL);
	return ret;
}


uint8  EV_bento_check(uint8 cabinet,ST_BIN *st_bento)
{
	int ret = 0;
    uint8 buf[20] = {0},i;
    if(st_bento == NULL) {
        return 0;
    }
    if(cabinet <= 0){
        return 0;
    }

	ret = BT_send(BT_TYPE_CHECK,1,0x00,buf);
	if(ret == 1)
	{
        st_bento->sum = buf[6];
		st_bento->ishot = (buf[8] & 0x01);
		st_bento->iscool = ((buf[8] >> 1) & 0x01);
		st_bento->islight = ((buf[8] >> 2) & 0x01);
        memset(st_bento->id,0,sizeof(st_bento->id));
        st_bento->id_len = 7;
        st_bento->type = 5;
		for(i = 0;i < 7;i++){
			st_bento->id[i] = buf[9 + i];
		}
        for(i = 0;i < st_bento->sum;i++){
            st_bento->col[i].empty = 0;
        }
		return 1;
	}
    return 0;
}





/**************************************End Of File*******************************************************/
