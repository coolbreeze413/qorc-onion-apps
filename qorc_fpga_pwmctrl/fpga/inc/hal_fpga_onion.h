//* SPDX-License-Identifier: MIT */
// Copyright © 2021 https://github.com/coolbreeze413

#ifndef __FPGA_ONION_H_
#define __FPGA_ONION_H_


// include files
#include "Fw_global_config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "eoss3_dev.h"
#include "s3x_clock_hal.h"

// Generic Onion FPGA Stuff

// module and register defines

#define FPGA_ONION_MISC_BASE_ADDR                       MISC_CTRL_BASE


#define FPGA_ONION_MISC_REG_OFFSET_FPGA_DEVID           0x3FC


#define FPGA_ONION_MISC_REG_ADDR_FPGA_DEVID             (uint32_t*)(FPGA_ONION_MISC_BASE_ADDR + \
                                                        FPGA_ONION_MISC_REG_OFFSET_FPGA_DEVID)




// public function prototypes

uint16_t    hal_fpga_onion_get_device_id    ();
void        hal_fpga_onion_test_wb          ();

// internal utility function prototypes

uint32_t    hal_fpga_onion_clock_cycles_for_msec    (uint32_t msec, S3x_CLK_ID clk_id);
uint32_t    hal_fpga_onion_msec_for_clock_cycles    (uint32_t clock_cycles, S3x_CLK_ID clk_id);
#endif // #ifndef __FPGA_ONION_H_