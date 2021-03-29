#include <stdint.h>
#include <stdio.h>

#include "Fw_global_config.h"

#include "QL_Trace.h"
#include "eoss3_hal_i2c.h"
#include "bit_manipulation.h"
#include "hx3313.h"


// hacky sleep! fix me soon.
static void sleep_ms(uint32_t time_ms)
{
    HAL_DelayUSec(time_ms*1000);
}


uint8_t hx3313_probe()
{
    uint8_t data = 0x00;
    
    HAL_I2C_Read(HX3313_DEV_ADDR, HX3313_REG_DEVICE_ID, &data, 1);
    
    printf("hx3313_probe: [0x%02x]\r\n", data);
    
    if(data == HX3313_VAL_DEVICE_ID)
    {
        return QL_STATUS_OK;
    }    
    
    return QL_STATUS_ERROR;
}


// use HX3313_LED_DR_* macros
uint8_t hx3313_config_led(uint8_t led_current)
{
    HAL_I2C_Write(HX3313_DEV_ADDR, HX3313_REG_LED_DR, &led_current, 1);
    
    return QL_STATUS_OK;
}


// useHX3313_PS_ADC_OSR_* macros
uint8_t hx3313_config_ps(uint8_t ps_osr, uint8_t ps_led_on_time, uint8_t ps_phase_offset_idac, uint8_t ps_interval)
{    
    uint8_t data = 0x0;
    
    HAL_I2C_Write(HX3313_DEV_ADDR, HX3313_REG_LED_PS, &ps_led_on_time, 1);
    HAL_I2C_Write(HX3313_DEV_ADDR, HX3313_REG_OFFSET_IDAC_PS, &ps_phase_offset_idac, 1);
    HAL_I2C_Write(HX3313_DEV_ADDR, HX3313_REG_PS_INTERVAL, &ps_interval, 1);
    
    // read current value in enable reg
    HAL_I2C_Read(HX3313_DEV_ADDR, HX3313_REG_ENABLE, &data, 1);
    
    // set the ps_osr: DATASHEET MISTAKE
    data &= ~(0x3 << 4);
    data |= ((ps_osr & 0x3) << 4);
    HAL_I2C_Write(HX3313_DEV_ADDR, HX3313_REG_ENABLE, &data, 1);
    
    return QL_STATUS_OK;
}


// use HX3313_HRS_ADC_OSR_* macros
uint8_t hx3313_config_hrs(uint8_t hrs_osr, uint8_t hrs_led_on_time, uint8_t hrs_phase_offset_idac)
{
    uint8_t data = 0x0;
    
    HAL_I2C_Write(HX3313_DEV_ADDR, HX3313_REG_LED_HRS, &hrs_led_on_time, 1);
    HAL_I2C_Write(HX3313_DEV_ADDR, HX3313_REG_OFFSET_IDAC_HRS, &hrs_phase_offset_idac, 1);
    
    // read current value in enable reg
    HAL_I2C_Read(HX3313_DEV_ADDR, HX3313_REG_ENABLE, &data, 1);
    
    // set the hrs_osr: DATASHEET MISTAKE
    data &= ~(0x3 << 0);
    data |= ((hrs_osr & 0x3) << 0);
    HAL_I2C_Write(HX3313_DEV_ADDR, HX3313_REG_ENABLE, &data, 1);
    
    return QL_STATUS_OK;
}


uint8_t hx3313_enable_ps(uint8_t en_ps)
{
    uint8_t data = 0x0;
    
    // read current value in enable reg
    HAL_I2C_Read(HX3313_DEV_ADDR, HX3313_REG_ENABLE, &data, 1);
    
    // set the en_ps: DATASHEET MISTAKE
    data &= ~(0x1 << 6);
    data |= ((en_ps &0x1) << 6);
    HAL_I2C_Write(HX3313_DEV_ADDR, HX3313_REG_ENABLE, &data, 1);
    
    return QL_STATUS_OK;
}


uint8_t hx3313_enable_hrs(uint8_t en_hrs)
{
    uint8_t data = 0x0;
    
    // read current value in enable reg
    HAL_I2C_Read(HX3313_DEV_ADDR, HX3313_REG_ENABLE, &data, 1);
    
    // set the en_hrs: DATASHEET MISTAKE
    data &= ~(0x1 << 2);
    data |= ((en_hrs &0x1) << 2);
    HAL_I2C_Write(HX3313_DEV_ADDR, HX3313_REG_ENABLE, &data, 1);
    
    return QL_STATUS_OK;
}


uint8_t hx3313_read_interrupt()
{
    uint8_t interrupt_1 = 0;
    uint8_t interrupt_2 = 0;
    uint8_t interrupt_3 = 0;
    
    HAL_I2C_Read(HX3313_DEV_ADDR, HX3313_REG_INTERRUPT_CFG_1, &interrupt_1, 1);
    HAL_I2C_Read(HX3313_DEV_ADDR, HX3313_REG_INTERRUPT_CFG_2, &interrupt_2, 1);
    HAL_I2C_Read(HX3313_DEV_ADDR, HX3313_REG_INTERRUPT_CFG_3, &interrupt_3, 1);
    
    printf("interrupt_1: [0x%02x]\r\n", interrupt_1);
    printf("interrupt_2: [0x%02x]\r\n", interrupt_2);
    printf("interrupt_3: [0x%02x]\r\n", interrupt_3);
    
    return QL_STATUS_OK;
}

uint8_t hx3313_read_hrs_data()
{
    uint8_t hrs_data[3] = {0};
    
    HAL_I2C_Read(HX3313_DEV_ADDR, HX3313_REG_HRS_DATA1, hrs_data, 3);
    
    printf("hrs_data : [0x%02x 0x%02x 0x%02x]\r\n", hrs_data[0], hrs_data[1], hrs_data[2]);
    
    return QL_STATUS_OK;
}

uint8_t hx3313_read_als_data()
{
    uint8_t als_data1[3] = {0};
    uint8_t als_data2[3] = {0};
    
    HAL_I2C_Read(HX3313_DEV_ADDR, HX3313_REG_ALS_DATA1, als_data1, 3);
    HAL_I2C_Read(HX3313_DEV_ADDR, HX3313_REG_ALS_DATA2, als_data2, 3);
    
    printf("als_data1: [0x%02x 0x%02x 0x%02x]\r\n", als_data1[0], als_data1[1], als_data1[2]);
    printf("als_data2: [0x%02x 0x%02x 0x%02x]\r\n", als_data2[0], als_data2[1], als_data2[2]);
    
    return QL_STATUS_OK;
}

uint8_t hx3313_read_ps1_data()
{
    uint8_t ps_data[3] = {0};
    
    HAL_I2C_Read(HX3313_DEV_ADDR, HX3313_REG_PS1_DATA1, ps_data, 3);
    
    printf("ps_data  : [0x%02x 0x%02x 0x%02x]\r\n", ps_data[0], ps_data[1], ps_data[2]);
    
    return QL_STATUS_OK;
}