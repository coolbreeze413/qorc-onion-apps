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


#if FEATURE_CLI_DEBUG_INTERFACE


static void set_gpio_output(const struct cli_cmd_entry *pEntry);

static void set_gpio_input(const struct cli_cmd_entry *pEntry);
static void get_gpio_value(const struct cli_cmd_entry *pEntry);


const struct cli_cmd_entry qorc_gpioctlr[] =
{
    CLI_CMD_SIMPLE( "setout", set_gpio_output, "setout IO_X VAL" ),

    CLI_CMD_SIMPLE( "setin", set_gpio_input, "setin IO_X" ),
    CLI_CMD_SIMPLE( "getval", get_gpio_value, "getval IO_X (must be setin first)" ),

    CLI_CMD_TERMINATE()
};

uint32_t scratch32;


uint8_t gpionum_output = 0;
uint8_t gpioval_output = 0;
static void set_gpio_output(const struct cli_cmd_entry *pEntry)
{
    (void)pEntry;

    CLI_uint8_getshow( "io", &gpionum_output);
    
    scratch32 = *(uint32_t*)(0x40024008);

    *(uint32_t*)(0x40024008) = scratch32 | (0x1 << gpionum_output);

    CLI_uint8_getshow( "val", &gpioval_output);

    scratch32 = *(uint32_t*)(0x40024004);

    if(gpioval_output > 0)
    {
        *(uint32_t*)(0x40024004) = scratch32 | (0x1 << gpionum_output);
    }
    else
    {
        *(uint32_t*)(0x40024004) = scratch32 & ~(0x1 << gpionum_output);
    }    

    return;
}


uint8_t gpionum_input = 0;
static void set_gpio_input(const struct cli_cmd_entry *pEntry)
{
    (void)pEntry;

    CLI_uint8_getshow( "io", &gpionum_input);
    
    scratch32 = *(uint32_t*)(0x40024008);

    *(uint32_t*)(0x40024008) = scratch32 | (0x1 << gpionum_output);
    return;
}


uint8_t gpioval_input = 0;
static void get_gpio_value(const struct cli_cmd_entry *pEntry)
{
    (void)pEntry;

    CLI_uint8_getshow( "io", &gpionum_input);
    
    CLI_uint8_getshow( "val", &gpioval_input);

    scratch32 = *(uint32_t*)(0x40024000);

    gpioval_input = (scratch32 >> gpionum_input) & 0x1;

    CLI_printf("read value = %d\n", gpioval_input);
 
    return;
}



const struct cli_cmd_entry my_main_menu[] = {
    
    CLI_CMD_SUBMENU( "gpioctlr", qorc_gpioctlr, "FPGA led controller" ),
    
    CLI_CMD_TERMINATE()
};

#endif
