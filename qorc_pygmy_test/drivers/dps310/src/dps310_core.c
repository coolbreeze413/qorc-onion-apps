#include <stdint.h>
#include <stdio.h>

#include "Fw_global_config.h"

#include "QL_Trace.h"
#include "eoss3_hal_i2c.h"
#include "eoss3_hal_timer.h"

#include "bit_manipulation.h"
#include "dps310.h"

#include "ql_sensorhub_poll_task.h"


uint8_t _coeff_source_temp_sensor;
uint8_t _selected_temp_sensor;
uint8_t _temp_measure_rate;
uint8_t _temp_oversampling_rate;
int32_t _tempr;
float _temp_scaled;
float _temp_compensated;

uint8_t _press_measure_rate;
uint8_t _press_oversampling_rate;
int32_t _pressr;
float _press_scaled;
float _press_compensated;

uint8_t _op_mode;

int16_t _c0, _c1, _c01, _c11, _c20, _c21, _c30;
int32_t _c00, _c10;

const uint32_t dps310_scaling_factors[8]
	= {524288, 1572864, 3670016, 7864320, 253952, 516096, 1040384, 2088960};


// hacky sleep! fix me soon.
static void sleep_ms(uint32_t time_ms)
{
    HAL_DelayUSec(time_ms*1000);
}


uint8_t dps310_probe()
{
    uint8_t data = 0x00;
    
    HAL_I2C_Read(DPS310_DEV_ADDR, DPS310_REG_PRODUCTID, &data, 1);
    
    printf("dps310_probe: [0x%02x]\r\n", data);
    
    if(data == DPS310_VAL_PRODUCTID)
    {
        return QL_STATUS_OK;
    }    
    
    return QL_STATUS_ERROR;
}


uint8_t dps310_soft_reset()
{
    uint8_t data = 0x9;
    uint8_t sensor_rdy = 0;
    
    // soft reset command
    HAL_I2C_Write(DPS310_DEV_ADDR, DPS310_REG_RESET, &data, 1);
    printf("dps310: soft_reset\r\n");
    
    // wait for sensor start up
    while(sensor_rdy == 0)
    {
        HAL_I2C_Read(DPS310_DEV_ADDR, DPS310_REG_MEAS_CFG, &data, 1);
        sensor_rdy = ((data >> 6) & 0x1);     
    }
    printf("dps310: sensor_rdy\r\n");
    
    // read and store the calibration coefficients
    dps310_read_coefficients();
    
    return QL_STATUS_OK;
}


