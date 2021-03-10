// SPDX-License-Identifier: MIT
// Copyright © 2021 https://github.com/coolbreeze413

#include "hal_fpga_onion_timerctrl.h"
#include "hal_fpga_onion.h"
#include "eoss3_hal_def.h"
#include "minimal_task.h"


void hal_fpga_onion_timerctrl0_isr(void)
{
    minimal_task_sendmsgFromISR_timerctrl0_mon(TIMERCTRL0_ISR);

    // disable the timer which fired the interrupt: questionable practice? but okay.
    hal_fpga_onion_timerctrl_disable_fromisr(0);
}

void hal_fpga_onion_timerctrl_init()
{
     //Clear any pending interrupt of FPGA 
    NVIC_ClearPendingIRQ(FbMsg_IRQn);

    //Register FPGA ISR callbacks FPGA Slow Down and Speed up interrupts 
    FB_RegisterISR(FB_INTERRUPT_0, hal_fpga_onion_timerctrl0_isr);
    FB_ConfigureInterrupt(FB_INTERRUPT_0, 
                            FB_INTERRUPT_TYPE_EDGE,
                            FB_INTERRUPT_POL_LEVEL_HIGH,
                            FB_INTERRUPT_DEST_AP_DISBLE, 
                            FB_INTERRUPT_DEST_M4_ENABLE);
    
    //Enable the FPGA interrupts
    NVIC_EnableIRQ(FbMsg_IRQn);
}

void hal_fpga_onion_timerctrl_enable (uint8_t timer_id, 
                                      uint32_t timer_period_ms)
{
    uint32_t clock_cycles = 0;

    // we are clocking the timer in the fpga with Sys_Clk0 (C16)
    clock_cycles = hal_fpga_onion_clock_cycles_for_msec(timer_period_ms, S3X_FB_16_CLK);
    if(timer_id == 0)
    {
        *FPGA_ONION_TIMERCTRL_REG_ADDR_TIMER_0_CONFIG = (1 << 31) | (clock_cycles & 0x7FFFFFFF);

        minimal_task_sendmsg_timerctrl0_mon(TIMERCTRL0_ENABLED);
    }
    
}

void hal_fpga_onion_timerctrl_disable (uint8_t timer_id)
{
    if(timer_id == 0)
    {
        *FPGA_ONION_TIMERCTRL_REG_ADDR_TIMER_0_CONFIG = 0;
        minimal_task_sendmsg_timerctrl0_mon(TIMERCTRL0_DISABLED);
    }
}

void hal_fpga_onion_timerctrl_disable_fromisr (uint8_t timer_id)
{
    if(timer_id == 0)
    {
        *FPGA_ONION_TIMERCTRL_REG_ADDR_TIMER_0_CONFIG = 0;
        minimal_task_sendmsgFromISR_timerctrl0_mon(TIMERCTRL0_DISABLED);
    }
}

uint32_t hal_fpga_onion_timerctrl_getval (uint8_t timer_id)
{
    uint32_t timer_config = 0;
    uint32_t timer_period = 0;
    uint8_t timer_enabled = 0;

    if(timer_id == 0)
    {
        timer_config = *FPGA_ONION_TIMERCTRL_REG_ADDR_TIMER_0_CONFIG;

        timer_enabled = (timer_config >> 31) & 0x1;

        if(timer_enabled)
        {
            // clock-cycles
            timer_period = timer_config & 0x7FFFFFFF;

            // msec
            timer_period = hal_fpga_onion_msec_for_clock_cycles(timer_period, S3X_FB_16_CLK);
        }
        
    }
    
    return timer_period;
}