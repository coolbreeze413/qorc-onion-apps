
#include <stdint.h>
#include <stdio.h>

#include "Fw_global_config.h"

#include "eoss3_hal_timer.h"

#include "bmx160.h"
#include "QL_Trace.h"



struct bmx160_sensor_data accel_data = {0};
struct bmx160_sensor_data gyro_data = {0};
struct bmx160_sensor_data mag_data = {0};
struct bmx160_rhall_data rhall_data = {0};
struct bmx160_temperature_data temperature_data = {0};

uint8_t bmx160_basic_test(uint32_t num_samples, uint32_t period_ms)
{    
    uint32_t read_count = 0;
    
    bmx160_probe();
    bmx160_soft_reset();

    bmx160_set_accel_config(BMX160_VAL_ACCEL_NORMAL_MODE, 
                            BMX160_VAL_ACCEL_RANGE_4G, 
                            BMX160_VAL_ACCEL_ODR_100HZ, 
                            BMX160_VAL_ACCEL_BW_NORMAL_AVG4);
    
    bmx160_set_gyro_config(BMX160_VAL_GYRO_NORMAL_MODE, 
                           BMX160_VAL_GYRO_RANGE_250_DPS, 
                           BMX160_VAL_GYRO_ODR_100HZ, 
                           BMX160_VAL_GYRO_BW_NORMAL_MODE);
    
    bmx160_set_mag_config(BMX160_VAL_MAG_ODR_12_5HZ);
    
    while(read_count < num_samples)
    {
        printf("count:[%u]\r\n", read_count);
        
        bmx160_read_all_data(&accel_data, 
                             &gyro_data, 
                             &mag_data, 
                             &rhall_data, 
                             &temperature_data);
        
        printf("accel: [0x%04x, 0x%04x, 0x%04x]\r\n", accel_data.xr, accel_data.yr, accel_data.zr);
        printf("accel: [%f, %f, %f]\r\n", accel_data.x, accel_data.y, accel_data.z);
        
        printf("gyro : [0x%04x, 0x%04x, 0x%04x]\r\n", gyro_data.xr, gyro_data.yr, gyro_data.zr);
        printf("gyro : [%f, %f, %f]\r\n", gyro_data.x, gyro_data.y, gyro_data.z);
        
        printf("mag  : [0x%04x, 0x%04x, 0x%04x]\r\n", mag_data.xr, mag_data.yr, mag_data.zr);
        printf("mag  : [%f, %f, %f]\r\n", mag_data.x, mag_data.y, mag_data.z);
        
        printf("rhall: [0x%04x]\r\n", rhall_data.rhallr);        
        
        printf("temp : [0x%04x]\r\n", temperature_data.tempr);
        printf("temp : [%f]\r\n", temperature_data.temp);
        
        printf("\r\n\r\n");
        
        read_count++;
        HAL_DelayUSec(period_ms*1000);
    }    
    
    return QL_STATUS_OK;
}

struct bmx160_sensor_data accel_data_arr[200] = {0};
uint8_t bmx160_accel_odr_test(uint32_t num_samples, uint32_t period_ms)
{    
    uint32_t read_count = 0;
    int32_t i = 0;
    
    bmx160_probe();
    bmx160_soft_reset();

    bmx160_set_accel_config(BMX160_VAL_ACCEL_NORMAL_MODE, 
                            BMX160_VAL_ACCEL_RANGE_4G, 
                            BMX160_VAL_ACCEL_ODR_100HZ, 
                            BMX160_VAL_ACCEL_BW_NORMAL_AVG4);
    
    bmx160_set_gyro_config(BMX160_VAL_GYRO_NORMAL_MODE, 
                           BMX160_VAL_GYRO_RANGE_250_DPS, 
                           BMX160_VAL_GYRO_ODR_100HZ, 
                           BMX160_VAL_GYRO_BW_NORMAL_MODE);
    
    bmx160_set_mag_config(BMX160_VAL_MAG_ODR_12_5HZ);
    
    while(read_count < num_samples)
    {
        //printf("count:[%u]\r\n", read_count);
        
        bmx160_read_all_data(&accel_data_arr[i], 
                             &gyro_data, 
                             &mag_data, 
                             &rhall_data, 
                             &temperature_data);
        i++;
        read_count++;
        //HAL_DelayUSec(period_ms*1000);
    }

    for(i=0; i<200; i++)
    {
        //printf("accel: [0x%04x, 0x%04x, 0x%04x]\r\n", accel_data_arr[i].xr, accel_data_arr[i].yr, accel_data_arr[i].zr);
        printf("accel: [%u, %f, %f, %f]\r\n", accel_data_arr[i].ts, accel_data_arr[i].x, accel_data_arr[i].y, accel_data_arr[i].z);
    }
    
    return QL_STATUS_OK;
}