#include <stdint.h>
#include <stdio.h>

#include "Fw_global_config.h"

#include "eoss3_hal_timer.h"

#include "dps310.h"
#include "QL_Trace.h"


uint8_t dps310_basic_test(uint32_t num_samples, uint32_t period_ms)
{
    uint32_t read_count = 0;
    float temp = 0;
    float press = 0;
    
    dps310_probe();
    dps310_soft_reset();    
    
    while(read_count < num_samples)
    {
        dps310_set_temp_sensor(DPS310_VAL_TEMP_SENSOR_INT_ASIC);
        temp = dps310_measure_single_temp(0x7);
        printf("temp_asic: [%f]\r\n", temp);
    
        dps310_set_temp_sensor(DPS310_VAL_TEMP_SENSOR_EXT_MEMS);
        temp = dps310_measure_single_temp(0x7);
        printf("temp_mems: [%f]\r\n", temp);
    
        dps310_set_temp_sensor(DPS310_VAL_TEMP_SENSOR_AUTO);
        temp = dps310_measure_single_temp(0x7);
        printf("temp_auto: [%f]\r\n", temp);
    
        press = dps310_measure_single_press(0x7);
        printf("press    : %f\r\n", press);
        
        HAL_DelayUSec(period_ms*1000);
        
        printf("\r\n");
        
        read_count++;
    }
    
    return QL_STATUS_OK;
}


uint8_t dps310_continuous_test(uint32_t num_samples, uint32_t measure_rate)
{
    uint8_t status = QL_STATUS_OK;
    uint32_t read_count = 0;
    float temp = 0;
    float press = 0;
    uint32_t period_ms = (1000/measure_rate);
    
    dps310_probe();
    dps310_soft_reset();
    
    dps310_set_temp_sensor(DPS310_VAL_TEMP_SENSOR_AUTO);
    //initial reading of temperature so that we have a reading for pressure compensation later.
    temp = dps310_measure_single_temp(0x7);
    printf("temp_auto: [%f]\r\n", temp);
    
    
    // highest precision
    dps310_start_continuous_measurement_both(0x0, 0x7, 
                                             0x0, 0x7);
    
    while(read_count < num_samples)
    {    
        HAL_DelayUSec(period_ms*1000);
        
        status = dps310_read_both(&temp, &press);
        
        printf("temp  : [%f] \r\n", temp);
        printf("press : [%f] \r\n", press);
        
        printf("\r\n");
        
        read_count++;
    }
  
    return status;
}