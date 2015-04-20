
#include "../config.h"
#include "../APP/task_device.h"


#define DB_BUF_SIZE		256
static unsigned char recvbuf[DB_BUF_SIZE] = {0};//接收缓存区
static unsigned char sendbuf[DB_BUF_SIZE] = {0};

static unsigned char addr = 0x31,sn = 0;





/*********************************************************************************************************
** Function name:     	xorCheck
** Descriptions:	    
** input parameters:    
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
uchar xorCheck(uchar *ptr,uchar len)
{
	uchar  i,crc = 0;
	for(i = 0;i < len;i++)
		crc ^= ptr[i];
		
	return crc;	
}


/*********************************************************************************************************
** Function name:     	DB_package
** Descriptions:	    驱动板数据组包
** input parameters:   mt 发送类型 len 发送数据长度
** output parameters:   无
** Returned value:      1成功  0失败
*********************************************************************************************************/
void DB_package(uchar mt,uchar len)
{
	sendbuf[SF] = HEAD_DB;
	sendbuf[LEN] = len;
	sendbuf[ADDR] = addr;
	sendbuf[SN] = sn;
	sendbuf[MT] = mt;
	sendbuf[len] = xorCheck(sendbuf,len);
}


/*********************************************************************************************************
** Function name:     	DB_uartSend
** Descriptions:	    驱动板往PC串口发送数据接口
** input parameters:    无
** output parameters:   无
** Returned value:      1成功  0失败
*********************************************************************************************************/
void DB_uartSend(void)
{
	ClrUart0Buff();
	Uart0PutStr(sendbuf,sendbuf[LEN] + 1);
}


/*********************************************************************************************************
** Function name:     	DB_uartRecv
** Descriptions:	      串口数据接收函数
** input parameters:    无
** output parameters:   无
** Returned value:      0 无数据接收 1接收正确数据  2接收数据错误 3超时
*********************************************************************************************************/
uchar DB_uartRecv(void)
{
	uchar ch,index = 0,len;
	uchar crc;
	if(!Uart0BuffIsNotEmpty()){
		return 0;
	}
	Timer.pcm_handle_timeout = 50;
	memset(recvbuf,0,sizeof(recvbuf));
	while(Timer.pcm_handle_timeout){
			if(Uart0BuffIsNotEmpty()){
					ch = Uart0GetCh();
					recvbuf[index++] = ch;
					if(index == (SF + 1)){
							if(ch != HEAD_PC ) 
								return 0;
					}
					else if(index == (LEN + 1))
							len = ch;
					else if(index >= (len + 1))
							break;
			}
	}
	if(!Timer.pcm_handle_timeout){
			return 3;
	}
	crc = xorCheck(recvbuf,len);
	if(crc != recvbuf[len]){
			return 2;
	}
	return 1;//正确接收包
}


void DB_control_rpt()
{
	
}


/*********************************************************************************************************
** Function name:     	DB_task
** Descriptions:	      通信协议主函数
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void DB_task(void)
{
	uchar type;
	if(DB_uartRecv() != 1){
		return;
	}
	if(recvbuf[ADDR] == 0xFF){
		//DB_addrRpt();//配置地址
	}
	else if(recvbuf[ADDR] == addr){
		//本机地址
		if(recvbuf[SN] != sn){
			sn = recvbuf[SN];
			type = recvbuf[MT + 1];
			if(recvbuf[MT] == DB_MT_CHECK_REQ)//货道驱动板查询命令
			{
				if(type == MDB_CHECK_REQ){

				}
				else if(type == MDB_BILL_INFO_REQ){//查询驱动板信息
					
				}
				else if(type == MDB_COIN_INFO_REQ){

				}
				else{
					return;
				}
					
			}
			else if(recvbuf[MT] == DB_MT_ACTION_REQ)//货道驱动板控制命令
			{
				if(type == MDB_INIT_REQ)
				{
					//sendbuf[MT + 1] = COL_SET_CAB_RPT;	
				}
				else if(type == MDB_CONTROL_REQ){
					DB_control_rpt();
				}
				else if(type == MDB_PAYBACK_REQ){

				}
				else if(type == MDB_COST_REQ){

				}
				else if(type == MDB_PAYOUT_REQ){

				}
				else
					return;
			}
			else{
				return;
			}
		}
		else{
			//相同序列号返回上一次结果
			
		}

	}
	else{
		return;
	}
	DB_uartSend();	
	
}










