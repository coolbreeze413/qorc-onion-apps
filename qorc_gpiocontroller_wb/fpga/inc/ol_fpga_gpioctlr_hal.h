//* SPDX-License-Identifier: Apache-2.0 OR MIT */
// Copyright © 2021 https://github.com/coolbreeze413

#ifndef __OL_FPGA_GPIOCTLR_H_
#define __OL_FPGA_GPIOCTLR_H_

// include files
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "eoss3_dev.h"


// module and register defines

#define OL_FPGA_PERIPH_BASE_ADDR                    FPGA_PERIPH_BASE

#define OL_FPGA_GPIOCTLR_MODULE_OFFSET              0x00004000

#define OL_FPGA_GPIOCTLR_REG_OFFSET_OUTPUT_ENABLE   0x00008
#define OL_FPGA_GPIOCTLR_REG_OFFSET_OUTPUT_VALUE    0x00004
#define OL_FPGA_GPIOCTLR_REG_OFFSET_INPUT_VALUE     0x00000


#define OL_FPGA_GPIOCTLR_REG_ADDR_OUTPUT_ENABLE     (uint32_t*)(OL_FPGA_PERIPH_BASE_ADDR + \
                                                        OL_FPGA_GPIOCTLR_MODULE_OFFSET + \
                                                        OL_FPGA_GPIOCTLR_REG_OFFSET_OUTPUT_ENABLE)

#define OL_FPGA_GPIOCTLR_REG_ADDR_OUTPUT_VALUE      (uint32_t*)(OL_FPGA_PERIPH_BASE_ADDR + \
                                                        OL_FPGA_GPIOCTLR_MODULE_OFFSET + \
                                                        OL_FPGA_GPIOCTLR_REG_OFFSET_OUTPUT_VALUE)

#define OL_FPGA_GPIOCTLR_REG_ADDR_INPUT_VALUE       (uint32_t*)(OL_FPGA_PERIPH_BASE_ADDR + \
                                                        OL_FPGA_GPIOCTLR_MODULE_OFFSET + \
                                                        OL_FPGA_GPIOCTLR_REG_OFFSET_INPUT_VALUE)


// public function prototypes
void hal_ol_fpga_gpioctlr_set_output(uint8_t io_pad_num, uint8_t io_pad_value);
void hal_ol_fpga_gpioctlr_set_input(uint8_t io_pad_num);
uint8_t hal_ol_fpga_gpioctlr_get_value(uint8_t io_pad_num);

#endif // #ifndef __OL_FPGA_GPIOCTLR_H_