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

/*==========================================================
 *
 *    File   : main.c
 *    Purpose: main for advancedfpga example using ledctlr.v
 *                                                          
 *=========================================================*/

#include "Fw_global_config.h"
#include "Bootconfig.h"

#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "timers.h"
#include "RtosTask.h"

/*    Include the generic headers required for sensorHub */
#include "eoss3_hal_gpio.h"
#include "eoss3_hal_rtc.h"

#include "ql_time.h"
#include "s3x_clock_hal.h"
#include "s3x_clock.h"
#include "s3x_pi.h"
#include "dbg_uart.h"
#include "eoss3_hal_spi.h"
#include "cli.h"

extern const struct cli_cmd_entry my_main_menu[];

//#include "fpga_loader.h"        	// API for loading FPGA
// #include "AL4S3B_FPGA_Top_bit.h"
// #include "AL4S3B_FPGA_Top_bit_onion_breathe.h"  // FPGA bitstream to load into FPGA
// #include "AL4S3B_FPGA_Top_bit_onion_pwm.h"  // FPGA bitstream to load into FPGA
// #include "helloworldfpga_bit.h"


const char *SOFTWARE_VERSION_STR;


/*
 * Global variable definition
 */


extern void qf_hardwareSetup();
static void nvic_init(void);

int main(void)
{
	int i=0;

    SOFTWARE_VERSION_STR = "qorc-sdk/qf_apps/qf_fpgareload";
    
    qf_hardwareSetup();                                     // Note: pincfg_table.c has been updated to give FPGA control of LEDs
    nvic_init();
    HAL_Delay_Init();

#if 0 // QUICKIE TEST
    S3x_Clk_Disable(S3X_FB_21_CLK);
    S3x_Clk_Disable(S3X_FB_16_CLK);
    S3x_Clk_Enable(S3X_A1_CLK);
    S3x_Clk_Enable(S3X_CFG_DMA_A1_CLK);
    load_fpga(sizeof(axFPGABitStream3),axFPGABitStream3);     // Load bitstream into FPGA
    // fpga_ledctlr_init();								    // Start FPGA clock

    // wait some time for fpga to run pulse
        for (i=0;i<50000000; i++) {
        	PMU->GEN_PURPOSE_1  = i << 4;
    }

    S3x_Clk_Disable(S3X_FB_21_CLK);
    S3x_Clk_Disable(S3X_FB_16_CLK);
    (*( volatile unsigned int *) (0x400044a4)) = 0x2;
    (*( volatile unsigned int *) (0x40004600)) = 0x2;
    // wait some time for fpga to get reset pulse
    for (i=0;i<50000000; i++) {
    	PMU->GEN_PURPOSE_1  = i << 4;
    }

    S3x_Clk_Disable(S3X_FB_21_CLK);
    S3x_Clk_Disable(S3X_FB_16_CLK);
    S3x_Clk_Enable(S3X_A1_CLK);
    S3x_Clk_Enable(S3X_CFG_DMA_A1_CLK);
    load_fpga(sizeof(axFPGABitStream1),axFPGABitStream1);     // Load bitstream into FPGA
#endif
    
    dbg_str("\n\n");
    dbg_str( "##########################\n");
    dbg_str( "Quicklogic QuickFeather FPGA reload Example\n");
    dbg_str( "SW Version: ");
    dbg_str( SOFTWARE_VERSION_STR );
    dbg_str( "\n" );
    dbg_str( __DATE__ " " __TIME__ "\n" );
    dbg_str( "##########################\n\n");
        
    dbg_str("\r\n!!Remove the bootstraps from IO_19 and IO_20 if mounted!!\r\n");
      
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


