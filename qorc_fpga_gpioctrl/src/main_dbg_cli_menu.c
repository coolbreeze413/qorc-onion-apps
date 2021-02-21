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

#include "hal_fpga_onion_gpioctrl.h"


#if FEATURE_CLI_DEBUG_INTERFACE


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



const struct cli_cmd_entry my_main_menu[] = {
    
    CLI_CMD_SUBMENU( "gpioctrl", qorc_gpioctrl, "FPGA GPIO Controller" ),
    
    CLI_CMD_TERMINATE()
};

#endif
