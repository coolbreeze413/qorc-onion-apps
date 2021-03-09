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

#include "hal_fpga_onion_breathectrl.h"


#if FEATURE_CLI_DEBUG_INTERFACE


static void enable_breathe_output(const struct cli_cmd_entry *pEntry);

static void disable_breathe_output(const struct cli_cmd_entry *pEntry);
static void get_breathe_value(const struct cli_cmd_entry *pEntry);


const struct cli_cmd_entry qorc_breathectrl[] =
{
    CLI_CMD_SIMPLE( "enbreathe", enable_breathe_output, "enbreathe IO_X VAL(24-bit)" ),
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
    uint32_t breathe_config = 0;
    uint8_t breathe_enabled = 0;
    uint32_t breathe_period = 0;
    (void)pEntry;

    CLI_uint8_getshow( "io", &io_pad_num);

    breathe_config = hal_fpga_onion_breathectrl_getval(io_pad_num);

    CLI_printf("breathe_config = 0x%08x\n", breathe_config);

    breathe_enabled = (breathe_config >> 31) & 0x1;

    if(breathe_enabled)
    {
        breathe_period = breathe_config & 0xFFFFFF;
        CLI_printf("breathe_period = %d [0x%06x]\n", breathe_period, breathe_period);
    }
    else
    {
        CLI_printf("breathe is disabled\n");
    }

    return;
}



const struct cli_cmd_entry my_main_menu[] = {
    
    CLI_CMD_SUBMENU( "breathectrl", qorc_breathectrl, "FPGA BREATHE Controller" ),
    
    CLI_CMD_TERMINATE()
};

#endif
