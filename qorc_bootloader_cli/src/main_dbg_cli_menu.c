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
#include "s3x_clock_hal.h"
#include "s3x_clock.h"
#include "s3x_pi.h"

#include "bootloader_defines.h"


//#if FEATURE_CLI_DEBUG_INTERFACE

// clean this up, should be repeated here
#define METADATA_UNKNOWN_FLASH_STATE        (0x0)
#define METADATA_APPFPGA_IS_FLASHED         (0x1)
#define METADATA_M4APP_IS_FLASHED           (0x2)
#define METADATA_APPFFE_IS_FLASHED          (0x4)
#define METADATA_VALID_IMAGE_IS_FLASHED     (METADATA_APPFPGA_IS_FLASHED|METADATA_M4APP_IS_FLASHED) // ffe not supported yet


static void bootloader_cli_set_flash_mode(const struct cli_cmd_entry *pEntry);

static void bootloader_cli_run_m4app(const struct cli_cmd_entry *pEntry);
static void bootloader_cli_run_appfpga(const struct cli_cmd_entry *pEntry);
static void bootloader_cli_run_appfpga_m4app(const struct cli_cmd_entry *pEntry);


const struct cli_cmd_entry qorc_bootloader_cli[] =
{
    CLI_CMD_SIMPLE( "flash", bootloader_cli_set_flash_mode, "goto flash mode" ),

    CLI_CMD_SIMPLE( "runm4", bootloader_cli_run_m4app, "run m4 only code" ),
    CLI_CMD_SIMPLE( "runfpga", bootloader_cli_run_appfpga, "run fpga only code" ),
    CLI_CMD_SIMPLE( "runfpgam4", bootloader_cli_run_appfpga_m4app, "run fpga+m4 code" ),

    CLI_CMD_TERMINATE()
};


extern int invoke_flash_mode(void);
extern int check_active_images();
extern int load_m4app(void);
extern int load_appfpga(void);


static void bootloader_cli_set_flash_mode(const struct cli_cmd_entry *pEntry)
{
    (void)pEntry;

    HAL_GPIO_Write(5, 0); // switch off green led only - goto red-blue blink for flash mode

    // set to flashing mode, we don't return from here
    // TODO: add a timeout or other way to return back to the CLI.
    //CLI_printf("Entering Flashing Mode, no further output will be seen on the USBSERIAL until reset.\r\n");
    invoke_flash_mode();

    HAL_GPIO_Write(5, 1); // switch on green led only - back to yellow-cyan blink

    return;
}


static void bootloader_cli_run_m4app(const struct cli_cmd_entry *pEntry)
{
    uint8_t current_active_images;
    (void)pEntry;

    //HAL_GPIO_Write(6, 0);
    //HAL_GPIO_Write(5, 0); 
    //HAL_GPIO_Write(4, 0); 
    

    current_active_images = check_active_images();

    if(current_active_images & METADATA_M4APP_IS_FLASHED)
    {
        //CLI_printf("Loading M4 Application...\r\n");
        int error = load_m4app(); //this should never return
        //if the M4 image is corrupted it needs to be re-flashed. wait indefinitely
        if(error)
        {
            //CLI_printf("Error encountered loading m4app!\r\n");
        }
    }
    else
    {
        //CLI_printf("Invalid m4app metadata - Looks like no valid m4app is flashed?\r\n");
    }

    return;
}


static void bootloader_cli_run_appfpga(const struct cli_cmd_entry *pEntry)
{
    uint8_t current_active_images;
    (void)pEntry;

    //HAL_GPIO_Write(6, 0);
    //HAL_GPIO_Write(5, 0); 
    //HAL_GPIO_Write(4, 0); 

    current_active_images = check_active_images();

    if(current_active_images & METADATA_APPFPGA_IS_FLASHED)
    {

#if 0 // FPGA power domain off using clock/power API - not using.

    s3x_pi_set_cfg_st(s3x_get_pi(S3X_FB_16_CLK));
    dbg_str("1\r\n");

#if 0
    // wait for HW to clear, auto clear once power down sequence is finished
    while(1)
    {
        if(*(uint32_t*)(0x40004600) & 0x2 == 0x0) break;
    }

    // this also should show, should we check this?
    // wait till status is reflected - PMU 0x40004400 Status 0x0A0
    while(1)
    {
        if(*(uint32_t*)(0x400044A0) & 0x2 == 0x2) break;
    }
#endif

    // ensure IO_19 is low (externally we keep it low for now.)
    // important that if we are in debugger mode, we make it low after bootstrapping is done.

    s3x_pi_set_active_st(s3x_get_pi(S3X_FB_16_CLK));
    dbg_str("2\r\n");
#endif

#if 1 // FPGA domain off using direct register write

    // set configuration of FB to shutdown - PMU 0x40004400 FB_Power_Mode_Cfg 0x0A4
    *(volatile uint32_t*)(0x400044A4) = 0x2;

    // execute configuration of FB - PMU 0x40004400 FFE_FB_PF_Software_PD 0x200
    *(volatile uint32_t*)(0x40004600) = 0x2;

    // wait for sometime
    vTaskDelay(1000);

#endif

        //CLI_printf("Loading Application FPGA...\r\n");
        int error = load_appfpga();

        // ENABLE THIS to have a read register from FPGA to check, gives us hardfault!!
        // fpga configuration has failed? or power domain not set correctly?
        //int error = load_usb_serial_ip();

        if(error != BL_NO_ERROR) // error occurred?
        {
            //CLI_printf("ERROR loading App FPGA!\n");
        }
        while(1); // prevent CLI from running after loading the appfpga!!!
    }
    else
    {
        //CLI_printf("Invalid appfpga metadata - Looks like no valid appfpga is flashed?\r\n");
    }
 
    return;
}

// dont use this yet until appfpga works!
static void bootloader_cli_run_appfpga_m4app(const struct cli_cmd_entry *pEntry)
{
    uint8_t current_active_images;
    (void)pEntry;

    current_active_images = check_active_images();

    if(current_active_images & METADATA_APPFPGA_IS_FLASHED)
    {
        //CLI_printf("Loading Application FPGA...\r\n");
        int error = load_appfpga();

        if(error != BL_NO_ERROR) // error occurred?
        {
            //CLI_printf("ERROR loading App FPGA!\n");
        }
    }
    else
    {
        //CLI_printf("Invalid appfpga metadata - Looks like no valid appfpga is flashed?\r\n");
    }


    if(current_active_images & METADATA_M4APP_IS_FLASHED)
    {
        //CLI_printf("Loading M4 Application...\r\n");
        int error = load_m4app(); //this should never return
        //if the M4 image is corrupted it needs to be re-flashed. wait indefinitely
        if(error)
        {
            //CLI_printf("Error encountered loading m4app!\r\n");
        }
    }
    else
    {
        //CLI_printf("Invalid m4app metadata - Looks like no valid m4app is flashed?\r\n");
    }

}


const struct cli_cmd_entry my_main_menu[] = {
    
    CLI_CMD_SUBMENU( "bl", qorc_bootloader_cli, "bootloader cli" ),
    
    CLI_CMD_TERMINATE()
};

//#endif
