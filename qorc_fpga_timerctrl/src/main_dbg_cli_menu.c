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
#include "hal_fpga_onion_timerctrl.h"


#if FEATURE_CLI_DEBUG_INTERFACE


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
    
    CLI_CMD_SUBMENU( "timerctrl", qorc_timerctrl, "FPGA TIMER Controller" ),
    
    CLI_CMD_TERMINATE()
};

#endif