uint8_t dps310_read_coefficients()
{
    uint8_t data = 0x0;
    uint8_t temp_coeff_source = 0;
    uint8_t coef_rdy = 0;
    uint8_t coeffs[18] = {0};
    
    // find out which sensor is calibrated with coefficients
    HAL_I2C_Read(DPS310_DEV_ADDR, DPS310_REG_COEF_SRCE, &data, 1);
    temp_coeff_source = (data >> 7) & 0x1;    
    printf("dps310: temp_coeff_source: %d\r\n", temp_coeff_source);
    
    // update the temp_coeff_source, also this as selected by default
    _coeff_source_temp_sensor = temp_coeff_source;
    _selected_temp_sensor = _coeff_source_temp_sensor;
    
    // wait till the coefficients are ready    
    while(coef_rdy == 0)
    {
        HAL_I2C_Read(DPS310_DEV_ADDR, DPS310_REG_MEAS_CFG, &data, 1);
        coef_rdy = ((data >> 7) & 0x1);        
    }
    printf("dps310: coef_rdy\r\n");
    
    // read coefficients
    HAL_I2C_Read(DPS310_DEV_ADDR, DPS310_REG_COEF, coeffs, 18);
    
    // calculate coefficients
    _c0 = ((uint16_t)coeffs[0] << 4) | (((uint16_t)coeffs[1] >> 4) & 0x0F);
    _c0 = twosComplement(_c0, 12);

    _c1 = twosComplement((((uint16_t)coeffs[1] & 0x0F) << 8) | coeffs[2], 12);

    _c00 = ((uint32_t)coeffs[3] << 12) | ((uint32_t)coeffs[4] << 4) |
         (((uint32_t)coeffs[5] >> 4) & 0x0F);
    _c00 = twosComplement(_c00, 20);

    _c10 = (((uint32_t)coeffs[5] & 0x0F) << 16) | ((uint32_t)coeffs[6] << 8) |
         (uint32_t)coeffs[7];
    _c10 = twosComplement(_c10, 20);

    _c01 = twosComplement(((uint16_t)coeffs[8] << 8) | (uint16_t)coeffs[9], 16);
    _c11 = twosComplement(((uint16_t)coeffs[10] << 8) | (uint16_t)coeffs[11], 16);
    _c20 = twosComplement(((uint16_t)coeffs[12] << 8) | (uint16_t)coeffs[13], 16);
    _c21 = twosComplement(((uint16_t)coeffs[14] << 8) | (uint16_t)coeffs[15], 16);
    _c30 = twosComplement(((uint16_t)coeffs[16] << 8) | (uint16_t)coeffs[17], 16);
    
#if 0
    printf("dps310: _c0 :[0x%04x]\r\n", _c0);
    printf("dps310: _c1 :[0x%04x]\r\n", _c1);
    printf("dps310: _c01:[0x%04x]\r\n", _c01);
    printf("dps310: _c11:[0x%04x]\r\n", _c11);
    printf("dps310: _c20:[0x%04x]\r\n", _c20);
    printf("dps310: _c21:[0x%04x]\r\n", _c21);
    printf("dps310: _c30:[0x%04x]\r\n", _c30);
    
    printf("dps310: _c00:[0x%08x]\r\n", _c00);
    printf("dps310: _c10:[0x%08x]\r\n", _c10);
#endif // #if 0
    
    return QL_STATUS_OK;
}



// pass one of DPS310_VAL_OPMODE_* macros
uint8_t dps310_set_op_mode(uint8_t op_mode)
{
    uint8_t data = (op_mode & 0x7);
  
    HAL_I2C_Write(DPS310_DEV_ADDR, DPS310_REG_MEAS_CFG, &data, 1); 
    
    return QL_STATUS_OK;
}


// pass one of DPS310_VAL_TEMP_SENSOR_* macros
// remember to set to AUTO after use!
void dps310_set_temp_sensor(uint8_t temp_sensor_to_use)
{
    if(temp_sensor_to_use == DPS310_VAL_TEMP_SENSOR_AUTO)
    {
        _selected_temp_sensor = _coeff_source_temp_sensor;
    }
    else
    {
        _selected_temp_sensor = temp_sensor_to_use;
    }
}

uint8_t dps310_read_temp(float* temp_p)
{
    uint8_t data;
    uint8_t temp_measurement_rdy = 0;
    uint8_t temp_data[3] = {0};
    int32_t tempr = 0;
    float temp_scaled = 0;
    
    // check if measurement is ready:
    HAL_I2C_Read(DPS310_DEV_ADDR, DPS310_REG_MEAS_CFG, &data, 1);
    temp_measurement_rdy = ((data >> 5) & 0x1);
    if(temp_measurement_rdy)
    {
        // read temperature
        HAL_I2C_Read(DPS310_DEV_ADDR, DPS310_REG_TMP_B2, temp_data, 3);
        
        tempr = (uint32_t)temp_data[0] << 16
					| (uint32_t)temp_data[1] << 8
					| (uint32_t)temp_data[2];
        //printf("tempr: 0x%08x\r\n", tempr);
        
        //recognize non-32-bit negative numbers
        //and convert them to 32-bit negative numbers using 2's complement
        tempr = twosComplement(tempr, 24);
        //printf("tempr: 0x%08x\r\n", tempr);
        
        // scale tempr according to oversampling_rate
        temp_scaled = (float)tempr/dps310_scaling_factors[_temp_oversampling_rate];
        //printf("temp_scaled: %f\r\n", temp_scaled);
        
        // update temp_scaled value, can be used for next pressure compensation TODO
        _temp_scaled = temp_scaled;
        
        //calculate compensated temperature, and update internal state variable
        _temp_compensated = (temp_scaled * _c1) + (_c0 / 2.0);
    }
    else
    {
        // we will take the previous reading!
        printf("temp not rdy!!\r\n");
    }
    
    *temp_p = _temp_compensated;
    return QL_STATUS_OK;
}


