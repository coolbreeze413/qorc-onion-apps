//* SPDX-License-Identifier: MIT */
// Copyright © 2021 https://github.com/coolbreeze413

#include "hal_fpga_onion.h"


uint16_t hal_fpga_onion_get_device_id ()
{
    return (uint16_t)(*FPGA_ONION_MISC_REG_ADDR_FPGA_DEVID);
}


void hal_fpga_onion_test_wb ()
{
    // undefined ip-module address space in FPGA IP     : 0xBADFABAC
    // undefined register space in existing ip-module   : 0xDEFFABAC
    // defined stuff - as per ip-module

    // register access tests
    dbg_str_hex32("read unused addr,        0x40027000", *(uint32_t*)(0x40027000));
    dbg_str_hex32("read unused addr,        0x40026000", *(uint32_t*)(0x40026000));

    // access register in gpio ctrl outside of defined registers:
    dbg_str_hex32("read used gpioctrl       0x40021000", *(uint32_t*)(0x40021000));
    dbg_str_hex32("read used gpioctrl       0x40021004", *(uint32_t*)(0x40021004));
    dbg_str_hex32("read used gpioctrl       0x40021008", *(uint32_t*)(0x40021008));
    dbg_str_hex32("read unused gpioctrl     0x4002100C", *(uint32_t*)(0x4002100C));
    dbg_str_hex32("read unused gpioctrl     0x40021010", *(uint32_t*)(0x40021010));

    // access register in pwm ctrl outside of defined registers:
    dbg_str_hex32("read used pwmctrl        0x40022000", *(uint32_t*)(0x40022000));
    dbg_str_hex32("read used pwmctrl        0x40022004", *(uint32_t*)(0x40022004));
    dbg_str_hex32("read used pwmctrl        0x40022008", *(uint32_t*)(0x40022008));
    dbg_str_hex32("read unused pwmctrl      0x4002200C", *(uint32_t*)(0x4002200C));
    dbg_str_hex32("read unused pwmctrl      0x40022010", *(uint32_t*)(0x40022010));

    // access register in breathe ctrl outside of defined registers:
    dbg_str_hex32("read used breathectrl    0x40023000", *(uint32_t*)(0x40023000));
    dbg_str_hex32("read used breathectrl    0x40023004", *(uint32_t*)(0x40023004));
    dbg_str_hex32("read used breathectrl    0x40023008", *(uint32_t*)(0x40023008));
    dbg_str_hex32("read unused breathectrl  0x4002300C", *(uint32_t*)(0x4002300C));
    dbg_str_hex32("read unused breathectrl  0x40023010", *(uint32_t*)(0x40023010));

    // access register in timer ctrl outside of defined registers:
    dbg_str_hex32("read used timerctrl      0x40024000", *(uint32_t*)(0x40024000));
    dbg_str_hex32("read unused timerctrl    0x4002400C", *(uint32_t*)(0x4002400C));

    // access QL_RESERVED
    dbg_str_hex32("read QL_RESERVED         0x400251FC", *(uint32_t*)(0x400251FC));
    dbg_str_hex32("read QL_RESERVED         0x400251F8", *(uint32_t*)(0x400251F8));

    // access register in QL_RESERVED outside of defined registers:
    dbg_str_hex32("read QL_RESERVED         0x40025000", *(uint32_t*)(0x40025000));

}

uint32_t hal_fpga_onion_clock_cycles_for_msec (uint32_t msec, S3x_CLK_ID clk_id)
{
    uint32_t clock_cycles = 0;
    uint32_t clock_rate_hz = 0;

    // get the clock frequency
    clock_rate_hz = S3x_Clk_Get_Rate(clk_id);

    // clock_cycles/1000msec = clock_rate_hz
    clock_cycles = ((float)clock_rate_hz/1000) * msec; // prevent 32-bit overflow!

    return clock_cycles;
}


uint32_t    hal_fpga_onion_msec_for_clock_cycles    (uint32_t clock_cycles, S3x_CLK_ID clk_id)
{
    uint32_t msec = 0;
    uint32_t clock_rate_hz = 0;

    // get the clock frequency
    clock_rate_hz = S3x_Clk_Get_Rate(clk_id);

    // clock_cycles/1000msec = clock_rate_hz
    msec = ((float)clock_cycles/clock_rate_hz) * 1000; // prevent 32-bit overflow!

    return msec;
}