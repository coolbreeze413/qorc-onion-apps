// BMX160 core driver
// we need the following API:
// open (initialize)
// close (shutdown)
// suspend (low-power or shutdown)
// resume (normal-power)
// read() A.M.G.T.
// 
// accel configuration
// range, odr, powermode, bandwidth
// gyro configuration
// range, odr, powermode, bandwidth
// mag configuration
// range, odr, powermode, bandwidth
// each needs conversions:
// ODR num -> ODR regval
// Range enum -> Range regval
// multipliers for data:
// accel_val = accel_regval * multiplier_for_range
// gyro_val = gyro_regval * multiplier_for_range (also called sensitivity)


#include <stdint.h>
#include <stdio.h>

#include "Fw_global_config.h"

#include "QL_Trace.h"

#include "eoss3_hal_i2c.h"
#include "eoss3_hal_timer.h"

#include "bmx160.h"

#include "ql_sensorhub_poll_task.h"


// globals are evil, yes we know
static struct bmx160_config bmx160_configuration = {0};

// hacky sleep! fix me soon.
static void sleep_ms(uint32_t time_ms)
{
    HAL_DelayUSec(time_ms*1000);
}


uint8_t bmx160_probe()
{
    uint8_t data = 0x00;
    
    HAL_I2C_Read(BMX160_DEV_ADDR, BMX160_REG_CHIPID, &data, 1);
    
    printf("bmx160_probe: [0x%02x]\r\n", data);
    
    if(data == BMX160_VAL_CHIPID)
    {
        return QL_STATUS_OK;
    }
    
    return QL_STATUS_ERROR;
}


uint8_t bmx160_soft_reset()
{
    uint8_t data = BMX160_VAL_SOFT_RESET_CMD;

    if(HAL_I2C_Write(BMX160_DEV_ADDR, BMX160_REG_CMD, &data, 1) != HAL_OK)
    {
        return QL_STATUS_ERROR;
    }

    sleep_ms(BMX160_VAL_SOFT_RESET_DELAY_MS);

    return QL_STATUS_OK;
}


uint8_t bmx160_wakeup(uint8_t accel_wakeup, uint8_t gyro_wakeup, uint8_t mag_wakeup)
{
    uint8_t data = 0x00;
    
    if(accel_wakeup == 1)
    {
        data = BMX160_VAL_ACCEL_NORMAL_MODE;
        if(HAL_I2C_Write(BMX160_DEV_ADDR, BMX160_REG_CMD, &data, 1) != HAL_OK)
        {
            return QL_STATUS_ERROR;
        }
    }
    
    if(gyro_wakeup == 1)
    {
        data = BMX160_VAL_GYRO_NORMAL_MODE;
        if(HAL_I2C_Write(BMX160_DEV_ADDR, BMX160_REG_CMD, &data, 1) != HAL_OK)
        {
            return QL_STATUS_ERROR;
        }
    }
    
    
    if(mag_wakeup == 1)
    {
        data = BMX160_VAL_MAG_NORMAL_MODE;
        if(HAL_I2C_Write(BMX160_DEV_ADDR, BMX160_REG_CMD, &data, 1) != HAL_OK)
        {
            return QL_STATUS_ERROR;
        }    
    }
    
    return QL_STATUS_OK;
}


uint8_t bmx160_suspend(uint8_t accel_suspend, uint8_t gyro_suspend, uint8_t mag_suspend)
{
    uint8_t data = 0x00;
    
    if(accel_suspend == 1)
    {
        data = BMX160_VAL_ACCEL_SUSPEND_MODE;
        if(HAL_I2C_Write(BMX160_DEV_ADDR, BMX160_REG_CMD, &data, 1) != HAL_OK)
        {
            return QL_STATUS_ERROR;
        }
    }
    
    if(gyro_suspend == 1)
    {
        data = BMX160_VAL_GYRO_SUSPEND_MODE;
        if(HAL_I2C_Write(BMX160_DEV_ADDR, BMX160_REG_CMD, &data, 1) != HAL_OK)
        {
            return QL_STATUS_ERROR;
        }
    }
    
    
    if(mag_suspend == 1)
    {
        data = BMX160_VAL_MAG_SUSPEND_MODE;
        if(HAL_I2C_Write(BMX160_DEV_ADDR, BMX160_REG_CMD, &data, 1) != HAL_OK)
        {
            return QL_STATUS_ERROR;
        }    
    }
    
    return QL_STATUS_OK;
}


