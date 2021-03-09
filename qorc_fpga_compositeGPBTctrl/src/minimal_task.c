#include "Fw_global_config.h"   // This defines application specific charactersitics

#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "timers.h"
#include "RtosTask.h"

#include "minimal_task.h"

xQueueHandle xQH_timerctrl0_mon;
xTaskHandle xTH_timerctrl0_mon;


void minimal_taskfunc_timerctrl0_mon(void *pvParameters) 
{
    uint8_t msg;
    portBASE_TYPE status;

    while(1)
    {
        status = xQueueReceive(xQH_timerctrl0_mon, &msg, portMAX_DELAY);

        if (status == pdTRUE)
        {
            switch(msg)
            {
                case TIMERCTRL0_ENABLED:
                    dbg_str("\ntimer0 enabled\n");
                    break;

                case TIMERCTRL0_ISR:
                    dbg_str("\ntimer0 ISR\n");
                    break;

                case TIMERCTRL0_DISABLED:
                    dbg_str("\ntimer0 disabled\n");
                    break;

                default:
                    dbg_str("unknown");
                    break;    
            }
        }
    }
}


void minimal_task_init_timerctrl0_mon()
{
    // create task
    xTaskCreate(minimal_taskfunc_timerctrl0_mon, 
                "tmr0mon", 
                configMINIMAL_STACK_SIZE, 
                (void *)NULL,
                tskIDLE_PRIORITY + 2UL, 
                &xTH_timerctrl0_mon);

    // create queue
    xQH_timerctrl0_mon = xQueueCreate(10, sizeof(uint8_t));
}

void minimal_task_sendmsg_timerctrl0_mon(uint8_t msg)
{
    xQueueSendToBack(xQH_timerctrl0_mon, &msg, 0);
}

void minimal_task_sendmsgFromISR_timerctrl0_mon(uint8_t msg)
{
    BaseType_t xHigherPriorityTaskWoken;
    xHigherPriorityTaskWoken = pdFALSE;

    xQueueSendFromISR(xQH_timerctrl0_mon, &msg, &xHigherPriorityTaskWoken);

    portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}