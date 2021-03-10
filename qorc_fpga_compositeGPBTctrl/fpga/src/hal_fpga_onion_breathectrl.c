// SPDX-License-Identifier: MIT
// Copyright © 2021 https://github.com/coolbreeze413

#include "hal_fpga_onion.h"
#include "hal_fpga_onion_breathectrl.h"


void hal_fpga_onion_breathectrl_enable (uint8_t io_pad_num, 
                                        uint32_t breathe_period_ms)
{
    uint32_t clock_cycles = 0;
    uint32_t clock_cycles_per_step = 0;

    // we are clocking the timer in the fpga with Sys_Clk0 (C16)
    clock_cycles = hal_fpga_onion_clock_cycles_for_msec(breathe_period_ms, S3X_FB_16_CLK);

    // remember that the value passed in here is for the duration of the breathe cycle (inhale+exhale)
    // we need to convert this to clock_cycles/step, where total_steps = 2*(1<<PWM_RESOLUTION_BITS)
    clock_cycles_per_step = (float)clock_cycles/512;


    if(io_pad_num == 22)
    {
        *FPGA_ONION_BREATHECTRL_REG_ADDR_BREATHE_0_CONFIG = (1 << 31) | (clock_cycles_per_step & 0xFFFFFF);
    }
    else if(io_pad_num == 21)
    {
        *FPGA_ONION_BREATHECTRL_REG_ADDR_BREATHE_1_CONFIG = (1 << 31) | (clock_cycles_per_step & 0xFFFFFF);
    }
    else if(io_pad_num == 18)
    {
        *FPGA_ONION_BREATHECTRL_REG_ADDR_BREATHE_2_CONFIG = (1 << 31) | (clock_cycles_per_step & 0xFFFFFF);
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
    uint32_t breathe_config = 0;
    uint8_t breathe_enabled = 0;
    uint32_t breathe_period = 0;

    if(io_pad_num == 22)
    {
        breathe_config = *FPGA_ONION_BREATHECTRL_REG_ADDR_BREATHE_0_CONFIG;
    }
    else if(io_pad_num == 21)
    {
        breathe_config = *FPGA_ONION_BREATHECTRL_REG_ADDR_BREATHE_1_CONFIG;
    }
    else if(io_pad_num == 18)
    {
        breathe_config = *FPGA_ONION_BREATHECTRL_REG_ADDR_BREATHE_2_CONFIG;
    }

    breathe_enabled = (breathe_config >> 31) & 0x1;
    if(breathe_enabled)
    {
        breathe_period = breathe_config & 0xFFFFFF;
        // this period is number_of_clock_cycles_per_step!
        // total_clock_cycles = breathe_period * (2*(1<<PWM_RESOLUTION_BITS))
        breathe_period = breathe_period*512;

        // in terms of msec:
        breathe_period = hal_fpga_onion_msec_for_clock_cycles(breathe_period, S3X_FB_16_CLK);
    }
    else
    {
        breathe_period = 0;
    }

    return breathe_period;
}