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

#include "s3x_clock_hal.h"
#include "s3x_clock.h"
#include "s3x_pi.h"

#include "fpga_loader.h"
//#include "AL4S3B_FPGA_Top_bit_blinky.h"
//#include "AL4S3B_FPGA_Top_bit_onion_pwm.h"
//#include "AL4S3B_FPGA_Top_bit_onion_breathe.h"

#if FEATURE_CLI_DEBUG_INTERFACE

//-------------------------------------------------
//
//   FPGA LOADER CLI
//
//-------------------------------------------------

//------------- Local functions -------------------
static void cli_load_fpga(const struct cli_cmd_entry *pEntry);
static void cli_show_fpga(const struct cli_cmd_entry *pEntry);

#define FPGA_BIN_FLASH_ADDR_0       0x0
#define FPGA_BIN_FLASH_ADDR_1       0x0
#define FPGA_BIN_FLASH_ADDR_2       0x0

const struct cli_cmd_entry qorc_fpga_loader[] =
{
    CLI_CMD_SIMPLE( "load", cli_load_fpga, "load [FPGA_NUM]" ),
	CLI_CMD_SIMPLE( "show", cli_show_fpga, "show FPGA_IMAGES" ),

    CLI_CMD_TERMINATE()
};

uint8_t fpga_num = 0;
static void cli_load_fpga(const struct cli_cmd_entry *pEntry)
{
    int i;
    
    (void)pEntry;
    CLI_uint8_getshow( "fpga_num", &fpga_num );

    // hack alert! restructure menu properly.
    // power down fpga without configuring again leaves us in undesirable state!
    if(fpga_num <= 2)
    {
        // disable clocks
        S3x_Clk_Disable(S3X_FB_21_CLK);
        S3x_Clk_Disable(S3X_FB_16_CLK);

        // power cycle fpga domain
        (*( volatile unsigned int *) (0x400044a4)) = 0x2;
        (*( volatile unsigned int *) (0x40004600)) = 0x2;
        
        // wait some time for fpga to get reset pulse
        // for (i=0;i<50000000; i++) {
        // 	PMU->GEN_PURPOSE_1  = i << 4;
        // }
        HAL_DelayUSec(100);

        // load fpga
        dbg_str("loading fpga... ");
    }

    switch(fpga_num)
    {
        case 0:
            dbg_str("blinky\r\n");
            load_appfpga(FPGA_BIN_FLASH_ADDR_0);
            break;

        case 1:
            dbg_str("pwm\r\n");
            load_appfpga(FPGA_BIN_FLASH_ADDR_1);
            break;

        case 2:
            dbg_str("breathe\r\n");
            load_appfpga(FPGA_BIN_FLASH_ADDR_2);
            break;

        default:
            dbg_str("Invalid FPGA_NUM! Enter Number in [0-2]\r\n");
            break;
    }

    return;
}

static void cli_show_fpga(const struct cli_cmd_entry *pEntry)
{
    int i;
    (void)pEntry;
    
    for(i=0; i<=2; i++)
    {
        if(i==fpga_num)
        {
            dbg_str("running -> ");
        }
        switch (i)
        {            
            case 0:
                dbg_str("[0] blinky\r\n");
                break;
            case 1:
                dbg_str("[1] pwm\r\n");
                break;
            case 2:
                dbg_str("[2] breathe\r\n");
                break;
            default:
                dbg_str(" [?] oops! did we forget to add entry to menu?\r\n");
                break;
        }
    }
    

    return;
}


const struct cli_cmd_entry my_main_menu[] = {
	CLI_CMD_SUBMENU( "fpgaloader", qorc_fpga_loader, "FPGA image loader" ),
    CLI_CMD_TERMINATE()
};

#endif
