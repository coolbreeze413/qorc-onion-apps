//* SPDX-License-Identifier: Apache-2.0 OR MIT */
// Copyright © 2021 https://github.com/coolbreeze413

#ifndef __FPGA_ONION_TIMERCTRL_H_
#define __FPGA_ONION_TIMERCTRL_H_


// include files
#include "Fw_global_config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "eoss3_dev.h"
#include "s3x_clock_hal.h"


// module and register defines

#define FPGA_ONION_PERIPH_BASE_ADDR                         FPGA_PERIPH_BASE

#define FPGA_ONION_TIMERCTRL_MODULE_OFFSET                  0x00004000

#define FPGA_ONION_TIMERCTRL_REG_OFFSET_TIMER_0_CONFIG      0x0000



#define FPGA_ONION_TIMERCTRL_REG_ADDR_TIMER_0_CONFIG     (uint32_t*)(FPGA_ONION_PERIPH_BASE_ADDR + \
                                                            FPGA_ONION_TIMERCTRL_MODULE_OFFSET + \
                                                            FPGA_ONION_TIMERCTRL_REG_OFFSET_TIMER_0_CONFIG)


// public function prototypes
void hal_fpga_onion_timerctrl_enable (uint8_t timer_id, 
                                      uint32_t timer_period_ms);

void hal_fpga_onion_timerctrl_disable (uint8_t timer_id);
void hal_fpga_onion_timerctrl_disable_fromisr (uint8_t timer_id);

uint32_t hal_fpga_onion_timerctrl_getval (uint8_t timer_id);

#endif // #ifndef __FPGA_ONION_TIMERCTRL_H_