uint8_t bmx160_set_accel_config(uint16_t power_mode, uint16_t range, uint16_t odr, uint16_t bw)
{
    uint8_t data = 0x00;
    
    // power_mode
    data = power_mode;
    HAL_I2C_Write(BMX160_DEV_ADDR, BMX160_REG_CMD, &data, 1);
    sleep_ms(50);
    
    // range
    data = range;
    bmx160_configuration.accel_range = range;
    bmx160_configuration.accel_scale = get_accel_scale(bmx160_configuration.accel_range);
    HAL_I2C_Write(BMX160_DEV_ADDR, BMX160_REG_ACC_RANGE, &data, 1);
    
    // odr, bw
    data = ((bw << 4) | odr);
    HAL_I2C_Write(BMX160_DEV_ADDR, BMX160_REG_ACC_CONF, &data, 1);
    
    return QL_STATUS_OK;
}


uint8_t bmx160_set_gyro_config(uint16_t power_mode, uint16_t range, uint16_t odr, uint16_t bw)
{
    uint8_t data = 0x00;
    
    // power_mode
    data = power_mode;
    HAL_I2C_Write(BMX160_DEV_ADDR, BMX160_REG_CMD, &data, 1);
    sleep_ms(50);
    
    // range
    data = range;
    bmx160_configuration.gyro_range = BMX160_VAL_GYRO_RANGE_250_DPS;
    bmx160_configuration.gyro_scale = get_gyro_scale(bmx160_configuration.gyro_range);
    HAL_I2C_Write(BMX160_DEV_ADDR, BMX160_REG_GYRO_RANGE, &data, 1);
    
    // odr, bw
    data = ((bw << 4) | odr);
    HAL_I2C_Write(BMX160_DEV_ADDR, BMX160_REG_GYRO_CONF, &data, 1);
    
    return QL_STATUS_OK;
}


uint8_t bmx160_set_mag_config(uint16_t odr)
{
    // ADD: other presets as well (LP/ENHANCED/HIGHACCURACY)
    uint8_t data = 0x00;
    
    // put MAG_IF into normal mode
    data = BMX160_VAL_MAG_NORMAL_MODE;
    HAL_I2C_Write(BMX160_DEV_ADDR, BMX160_REG_CMD, &data, 1);
    
    sleep_ms(2);
    
    // switch to setup mode
    data = 0x80; HAL_I2C_Write(BMX160_DEV_ADDR, BMX160_REG_MAG_IF_0, &data, 1);
    
    // put MAG into sleep mode
    data = 0x01; HAL_I2C_Write(BMX160_DEV_ADDR, BMX160_REG_MAG_IF_3, &data, 1);
    data = 0x4B; HAL_I2C_Write(BMX160_DEV_ADDR, BMX160_REG_MAG_IF_2, &data, 1);
    
    // REPXY regular preset
    data = 0x01; HAL_I2C_Write(BMX160_DEV_ADDR, BMX160_REG_MAG_IF_3, &data, 1);
    data = 0x51; HAL_I2C_Write(BMX160_DEV_ADDR, BMX160_REG_MAG_IF_2, &data, 1);
    
    // REPZ regular preset
    data = 0x02; HAL_I2C_Write(BMX160_DEV_ADDR, BMX160_REG_MAG_IF_3, &data, 1);
    data = 0x52; HAL_I2C_Write(BMX160_DEV_ADDR, BMX160_REG_MAG_IF_2, &data, 1);
    
    // prepare to switch to data mode (forced mode)
    data = 0x02; HAL_I2C_Write(BMX160_DEV_ADDR, BMX160_REG_MAG_IF_3, &data, 1);
    data = 0x4C; HAL_I2C_Write(BMX160_DEV_ADDR, BMX160_REG_MAG_IF_2, &data, 1);    
    // trigger read from mag data register
    data = 0x42; HAL_I2C_Write(BMX160_DEV_ADDR, BMX160_REG_MAG_IF_1, &data, 1);
    
    // set ODR at bmx level
    data = odr;
    HAL_I2C_Write(BMX160_DEV_ADDR, BMX160_REG_MAG_CONF, &data, 1);
    
    // switch to data mode
    data = 0x03; HAL_I2C_Write(BMX160_DEV_ADDR, BMX160_REG_MAG_IF_0, &data, 1);
    
    // put MAG_IF into low power mode
    data = BMX160_VAL_MAG_NORMAL_MODE;
    HAL_I2C_Write(BMX160_DEV_ADDR, BMX160_REG_CMD, &data, 1);
    
    sleep_ms(50);
    
    return QL_STATUS_OK;
}


