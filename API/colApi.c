
#include "colApi.h"
#include "..\config.h"


#define COL_GOODS_1		0xE0
#define COL_GOODS_2		0xE8

#define COL_FOODS_1		0x80
#define COL_FOODS_2		0x88

#define RESET			0x00
#define SWITCH			0x01
#define CTRL			0x02
#define COLUMN			0x03
#define POLL			0x04
#define STATUS			0x05




static unsigned char recvbuf[36] = {0};
static unsigned char recvlen = 0;

COL_BIN stColBin;
COL_INFO stColInfo;
unsigned char colState = COL_POLL_IDLE;

static unsigned char colDev = COL_GOODS_1;

/*********************************************************************************************************
** Function name:       col_send
** Descriptions:        货道指令发送
** input parameters:    dev:设备号
**						*data:发送缓冲区 len:发送长度
** output parameters:   
** Returned value:      0:通信超时  1接受正确  2 数据错误
*********************************************************************************************************/
static unsigned char col_send(unsigned char dev,unsigned char *data,unsigned char len)
{
	unsigned char res;
	memset(recvbuf,0,sizeof(recvbuf));
	res = MDB_conversation(dev,data,len,recvbuf,&recvlen);
	return (res == 1) ?  1 : (res == 0 ? 0 : 2);
}




/*********************************************************************************************************
** Function name:       col_reset
** Descriptions:        货道复位
** input parameters:    dev 机型 
**						
** output parameters:   
** Returned value:      0:通信超时  1接受正确  2 数据错误
*********************************************************************************************************/
unsigned char col_reset(unsigned char dev)
{
	unsigned char res;
	res = col_send(dev + RESET,NULL,0);
	return (res == 1);
}



/*********************************************************************************************************
** Function name:       col_switch
** Descriptions:        货道出货指令
** input parameters:    dev 机型  column货道号
**						
** output parameters:   
** Returned value:      0:通信超时  1接受正确  2 数据错误
*********************************************************************************************************/

unsigned char col_switch(unsigned char dev,unsigned char column)
{
	unsigned char res,buf[2] = {0x00};
	buf[0] = column;
	res = col_send(dev + SWITCH,buf,2);
	return (res == 1);
}


/*********************************************************************************************************
** Function name:       col_ctrl
** Descriptions:        货道控制指令
** input parameters:    dev 机型 
**						
** output parameters:   
** Returned value:      0:通信超时  1接受正确  2 数据错误
*********************************************************************************************************/

unsigned char col_ctrl(unsigned char dev,COL_CTRL *col_ctrl)
{
	unsigned char res,buf[4] = {0x00};
	
	if(col_ctrl->cool == 1){
		buf[0] |= COL_BIT_COOL;
		buf[1] = col_ctrl->coolTemp;
	}
	
	if(col_ctrl->light == 1){
		buf[0] |= COL_BIT_LIGHT;
	}
	if(col_ctrl->hot == 1){
		buf[0] |= COL_BIT_HOT;
		buf[2] = col_ctrl->hotTemp;
	}
	
	res = col_send(dev + CTRL,buf,4);
	return (res == 1);
}



/*********************************************************************************************************
** Function name:       col_column
** Descriptions:        货道获取货道
** input parameters:    dev 机型 
**						
** output parameters:   
** Returned value:      0:通信超时  1接受正确  2 数据错误
*********************************************************************************************************/

unsigned char col_column(unsigned char dev,COL_BIN *bin)
{
	unsigned char res,i,temp,j,index = 0;
	res = col_send(dev + COLUMN,NULL,0);
	if(res == 1){
		for(i = 0;i < 8;i++){
			temp = recvbuf[7 - i];
			for(j = 0;j < 8;j++){
				if(temp & (0x01 << j)){ //售完
					bin->col[index].empty = 1;
				}
				else{
					bin->col[index].empty = 1;
				}
				index++;
			}
		}
		
		bin->sum = recvbuf[8];
		bin->sensorFault = recvbuf[9];
		bin->coolTemp = recvbuf[10];
		bin->hotTemp = recvbuf[11];
	}
	
	
	return (res == 1);
}



/*********************************************************************************************************
** Function name:       col_poll
** Descriptions:        货道轮询
** input parameters:    dev 机型   
**						
** output parameters:   
** Returned value:      0:通信超时  1接受正确  2 数据错误
*********************************************************************************************************/
unsigned char col_poll(unsigned char dev,unsigned char *s)
{
	unsigned char res;
	res = col_send(dev + POLL,NULL,0);
	if(res == 1){
		*s = recvbuf[0];
	}
	else{
		*s = COL_POLL_IDLE;
	}
	
	return (res == 1);
}

/*********************************************************************************************************
** Function name:       col_status
** Descriptions:        货道获取货道
** input parameters:    dev 机型 
**						
** output parameters:   
** Returned value:      0:通信超时  1接受正确  2 数据错误
*********************************************************************************************************/
unsigned char col_status(unsigned char dev,COL_INFO *info)
{
	unsigned char res,index = 0;
	res = col_send(dev + STATUS,NULL,0);
	if(res == 1){
		info->mcb_ver = recvbuf[index++];
		info->svc_ver = recvbuf[index++];
		info->columnNums = recvbuf[index++];
		info->coolCtrl = recvbuf[index++];
		info->hotCtrl = recvbuf[index++];
		info->emptyCtrl = recvbuf[index++];
		
	}
	
	
	return (res == 1);

}



/*********************************************************************************************************
** Function name:       colTaskPoll
** Descriptions:        货道轮询控制
** input parameters:    dev 机型 
**						
** output parameters:   
** Returned value:      
*********************************************************************************************************/

void colTaskPoll(void)
{
	col_poll(colDev,&colState);
}



