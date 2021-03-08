// SPDX-License-Identifier: MIT
// Copyright © 2021 https://github.com/coolbreeze413

#include "hal_fpga_onion_breathectrl.h"


void hal_fpga_onion_breathectrl_enable (uint8_t io_pad_num, 
                                    uint32_t io_pad_period_value)
{
    if(io_pad_num == 22)
    {
        *FPGA_ONION_BREATHECTRL_REG_ADDR_BREATHE_0_CONFIG = (1 << 31) | (io_pad_period_value & 0xFFFFFF);
    }
    else if(io_pad_num == 21)
    {
        *FPGA_ONION_BREATHECTRL_REG_ADDR_BREATHE_1_CONFIG = (1 << 31) | (io_pad_period_value & 0xFFFFFF);
    }
    else if(io_pad_num == 18)
    {
        *FPGA_ONION_BREATHECTRL_REG_ADDR_BREATHE_2_CONFIG = (1 << 31) | (io_pad_period_value & 0xFFFFFF);
    }
}

void hal_fpga_onion_breathectrl_disable (uint8_t io_pad_num)
{
    if(io_pad_num == 22)
    {
        *FPGA_ONION_BREATHECTRL_REG_ADDR_BREATHE_0_CONFIG = 0;
    }
    else if(io_pad_num == 21)
    {
        *FPGA_ONION_BREATHECTRL_REG_ADDR_BREATHE_1_CONFIG = 0;
    }
    else if(io_pad_num == 18)
    {
        *FPGA_ONION_BREATHECTRL_REG_ADDR_BREATHE_2_CONFIG = 0;
    }
}

uint32_t hal_fpga_onion_breathectrl_getval (uint8_t io_pad_num)
{
    uint32_t pwm_config = 0;

    if(io_pad_num == 22)
    {
        pwm_config = *FPGA_ONION_BREATHECTRL_REG_ADDR_BREATHE_0_CONFIG;
    }
    else if(io_pad_num == 21)
    {
        pwm_config = *FPGA_ONION_BREATHECTRL_REG_ADDR_BREATHE_1_CONFIG;
    }
    else if(io_pad_num == 18)
    {
        pwm_config = *FPGA_ONION_BREATHECTRL_REG_ADDR_BREATHE_2_CONFIG;
    }

    return pwm_config;
}