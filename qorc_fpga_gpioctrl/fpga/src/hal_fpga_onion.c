//* SPDX-License-Identifier: MIT */
// Copyright © 2021 https://github.com/coolbreeze413

#include "hal_fpga_onion.h"


uint16_t hal_fpga_onion_get_device_id ()
{
    return (uint16_t)(*FPGA_ONION_MISCL_REG_ADDR_FPGA_DEVID);
}