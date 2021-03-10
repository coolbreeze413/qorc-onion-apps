//* SPDX-License-Identifier: Apache-2.0 OR MIT */
// Copyright © 2021 https://github.com/coolbreeze413

#ifndef __FPGA_ONION_BREATHECTRL_H_
#define __FPGA_ONION_BREATHECTRL_H_


// include files
#include "Fw_global_config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "eoss3_dev.h"


// module and register defines

#define FPGA_ONION_PERIPH_BASE_ADDR                             FPGA_PERIPH_BASE

#define FPGA_ONION_BREATHECTRL_MODULE_OFFSET                    0x00003000

#define FPGA_ONION_BREATHECTRL_REG_OFFSET_BREATHE_0_CONFIG      0x0000
#define FPGA_ONION_BREATHECTRL_REG_OFFSET_BREATHE_1_CONFIG      0x0004
#define FPGA_ONION_BREATHECTRL_REG_OFFSET_BREATHE_2_CONFIG      0x0008



#define FPGA_ONION_BREATHECTRL_REG_ADDR_BREATHE_0_CONFIG     (uint32_t*)(FPGA_ONION_PERIPH_BASE_ADDR + \
                                                            FPGA_ONION_BREATHECTRL_MODULE_OFFSET + \
                                                            FPGA_ONION_BREATHECTRL_REG_OFFSET_BREATHE_0_CONFIG)

#define FPGA_ONION_BREATHECTRL_REG_ADDR_BREATHE_1_CONFIG     (uint32_t*)(FPGA_ONION_PERIPH_BASE_ADDR + \
                                                            FPGA_ONION_BREATHECTRL_MODULE_OFFSET + \
                                                            FPGA_ONION_BREATHECTRL_REG_OFFSET_BREATHE_1_CONFIG)

#define FPGA_ONION_BREATHECTRL_REG_ADDR_BREATHE_2_CONFIG     (uint32_t*)(FPGA_ONION_PERIPH_BASE_ADDR + \
                                                            FPGA_ONION_BREATHECTRL_MODULE_OFFSET + \
                                                            FPGA_ONION_BREATHECTRL_REG_OFFSET_BREATHE_2_CONFIG)


// public function prototypes
void        hal_fpga_onion_breathectrl_enable       (uint8_t io_pad_num, 
                                                    uint32_t breathe_period_ms);

void        hal_fpga_onion_breathectrl_disable      (uint8_t io_pad_num);

uint32_t    hal_fpga_onion_breathectrl_getval       (uint8_t io_pad_num);

#endif // #ifndef __FPGA_ONION_BREATHECTRL_H_