float dps310_measure_single_temp(uint8_t temp_oversampling_rate)
{
    float temp_compensated = 0;
  
    // kickoff the temperature measurement
    dps310_start_single_measurement_temp(temp_oversampling_rate);
    
    // wait time is calculated and is in 0.1ms units
    // so, we convert to ms units by dividing by 10, however, due to integer division
    // this may be less than required, so we divide by 9 instead ( or /10 + 1 etc.)
    sleep_ms(calculate_busy_time(0U, temp_oversampling_rate)/9);
    
    // read final temperature value:
    dps310_read_temp(&temp_compensated);
    
    return temp_compensated;
}


uint8_t dps310_start_single_measurement_temp(uint8_t temp_oversampling_rate)
{
    // check that device is IDLE first TODO
    // if(dps310_state != IDLE) RETURN
  
    // if already configured with same oversampling_rate skip this TODO
    // if(current_temp_oversampling_rate == temp_oversampling_rate) SKIP
    dps310_configure_temp(0,temp_oversampling_rate);
    
    // set opmode to temperature measurement mode
    dps310_set_op_mode(DPS310_VAL_OPMODE_CMD_TMP);
    
    return QL_STATUS_OK;
}


uint8_t dps310_start_continuous_measurement_temp(uint8_t temp_measure_rate, uint8_t temp_oversampling_rate)
{
    uint32_t measurement_time = 0;
    // check that device is IDLE first TODO
    // if(dps310_state != IDLE) RETURN
  
    // check if speed and precision are too high
    measurement_time = calculate_busy_time(temp_measure_rate, temp_oversampling_rate);
    printf("measurement_time: [%d]\r\n", measurement_time);
    if (measurement_time >= DPS310__MAX_BUSYTIME)
	{
		printf("measurement rate too high: [%d] / [%d]\r\n", measurement_time, DPS310__MAX_BUSYTIME);
        return QL_STATUS_ERROR;
	}
    
    // configure
    dps310_configure_temp(temp_measure_rate, temp_oversampling_rate);
    
    // set opmode to BG temperature measurement mode
    dps310_set_op_mode(DPS310_VAL_OPMODE_BG_TMP_CONT);
    
    return QL_STATUS_OK;
}



uint8_t dps310_configure_temp(uint8_t temp_measure_rate, uint8_t temp_oversampling_rate)
{
    // _selected_temp_sensor comes from the coef_srce.
  
    uint8_t tmp_cfg_val = 0x0;
    uint8_t cfg_reg_val = 0x0;
    
    _temp_oversampling_rate = temp_oversampling_rate;
    
    // set configuration to TMP_CFG
    tmp_cfg_val |= ((temp_oversampling_rate & 0x7) << 0);
    tmp_cfg_val |= ((temp_measure_rate & 0x7) << 4);
    tmp_cfg_val |= ((_selected_temp_sensor & 0x1) << 7);
    
    HAL_I2C_Write(DPS310_DEV_ADDR, DPS310_REG_TMP_CFG, &tmp_cfg_val, 1);
    
    // if temp_oversampling_rate > 8, set TEMP_SHIFT_ENABLE in CFG_REG, else clear it.
    HAL_I2C_Read(DPS310_DEV_ADDR, DPS310_REG_CFG_REG, &cfg_reg_val, 1);
    if(temp_oversampling_rate > 3)
    {
        cfg_reg_val |= (1 << 3);
    }
    else
    {
        cfg_reg_val &= ~(1 << 3);
    }
    HAL_I2C_Write(DPS310_DEV_ADDR, DPS310_REG_CFG_REG, &cfg_reg_val, 1);
 
    return QL_STATUS_OK;
}


