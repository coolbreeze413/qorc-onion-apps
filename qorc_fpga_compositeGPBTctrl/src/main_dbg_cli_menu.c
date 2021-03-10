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

#include "hal_fpga_onion.h"
#include "hal_fpga_onion_gpioctrl.h"
#include "hal_fpga_onion_pwmctrl.h"
#include "hal_fpga_onion_breathectrl.h"
#include "hal_fpga_onion_timerctrl.h"


#if FEATURE_CLI_DEBUG_INTERFACE


// GPIOCTRL CLI
static void set_gpio_output(const struct cli_cmd_entry *pEntry);

static void set_gpio_input(const struct cli_cmd_entry *pEntry);
static void get_gpio_value(const struct cli_cmd_entry *pEntry);


const struct cli_cmd_entry qorc_gpioctrl[] =
{
    CLI_CMD_SIMPLE( "setout", set_gpio_output, "setout IO_X VAL" ),

    CLI_CMD_SIMPLE( "setin", set_gpio_input, "setin IO_X" ),
    CLI_CMD_SIMPLE( "getval", get_gpio_value, "getval IO_X (must be setin first)" ),

    CLI_CMD_TERMINATE()
};

uint32_t scratch32;


uint8_t io_pad_num;
uint8_t io_pad_val;

static void set_gpio_output(const struct cli_cmd_entry *pEntry)
{
    (void)pEntry;

    CLI_uint8_getshow( "io", &io_pad_num);
    
    CLI_uint8_getshow( "val", &io_pad_val);

    hal_fpga_onion_gpioctrl_set_output(io_pad_num, io_pad_val);

    return;
}


static void set_gpio_input(const struct cli_cmd_entry *pEntry)
{
    (void)pEntry;

    CLI_uint8_getshow( "io", &io_pad_num);

    hal_fpga_onion_gpioctrl_set_input(io_pad_num);
    
    return;
}


static void get_gpio_value(const struct cli_cmd_entry *pEntry)
{
    (void)pEntry;

    CLI_uint8_getshow( "io", &io_pad_num);

    io_pad_val = hal_fpga_onion_gpioctrl_get_value(io_pad_num);
    
    CLI_printf("read value = %d\n", io_pad_val);
 
    return;
}


// PWMCTRL CLI
static void enable_pwm_output(const struct cli_cmd_entry *pEntry);

static void disable_pwm_output(const struct cli_cmd_entry *pEntry);
static void get_pwm_value(const struct cli_cmd_entry *pEntry);


const struct cli_cmd_entry qorc_pwmctrl[] =
{
    CLI_CMD_SIMPLE( "enpwm", enable_pwm_output, "enpwm IO_X VAL(0-255)" ),
    CLI_CMD_SIMPLE( "dispwm", disable_pwm_output, "dispwm IO_X" ),
    CLI_CMD_SIMPLE( "getpwm", get_pwm_value, "getpwm IO_X" ),

    CLI_CMD_TERMINATE()
};


uint8_t io_pad_num;
uint8_t io_pad_pwm_val;