void bmx160_read_all_data(struct bmx160_sensor_data* accel_data, 
                          struct bmx160_sensor_data* gyro_data, 
                          struct bmx160_sensor_data* mag_data,
                          struct bmx160_rhall_data* rhall_data,
                          struct bmx160_temperature_data* temperature_data)
{
    //printf("++");
    uint8_t data[23] = {0};  
    
    // read STATUS register and check which data are ready as well:
    HAL_I2C_Read(BMX160_DEV_ADDR, BMX160_REG_STATUS, data, 1);
    printf("status     : [0x%02x]\r\n", data[0]);
    
    // read ERR register
    HAL_I2C_Read(BMX160_DEV_ADDR, BMX160_REG_ERR_REG, data, 1);
    printf("error      : [0x%02x]\r\n", data[0]);
    
    // read PMU_STATUS register:
    HAL_I2C_Read(BMX160_DEV_ADDR, BMX160_REG_PMU_STATUS, data, 1);
    printf("pmu_status : [0x%02x]\r\n", data[0]);
    
    // read (6 + 6 + 6 + 2 + 3)B of data from the BMX160_REG_DATA reg    
    HAL_I2C_Read(BMX160_DEV_ADDR, BMX160_REG_DATA, data, 23);
    
    uint32_t sensortimer = (uint32_t)((data[22] << 16) | (data[21] << 8) | data[20]);
    uint32_t stime_us = (sensortimer * BMX160_SENSORTIME_SCALE_us);
    printf("sensortimer: %u\r\n",sensortimer);
    printf("stime_us   : %u\r\n",stime_us);
    
    if(accel_data)
    {      
        accel_data->xr = (int16_t) ((data[15] << 8) | data[14]);
        accel_data->yr = (int16_t) ((data[17] << 8) | data[16]);
        accel_data->zr = (int16_t) ((data[19] << 8) | data[18]);
        
        accel_data->x = accel_data->xr * bmx160_configuration.accel_scale;
        accel_data->y = accel_data->yr * bmx160_configuration.accel_scale;
        accel_data->z = accel_data->zr * bmx160_configuration.accel_scale;
        
        accel_data->ts = stime_us;
    }    
    
    if(gyro_data)
    {
        gyro_data->xr = (int16_t) ((data[9] << 8) | data[8]);
        gyro_data->yr = (int16_t) ((data[11] << 8) | data[10]);
        gyro_data->zr = (int16_t) ((data[13] << 8) | data[12]);
        
        gyro_data->x = gyro_data->xr * bmx160_configuration.gyro_scale;
        gyro_data->y = gyro_data->yr * bmx160_configuration.gyro_scale;
        gyro_data->z = gyro_data->zr * bmx160_configuration.gyro_scale;
    }
    
    if(mag_data)
    {
        mag_data->xr = (int16_t) ((data[1] << 8) | data[0]);
        mag_data->yr = (int16_t) ((data[3] << 8) | data[2]);
        mag_data->zr = (int16_t) ((data[5] << 8) | data[4]);
        
        mag_data->x = mag_data->xr * BMX160_MAG_SCALE_ALL_uT;
        mag_data->y = mag_data->yr * BMX160_MAG_SCALE_ALL_uT;
        mag_data->z = mag_data->zr * BMX160_MAG_SCALE_ALL_uT;
    }
    
