#ifndef __DPS310_H__
#define __DPS310_H__

#include <stdint.h>

// references:
// https://github.com/Infineon/DPS310-Pressure-Sensor
// https://github.com/adafruit/Adafruit_DPS310


// device address
#define DPS310_DEV_ADDR                         0x77

// registers
#define DPS310_REG_PRODUCTID                    0x0D
#define DPS310_REG_RESET                        0x0C

#define DPS310_REG_COEF                         0x10    // 18B
#define DPS310_REG_COEF_SRCE                    0x28

#define DPS310_REG_CFG_REG                      0x09
#define DPS310_REG_MEAS_CFG                     0x08

#define DPS310_REG_PRS_CFG                      0x06
#define DPS310_REG_TMP_CFG                      0x07

#define DPS310_REG_INT_STS                      0x0A
#define DPS310_REG_FIFO_STS                     0x0B

#define DPS310_REG_PRS_B2                       0x00
#define DPS310_REG_TMP_B2                       0x03


// default or common values
#define DPS310_VAL_PRODUCTID                    0x10
#define DPS310_VAL_SOFT_RESET_CMD               0xB6

// operating modes
#define DPS310_VAL_OPMODE_IDLE                  0x0
#define DPS310_VAL_OPMODE_CMD_PRS               0x1
#define DPS310_VAL_OPMODE_CMD_TMP               0x2
#define DPS310_VAL_OPMODE_BG_PRS_CONT           0x5
#define DPS310_VAL_OPMODE_BG_TMP_CONT           0x6
#define DPS310_VAL_OPMODE_BG_PRS_TMP_CONT       0x7

#define DPS310_VAL_TEMP_SENSOR_INT_ASIC         0x0
#define DPS310_VAL_TEMP_SENSOR_EXT_MEMS         0x1
#define DPS310_VAL_TEMP_SENSOR_AUTO             0x2 // automatically take coef_srce temp sensor!

// from the infineon driver:
// DPS310 has 10 milliseconds of spare time for each synchronous measurement / per second for asynchronous measurements
// this is for error prevention on friday-afternoon-products :D
// you can set it to 0 if you dare, but there is no warranty that it will still work
// we use 0.1 ms units for time calculations, so 10 units are one millisecond
#define DPS__BUSYTIME_SCALING 10U
#define DPS310__BUSYTIME_FAILSAFE 10U
#define DPS310__MAX_BUSYTIME ((1000U - DPS310__BUSYTIME_FAILSAFE) * DPS__BUSYTIME_SCALING)


uint8_t dps310_probe();
uint8_t dps310_soft_reset();
float dps310_measure_single_temp(uint8_t temp_oversampling_rate);
float dps310_measure_single_press(uint8_t press_oversampling_rate);
uint8_t dps310_read_temp(float* temp_p);
uint8_t dps310_read_press(float* press_p);
uint8_t dps310_read_both(float* temp_p, float* press_p);

// internal
uint8_t dps310_read_coefficients();
uint8_t dps310_set_op_mode(uint8_t op_mode);

void dps310_set_temp_sensor(uint8_t temp_sensor_to_use);
uint8_t dps310_start_single_measurement_temp(uint8_t temp_oversampling_rate);
uint8_t dps310_start_continuous_measurement_temp(uint8_t temp_measure_rate, uint8_t temp_oversampling_rate);
uint8_t dps310_configure_temp(uint8_t temp_measure_rate, uint8_t temp_oversampling_rate);

uint8_t dps310_start_single_measurement_press(uint8_t press_oversampling_rate);
uint8_t dps310_start_continuous_measurement_press(uint8_t press_measure_rate, uint8_t press_oversampling_rate);
uint8_t dps310_configure_press(uint8_t press_measure_rate, uint8_t press_oversampling_rate);

uint8_t dps310_start_continuous_measurement_both(int8_t temp_measure_rate, uint8_t temp_oversampling_rate, 
                                                 uint8_t press_measure_rate, uint8_t press_oversampling_rate);

uint16_t calculate_busy_time(uint8_t x_measure_rate, uint8_t x_oversampling_rate);


uint8_t dps310_basic_test();

#endif // #ifndef __DPS310_H__
