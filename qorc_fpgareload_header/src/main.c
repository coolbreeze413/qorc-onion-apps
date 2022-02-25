/*==========================================================
 * Copyright 2020 QuickLogic Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *==========================================================*/

#include "Fw_global_config.h"   // This defines application specific charactersitics

#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "timers.h"
#include "RtosTask.h"

/*    Include the generic headers required for QORC */
#include "eoss3_hal_gpio.h"
#include "eoss3_hal_rtc.h"

#include "ql_time.h"
#include "s3x_clock_hal.h"
#include "s3x_clock.h"
#include "s3x_pi.h"
#include "dbg_uart.h"

#include "cli.h"

extern const struct cli_cmd_entry my_main_menu[];

// #include "fpga_loader.h"
// #include <stdint.h>
// #include "AL4S3B_FPGA_Top_bit.h"
// #include "AL4S3B_FPGA_Top_bit_onion_breathe.h"
// #include "AL4S3B_FPGA_Top_bit_onion_pwm.h"
// #include "helloworldfpga_bit.h"
//extern uint32_t axFPGABitStream [];
//extern uint32_t axFPGAMemInit [];
//extern uint32_t axFPGAIOMuxInit [];


const char *SOFTWARE_VERSION_STR;


/*
 * Global variable definition
 */


void qorc_hardwareSetup(void);
static void nvic_init(void);

int main(void)
{
    int i=0;

    SOFTWARE_VERSION_STR = "qorc-onion-apps/qorc_fpgareload_header";
    
    qorc_hardwareSetup();
    nvic_init();
    HAL_Delay_Init();

    dbg_str("\n\n");
    dbg_str( "##########################\n");
    dbg_str( "Onion FPGA reload Example\n");
    dbg_str( "SW Version: ");
    dbg_str( SOFTWARE_VERSION_STR );
    dbg_str( "\n" );
    dbg_str( __DATE__ " " __TIME__ "\n" );
    dbg_str( "##########################\n\n");
        
    dbg_str("\n\n");
    dbg_str("FPGA Reconfiguration involves a power cycle of the power domain\r\n");
    dbg_str("This *requires* that IO_19 be pulled *LOW*\r\n");
    dbg_str("\n\n");
    dbg_str("If there are jumpers on the board, remove them\r\n");
    dbg_str("\n\n");
    dbg_str("If there are pullups on the board (pygmy-stamp based)\r\n");
    dbg_str("short the IO_19 to a GND pin using a jumper wire\r\n");
    dbg_str("\n\n");
    dbg_str("side note: if this application is being run via debugger\r\n");
    dbg_str("           ensure IO_19 is *HIGH* if(and until) the board is reset\r\n");
    dbg_str("           so that the EOS-S3 boots up in DEBUGGER mode, \r\n");
    dbg_str("           and then change it later to be *LOW* when changing the FPGA images \r\n");
    dbg_str("\n\n");
      
    CLI_start_task( my_main_menu );

    /* Start the tasks and timer running. */
    vTaskStartScheduler();
    dbg_str("\n");

    while(1);
}


static void nvic_init(void)
 {
    // To initialize system, this interrupt should be triggered at main.
    // So, we will set its priority just before calling vTaskStartScheduler(), not the time of enabling each irq.
    NVIC_SetPriority(Ffe0_IRQn, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY);
    NVIC_SetPriority(SpiMs_IRQn, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY);
    NVIC_SetPriority(CfgDma_IRQn, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY);
    NVIC_SetPriority(Uart_IRQn, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY);
    NVIC_SetPriority(FbMsg_IRQn, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY);
 }


//needed for startup_EOSS3b.s asm file
void SystemInit(void)
{

}