    if(rhall_data)
    {
        rhall_data->rhallr = (int16_t) ((data[1] << 7) | data[6]);
    }
    
    if(temperature_data)
    {
        HAL_I2C_Read(BMX160_DEV_ADDR, BMX160_REG_TEMPERATURE, data, 2);
        temperature_data->tempr = (int16_t) ((data[1] << 8) | data[0]);
        
        // negative value?
        if((temperature_data->tempr >> 15) & 0x1)
        {
            temperature_data->temp = (-41 + BMX160_TEMPERATURE_SCALE_degC) + 
                                     ((0xFFFF - temperature_data->tempr) * (BMX160_TEMPERATURE_SCALE_degC));
        }
        else
        {
            temperature_data->temp = 23 + 
                                     ((temperature_data->tempr) * (BMX160_TEMPERATURE_SCALE_degC));
        }        
        
    }
    //printf("--");
}


void bmx160_read_sensorpoll_data(struct sensorhub_poll_data_bmx160r* bmx160r_data, 
                                    struct sensorhub_poll_data_bmx160* bmx160_data)
{
    uint8_t data[23] = {0};
    uint8_t data_temp[2] = {0};
    
    // read STATUS register and check which data are ready as well:
    //HAL_I2C_Read(BMX160_DEV_ADDR, BMX160_REG_STATUS, data, 1);
    //printf("status     : [0x%02x]\r\n", data[0]);
    
    // read ERR register
    //HAL_I2C_Read(BMX160_DEV_ADDR, BMX160_REG_ERR_REG, data, 1);
    //printf("error      : [0x%02x]\r\n", data[0]);
    
    // read PMU_STATUS register:
    //HAL_I2C_Read(BMX160_DEV_ADDR, BMX160_REG_PMU_STATUS, data, 1);
    //printf("pmu_status : [0x%02x]\r\n", data[0]);
    
    // read (6 + 6 + 6 + 2 + 3)B of data from the BMX160_REG_DATA reg    
    HAL_I2C_Read(BMX160_DEV_ADDR, BMX160_REG_DATA, data, 23);
    // read 2B data from BMX160_REG_TEMPERATURE reg
    HAL_I2C_Read(BMX160_DEV_ADDR, BMX160_REG_TEMPERATURE, data_temp, 2);
    
    uint32_t sensortimer = (uint32_t)((data[22] << 16) | (data[21] << 8) | data[20]);
    uint32_t stime_us = (sensortimer * BMX160_SENSORTIME_SCALE_us);
    //printf("sensortimer: %u\r\n",sensortimer);
    //printf("stime_us   : %u\r\n",stime_us);
    
    if(bmx160r_data)
    {
        bmx160r_data->bmx160_ts = sensortimer;
        bmx160r_data->bmx160_a_x = (int16_t) ((data[15] << 8) | data[14]);
        bmx160r_data->bmx160_a_y = (int16_t) ((data[17] << 8) | data[16]);
        bmx160r_data->bmx160_a_z = (int16_t) ((data[19] << 8) | data[18]);
        bmx160r_data->bmx160_a_f1 = 0;
        bmx160r_data->bmx160_g_x = (int16_t) ((data[9] << 8) | data[8]);
        bmx160r_data->bmx160_g_y = (int16_t) ((data[11] << 8) | data[10]);
        bmx160r_data->bmx160_g_z = (int16_t) ((data[13] << 8) | data[12]);
        bmx160r_data->bmx160_g_f1 = 0;
        bmx160r_data->bmx160_m_x = (int16_t) ((data[1] << 8) | data[0]);
        bmx160r_data->bmx160_m_y = (int16_t) ((data[3] << 8) | data[2]);
        bmx160r_data->bmx160_m_z = (int16_t) ((data[5] << 8) | data[4]);
        bmx160r_data->bmx160_m_f1 = 0;
        bmx160r_data->bmx160_h = (int16_t) ((data[1] << 7) | data[6]);
        bmx160r_data->bmx160_t = (int16_t) ((data_temp[1] << 8) | data_temp[0]);
    }
    
