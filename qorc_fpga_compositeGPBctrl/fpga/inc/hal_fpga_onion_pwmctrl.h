//* SPDX-License-Identifier: Apache-2.0 OR MIT */
// Copyright © 2021 https://github.com/coolbreeze413

#ifndef __FPGA_ONION_PWMCTRL_H_
#define __FPGA_ONION_PWMCTRL_H_


// include files
#include "Fw_global_config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "eoss3_dev.h"


// module and register defines

#define FPGA_ONION_PERIPH_BASE_ADDR                     FPGA_PERIPH_BASE

#define FPGA_ONION_PWMCTRL_MODULE_OFFSET                0x00002000

#define FPGA_ONION_PWMCTRL_REG_OFFSET_PWM_0_CONFIG      0x0000
#define FPGA_ONION_PWMCTRL_REG_OFFSET_PWM_1_CONFIG      0x0004
#define FPGA_ONION_PWMCTRL_REG_OFFSET_PWM_2_CONFIG      0x0008



#define FPGA_ONION_PWMCTRL_REG_ADDR_PWM_0_CONFIG     (uint32_t*)(FPGA_ONION_PERIPH_BASE_ADDR + \
                                                        FPGA_ONION_PWMCTRL_MODULE_OFFSET + \
                                                        FPGA_ONION_PWMCTRL_REG_OFFSET_PWM_0_CONFIG)

#define FPGA_ONION_PWMCTRL_REG_ADDR_PWM_1_CONFIG     (uint32_t*)(FPGA_ONION_PERIPH_BASE_ADDR + \
                                                        FPGA_ONION_PWMCTRL_MODULE_OFFSET + \
                                                        FPGA_ONION_PWMCTRL_REG_OFFSET_PWM_1_CONFIG)

#define FPGA_ONION_PWMCTRL_REG_ADDR_PWM_2_CONFIG     (uint32_t*)(FPGA_ONION_PERIPH_BASE_ADDR + \
                                                        FPGA_ONION_PWMCTRL_MODULE_OFFSET + \
                                                        FPGA_ONION_PWMCTRL_REG_OFFSET_PWM_2_CONFIG)


// public function prototypes
void        hal_fpga_onion_pwmctrl_enable       (uint8_t io_pad_num, 
                                                 uint8_t io_pad_pwm_value);

void        hal_fpga_onion_pwmctrl_disable      (uint8_t io_pad_num);

uint32_t    hal_fpga_onion_pwmctrl_getval       (uint8_t io_pad_num);

#endif // #ifndef __FPGA_ONION_PWMCTRL_H_