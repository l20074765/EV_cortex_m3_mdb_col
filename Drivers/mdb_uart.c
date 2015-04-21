/****************************************Copyright (c)*************************************************
**                      Fujian Junpeng Communicaiton Technology Co.,Ltd.
**                               http://www.easivend.com.cn
**--------------File Info------------------------------------------------------------------------------
** File name:           mdb_uart.h
** Last modified Date:  2013-01-08
** Last Version:         
** Descriptions:        uart2配置MDB 从机模式 收发数据                   
**------------------------------------------------------------------------------------------------------
** Created by:          yoc
** Created date:        2015-04-20
** Version:             V0.1
** Descriptions:        The original version       
********************************************************************************************************/

#include "mdb_uart.h"
#include "..\config.h"

#define MDB_BUF_SIZE	36

static volatile unsigned char  recvbuf[MDB_BUF_SIZE];
static volatile unsigned char  rx;
static volatile unsigned char  tx;
static volatile unsigned char  crc;


#define MDB_DEV_IDLE			0
#define MDB_DEV_START			1
#define MDB_DEV_RECV_ACK		2


static volatile unsigned char  mdb_status = MDB_DEV_IDLE;
static volatile unsigned char mdb_addr = 0;
static volatile unsigned char mdb_cmd = 0;

static volatile uint8 mdb_col_status = MDB_COL_IDLE;
ST_BIN stBin;


/*********************************************************************************************************
** MDB通信消息队列
*********************************************************************************************************/
static uint8 g_mdb_in	= 	0;
OS_EVENT *g_mdb_event; //定义MDB事件
G_MDB_ST  g_mdb_st[G_MDB_SIZE];//定义MDB消息结构体
void *g_mdb[G_MDB_SIZE]; //定义MDB消息数组指针


