#include <stdio.h>

/*	Include the generic headers required for sensorHub */

#include "Fw_global_config.h"

#include "eoss3_hal_i2c.h"
#include "eoss3_hal_pad_config.h"
#include "eoss3_hal_gpio.h"

#define BMA400_DEV_ADDR         0x14
#define BMA400_REG_CHIPID       0x00
#define BMA400_VAL_CHIPID       0x90
#define BMA400_REG_CONFIG       0x1A
#define BMA400_VAL_CONFIG       0x49

#define GMC306A_DEV_ADDR        0x0C // 0x0D
#define GMC306A_REG_CMPID       0x50
#define GMC306A_VAL_CMPID       0x43
#define GMC306A_REG_DEVID       0x51
#define GMC306A_VAL_DEVID       0x00

#define BMI160_DEV_ADDR         0x68 // 0x69
#define BMI160_REG_CHIPID       0x00
#define BMI160_VAL_CHIPID       0xD1
#define BMI160_REG_GYRO_CONFIG  0x42
#define BMI160_VAL_GYRO_CONFIG  0x28

#define ICM42605_DEV_ADDR           0x68 // 0x69
#define ICM42605_REG_BANKSEL        0x76 // BANK *
#define ICM42605_VAL_BANKSEL        0x00 // BANK *
#define ICM42605_REG_WHOAMI         0x75 // BANK 0
#define ICM42605_VAL_WHOAMI         0x42 // BANK 0
#define ICM42605_REG_DRIVECONFIG    0x13 // BANK 0
#define ICM42605_VAL_DRIVECONFIG    0x05 // BANK 0

#define BMX160_DEV_ADDR         0x68 // 0x69
#define BMX160_REG_CHIPID       0x00
#define BMX160_VAL_CHIPID       0xD8
#define BMX160_REG_GYRO_CONFIG  0x42
#define BMX160_VAL_GYRO_CONFIG  0x28

#define DPS310_DEV_ADDR         0x77
#define DPS310_REG_REVID        0x0D
#define DPS310_VAL_REVID        0x10
#define DPS310_REG_MEASCFG      0x08
#define DPS310_VAL_MEASCFG      0xC0

#define HX3313_DEV_ADDR         0x44
#define HX3313_REG_CHIPID       0x00
#define HX3313_VAL_CHIPID       0x22
#define HX3313_REG_RESERVED_3   0x03
#define HX3313_VAL_RESERVED_3   0x8F

#define BME280_DEV_ADDR         0x76
#define BME280_REG_CHIPID       0xD0
#define BME280_VAL_CHIPID       0x60
#define BME280_REG_RESET        0xE0
#define BME280_VAL_RESET        0x00

#define BNO055_DEV_ADDR         0x28
#define BNO055_REG_CHIPID       0x00
#define BNO055_VAL_CHIPID       0xA0
#define BNO055_REG_ACCID        0x01
#define BNO055_VAL_ACCID        0xFB
#define BNO055_REG_MAGID        0x02
#define BNO055_VAL_MAGID        0x32
#define BNO055_REG_GYRID        0x03
#define BNO055_VAL_GYRID        0x0F
#define BNO055_REG_GYRID        0x03
#define BNO055_VAL_GYRID        0x0F
#define BNO055_REG_SWREVIDLSB        0x04
#define BNO055_VAL_SWREVIDLSB        0x11
#define BNO055_REG_SWREVIDMSB        0x05
#define BNO055_VAL_SWREVIDMSB        0x03


void setup_I2C0_M4()
{  
    I2C_Config xI2CConfig;
    
    // I2C config for I2C0
    xI2CConfig.eI2CFreq = I2C_400KHZ;
    xI2CConfig.eI2CInt = I2C_DISABLE;
    xI2CConfig.ucI2Cn = 0;
    HAL_I2C_Init(xI2CConfig);  
}

void setup_I2C1_M4()
{  
    I2C_Config xI2CConfig;
    
    // I2C config for I2C1
    xI2CConfig.eI2CFreq = I2C_400KHZ;
    xI2CConfig.eI2CInt = I2C_DISABLE;
    xI2CConfig.ucI2Cn = 1;
    HAL_I2C_Init(xI2CConfig);  
}


