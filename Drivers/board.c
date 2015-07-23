/****************************************Copyright (c)*************************************************
**                      Fujian Junpeng Communicaiton Technology Co.,Ltd.
**                               http://www.easivend.com.cn
**--------------File Info------------------------------------------------------------------------------
** File name:           board.c
** Last modified Date:  2013-01-06
** Last Version:        No
** Descriptions:        系统参数定义及通用函数部分                     
**------------------------------------------------------------------------------------------------------
** Created by:          yoc
** Created date:        2015-04-20
** Version:             V0.1
** Descriptions:        The original version        
********************************************************************************************************/

#include "board.h"
#include "..\config.h"




static uint8 m_ledModel = 0;
static uint8 m_ledIsOpen = 0;
static uint8 m_ledTimeout = 0;

/*********************************************************************************************************
** Function name:     	XorCheck
** Descriptions:	    累加校验和
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
unsigned char XorCheck(unsigned char *pstr,unsigned short len)
{
	unsigned char xor_check = 0;
	unsigned short i;
	for(i=0;i<len;i++) 
	{
       	xor_check = xor_check^pstr[i];
   	}
	return xor_check;
}
/*********************************************************************************************************
** Function name:     	CrcCheck
** Descriptions:	    CRC校验和
** input parameters:    msg需要检验的数据;len数据长度
** output parameters:   无
** Returned value:      CRC检验结果
*********************************************************************************************************/
unsigned short CrcCheck(unsigned char *msg, unsigned short len) 
{
    unsigned short i, j;
    unsigned short crc = 0;
    unsigned short current = 0;
    for(i=0;i<len;i++) 
    {
        current = msg[i] << 8;
        for(j=0;j<8;j++) 
        {
            if((short)(crc^current)<0)
                crc = (crc<<1)^0x1021;
            else 
                crc <<= 1; 
            current <<= 1; 
        }
    }
    return crc;
}






/*********************************************************************************************************
** Function name:     	msleep
** Descriptions:	    毫秒睡眠函数  比较
** input parameters:    msg需要检验的数据;len数据长度
** output parameters:   无
** Returned value:      CRC检验结果
*********************************************************************************************************/

void msleep(unsigned int msec)
{
	unsigned int temp;
	temp = (msec > 10) ? msec/5 : 2;
	OSTimeDly(temp);
}


void LED_ctrl(uint8 s)
{
	//no += 4;
	FIO3DIR |= (0x01UL << 26);
	if(s == 0){
		FIO3SET |= (0x01UL << 26);
		m_ledIsOpen = 0;
	}
	else{
		m_ledIsOpen = 1;
		FIO3CLR |= (0x01UL << 26);
	}
}




void LED_setModel(uint8 m)
{
	m_ledModel = m;
	if(m == 2){
		m_ledTimeout = 100; //1秒超时
	}
}

void LED_model(void)
{
	static uint8 tick = 0;
	//格子柜初始化无柜子连接  1秒交替闪烁
	if(m_ledModel == 1){
		if(tick >= 100){
			tick = 0;
			if(m_ledIsOpen == 1){
				LED_ctrl(0);
			}
			else{
				LED_ctrl(1);
			}
		}
		else{
			tick++;
		}
	}
	else if(m_ledModel == 2){ //数据通信 快速闪烁
		
		if(tick >= 3){
			tick = 0;
			if(m_ledIsOpen == 1){
				LED_ctrl(0);
			}
			else{
				LED_ctrl(1);
			}
		}
		else{
			tick++;
		}
		if(m_ledTimeout){
			m_ledTimeout--;
		}
		else{
			m_ledModel = 0;
		}
	}
	else {
		if(m_ledIsOpen == 0){
			LED_ctrl(1);
		}
	}
	
	
	
}



/**************************************End Of File*******************************************************/