uint8_t dps310_read_press(float* press_p)
{
    uint8_t data;
    uint8_t press_measurement_rdy = 0;
    uint8_t temp_data[3] = {0};
    int32_t pressr = 0;
    float press_scaled = 0;
    
    // check if measurement is ready:
    HAL_I2C_Read(DPS310_DEV_ADDR, DPS310_REG_MEAS_CFG, &data, 1);
    press_measurement_rdy = ((data >> 4) & 0x1);
    if(press_measurement_rdy)
    {
        // read pressure
        HAL_I2C_Read(DPS310_DEV_ADDR, DPS310_REG_PRS_B2, temp_data, 3);
        
        pressr = (uint32_t)temp_data[0] << 16
					| (uint32_t)temp_data[1] << 8
					| (uint32_t)temp_data[2];
        
        //recognize non-32-bit negative numbers
        //and convert them to 32-bit negative numbers using 2's complement
        pressr = twosComplement(pressr, 24);
        //printf("pressr: 0x%08x\r\n", pressr);
        
        // scale tempr according to oversampling_rate
        press_scaled = (float)pressr/dps310_scaling_factors[_press_oversampling_rate];
        //printf("press_scaled: %f\r\n", press_scaled);
        
        // update press_scaled value
        _press_scaled = press_scaled;
        
        //calculate compensated pressure and update internal state variable                            
        _press_compensated = _c00 +
                            press_scaled * (_c10 + press_scaled * (_c20 + press_scaled * _c30)) +
                            _temp_scaled *(_c01 + press_scaled * (_c11 + press_scaled * _c21));
    }
    
    *press_p = _press_compensated;
    
    return QL_STATUS_OK;
}

float dps310_measure_single_press(uint8_t press_oversampling_rate)
{
    float press_compensated = 0;
  
    // kickoff the temperature measurement
    dps310_start_single_measurement_press(press_oversampling_rate);
    
    // wait time is calculated and is in 0.1ms units
    // so, we convert to ms units by dividing by 10, however, due to integer division
    // this may be less than required, so we divide by 9 instead ( or /10 + 1 etc.)
    sleep_ms(calculate_busy_time(0U, press_oversampling_rate)/9);
    
    // read final pressure value:
    dps310_read_press(&press_compensated);
    
    return press_compensated;
}


uint8_t dps310_start_single_measurement_press(uint8_t press_oversampling_rate)
{
    // check that device is IDLE first TODO
    // if(dps310_state != IDLE) RETURN
  
    // if already configured with same oversampling_rate skip this TODO
    // if(current_press_oversampling_rate == press_oversampling_rate) SKIP
    dps310_configure_press(0,press_oversampling_rate);
    
    // set opmode to pressure measurement mode
    dps310_set_op_mode(DPS310_VAL_OPMODE_CMD_PRS);
    
    return QL_STATUS_OK;
}


uint8_t dps310_start_continuous_measurement_press(uint8_t press_measure_rate, uint8_t press_oversampling_rate)
{
    uint32_t measurement_time = 0;
    // check that device is IDLE first TODO
    // if(dps310_state != IDLE) RETURN
  
    // check if speed and precision are too high
    measurement_time = calculate_busy_time(press_measure_rate, press_oversampling_rate);
    printf("measurement_time: [%d]\r\n", measurement_time);
    if (measurement_time >= DPS310__MAX_BUSYTIME)
	{
		printf("measurement rate too high: [%d] / [%d]\r\n", measurement_time, DPS310__MAX_BUSYTIME);
        return QL_STATUS_ERROR;
	}
    
    // configure
    dps310_configure_press(press_measure_rate, press_oversampling_rate);
    
    // set opmode to BG pressure measurement mode
    dps310_set_op_mode(DPS310_VAL_OPMODE_BG_PRS_CONT);
    
    return QL_STATUS_OK;
}


