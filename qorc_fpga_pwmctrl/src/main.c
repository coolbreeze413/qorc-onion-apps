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
 *    Purpose: qorc_pwmctrl main
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
#include "ql_time.h"
#include "s3x_clock_hal.h"
#include "s3x_clock.h"
#include"eoss3_hal_timer.h"
#include "s3x_pi.h"
#include "dbg_uart.h"

#include "cli.h"

#include "hal_fpga_onion_pwmctrl.h"


extern const struct cli_cmd_entry my_main_menu[];


const char *SOFTWARE_VERSION_STR;


/*
 * Global variable definition
 */


extern void qf_hardwareSetup();
static void nvic_init(void);

int main(void)
{

    SOFTWARE_VERSION_STR = "qorc-onion-apps/qorc_fpga_pwmctrl";
    
    qf_hardwareSetup();
    nvic_init();

    dbg_str("\n\n");
    dbg_str( "##########################\n");
    dbg_str( "ONION FPGA PWM Controller Experiment\n");
    dbg_str( "SW Version: ");
    dbg_str( SOFTWARE_VERSION_STR );
    dbg_str( "\n" );
    dbg_str( __DATE__ " " __TIME__ "\n" );
    dbg_str( "##########################\n\n");

    dbg_str( "\n\nHello PWM CONTROLLER!\n\n");	// <<<<<<<<<<<<<<<<<<<<<  Change me!

    CLI_start_task( my_main_menu );

    HAL_Delay_Init();

    // print DEVICE ID:
    dbg_str_hex32("FPGA Device ID", hal_fpga_onion_get_device_id());

    // Visual Init Sequence Test.
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
    HAL_DelayUSec(1000000);

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
    HAL_DelayUSec(1000000);

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

    hal_fpga_onion_pwmctrl_disable(22);
    hal_fpga_onion_pwmctrl_disable(21);
    hal_fpga_onion_pwmctrl_disable(18);

#if 0 // USED ONLY FOR TESTING SOME FPGA WB INTERFACE EDGE CASES

    // undefined ip-module address space in FPGA IP     : 0xBADFABAC
    // undefined register space in existing ip-module   : 0xDEFFABAC
    // defined stuff - as per ip-module

    // access register outside of defined IP Modules:
    dbg_str_hex32("read unused addr, 0x40021000      ", *(uint32_t*)(0x40021000));
    dbg_str_hex32("read unused addr, 0x40026000      ", *(uint32_t*)(0x40026000));

    // access register in gpio ctrl outside of defined registers:
    dbg_str_hex32("read used gpioctrl, 0x40024000   ", *(uint32_t*)(0x40024000));
    dbg_str_hex32("read used gpioctrl, 0x40024004   ", *(uint32_t*)(0x40024004));
    dbg_str_hex32("read used gpioctrl, 0x40024008   ", *(uint32_t*)(0x40024008));
    dbg_str_hex32("read unused gpioctrl, 0x4002400C ", *(uint32_t*)(0x4002400C));
    dbg_str_hex32("read unused gpioctrl, 0x40024010 ", *(uint32_t*)(0x40024010));

    // access register in pwm ctrl outside of defined registers:
    dbg_str_hex32("read used pwmctrl, 0x40023000   ", *(uint32_t*)(0x40023000));
    dbg_str_hex32("read used pwmctrl, 0x40023004   ", *(uint32_t*)(0x40023004));
    dbg_str_hex32("read used pwmctrl, 0x40023008   ", *(uint32_t*)(0x40023008));
    dbg_str_hex32("read unused pwmctrl, 0x4002300C ", *(uint32_t*)(0x4002300C));
    dbg_str_hex32("read unused pwmctrl, 0x40023010 ", *(uint32_t*)(0x40023010));

    // access QL_RESERVED
    dbg_str_hex32("read QL_RESERVED, 0x400251FC      ", *(uint32_t*)(0x400251FC));
    dbg_str_hex32("read QL_RESERVED, 0x400251F8      ", *(uint32_t*)(0x400251F8));

    // access register in QL_RESERVED outside of defined registers:
    dbg_str_hex32("read QL_RESERVED, 0x40025000      ", *(uint32_t*)(0x40025000));

#endif


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