uint8_t verify_I2C_reg_read(uint8_t i2c_bus_num, 
                            const char* device_name, 
                            const char* reg_name, 
                            uint8_t device_addr, 
                            uint8_t reg_addr, 
                            uint8_t reg_ref_value)
{
    uint8_t i2c_read_data;
    uint8_t retval = 0;

    if(i2c_bus_num == 0)
    {
        HAL_I2C0_Select();
    }
    else if(i2c_bus_num == 1)
    {
        HAL_I2C1_Select();
    }
    else
    {
        printf("Invalid I2C Bus Num: %d\r\n", i2c_bus_num);
    }

    HAL_I2C_Read(device_addr, reg_addr, &i2c_read_data, 1);
    if(i2c_read_data == reg_ref_value) 
    { 
        printf("[PASS] ");
        retval = 1; // ok 
    } 
    else 
    { 
        printf("!!! FAIL !!! -> "); 
    }
    printf("%s, %s = 0x%02X [0x%02X]\r\n", device_name, reg_name, i2c_read_data, reg_ref_value);

    return retval;
}

void TestI2C(void)
{
    uint8_t i2c_ref_data;
    uint8_t i2c_read_data;

    HAL_I2C0_Select();

    // BMI160
    i2c_read_data = 0xFF;i2c_ref_data = BMI160_VAL_CHIPID;
    HAL_I2C_Read(BMI160_DEV_ADDR, BMI160_REG_CHIPID, &i2c_read_data, 1);
    if(i2c_read_data == i2c_ref_data) { printf("[PASS] "); } else { printf(">>> FAIL <<< "); }
    printf("BMI160, CHIPID = 0x%02X [0x%02X]\r\n", i2c_read_data, i2c_ref_data);

    i2c_read_data = 0xFF;i2c_ref_data = BMI160_VAL_GYRO_CONFIG;
    HAL_I2C_Read(BMI160_DEV_ADDR, BMI160_REG_GYRO_CONFIG, &i2c_read_data, 1);
    if(i2c_read_data == i2c_ref_data) { printf("[PASS] "); } else { printf(">>> FAIL <<< "); }
    printf("BMI160, GYRO_CONFIG = 0x%02X [0x%02X]\r\n", i2c_read_data, i2c_ref_data);

    // BMX160
    i2c_read_data = 0xFF;i2c_ref_data = BMX160_VAL_CHIPID;
    HAL_I2C_Read(BMX160_DEV_ADDR, BMX160_REG_CHIPID, &i2c_read_data, 1);
    if(i2c_read_data == i2c_ref_data) { printf("[PASS] "); } else { printf(">>> FAIL <<< "); }
    printf("BMX160, CHIPID = 0x%02X [0x%02X]\r\n", i2c_read_data, i2c_ref_data);
    
    i2c_read_data = 0xFF;i2c_ref_data = BMX160_VAL_GYRO_CONFIG;
    HAL_I2C_Read(BMX160_DEV_ADDR, BMX160_REG_GYRO_CONFIG, &i2c_read_data, 1);
    if(i2c_read_data == i2c_ref_data) { printf("[PASS] "); } else { printf(">>> FAIL <<< "); }
    printf("BMX160, GYRO_CONFIG = 0x%02X [0x%02X]\r\n", i2c_read_data, i2c_ref_data);
    
    // DPS310
    i2c_read_data = 0xFF;i2c_ref_data = DPS310_VAL_REVID;
    HAL_I2C_Read(DPS310_DEV_ADDR, DPS310_REG_REVID, &i2c_read_data, 1);
    if(i2c_read_data == i2c_ref_data) { printf("[PASS] "); } else { printf(">>> FAIL <<< "); }
    printf("DPS310, REVID = 0x%02X [0x%02X]\r\n", i2c_read_data, i2c_ref_data);

    i2c_read_data = 0xFF;i2c_ref_data = DPS310_VAL_MEASCFG;
    HAL_I2C_Read(DPS310_DEV_ADDR, DPS310_REG_MEASCFG, &i2c_read_data, 1);
    if(i2c_read_data == i2c_ref_data) { printf("[PASS] "); } else { printf(">>> FAIL <<< "); }
    printf("DPS310, CONFIG = 0x%02X [0x%02X]\r\n", i2c_read_data, i2c_ref_data);

    HAL_I2C1_Select();

    // BME280
    i2c_read_data = 0xFF;i2c_ref_data = BME280_VAL_CHIPID;
    HAL_I2C_Read(BME280_DEV_ADDR,BME280_REG_CHIPID, &i2c_read_data, 1);
    if(i2c_read_data == i2c_ref_data) { printf("[PASS] "); } else { printf(">>> FAIL <<< "); }
    printf("BME280, CHIPID = 0x%02X [0x%02X]\r\n", i2c_read_data, i2c_ref_data);    
    
    i2c_read_data = 0xFF;i2c_ref_data = BME280_VAL_RESET;
    HAL_I2C_Read(BME280_DEV_ADDR, BME280_REG_RESET, &i2c_read_data, 1);
    if(i2c_read_data == i2c_ref_data) { printf("[PASS] "); } else { printf(">>> FAIL <<< "); }
    printf("BME280, RESET = 0x%02X [0x%02X]\r\n", i2c_read_data, i2c_ref_data);


    // BNO055
    i2c_read_data = 0xFF;i2c_ref_data = BNO055_VAL_CHIPID;
    HAL_I2C_Read(BNO055_DEV_ADDR,BNO055_REG_CHIPID, &i2c_read_data, 1);
    if(i2c_read_data == i2c_ref_data) { printf("[PASS] "); } else { printf(">>> FAIL <<< "); }
    printf("BNO055, CHIPID = 0x%02X [0x%02X]\r\n", i2c_read_data, i2c_ref_data);
    
    i2c_read_data = 0xFF;i2c_ref_data = BNO055_VAL_ACCID;
    HAL_I2C_Read(BNO055_DEV_ADDR, BNO055_REG_ACCID, &i2c_read_data, 1);
    if(i2c_read_data == i2c_ref_data) { printf("[PASS] "); } else { printf(">>> FAIL <<< "); }
    printf("BNO055, ACCID = 0x%02X [0x%02X]\r\n", i2c_read_data, i2c_ref_data);
    
    i2c_read_data = 0xFF;i2c_ref_data = BNO055_VAL_MAGID;
    HAL_I2C_Read(BNO055_DEV_ADDR, BNO055_REG_MAGID, &i2c_read_data, 1);
    if(i2c_read_data == i2c_ref_data) { printf("[PASS] "); } else { printf(">>> FAIL <<< "); }
    printf("BNO055, MAGID = 0x%02X [0x%02X]\r\n", i2c_read_data, i2c_ref_data);
    
    i2c_read_data = 0xFF;i2c_ref_data = BNO055_VAL_GYRID;
    HAL_I2C_Read(BNO055_DEV_ADDR, BNO055_REG_GYRID, &i2c_read_data, 1);
    if(i2c_read_data == i2c_ref_data) { printf("[PASS] "); } else { printf(">>> FAIL <<< "); }
    printf("BNO055, GYRID = 0x%02X [0x%02X]\r\n", i2c_read_data, i2c_ref_data);
    
    i2c_read_data = 0xFF;i2c_ref_data = BNO055_VAL_SWREVIDLSB;
    HAL_I2C_Read(BNO055_DEV_ADDR, BNO055_REG_SWREVIDLSB, &i2c_read_data, 1);
    if(i2c_read_data == i2c_ref_data) { printf("[PASS] "); } else { printf(">>> FAIL <<< "); }
    printf("BNO055, ACCID = 0x%02X [0x%02X]\r\n", i2c_read_data, i2c_ref_data);
    
    i2c_read_data = 0xFF;i2c_ref_data = BNO055_VAL_SWREVIDMSB;
    HAL_I2C_Read(BNO055_DEV_ADDR, BNO055_REG_SWREVIDMSB, &i2c_read_data, 1);
    if(i2c_read_data == i2c_ref_data) { printf("[PASS] "); } else { printf(">>> FAIL <<< "); }
    printf("BNO055, ACCID = 0x%02X [0x%02X]\r\n", i2c_read_data, i2c_ref_data);
}
