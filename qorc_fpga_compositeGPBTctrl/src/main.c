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
#include"eoss3_hal_timer.h"
#include "s3x_pi.h"
#include "dbg_uart.h"

#include "cli.h"

#include "hal_fpga_onion.h"
#include "hal_fpga_onion_gpioctrl.h"
#include "hal_fpga_onion_pwmctrl.h"
#include "hal_fpga_onion_breathectrl.h"


extern const struct cli_cmd_entry my_main_menu[];


const char *SOFTWARE_VERSION_STR;


/*
 * Global variable definition
 */


void qorc_hardwareSetup(void);
static void nvic_init(void);

int main(void)
{

    SOFTWARE_VERSION_STR = "qorc-onion-apps/qorc_fpga_compositeGPBTctrl";
    
    qorc_hardwareSetup();
    nvic_init();

    dbg_str("\n\n");
    dbg_str( "##########################\n");
    dbg_str( "ONION FPGA Composite Controller Experiment\n");
    dbg_str( "SW Version: ");
    dbg_str( SOFTWARE_VERSION_STR );
    dbg_str( "\n" );
    dbg_str( __DATE__ " " __TIME__ "\n" );
    dbg_str( "##########################\n\n");

    dbg_str( "\n\nHello GPIO/PWM/BREATHE/TIMER CONTROLLER!\n\n");	// <<<<<<<<<<<<<<<<<<<<<  Change me!

    CLI_start_task( my_main_menu );

    HAL_Delay_Init();

    // print DEVICE ID:
    dbg_str_hex32("FPGA Device ID", hal_fpga_onion_get_device_id());

#if 0 // WB ACCESS TESTCASES
    hal_fpga_onion_test_wb();
#endif


    // Visual Init Sequence Test : GPIOCTRL : Triple-Magenta-Blink
    hal_fpga_onion_gpioctrl_set_output(22, 1);
    hal_fpga_onion_gpioctrl_set_output(18, 1);
    HAL_DelayUSec(400000);
    hal_fpga_onion_gpioctrl_set_output(22, 0);
    hal_fpga_onion_gpioctrl_set_output(18, 0);
    HAL_DelayUSec(400000);
    hal_fpga_onion_gpioctrl_set_output(22, 1);
    hal_fpga_onion_gpioctrl_set_output(18, 1);
    HAL_DelayUSec(400000);
    hal_fpga_onion_gpioctrl_set_output(22, 0);
    hal_fpga_onion_gpioctrl_set_output(18, 0);
    HAL_DelayUSec(400000);
    hal_fpga_onion_gpioctrl_set_output(22, 1);
    hal_fpga_onion_gpioctrl_set_output(18, 1);
    HAL_DelayUSec(400000);
    hal_fpga_onion_gpioctrl_set_output(22, 0);
    hal_fpga_onion_gpioctrl_set_output(18, 0);



    // Visual Init Sequence Test: PWMCTRL : Triple-Magenta-PWMBlink
    hal_fpga_onion_pwmctrl_disable(22);
    hal_fpga_onion_pwmctrl_disable(21);
    hal_fpga_onion_pwmctrl_disable(18);

    hal_fpga_onion_pwmctrl_enable(22, 20);
    hal_fpga_onion_pwmctrl_enable(18, 20);
    HAL_DelayUSec(500000);
    hal_fpga_onion_pwmctrl_enable(22, 70);
    hal_fpga_onion_pwmctrl_enable(18, 70);
    HAL_DelayUSec(500000);
    hal_fpga_onion_pwmctrl_enable(22, 120);
    hal_fpga_onion_pwmctrl_enable(18, 120);
    HAL_DelayUSec(500000);
    hal_fpga_onion_pwmctrl_disable(22);
    hal_fpga_onion_pwmctrl_disable(18);
    HAL_DelayUSec(500000);

    hal_fpga_onion_pwmctrl_disable(22);
    hal_fpga_onion_pwmctrl_disable(21);
    hal_fpga_onion_pwmctrl_disable(18);


    // Visual Init Sequence Test: BREATHECTRL : Triple-Magenta-Breathe
    hal_fpga_onion_breathectrl_disable(22);
    hal_fpga_onion_breathectrl_disable(21);
    hal_fpga_onion_breathectrl_disable(18);

    hal_fpga_onion_breathectrl_enable(22, 1800); //43690
    hal_fpga_onion_breathectrl_enable(18, 1800); //43690
    HAL_DelayUSec(3*1800000);
    hal_fpga_onion_breathectrl_disable(22);
    hal_fpga_onion_breathectrl_disable(18);

    hal_fpga_onion_breathectrl_disable(22);
    hal_fpga_onion_breathectrl_disable(21);
    hal_fpga_onion_breathectrl_disable(18);

    // TIMERCTRL : monitor task to see the timer state changes/ISR
    minimal_task_init_timerctrl0_mon();
    // TIMERCTRL : initialize for interrupt-handling
    hal_fpga_onion_timerctrl_init();

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


