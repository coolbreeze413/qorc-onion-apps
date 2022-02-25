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
//#if 0

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
#include "AL4S3B_FPGA_Top_bit_blinky.h"
#include "AL4S3B_FPGA_Top_bit_onion_pwm.h"
#include "AL4S3B_FPGA_Top_bit_onion_breathe.h"

#if FEATURE_CLI_DEBUG_INTERFACE

//-------------------------------------------------
//
//   FPGA LOADER CLI
//
//-------------------------------------------------

//------------- Local functions -------------------
static void cli_load_fpga(const struct cli_cmd_entry *pEntry);
static void cli_show_fpga(const struct cli_cmd_entry *pEntry);

const struct cli_cmd_entry qorc_fpga_loader[] = {
        CLI_CMD_SIMPLE("load", cli_load_fpga, "load [FPGA_NUM]"),
        CLI_CMD_SIMPLE("show", cli_show_fpga, "show FPGA_IMAGES"),

        CLI_CMD_TERMINATE()
};

uint8_t fpga_num = 99;
static void cli_load_fpga(const struct cli_cmd_entry *pEntry)
{
    (void)pEntry;

    dbg_str("\n");
    CLI_uint8_getshow("fpga_num", &fpga_num);

    // hack alert! restructure menu properly.
    // power down fpga without configuring again leaves us in undesirable state!
    if (fpga_num <= 2)
    {
        // ensure that the IO_19 is held low while power cycling the FPGA power domain
        // NOTE: if the board has external PU for IO_19, ensure that IO_19 is directly
        //          shorted to ground (strong PD 🐉) for the fpga-reload scenario to work.
        //       This is applicable to all pygmy-stamp based boards.
        uint32_t old_io_19_val = *((volatile unsigned int*) 0x40004c4c);
        *((volatile unsigned int*) 0x40004c4c) = 0x00000180;

        // set FPGA low power configuration to shutdown:
        // PMU 0x40004000 FB_Power_Mode_Cfg 0x00A4
        // (1:0) FB_Power_Mode_Cfg 0x1 deepsleep, 0x2 shutdown, 0x0/0x3 reserved.
        *((volatile unsigned int*) 0x400044a4) = 0x2;

        // trigger FPGA lower power configuration
        // PMU 0x40004000 FFE_FB_PF_Software_PD 0x0200
        // (2) PF_Software_PD
        // (1) FB_Software_PD -> set this to 1 to trigger FPGA low power config (according to config reg above!)
        // (0) FFE_Software_PD
        // HW clears the bit once power down sequence is complete
        *((volatile unsigned int*) 0x40004600) = 0x2;

        // wait until power down sequence is done : change to check for bit clear!
        HAL_DelayUSec(100);

        // PMU 0x40004400 FFE_FB_PF_Software_WU 0x0210
        // PF_software_Wakeup
        // FB_Software_Wakeup -> set this to 1 to trigger FPGA wakeup from low power config
        // FFE_Software_Wakeup
        // HW clears the bit once power up sequence is complete
        *((volatile unsigned int*) 0x40004610) = 0x2;

        // wait until power up sequence is done : change to check for bit clear!
        HAL_DelayUSec(100);

        // set IO_19 back to original configuration
        *((volatile unsigned int*) 0x40004c4c) = old_io_19_val;

        // load fpga
        dbg_str("loading fpga >>>> ");
    }

    switch (fpga_num)
    {
    case 0:
        dbg_str("blinky\n\n");
        load_fpga_with_mem_init(sizeof(axFPGABitStream_blinky), axFPGABitStream_blinky, sizeof(axFPGAMemInit_blinky), axFPGAMemInit_blinky);
        fpga_iomux_init(sizeof(axFPGAIOMuxInit_blinky), axFPGAIOMuxInit_blinky);
        break;

    case 1:
        dbg_str("pwm\n\n");
        load_fpga_with_mem_init(sizeof(axFPGABitStream_onionpwm), axFPGABitStream_onionpwm, sizeof(axFPGAMemInit_onionpwm), axFPGAMemInit_onionpwm);
        fpga_iomux_init(sizeof(axFPGAIOMuxInit_onionpwm), axFPGAIOMuxInit_onionpwm);
        break;

    case 2:
        dbg_str("breathe\n\n");
        load_fpga_with_mem_init(sizeof(axFPGABitStream_onionbreathe), axFPGABitStream_onionbreathe, sizeof(axFPGAMemInit_onionbreathe), axFPGAMemInit_onionbreathe);
        fpga_iomux_init(sizeof(axFPGAIOMuxInit_onionbreathe), axFPGAIOMuxInit_onionbreathe);
        break;

    default:
        dbg_str("Invalid FPGA_NUM! Enter Number in [0-2]\n\n");
        break;
    }

    return;
}

static void cli_show_fpga(const struct cli_cmd_entry *pEntry)
{
    int i;
    (void)pEntry;

    dbg_str("\n");

    for (i = 0; i <= 2; i++)
    {
        switch (i)
        {
        case 0:
            dbg_str("    [0] blinky");
            break;
        case 1:
            dbg_str("    [1] pwm");
            break;
        case 2:
            dbg_str("    [2] breathe");
            break;
        default:
            //dbg_str("    [?] oops! did we forget to add entry to menu?\r\n");
            break;
        }

        if (i == fpga_num)
        {
            dbg_str(" <=== [running]");
        }

        dbg_str("\n\n");
    }

    return;
}

const struct cli_cmd_entry my_main_menu[] = {
    CLI_CMD_SUBMENU("fpgaloader", qorc_fpga_loader, "FPGA image loader"),
    CLI_CMD_TERMINATE()};

#endif

//#endif