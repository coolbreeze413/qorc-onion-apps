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
 *    Purpose: main for QuickFeather helloworldsw and LED/UserButton test
 *                                                          
 *=========================================================*/

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
#include "eoss3_hal_fpga_usbserial.h"
#include "eoss3_hal_i2c.h"
#include "eoss3_hal_timer.h"
#include "ql_time.h"
#include "s3x_clock_hal.h"
#include "s3x_clock.h"
#include "s3x_pi.h"
#include "dbg_uart.h"

#include "cli.h"


extern const struct cli_cmd_entry my_main_menu[];


const char *SOFTWARE_VERSION_STR;


/*
 * Global variable definition
 */


extern void qf_hardwareSetup();
static void nvic_init(void);

I2C_Config i2c0config =
{
    .eI2CFreq = I2C_400KHZ,    // 400kHz
    .eI2CInt = I2C_DISABLE,    // enabled interrupt
    .ucI2Cn = 0
};
uint8_t i2c_read_data;


int main(void)
{

    SOFTWARE_VERSION_STR = "qorc_pygmy_test_1.0.0";
    
    qf_hardwareSetup();
    nvic_init();

    dbg_str("\n\n");
    dbg_str( "##########################\n");
    dbg_str( "OptimusLogic Pygmy Test Suite\n");
    dbg_str( "SW Version: ");
    dbg_str( SOFTWARE_VERSION_STR );
    dbg_str( "\n" );
    dbg_str( __DATE__ " " __TIME__ "\n" );
    dbg_str( "##########################\n\n");

    dbg_str( "\n\n");
    dbg_str( "██╗     ███████╗██╗   ██╗██╗ █████╗ ████████╗██╗  ██╗ █████╗ ███╗   ██╗\n");
    dbg_str( "██║     ██╔════╝██║   ██║██║██╔══██╗╚══██╔══╝██║  ██║██╔══██╗████╗  ██║\n");
    dbg_str( "██║     █████╗  ██║   ██║██║███████║   ██║   ███████║███████║██╔██╗ ██║\n");
    dbg_str( "██║     ██╔══╝  ╚██╗ ██╔╝██║██╔══██║   ██║   ██╔══██║██╔══██║██║╚██╗██║\n");
    dbg_str( "███████╗███████╗ ╚████╔╝ ██║██║  ██║   ██║   ██║  ██║██║  ██║██║ ╚████║\n");
    dbg_str( "╚══════╝╚══════╝  ╚═══╝  ╚═╝╚═╝  ╚═╝   ╚═╝   ╚═╝  ╚═╝╚═╝  ╚═╝╚═╝  ╚═══╝\n");
    dbg_str( "\n\n\n");
    dbg_str("██╗    ██╗ █████╗ ██╗  ██╗███████╗███████╗\n");
    dbg_str("██║    ██║██╔══██╗██║ ██╔╝██╔════╝██╔════╝\n");
    dbg_str("██║ █╗ ██║███████║█████╔╝ █████╗  ███████╗\n");
    dbg_str("██║███╗██║██╔══██║██╔═██╗ ██╔══╝  ╚════██║\n");
    dbg_str("╚███╔███╔╝██║  ██║██║  ██╗███████╗███████║\n");
    dbg_str(" ╚══╝╚══╝ ╚═╝  ╚═╝╚═╝  ╚═╝╚══════╝╚══════╝\n");
    dbg_str( "\n\n\n");


    setup_I2C0_M4();
    setup_I2C1_M4();
    
// individual tests ++
    HAL_Delay_Init();
    //printf("float: %d\n", sizeof(float));
    //printf("double: %d\n", sizeof(double));
    
    TestI2C();
    

    //HAL_I2C0_Select();
    //HAL_I2C1_Select();
    //bmx160_basic_test(100, 1000); // mag not operating
    //bmx160_accel_odr_test(200, 5);
    //dps310_basic_test(50, 1000);
    //dps310_continuous_test(50, 1); // 50 times, 1Hz
    //hx3313_ldo_drive_config();
    //hx3313_basic_test(1000, 1000); // caution: incomplete!
    //bma400_basic_test();
    //bma400_step_counter_test();
    //bma400_tap_detection_test();
    //bma400_combo_1_test();
// individual tests --   

    
// poll task    
    //HAL_Init_Timer_Task();
    //sensorhub_poll_task_init();
// poll task    



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