uint8_t dps310_configure_press(uint8_t press_measure_rate, uint8_t press_oversampling_rate)
{
    uint8_t prs_cfg_val = 0x0;
    uint8_t cfg_reg_val = 0x0;
    
    _press_oversampling_rate = press_oversampling_rate;
    
    // set configuration to PRS_CFG
    prs_cfg_val |= ((press_oversampling_rate & 0x7) << 0);
    prs_cfg_val |= ((press_measure_rate & 0x7) << 4);
    
    HAL_I2C_Write(DPS310_DEV_ADDR, DPS310_REG_PRS_CFG, &prs_cfg_val, 1);
    
    // if press_oversampling_rate > 8, set PRS_SHIFT_ENABLE in CFG_REG, else clear it.
    HAL_I2C_Read(DPS310_DEV_ADDR, DPS310_REG_CFG_REG, &cfg_reg_val, 1);
    if(press_oversampling_rate > 3)
    {
        cfg_reg_val |= (1 << 2);
    }
    else
    {
        cfg_reg_val &= ~(1 << 2);
    }
    HAL_I2C_Write(DPS310_DEV_ADDR, DPS310_REG_CFG_REG, &cfg_reg_val, 1);
 
    return QL_STATUS_OK;
}


uint8_t dps310_start_continuous_measurement_both(int8_t temp_measure_rate, uint8_t temp_oversampling_rate, 
                                                 uint8_t press_measure_rate, uint8_t press_oversampling_rate)
{
    uint32_t measurement_time = 0;
    // check that device is IDLE first TODO
    // if(dps310_state != IDLE) RETURN
  
    // check if speed and precision are too high
    measurement_time = calculate_busy_time(press_measure_rate, press_oversampling_rate);
    measurement_time += calculate_busy_time(temp_measure_rate, temp_oversampling_rate);
    printf("measurement_time: [%d]\r\n", measurement_time);
    if (measurement_time >= DPS310__MAX_BUSYTIME)
	{
        printf("measurement rate too high: [%d] / [%d]\r\n", measurement_time, DPS310__MAX_BUSYTIME);
        return QL_STATUS_ERROR;
	}
    
    // configure
    dps310_configure_temp(temp_measure_rate, temp_oversampling_rate);
    dps310_configure_press(press_measure_rate, press_oversampling_rate);
    
    // set opmode to BG pressure + temperature measurement mode
    dps310_set_op_mode(DPS310_VAL_OPMODE_BG_PRS_TMP_CONT);
    
    return QL_STATUS_OK;    
}


uint8_t dps310_read_both(float* temp_p, float* press_p)
{
    uint8_t status;
    
    *temp_p = 0;
    *press_p = 0;
    
    // read both temperature and pressure:
    status = dps310_read_temp(temp_p);
    
    if(status == QL_STATUS_OK)
    {
        status = dps310_read_press(press_p);
    }
    
    return status;
}


