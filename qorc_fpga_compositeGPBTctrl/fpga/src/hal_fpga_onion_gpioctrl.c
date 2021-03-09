//* SPDX-License-Identifier: MIT */
// Copyright © 2021 https://github.com/coolbreeze413

#include "hal_fpga_onion_gpioctrl.h"


void hal_fpga_onion_gpioctrl_set_output(uint8_t io_pad_num, uint8_t io_pad_value)
{
    uint32_t read_val;
    uint32_t write_val;

    // remove possibility of incorrect usage of io_pad_value (must be 0 or 1)
    io_pad_value = (io_pad_value > 0) ? 1 : 0;

    // read current output enable register val
    read_val = *(FPGA_ONION_GPIOCTRL_REG_ADDR_OUTPUT_ENABLE);

    // set output enable register io_pad_num bit to 1
    write_val = read_val | (0x1 << io_pad_num);
    *(FPGA_ONION_GPIOCTRL_REG_ADDR_OUTPUT_ENABLE) = write_val;

    // read current output value register val
    read_val = *(FPGA_ONION_GPIOCTRL_REG_ADDR_OUTPUT_VALUE);

    // set output value register == io_pad_value
    write_val = read_val & ~(0x1 << io_pad_num); // clear
    write_val = write_val | (io_pad_value << io_pad_num); // set
    *(FPGA_ONION_GPIOCTRL_REG_ADDR_OUTPUT_VALUE) = write_val;
}


void hal_fpga_onion_gpioctrl_set_input(uint8_t io_pad_num)
{
    uint32_t read_val;
    uint32_t write_val;

    // read current output enable register val
    read_val = *(FPGA_ONION_GPIOCTRL_REG_ADDR_OUTPUT_ENABLE);

    // set output enable register io_pad_num bit to 0
    write_val = read_val & ~(0x1 << io_pad_num);
    *(FPGA_ONION_GPIOCTRL_REG_ADDR_OUTPUT_ENABLE) = write_val;

    // read current output value register val
    read_val = *(FPGA_ONION_GPIOCTRL_REG_ADDR_OUTPUT_VALUE);

    // set output value register io_pad_num bit to 0
    write_val = read_val & ~(0x1 << io_pad_num);
    *(FPGA_ONION_GPIOCTRL_REG_ADDR_OUTPUT_VALUE) = write_val;    
}

uint8_t hal_fpga_onion_gpioctrl_get_value(uint8_t io_pad_num)
{
    uint32_t read_val;

    // get input value register io_pad_num bit
    read_val = *(FPGA_ONION_GPIOCTRL_REG_ADDR_INPUT_VALUE);
    read_val = (read_val >> io_pad_num) & 0x1;

    return read_val;
}