static void enable_pwm_output(const struct cli_cmd_entry *pEntry)
{
    (void)pEntry;

    CLI_uint8_getshow( "io", &io_pad_num);
    
    CLI_uint8_getshow( "val", &io_pad_pwm_val);

    hal_fpga_onion_pwmctrl_enable(io_pad_num, io_pad_pwm_val);

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


// BREATHECTRL CLI
static void enable_breathe_output(const struct cli_cmd_entry *pEntry);

static void disable_breathe_output(const struct cli_cmd_entry *pEntry);
static void get_breathe_value(const struct cli_cmd_entry *pEntry);


const struct cli_cmd_entry qorc_breathectrl[] =
{
    CLI_CMD_SIMPLE( "enbreathe", enable_breathe_output, "enbreathe IO_X period_msec" ),
    CLI_CMD_SIMPLE( "disbreathe", disable_breathe_output, "disbreathe IO_X" ),
    CLI_CMD_SIMPLE( "getbreathe", get_breathe_value, "getbreathe IO_X" ),

    CLI_CMD_TERMINATE()
};


uint8_t io_pad_num;
uint32_t io_pad_breathe_val;

static void enable_breathe_output(const struct cli_cmd_entry *pEntry)
{
    (void)pEntry;

    CLI_uint8_getshow( "io", &io_pad_num);
    
    CLI_uint32_getshow( "val", &io_pad_breathe_val);

    hal_fpga_onion_breathectrl_enable(io_pad_num, io_pad_breathe_val);

    return;
}


static void disable_breathe_output(const struct cli_cmd_entry *pEntry)
{
    (void)pEntry;

    CLI_uint8_getshow( "io", &io_pad_num);

    hal_fpga_onion_breathectrl_disable(io_pad_num);
    
    return;
}


static void get_breathe_value(const struct cli_cmd_entry *pEntry)
{
    uint32_t breathe_period = 0;
    (void)pEntry;

    CLI_uint8_getshow( "io", &io_pad_num);

    breathe_period = hal_fpga_onion_breathectrl_getval(io_pad_num);

    if(breathe_period)
    {
        CLI_printf("breathe_period = %d [0x%08x] msec\n", breathe_period, breathe_period);
    }
    else
    {
        CLI_printf("breathe is disabled\n");
    }

    return;
}


// TIMERCTRL CLI
static void enable_timer_output(const struct cli_cmd_entry *pEntry);

static void disable_timer_output(const struct cli_cmd_entry *pEntry);
static void get_timer_value(const struct cli_cmd_entry *pEntry);


const struct cli_cmd_entry qorc_timerctrl[] =
{
    CLI_CMD_SIMPLE( "entimer", enable_timer_output, "entimer ID period_msec" ),
    CLI_CMD_SIMPLE( "distimer", disable_timer_output, "distimer ID" ),
    CLI_CMD_SIMPLE( "gettimer", get_timer_value, "gettimer ID" ),

    CLI_CMD_TERMINATE()
};


uint8_t timer_id;
uint32_t timer_val;

static void enable_timer_output(const struct cli_cmd_entry *pEntry)
{
    (void)pEntry;

    CLI_uint8_getshow( "id", &timer_id);
    
    CLI_uint32_getshow( "val", &timer_val);

    hal_fpga_onion_timerctrl_enable(timer_id, timer_val);

    return;
}


static void disable_timer_output(const struct cli_cmd_entry *pEntry)
{
    (void)pEntry;

    CLI_uint8_getshow( "id", &timer_id);

    hal_fpga_onion_timerctrl_disable(timer_id);
    
    return;
}


static void get_timer_value(const struct cli_cmd_entry *pEntry)
{
    uint32_t timer_period = 0;
    (void)pEntry;

    CLI_uint8_getshow( "id", &timer_id);

    timer_period = hal_fpga_onion_timerctrl_getval(timer_id);

    if(timer_period)
    {
        CLI_printf("timer_period = %d [0x%08x] msec\n", timer_period, timer_period);
    }
    else
    {
        CLI_printf("timer is disabled\n");
    }

    return;
}


const struct cli_cmd_entry my_main_menu[] = {
    
    CLI_CMD_SUBMENU( "gpioctrl", qorc_gpioctrl, "FPGA GPIO Controller" ),
    CLI_CMD_SUBMENU( "pwmctrl", qorc_pwmctrl, "FPGA PWM Controller" ),
    CLI_CMD_SUBMENU( "breathectrl", qorc_breathectrl, "FPGA BREATHE Controller" ),
    CLI_CMD_SUBMENU( "timerctrl", qorc_timerctrl, "FPGA TIMER Controller" ),
    
    CLI_CMD_TERMINATE()
};

#endif
