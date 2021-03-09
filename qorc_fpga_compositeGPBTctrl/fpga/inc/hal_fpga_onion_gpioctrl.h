//* SPDX-License-Identifier: MIT */
// Copyright © 2021 https://github.com/coolbreeze413

#ifndef __FPGA_ONION_GPIOCTRL_H_
#define __FPGA_ONION_GPIOCTRL_H_


// include files
#include "Fw_global_config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "eoss3_dev.h"


// module and register defines

#define FPGA_ONION_PERIPH_BASE_ADDR                    FPGA_PERIPH_BASE

#define FPGA_ONION_GPIOCTRL_MODULE_OFFSET              0x00001000

#define FPGA_ONION_GPIOCTRL_REG_OFFSET_OUTPUT_ENABLE   0x0008
#define FPGA_ONION_GPIOCTRL_REG_OFFSET_OUTPUT_VALUE    0x0004
#define FPGA_ONION_GPIOCTRL_REG_OFFSET_INPUT_VALUE     0x0000


#define FPGA_ONION_GPIOCTRL_REG_ADDR_OUTPUT_ENABLE     (uint32_t*)(FPGA_ONION_PERIPH_BASE_ADDR + \
                                                        FPGA_ONION_GPIOCTRL_MODULE_OFFSET + \
                                                        FPGA_ONION_GPIOCTRL_REG_OFFSET_OUTPUT_ENABLE)

#define FPGA_ONION_GPIOCTRL_REG_ADDR_OUTPUT_VALUE      (uint32_t*)(FPGA_ONION_PERIPH_BASE_ADDR + \
                                                        FPGA_ONION_GPIOCTRL_MODULE_OFFSET + \
                                                        FPGA_ONION_GPIOCTRL_REG_OFFSET_OUTPUT_VALUE)

#define FPGA_ONION_GPIOCTRL_REG_ADDR_INPUT_VALUE       (uint32_t*)(FPGA_ONION_PERIPH_BASE_ADDR + \
                                                        FPGA_ONION_GPIOCTRL_MODULE_OFFSET + \
                                                        FPGA_ONION_GPIOCTRL_REG_OFFSET_INPUT_VALUE)


// public function prototypes
void        hal_fpga_onion_gpioctrl_set_output  (uint8_t io_pad_num, 
                                                 uint8_t io_pad_value);

void        hal_fpga_onion_gpioctrl_set_input   (uint8_t io_pad_num);

uint8_t     hal_fpga_onion_gpioctrl_get_value   (uint8_t io_pad_num);


#endif // #ifndef __FPGA_ONION_GPIOCTRL_H_