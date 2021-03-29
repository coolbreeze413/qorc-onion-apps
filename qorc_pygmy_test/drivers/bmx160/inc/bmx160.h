#ifndef __BMX160_H__
#define __BMX160_H__


// references:
// https://github.com/BoschSensortec/BMI160_driver
// https://github.com/BoschSensortec/BMI160_driver/wiki/How-to-use-an-auxiliary-sensor-or-magnetometer-with-the-BMI160.
// https://github.com/BoschSensortec/BMM150-Sensor-API
// https://github.com/DFRobot/DFRobot_BMX160
// https://github.com/drcpattison/BMX160
// https://community.bosch-sensortec.com/t5/MEMS-sensors-forum/BMX160-Magnetometer-data-not-changing/td-p/9801/page/3


// device address
#define BMX160_DEV_ADDR                         0x68

// registers
#define BMX160_REG_CHIPID                       0x00
#define BMX160_REG_ERR_REG                      0x02
#define BMX160_REG_PMU_STATUS                   0x03
#define BMX160_REG_DATA                         0x04    // (6+2+6+6) = 20B
#define BMX160_REG_SENSORTIME                   0x18    // 3B
#define BMX160_REG_STATUS                       0x1B
#define BMX160_REG_INT_STATUS                   0x1C    // 4B
#define BMX160_REG_FOC_CONF                     0x69
#define BMX160_REG_CONF                         0x6A
#define BMX160_REG_IF_CONF                      0x6B
#define BMX160_REG_PMU_TRIGGER                  0x6C
#define BMX160_REG_CMD                          0x7E


// accel
#define BMX160_REG_ACC_CONF                     0x40
#define BMX160_REG_ACC_RANGE                    0x41
#define BMX160_REG_DATA_ACC                     0x12    // 6B

// gyro
#define BMX160_REG_GYRO_CONF                    0x42
#define BMX160_REG_GYRO_RANGE                   0x43
#define BMX160_REG_DATA_GYRO                    0x0C    // 6B

// mag
#define BMX160_REG_MAG_CONF                     0x44
#define BMX160_REG_MAG_IF_0                     0x4C    // 4B
#define BMX160_REG_MAG_IF_1                     0x4D
#define BMX160_REG_MAG_IF_2                     0x4E
#define BMX160_REG_MAG_IF_3                     0x4F
#define BMX160_REG_DATA_MAG                     0x04    // 6B
#define BMX160_REG_DATA_RHALL                   0x0A    // 2B

// temperature (of sensor)
#define BMX160_REG_TEMPERATURE                  0x20    // 2B


// default or common values
#define BMX160_VAL_CHIPID                       0xD8
#define BMX160_VAL_SOFT_RESET_CMD               0xB6
#define BMX160_VAL_SOFT_RESET_DELAY_MS          15


// power mode
#define BMX160_VAL_ACCEL_SUSPEND_MODE           0x10
#define BMX160_VAL_ACCEL_NORMAL_MODE            0x11
#define BMX160_VAL_ACCEL_LOWPOWER_MODE          0x12

#define BMX160_VAL_GYRO_SUSPEND_MODE            0x14
#define BMX160_VAL_GYRO_NORMAL_MODE             0x15
#define BMX160_VAL_GYRO_FASTSTARTUP_MODE        0x17

#define BMX160_VAL_MAG_SUSPEND_MODE             0x18
#define BMX160_VAL_MAG_NORMAL_MODE              0x19
#define BMX160_VAL_MAG_LOWPOWER_MODE            0x1A


// range
#define BMX160_VAL_ACCEL_RANGE_2G               0x03
#define BMX160_VAL_ACCEL_RANGE_4G               0x05
#define BMX160_VAL_ACCEL_RANGE_8G               0x08
#define BMX160_VAL_ACCEL_RANGE_16G              0x0C

