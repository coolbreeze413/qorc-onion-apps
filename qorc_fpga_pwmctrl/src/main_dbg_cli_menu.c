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
 *    File   : main_dbg_cli_menu.c
 *    Purpose: 
 *                                                          
 *=========================================================*/

#include "Fw_global_config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <eoss3_hal_gpio.h>
#include "cli.h"
#include <stdbool.h>
#include "dbg_uart.h"

#include "hal_fpga_onion_pwmctrl.h"


#if FEATURE_CLI_DEBUG_INTERFACE


static void enable_pwm_output(const struct cli_cmd_entry *pEntry);

static void disable_pwm_output(const struct cli_cmd_entry *pEntry);
static void get_pwm_value(const struct cli_cmd_entry *pEntry);


const struct cli_cmd_entry qorc_gpioctrl[] =
{
    CLI_CMD_SIMPLE( "enpwm", enable_pwm_output, "enpwm IO_X VAL(0-255)" ),
    CLI_CMD_SIMPLE( "dispwm", disable_pwm_output, "dispwm IO_X" ),
    CLI_CMD_SIMPLE( "getpwm", get_pwm_value, "getpwm IO_X" ),

    CLI_CMD_TERMINATE()
};


uint8_t io_pad_num;
uint8_t io_pad_val;

static void enable_pwm_output(const struct cli_cmd_entry *pEntry)
{
    (void)pEntry;

    CLI_uint8_getshow( "io", &io_pad_num);
    
    CLI_uint8_getshow( "val", &io_pad_val);

    hal_fpga_onion_pwmctrl_enable(io_pad_num, io_pad_val);

    return;
}


static void disable_pwm_output(const struct cli_cmd_entry *pEntry)
{
    (void)pEntry;

    CLI_uint8_getshow( "io", &io_pad_num);

    hal_fpga_onion_pwmctrl_disable(io_pad_num);
    
    return;
}


static void get_pwm_value(const struct cli_cmd_entry *pEntry)
{
    uint32_t pwm_config = 0;
    uint8_t pwm_enabled = 0;
    uint16_t pwm_dutycycle = 0;
    (void)pEntry;

    CLI_uint8_getshow( "io", &io_pad_num);

    pwm_config = hal_fpga_onion_pwmctrl_getval(io_pad_num);

    CLI_printf("pwm_config = 0x%08x\n", pwm_config);

    pwm_enabled = (pwm_config >> 31) & 0x1;

    if(pwm_enabled)
    {
        pwm_dutycycle = pwm_config & 0xFFFF;
        CLI_printf("duty_cycle = %d [0x%04x]\n", pwm_dutycycle, pwm_dutycycle);
    }
    else
    {
        CLI_printf("pwm is disabled\n");
    }

    return;
}



const struct cli_cmd_entry my_main_menu[] = {
    
    CLI_CMD_SUBMENU( "pwmctrl", qorc_gpioctrl, "FPGA PWM Controller" ),
    
    CLI_CMD_TERMINATE()
};

#endif
