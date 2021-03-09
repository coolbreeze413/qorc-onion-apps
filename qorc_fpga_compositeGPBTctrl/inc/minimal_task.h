#include "Fw_global_config.h"   // This defines application specific charactersitics

#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "timers.h"
#include "RtosTask.h"


#define TIMERCTRL0_ENABLED      0x00
#define TIMERCTRL0_ISR          0x01
#define TIMERCTRL0_DISABLED     0x02


void minimal_task_init_timerctrl0_mon();

void minimal_task_sendmsg_timerctrl0_mon(uint8_t msg);
void minimal_task_sendmsgFromISR_timerctrl0_mon(uint8_t msg);