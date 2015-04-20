#include "task_device.h"
#include "..\config.h"

void TASK_Device(void *pdata)
{	
	Trace("TASK_Device\r\n");
	while(1){
		DB_task();
		colTaskPoll();
		msleep(100);
	}
}



