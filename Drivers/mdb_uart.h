#ifndef _MDB_UART_H_
#define _MDB_UART_H_

#include "board.h"


/*********************************************************************************************************
**串口配置定义
*********************************************************************************************************/

#define  UART_IIR_INTID_RLS		((uint32_t)(3<<1)) 	/*!<Interrupt identification: Receive line status*/
#define  UART_IIR_INTID_RDA		((uint32_t)(2<<1)) 	/*!<Interrupt identification: Receive data available*/
#define  UART_IIR_INTID_CTI		((uint32_t)(6<<1)) 	/*!<Interrupt identification: Character time-out indicator*/
#define  UART_IIR_INTID_MASK		((uint32_t)(7<<1))	/*!<Interrupt identification: Interrupt ID mask */
#define  UART_LSR_OE				((uint8_t)(1<<1)) 	/*!<Line status register: Overrun error*/
#define  UART_LSR_PE				((uint8_t)(1<<2)) 	/*!<Line status register: Parity error*/
#define  UART_LSR_FE				((uint8_t)(1<<3)) 	/*!<Line status register: Framing error*/
#define  UART_LSR_BI				((uint8_t)(1<<4)) 	/*!<Line status register: Break interrupt*/
#define  UART_LSR_THRE			((uint8_t)(1<<5)) 	/*!<Line status register: Transmit holding register empty*/
#define  UART_LSR_TEMT			((uint8_t)(1<<6)) 	/*!<Line status register: Transmitter empty*/
#define  UART_LSR_RXFE			((uint8_t)(1<<7)) 	/*!<Error in RX FIFO*/
#define  UART_RBR_MASKBIT   		((uint8_t)0xFF) 	/*!< UART Received Buffer mask bit (8 bits) */
#define  UART_LCR_WLEN8     		((uint8_t)(3<<0))   /*!< UART 8 bit data mode */
#define  UART_LCR_PARITY_EN		((uint8_t)(1<<3))	/*!< UART Parity Enable */
#define  UART_LCR_PARITY_ODD		((uint8_t)(0))      /*!< UART Odd Parity Select */
#define  UART_LCR_PARITY_EVEN	((uint8_t)(1<<4))	/*!< UART Even Parity Select */

#define  UART_LCR_PARITY_F_1		((uint8_t)(2<<4))	/*!< UART force 1 stick parity */
#define  UART_LCR_PARITY_F_0		((uint8_t)(3<<4))	/*!< UART force 0 stick parity */

#define	 UART2_BPS			9600

#define  MDB_RCV_START     	0x80			//开始接收MDB数据
#define  MDB_RCV_DATA     	0x81			//正在接收MDB数据
#define  MDB_RCV_OK       	0x82			//MDB数据接收正确
#define  MDB_OUT_RANGE    	0x83			//MDB数据接收超界
#define  MDB_CHKSUM_ERR   	0x84			//MDB数据接收校验出错

#define  MDB_ADD          	1			//MDB地址
#define  MDB_DAT          	0			//MDB数据

#define  UART2MODESELECT0 	(1<<29)			//P1.29 select as GPIO IN LPC1700PinCfg.h
#define  UART2MODESELECT1 	(1<<28)			//P1.28 select as GPIO IN LPC1700PinCfg.h

#define  UART2_MDB_MODE	  	0
#define  UART2_EVB_MODE	  	1
#define  UART2_BUFLEN     	36
#define  UART2_BPS		  	9600

#define  ERR_STX            0x81			//帧头出错
#define  ERR_ETX            0x82			//帧尾出错
#define  ERR_CHKSUM         0x83			//校验出错
#define  ERR_TIMEOUT        0x84			//超时

#define  PARITY_DIS         0
#define  PARITY_ODD         1
#define  PARITY_EVEN        2
#define  PARITY_F_1         3
#define  PARITY_F_0         4



/*********************************************************************************************************
** MDB通信消息队列
*********************************************************************************************************/
#define G_MDB_SIZE    20
typedef struct _g_mdb_st_{
	uint8 c;

}G_MDB_ST;




/*********************************************************************************************************
**MDB从设备定义
*********************************************************************************************************/


#define COL_GOODS_A		0xE0
#define COL_GOODS_B		0xE8

#define COL_FOODS_A		0x80
#define COL_FOODS_B		0x88


#define RESET			0x00
#define SWITCH			0x01
#define CTRL			0x02
#define COLUMN			0x03
#define POLL			0x04
#define STATUS			0x05

#define  MDB_ACK 		  	0x00			//应答
#define  MDB_RET 		  	0xAA			//重试
#define  MDB_NAK 		  	0xFF			//无应答


#define MDB_COL_IDLE		0
#define MDB_COL_BUSY		1
#define MDB_COL_FAILED		2
#define MDB_COL_RESERVED	3
#define MDB_COL_SUCCESS		4
#define MDB_COL_ERROR		5
#define MDB_COL_JUSTRESET	6

typedef struct _mdb_col_{
	unsigned char status;
	
}MDB_COL;




/*********************************************************************************************************
**uart串口函数声明
*********************************************************************************************************/
void uart2Init(void);
void uart2Clear(void) ;
void uart2SetParityMode(unsigned char mode) ;
void MDB_putChr(unsigned char dat,unsigned char mode);
void Uart2IsrHandler(void) ;

/*********************************************************************************************************
**mdb从设备函数声明
*********************************************************************************************************/
unsigned char MDB_colAddrIsOk(unsigned char addr);
unsigned char MDB_recvOk(unsigned char len);
void MDB_analysis(void);



#endif


/**************************************End Of File*******************************************************/