    if(bmx160_data)
    {
        bmx160_data->bmx160_ts = stime_us;
        bmx160_data->bmx160_a_x = bmx160r_data->bmx160_a_x * bmx160_configuration.accel_scale;
        bmx160_data->bmx160_a_y = bmx160r_data->bmx160_a_y * bmx160_configuration.accel_scale;
        bmx160_data->bmx160_a_z = bmx160r_data->bmx160_a_z * bmx160_configuration.accel_scale;
        bmx160_data->bmx160_g_x = bmx160r_data->bmx160_g_x * bmx160_configuration.gyro_scale;
        bmx160_data->bmx160_g_y = bmx160r_data->bmx160_g_y * bmx160_configuration.gyro_scale;
        bmx160_data->bmx160_g_z = bmx160r_data->bmx160_g_z * bmx160_configuration.gyro_scale;
        bmx160_data->bmx160_m_x = bmx160r_data->bmx160_m_x * BMX160_MAG_SCALE_ALL_uT;
        bmx160_data->bmx160_m_y = bmx160r_data->bmx160_m_y * BMX160_MAG_SCALE_ALL_uT;
        bmx160_data->bmx160_m_z = bmx160r_data->bmx160_m_z * BMX160_MAG_SCALE_ALL_uT;
        bmx160_data->bmx160_h = bmx160r_data->bmx160_h; // temporary, need to convert
        
        // negative value?
        if((bmx160r_data->bmx160_t >> 15) & 0x1)
        {
            bmx160_data->bmx160_t = (-41 + BMX160_TEMPERATURE_SCALE_degC) + 
                                     ((0xFFFF - bmx160r_data->bmx160_t) * (BMX160_TEMPERATURE_SCALE_degC));
        }
        else
        {
            bmx160_data->bmx160_t = 23 + 
                                     ((bmx160r_data->bmx160_t) * (BMX160_TEMPERATURE_SCALE_degC));
        } 
    }
}


double get_accel_scale(uint16_t accel_range)
{
    switch(accel_range)
    {
        case BMX160_VAL_ACCEL_RANGE_2G:
        {
            return BMX160_ACCEL_SCALE_2G_mg;
        }
        break;
        
        case BMX160_VAL_ACCEL_RANGE_4G:
        {
            return BMX160_ACCEL_SCALE_4G_mg;
        }
        break;
        
        case BMX160_VAL_ACCEL_RANGE_8G:
        {
            return BMX160_ACCEL_SCALE_8G_mg;
        }
        break;
        
        case BMX160_VAL_ACCEL_RANGE_16G:
        {
            return BMX160_ACCEL_SCALE_16G_mg;
        }
        break;
        
        default:
        {
            return 0;
        }
        break;      
    }
}


double get_gyro_scale(uint16_t gyro_range)
{
    switch(gyro_range)
    {
        case BMX160_VAL_GYRO_RANGE_2000_DPS:
        {
            return BMX160_GYRO_SCALE_2000DPS_dps;
        }
        break;
        
        case BMX160_VAL_GYRO_RANGE_1000_DPS:
        {
            return BMX160_GYRO_SCALE_1000DPS_dps;
        }
        break;
        
        case BMX160_VAL_GYRO_RANGE_500_DPS:
        {
            return BMX160_GYRO_SCALE_500DPS_dps;
        }
        break;
        
        case BMX160_VAL_GYRO_RANGE_250_DPS:
        {
            return BMX160_GYRO_SCALE_250DPS_dps;
        }
        break;
        
        case BMX160_VAL_GYRO_RANGE_125_DPS:
        {
            return BMX160_GYRO_SCALE_125DPS_dps;
        }
        break;
        
        default:
        {
            return 0;
        }
        break;      
    }
}