#define BMX160_VAL_GYRO_RANGE_2000_DPS          0x00
#define BMX160_VAL_GYRO_RANGE_1000_DPS          0x01
#define BMX160_VAL_GYRO_RANGE_500_DPS           0x02
#define BMX160_VAL_GYRO_RANGE_250_DPS           0x03
#define BMX160_VAL_GYRO_RANGE_125_DPS           0x04

// range = 2G, so 16-bit value = -2G to +2G
// 15 bits = 32767 = range = 2G
// val = ?
// val/32767 = conv_mg/2000mg
// conv_mg = (val /32767) * 2 * 1000 mg
// conv_mg = val * (axisrange) * 1000 * (1/32767)
// scale factor per LSB according to range ((axisrange * 1000)/32767)mg
#define BMX160_ACCEL_SCALE_2G_mg                0.06103702F
#define BMX160_ACCEL_SCALE_4G_mg                0.12207403F
#define BMX160_ACCEL_SCALE_8G_mg                0.24414807F
#define BMX160_ACCEL_SCALE_16G_mg               0.48829615F

// (axisrange/32767 dps)
#define BMX160_GYRO_SCALE_125DPS_dps            0.0038110F
#define BMX160_GYRO_SCALE_250DPS_dps            0.0076220F
#define BMX160_GYRO_SCALE_500DPS_dps            0.0152439F
#define BMX160_GYRO_SCALE_1000DPS_dps           0.0304878F
#define BMX160_GYRO_SCALE_2000DPS_dps           0.0609756F

#define BMX160_MAG_SCALE_ALL_uT                 0.3F        

#define BMX160_SENSORTIME_SCALE_us              39

#define BMX160_TEMPERATURE_SCALE_degC           0.001953125F

// ODR
#define BMX160_VAL_ACCEL_ODR_RESERVED           0x00
#define BMX160_VAL_ACCEL_ODR_0_78HZ             0x01
#define BMX160_VAL_ACCEL_ODR_1_56HZ             0x02
#define BMX160_VAL_ACCEL_ODR_3_12HZ             0x03
#define BMX160_VAL_ACCEL_ODR_6_25HZ             0x04
#define BMX160_VAL_ACCEL_ODR_12_5HZ             0x05
#define BMX160_VAL_ACCEL_ODR_25HZ               0x06
#define BMX160_VAL_ACCEL_ODR_50HZ               0x07
#define BMX160_VAL_ACCEL_ODR_100HZ              0x08
#define BMX160_VAL_ACCEL_ODR_200HZ              0x09
#define BMX160_VAL_ACCEL_ODR_400HZ              0x0A
#define BMX160_VAL_ACCEL_ODR_800HZ              0x0B
#define BMX160_VAL_ACCEL_ODR_1600HZ             0x0C
#define BMX160_VAL_ACCEL_ODR_RESERVED0          0x0D
#define BMX160_VAL_ACCEL_ODR_RESERVED1          0x0E
#define BMX160_VAL_ACCEL_ODR_RESERVED2          0x0F

#define BMX160_VAL_GYRO_ODR_RESERVED            0x00
#define BMX160_VAL_GYRO_ODR_25HZ                0x06
#define BMX160_VAL_GYRO_ODR_50HZ                0x07
#define BMX160_VAL_GYRO_ODR_100HZ               0x08
#define BMX160_VAL_GYRO_ODR_200HZ               0x09
#define BMX160_VAL_GYRO_ODR_400HZ               0x0A
#define BMX160_VAL_GYRO_ODR_800HZ               0x0B
#define BMX160_VAL_GYRO_ODR_1600HZ              0x0C
#define BMX160_VAL_GYRO_ODR_3200HZ              0x0D

#define BMX160_VAL_MAG_ODR_RESERVED             0x00
#define BMX160_VAL_MAG_ODR_0_78HZ               0x01
#define BMX160_VAL_MAG_ODR_1_56HZ               0x02
#define BMX160_VAL_MAG_ODR_3_12HZ               0x03
#define BMX160_VAL_MAG_ODR_6_25HZ               0x04
#define BMX160_VAL_MAG_ODR_12_5HZ               0x05
#define BMX160_VAL_MAG_ODR_25HZ                 0x06
#define BMX160_VAL_MAG_ODR_50HZ                 0x07
#define BMX160_VAL_MAG_ODR_100HZ                0x08
#define BMX160_VAL_MAG_ODR_200HZ                0x09
#define BMX160_VAL_MAG_ODR_400HZ                0x0A
#define BMX160_VAL_MAG_ODR_800HZ                0x0B


