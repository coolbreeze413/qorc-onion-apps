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
 *    File   : usb_fpga_loader.c
 *    Purpose: This file has function to load USB FPGA image
 *             from the Flash and wait for reset
 *
 *=========================================================*/

#include "Fw_global_config.h"
#include "eoss3_dev.h"
#include "eoss3_hal_spi.h"
#include "spi_flash.h"
#include "crc32.h"
#include "bootloader_defines.h"
#include "s3x_clock_hal.h"
#include "s3x_clock.h"
#include "s3x_pi.h"
#include "fpga_loader.h"
#include "eoss3_hal_pads.h"
#include "eoss3_hal_pad_config.h"
#include "eoss3_hal_fpga_usbserial.h"
#include "dbg_uart.h"

extern uint32_t image_metadata[FLASH_APP_META_SIZE / 4];
extern void read_flash(unsigned char *start_address, int length, unsigned char *destination);
extern void toggle_downloading_led(int toggle_time_msec);

extern int axFPGABitStream_length;
extern const uint32_t axFPGABitStream [];


int load_usb_flasher(void)
{
#if (UART_ID_BOOTLOADER == UART_ID_USBSERIAL)

    S3x_Clk_Disable(S3X_FB_21_CLK);
    S3x_Clk_Disable(S3X_FB_16_CLK);

    S3x_Clk_Enable(S3X_A1_CLK);
    S3x_Clk_Enable(S3X_CFG_DMA_A1_CLK);

    load_fpga(axFPGABitStream_length, axFPGABitStream);

    // Use 0x6140 as USB serial product ID (USB PID)
    HAL_usbserial_init2(false, false, 0x6140); // Start USB serial not using interrupts
    for (int i = 0; i != 4000000; i++)
        ; // Give it time to enumerate

    // remind what to do when done programming
    dbg_str("FPGA Programmed\n");

#endif

    dbg_str("Press Reset button after flashing ..\n");
    // wait for the reset button to be pressed
    program_flash();
    NVIC_SystemReset();

    // return BL_ERROR;
}