/*********************************************************************************************************
** Function name:     	MDB_createMbox
** Descriptions:	    创建MDB通信邮箱
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void MDB_createMbox(void)
{
	g_mdb_event = OSQCreate(&g_mdb[0],G_MDB_SIZE);
}



void MDB_mboxSend(uint8 type)
{
	g_mdb_st[g_mdb_in].type = type;
	g_mdb_st[g_mdb_in].bin = &stBin;
	OSQPost(g_mdb_event,&g_mdb_st[g_mdb_in]);
	g_mdb_in = (g_mdb_in + 1) % G_MDB_SIZE;
}




/*********************************************************************************************************
** Function name:     	uart2Init
** Descriptions:	    串口初始化，设置为8位数据位，1位停止位，波特率为9600
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void uart2Init(void)
{	
	PCONP = PCONP | (1<<24);
    U2LCR  = 0x80;                         
    U2DLM  = 0;
    U2DLL  = 125;
	U2FDR  = 0x41; 
    U2LCR  = 0x03;                      
    U2FCR  = 0x07;                      
    U2IER  = 0x05;
	uart2SetParityMode(PARITY_F_0);//初始化需要将串口设置成0校验模式 用于接收地址
	uart2Clear();
	memset((void *)recvbuf,0x00,MDB_BUF_SIZE);                       
    zyIsrSet(NVIC_UART2,(unsigned long)Uart2IsrHandler,PRIO_FIVE); 
	
}



/*********************************************************************************************************
** Function name:     	uart2Clear
** Descriptions:	    清除串口2接收缓冲区
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void uart2Clear(void) 
{
	rx = 0;
	tx = 0;
}

/*********************************************************************************************************
** Function name:     	uart2SetParityMode
** Descriptions:	    设置串口奇偶校验位
** input parameters:    mode:奇偶校验设置,即设置MDB协议的标志位
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void uart2SetParityMode(unsigned char mode) 
{
	switch(mode) 
	{		
		case PARITY_ODD	: 	U2LCR = UART_LCR_PARITY_ODD|UART_LCR_PARITY_EN|UART_LCR_WLEN8;
						 	break;
		case PARITY_EVEN:	U2LCR = UART_LCR_PARITY_EVEN|UART_LCR_PARITY_EN|UART_LCR_WLEN8;
						 	break;
		case PARITY_F_1	:	U2LCR = UART_LCR_PARITY_F_1|UART_LCR_PARITY_EN|UART_LCR_WLEN8;
						 	break;
		case PARITY_F_0	:	U2LCR = UART_LCR_PARITY_F_0|UART_LCR_PARITY_EN|UART_LCR_WLEN8;
						 	break;
		default:		 	U2LCR = UART_LCR_WLEN8;
						 	break;
	}
}



/*********************************************************************************************************
** Function name:     	Uart2PutCh
** Descriptions:	    向串口2发送一个字节
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void Uart2PutCh(unsigned char dat) 
{
   while(!(U2LSR & UART_LSR_THRE));
   U2THR = dat; 
}




/*********************************************************************************************************
** Function name:     	MDB_putChr
** Descriptions:	    向MDB总线上送出一个字节的数据
** input parameters:    dat--要送的数据；mode--发送方式：0为地址字节/1为数据字节;按MDB协议处理
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void MDB_putChr(unsigned char dat,unsigned char mode)
{
	switch(mode) 
	{
		case MDB_DAT :	uart2SetParityMode(PARITY_F_0); //强制0 校验 发送MDB数据
						Uart2PutCh(dat);
						break;
		case MDB_ADD:	uart2SetParityMode(PARITY_F_1); // 强制1校验 发送MDB地址
						Uart2PutCh(dat);
						break;
		default	:		uart2SetParityMode(PARITY_DIS);
						Uart2PutCh(dat);
						break;
	}
	while(!(U2LSR & UART_LSR_TEMT));
	
}



/*********************************************************************************************************
** Function name:     	Uart2IsrHandler
** Descriptions:	    串口2中断处理函数
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void Uart2IsrHandler(void) 
{
	volatile unsigned char dummy;
	unsigned int intsrc, tmp, tmp1;
	unsigned char udata;
	OSIntEnter();	
	intsrc = U2IIR;									//Determine the interrupt source 
	tmp = intsrc & UART_IIR_INTID_MASK;				//UART_IIR_INTID_MASK = 0x0000000E,U2IIR[3:1]为中断标识
	//U2IIR[3:1] = 011;RLS接收线状态，产生该中断为四个条件之一(OE,PE,FE,BI);需要通过查看LSR寄存区得到错误原因
	if(tmp == UART_IIR_INTID_RLS) 
	{
		tmp1 = U2LSR;
		tmp1 &= (UART_LSR_OE | UART_LSR_PE | UART_LSR_FE | UART_LSR_BI | UART_LSR_RXFE);
		if(tmp1 & UART_LSR_PE)//每次VMC发送完MDB数据后
		{
			udata = U2RBR & UART_RBR_MASKBIT;	//数据可用			
			if(mdb_status == MDB_DEV_IDLE){ //收到第一个字节 即为地址号
				mdb_addr = udata & 0xF8;
				mdb_cmd = udata & 0x07;
				if(MDB_colAddrIsOk(mdb_addr)){ //地址正确 开始接收MDB数据
					mdb_status = MDB_DEV_START;
					rx = 0;
					recvbuf[rx++] = udata;
					crc = udata;//校验码
				}		
			}				
		}
		else if(tmp1)//其他错误引起的中断则忽略掉 
		{		
		    dummy = U2RBR & UART_RBR_MASKBIT;
		}
	}
	//U2IIR[3:1]=010接收数据可用 || U2IIR[3:1]=110字符超时指示	  
	if((tmp == UART_IIR_INTID_RDA) || (tmp == UART_IIR_INTID_CTI))  
	{	    
		udata = U2RBR & UART_RBR_MASKBIT;
		if(mdb_status == MDB_DEV_START){
			recvbuf[rx++] = udata;
			if(rx < MDB_BUF_SIZE){
				if(MDB_recvOk(rx)){ //接收数据完成准备发送回应 必须尽快
					if(crc == recvbuf[rx - 1]){
						MDB_analysis();
						mdb_status = MDB_DEV_RECV_ACK;
					}
					else{
						mdb_status = MDB_DEV_IDLE;
					}
				}
				else{
					crc += udata;
				}
			}
			else{
				mdb_status = MDB_DEV_IDLE;
			}
		}
		else if(mdb_status == MDB_DEV_RECV_ACK){
			mdb_status = MDB_DEV_IDLE;
			if(udata == MDB_ACK){ //收到主机的ACK 至此确定主机收到从机的数据包
				mdb_status = MDB_DEV_IDLE;
				MDB_recv_ack(mdb_cmd);
			}
		}
	}
    OSIntExit();
}







unsigned char MDB_colAddrIsOk(unsigned char addr)
{
	if(addr == COL_GOODS_A || addr == COL_GOODS_B ||
		addr == COL_FOODS_A || addr == COL_FOODS_B){					
		return 1;			
	}
	else{
		return 0;
	}	
}


unsigned char MDB_recvOk(unsigned char len)
{
	unsigned char ok = 0;
	switch(mdb_cmd){
		case RESET : case COLUMN :case POLL :case STATUS :
			if(len >= 2) ok = 1;
			break;
		case SWITCH: 
			if(len >= 4) ok = 1;
			break;
		case CTRL:	
			if(len >= 6) ok = 1;
			break;
		default:break;
	}
	
	return ok;
}


void MDB_recv_ack(uint8 cmd)
{
	cmd = cmd;
	#if 0
	if(cmd == POLL){
		if(mdb_col_status != MDB_COL_BUSY && 
			mdb_col_status != MDB_COL_IDLE){
			mdb_col_status = MDB_COL_IDLE;
		}
	}
	#endif
}


uint8 MDB_send(uint8 *data,uint8 len)
{
	uint8 i,crc = 0; 
	//OSIntEnter(); //此函数已在中断中运行 中断已经关闭了
	if(len == 0){
		MDB_putChr(MDB_ACK,MDB_ADD);
	}
	else{
		for(i = 0;i < len;i++){
			MDB_putChr(data[i],MDB_DAT);
			crc += data[i];
		}
		MDB_putChr(crc,MDB_ADD);
	}
	uart2SetParityMode(PARITY_F_0);
	//OSIntExit();
	return 1;	
}






void MDB_setColStatus(uint8 type)
{
	OSIntEnter();
	mdb_col_status = type;
	OSIntExit();
}


static void MDB_poll_rpt(void)
{
	uint8 buf[2] = {0};
	buf[0] = mdb_col_status;
	MDB_send(buf,1);
}

static void MDB_reset_rpt(void)
{
	MDB_send(NULL,0);
	memset(&stBin,0,sizeof(stBin));
	mdb_col_status = MDB_COL_BUSY;
	MDB_mboxSend(G_MDB_RESET);
}


static void MDB_switch_rpt(void)
{
	uint8 column;
	column = recvbuf[1];
	MDB_send(NULL,0);
	mdb_col_status = MDB_COL_BUSY;
	g_mdb_st[g_mdb_in].column = column;
	MDB_mboxSend(G_MDB_SWITCH);
}

static void MDB_ctrl_rpt(void)
{
	g_mdb_st[g_mdb_in].ctrl = recvbuf[1];
	g_mdb_st[g_mdb_in].coolTemp = (int8)recvbuf[2];
	g_mdb_st[g_mdb_in].hotTemp  = (int8)recvbuf[3];
	
	MDB_send(NULL,0);
	mdb_col_status = MDB_COL_BUSY;
	MDB_mboxSend(G_MDB_CTRL);
}

static void MDB_column_rpt(void)
{
	uint8 index = 0,i,j,temp,colindex = 0;
	uint8 buf[36] = {0};
	stBin.sum = 64;
	for(i = 0;i < (stBin.sum / 8);i++){
		temp = 0;
		for(j = 0;j < 8;j++){
			if(stBin.col[colindex++].empty == 1){
				temp |= (0x01 << j);
			}
		}
		buf[index++] = temp;
	}
	
	if(stBin.sum % 8){
		temp = 0;
		for(j = 0;j < (stBin.sum % 8);j++){
			if(stBin.col[colindex++].empty == 1){
				temp |= (0x01 << j);
			}
		}
		buf[index++] = temp;
	}
	buf[index++] = stBin.sum;
	buf[index++] = stBin.sensorFault & 0x01;
	buf[index++] = stBin.coolTemp;
	buf[index++] = stBin.hotTemp;	
	MDB_send(buf,index);
	
}


static void MDB_status_rpt(void)
{
	uint8 index = 0,buf[16] = {0};
	buf[index++] = 0x12;
	buf[index++] = 0x34;
	buf[index++] = stBin.sum;
	buf[index++] = 0;//reserved
	buf[index++] = 0;//reserved
	buf[index++] = (0x00 << 3) | (stBin.ishot << 1) | (stBin.iscool << 0);//feature
	buf[index++] = 0;//reserved
	buf[index++] = 0;//reserved
	MDB_send(buf,index);
}

void MDB_analysis(void)
{
	switch(mdb_cmd){
		case RESET : 
			MDB_reset_rpt();
			break;
		case SWITCH:
			MDB_switch_rpt();
			break;
		case CTRL:
			MDB_ctrl_rpt();
			break;
		case POLL:
			MDB_poll_rpt();
			break;
		case COLUMN:
			MDB_column_rpt();
			break;
		case STATUS:
			MDB_status_rpt();
			break;
		default:break;
	}
}	