// bandwidth
#define BMX160_VAL_ACCEL_BW_OSR4_AVG1           0x00
#define BMX160_VAL_ACCEL_BW_OSR2_AVG2           0x01
#define BMX160_VAL_ACCEL_BW_NORMAL_AVG4         0x02
#define BMX160_VAL_ACCEL_BW_RES_AVG8            0x03
#define BMX160_VAL_ACCEL_BW_RES_AVG16           0x04
#define BMX160_VAL_ACCEL_BW_RES_AVG32           0x05
#define BMX160_VAL_ACCEL_BW_RES_AVG64           0x06
#define BMX160_VAL_ACCEL_BW_RES_AVG128          0x07

#define BMX160_VAL_GYRO_BW_OSR4_MODE            0x00
#define BMX160_VAL_GYRO_BW_OSR2_MODE            0x01
#define BMX160_VAL_GYRO_BW_NORMAL_MODE          0x02


// offset (fast offset calc)
#define BMX160_VAL_ACCEL_FOC_DISABLED         0x00
#define BMX160_VAL_ACCEL_FOC_POSITIVE_G       0x01
#define BMX160_VAL_ACCEL_FOC_NEGATIVE_G       0x02
#define BMX160_VAL_ACCEL_FOC_0G               0x03


// we need some structure(s)!
// configuration:


struct bmx160_config {
  
    uint8_t accel_en;
    uint16_t accel_power_mode;
    uint16_t accel_range;
    double accel_scale;
    uint16_t accel_odr;
    uint16_t accel_bw;

    uint8_t gyro_en;
    uint16_t gyro_power_mode;
    uint16_t gyro_range;
    double gyro_scale;
    uint16_t gyro_odr;
    uint16_t gyro_bw;

    uint8_t mag_en;
    uint16_t mag_power_mode;
    //uint16_t mag_range; [fixed]
    uint16_t mag_odr;
    //uint16_t mag_bw;[fixed]  
  
};



// data:
struct bmx160_sensor_data {
  
    int16_t xr;
    int16_t yr;
    int16_t zr;
    //int16_t filler; // 4B align?
  
    double x;
    double y;
    double z;
  
    uint32_t ts;
  
};

struct bmx160_rhall_data {
    
    int16_t rhallr;
    
};

struct bmx160_temperature_data {
  
    int16_t tempr;
    double temp;
  
};

uint8_t bmx160_probe();
uint8_t bmx160_soft_reset();
uint8_t bmx160_wakeup(uint8_t accel_wakeup, uint8_t gyro_wakeup, uint8_t mag_wakeup);
uint8_t bmx160_suspend(uint8_t accel_suspend, uint8_t gyro_suspend, uint8_t mag_suspend);
uint8_t bmx160_set_accel_config(uint16_t power_mode, uint16_t range, uint16_t odr, uint16_t bw);
uint8_t bmx160_set_gyro_config(uint16_t power_mode, uint16_t range, uint16_t odr, uint16_t bw);
uint8_t bmx160_set_mag_config(uint16_t odr);
void bmx160_read_all_data(struct bmx160_sensor_data* accel_data, 
                          struct bmx160_sensor_data* gyro_data, 
                          struct bmx160_sensor_data* mag_data,
                          struct bmx160_rhall_data* rhall_data,
                          struct bmx160_temperature_data* temperature_data);
double get_accel_scale(uint16_t accel_range);
double get_gyro_scale(uint16_t gyro_range);



uint8_t bmx160_basic_test(uint32_t num_samples, uint32_t period_ms);

#endif // #ifndef __BMX160_H__