uint8_t dps310_read_sensorpoll_data(struct sensorhub_poll_data_dps310r* dps310r_data, struct sensorhub_poll_data_dps310* dps310_data) {
    
    uint8_t status = QL_STATUS_OK;
    uint8_t reg_data[6] = {0};
    uint8_t data;
    
    uint8_t temp_measurement_rdy = 0;
    uint8_t press_measurement_rdy = 0;
    
    HAL_I2C_Read(DPS310_DEV_ADDR, DPS310_REG_MEAS_CFG, &data, 1);    
    
    // reg_data 0,1,2 - prs, 3,4,5 - tmp, 6 - prs_cfg, 7 - tmp_cfg, 8 - meas_cfg, 9 - cfg_reg
    
    temp_measurement_rdy = ((data >> 5) & 0x1);
    press_measurement_rdy = ((data >> 4) & 0x1);
    
    if(temp_measurement_rdy && press_measurement_rdy)
    {
        HAL_I2C_Read(DPS310_DEV_ADDR, DPS310_REG_PRS_B2, reg_data, 6);
    }
    
    if(temp_measurement_rdy)
    {
       
        _tempr = (uint32_t)reg_data[3] << 16
                | (uint32_t)reg_data[4] << 8
                | (uint32_t)reg_data[5];
        
        if(dps310r_data) 
        {
            dps310r_data->dps310_temp = _tempr;
        }
        //printf("tempr: 0x%08x\r\n", tempr);
        
        if(dps310_data) 
        {
            //recognize non-32-bit negative numbers
            //and convert them to 32-bit negative numbers using 2's complement
            _tempr = twosComplement(_tempr, 24);
            //printf("tempr: 0x%08x\r\n", tempr);
            
            // scale tempr according to oversampling_rate
            _temp_scaled = (float)_tempr/dps310_scaling_factors[_temp_oversampling_rate];
            //printf("temp_scaled: %f\r\n", temp_scaled);
            
            //calculate compensated temperature, and update internal state variable
            _temp_compensated = (_temp_scaled * _c1) + (_c0 / 2.0);
            
            dps310_data->dps310_temp = _temp_compensated;
        }
    }
    else
    {
        // we will take the previous reading!
        printf("temp not rdy!!\r\n");
        
        if(dps310r_data) {
            dps310r_data->dps310_temp = _tempr;
        }
        
        if(dps310_data) {
            dps310_data->dps310_temp = _temp_compensated;
        }
    }
    
   
    if(press_measurement_rdy)
    {
       
        _pressr = (uint32_t)reg_data[0] << 16
                | (uint32_t)reg_data[1] << 8
                | (uint32_t)reg_data[2];
        
        if(dps310r_data) 
        {
            dps310r_data->dps310_press = _pressr;
        }
        
        if(dps310_data) 
        {
            //recognize non-32-bit negative numbers
            //and convert them to 32-bit negative numbers using 2's complement
            _pressr = twosComplement(_pressr, 24);
            //printf("pressr: 0x%08x\r\n", pressr);
            
            // scale tempr according to oversampling_rate
            _press_scaled = (float)_pressr/dps310_scaling_factors[_press_oversampling_rate];
            //printf("press_scaled: %f\r\n", press_scaled);
            
            //calculate compensated pressure and update internal state variable                            
            _press_compensated = _c00 +
                                _press_scaled * (_c10 + _press_scaled * (_c20 + _press_scaled * _c30)) +
                                _temp_scaled *(_c01 + _press_scaled * (_c11 + _press_scaled * _c21));
            
            dps310_data->dps310_press = _press_compensated;
        }
    }
    else 
    {
      
        printf("press not rdy!!\r\n");
        
        if(dps310r_data) 
        {
            dps310r_data->dps310_press = _pressr;
        }
        
        if(dps310_data) 
        {
          dps310_data->dps310_press = _press_compensated;
        }
    }
    
    //printf("temp  : [%f] \r\n", dps310_data->dps310_temp);
    //printf("press : [%f] \r\n", dps310_data->dps310_press);
    
    return status;  
}


uint16_t calculate_busy_time(uint8_t x_measure_rate, uint8_t x_oversampling_rate)
{
    // formula from datasheet (optimized)
    uint32_t busy_time = (20U << x_measure_rate) +
                         (16U << (x_oversampling_rate + x_measure_rate));
    
	return busy_time;
}