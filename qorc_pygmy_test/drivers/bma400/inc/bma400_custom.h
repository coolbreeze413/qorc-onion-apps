#ifndef __DPS310_H__
#define __DPS310_H__

#include <stdint.h>


// device address
#define BMA400_DEV_ADDR                         0x14 // 0x15 if SDO HIGH

// registers
#define BMA400_REG_CHIPID                       0x00
#define BMA400_REG_ERR_REG                      0x02
#define BMA400_REG_STATUS                       0x03

#define BMA400_REG_ACC_X_LSB                    0x04
#define BMA400_REG_ACC_X_MSB                    0x05
#define BMA400_REG_ACC_Y_LSB                    0x06
#define BMA400_REG_ACC_Y_MSB                    0x07
#define BMA400_REG_ACC_Z_LSB                    0x08
#define BMA400_REG_ACC_Z_MSB                    0x09

#define BMA400_REG_SENSOR_TIME_0                0x0A
#define BMA400_REG_SENSOR_TIME_1                0x0B
#define BMA400_REG_SENSOR_TIME_2                0x0C

#define BMA400_REG_EVENT                        0x0D

#define BMA400_REG_INT_STAT0                    0x0E
#define BMA400_REG_INT_STAT1                    0x0F
#define BMA400_REG_INT_STAT2                    0x10

#define BMA400_REG_TEMP_DATA                    0x11

#define BMA400_REG_FIFO_LENGTH_0                0x12
#define BMA400_REG_FIFO_LENGTH_1                0x13
#define BMA400_REG_FIFO_DATA                    0x14

#define BMA400_REG_STEP_CNT_0                   0x15
#define BMA400_REG_STEP_CNT_1                   0x16
#define BMA400_REG_STEP_CNT_2                   0x17
#define BMA400_REG_STEP_STAT                    0x18

#define BMA400_REG_ACC_CONFIG_0                 0x19
#define BMA400_REG_ACC_CONFIG_1                 0x1A
#define BMA400_REG_ACC_CONFIG_2                 0x1B

#define BMA400_REG_INT_CONFIG_0                 0x1F
#define BMA400_REG_INT_CONFIG_1                 0x20
#define BMA400_REG_INT_1_MAP                    0x21
#define BMA400_REG_INT_2_MAP                    0x22
#define BMA400_REG_INT_1_2_MAP                  0x23
#define BMA400_REG_INT_1_2_CTRL                 0x24

#define BMA400_REG_FIFO_CONFIG_0                0x26
#define BMA400_REG_FIFO_CONFIG_1                0x27
#define BMA400_REG_FIFO_CONFIG_2                0x28
#define BMA400_REG_FIFO_PWR_CONFIG              0x29

#define BMA400_REG_AUTO_LOW_POW_0               0x2A
#define BMA400_REG_AUTO_LOW_POW_1               0x2B
#define BMA400_REG_AUTO_WAKE_UP_0               0x2C
#define BMA400_REG_AUTO_WAKE_UP_1               0x2D
#define BMA400_REG_WAKE_UP_INT_CONFIG_0         0x2F
#define BMA400_REG_WAKE_UP_INT_CONFIG_1         0x30
#define BMA400_REG_WAKE_UP_INT_CONFIG_2         0x31
#define BMA400_REG_WAKE_UP_INT_CONFIG_3         0x32
#define BMA400_REG_WAKE_UP_INT_CONFIG_4         0x33

#define BMA400_REG_ORIENTCH_CONFIG_0            0x35
#define BMA400_REG_ORIENTCH_CONFIG_1            0x36
#define BMA400_REG_ORIENTCH_CONFIG_2            0x37
#define BMA400_REG_ORIENTCH_CONFIG_3            0x38
#define BMA400_REG_ORIENTCH_CONFIG_4            0x39
#define BMA400_REG_ORIENTCH_CONFIG_5            0x3A
#define BMA400_REG_ORIENTCH_CONFIG_6            0x3B
#define BMA400_REG_ORIENTCH_CONFIG_7            0x3C
#define BMA400_REG_ORIENTCH_CONFIG_8            0x3D
#define BMA400_REG_ORIENTCH_CONFIG_9            0x3E

#define BMA400_REG_GEN_1_INT_CONFIG_0           0x3F
#define BMA400_REG_GEN_1_INT_CONFIG_1           0x40
#define BMA400_REG_GEN_1_INT_CONFIG_2           0x41
#define BMA400_REG_GEN_1_INT_CONFIG_3           0x42
#define BMA400_REG_GEN_1_INT_CONFIG_3_1         0x43
#define BMA400_REG_GEN_1_INT_CONFIG_4           0x44
#define BMA400_REG_GEN_1_INT_CONFIG_5           0x45
#define BMA400_REG_GEN_1_INT_CONFIG_6           0x46
#define BMA400_REG_GEN_1_INT_CONFIG_7           0x47
#define BMA400_REG_GEN_1_INT_CONFIG_8           0x48
#define BMA400_REG_GEN_1_INT_CONFIG_9           0x49

#define BMA400_REG_GEN_2_INT_CONFIG_0           0x4A
#define BMA400_REG_GEN_2_INT_CONFIG_1           0x4B
#define BMA400_REG_GEN_2_INT_CONFIG_2           0x4C
#define BMA400_REG_GEN_2_INT_CONFIG_3           0x4D
#define BMA400_REG_GEN_2_INT_CONFIG_3_1         0x4E
#define BMA400_REG_GEN_2_INT_CONFIG_4           0x4F
#define BMA400_REG_GEN_2_INT_CONFIG_5           0x50
#define BMA400_REG_GEN_2_INT_CONFIG_6           0x51
#define BMA400_REG_GEN_2_INT_CONFIG_7           0x52
#define BMA400_REG_GEN_2_INT_CONFIG_8           0x53
#define BMA400_REG_GEN_2_INT_CONFIG_9           0x54

#define BMA400_REG_ACT_CH_CONFIG_0              0x55
#define BMA400_REG_ACT_CH_CONFIG_1              0x56

#define BMA400_REG_TAP_CONFIG_0                 0x57
#define BMA400_REG_TAP_CONFIG_1                 0x58

#define BMA400_REG_STEP_COUNTER_CONFIG_0        0x59
#define BMA400_REG_STEP_COUNTER_CONFIG_1        0x5A
#define BMA400_REG_STEP_COUNTER_CONFIG_2        0x5B
#define BMA400_REG_STEP_COUNTER_CONFIG_3        0x5C
#define BMA400_REG_STEP_COUNTER_CONFIG_4        0x5D
#define BMA400_REG_STEP_COUNTER_CONFIG_5        0x5E
#define BMA400_REG_STEP_COUNTER_CONFIG_6        0x5F
#define BMA400_REG_STEP_COUNTER_CONFIG_7        0x60
#define BMA400_REG_STEP_COUNTER_CONFIG_8        0x61
#define BMA400_REG_STEP_COUNTER_CONFIG_9        0x62
#define BMA400_REG_STEP_COUNTER_CONFIG_10       0x63
#define BMA400_REG_STEP_COUNTER_CONFIG_11       0x64
#define BMA400_REG_STEP_COUNTER_CONFIG_12       0x65
#define BMA400_REG_STEP_COUNTER_CONFIG_13       0x66
#define BMA400_REG_STEP_COUNTER_CONFIG_14       0x67
#define BMA400_REG_STEP_COUNTER_CONFIG_15       0x68
#define BMA400_REG_STEP_COUNTER_CONFIG_16       0x69
#define BMA400_REG_STEP_COUNTER_CONFIG_17       0x6A
#define BMA400_REG_STEP_COUNTER_CONFIG_18       0x6B
#define BMA400_REG_STEP_COUNTER_CONFIG_19       0x6C
#define BMA400_REG_STEP_COUNTER_CONFIG_20       0x6D
#define BMA400_REG_STEP_COUNTER_CONFIG_21       0x6E
#define BMA400_REG_STEP_COUNTER_CONFIG_22       0x6F
#define BMA400_REG_STEP_COUNTER_CONFIG_23       0x70
#define BMA400_REG_STEP_COUNTER_CONFIG_24       0x71

#define BMA400_REG_IF_CONF                      0x7C
#define BMA400_REG_SELF_TEST                    0x7D
#define BMA400_REG_CMD                          0x7E


// default or common values
#define BMA400_VAL_CHIPID                       0x90
#define BMA400_VAL_SOFT_RESET_CMD               0xB6
#define BMA400_VAL_FIFO_FLUSH_CMD               0xB6
#define BMA400_VAL_SOFT_RESET_DELAY_MS          5


// power mode
#define BMA400_VAL_ACCEL_SUSPEND_MODE           0x10
#define BMA400_VAL_ACCEL_NORMAL_MODE            0x11
#define BMA400_VAL_ACCEL_LOWPOWER_MODE          0x12

// range
#define BMA400_VAL_ACCEL_RANGE_2G               0x03
#define BMA400_VAL_ACCEL_RANGE_4G               0x05
#define BMA400_VAL_ACCEL_RANGE_8G               0x08
#define BMA400_VAL_ACCEL_RANGE_16G              0x0C

// range = 2G, so 16-bit value = -2G to +2G
// 15 bits = 32767 = range = 2G
// val = ?
// val/32767 = conv_mg/2000mg
// conv_mg = (val /32767) * 2 * 1000 mg
// conv_mg = val * (axisrange) * 1000 * (1/32767)
// scale factor per LSB according to range ((axisrange * 1000)/32767)mg
#define BMA400_ACCEL_SCALE_2G_mg                0.06103702F
#define BMA400_ACCEL_SCALE_4G_mg                0.12207403F
#define BMA400_ACCEL_SCALE_8G_mg                0.24414807F
#define BMA400_ACCEL_SCALE_16G_mg               0.48829615F

// ODR
#define BMA400_VAL_ACCEL_ODR_RESERVED           0x00
#define BMA400_VAL_ACCEL_ODR_0_78HZ             0x01
#define BMA400_VAL_ACCEL_ODR_1_56HZ             0x02
#define BMA400_VAL_ACCEL_ODR_3_12HZ             0x03
#define BMA400_VAL_ACCEL_ODR_6_25HZ             0x04
#define BMA400_VAL_ACCEL_ODR_12_5HZ             0x05
#define BMA400_VAL_ACCEL_ODR_25HZ               0x06
#define BMA400_VAL_ACCEL_ODR_50HZ               0x07
#define BMA400_VAL_ACCEL_ODR_100HZ              0x08
#define BMA400_VAL_ACCEL_ODR_200HZ              0x09
#define BMA400_VAL_ACCEL_ODR_400HZ              0x0A
#define BMA400_VAL_ACCEL_ODR_800HZ              0x0B
#define BMA400_VAL_ACCEL_ODR_1600HZ             0x0C
#define BMA400_VAL_ACCEL_ODR_RESERVED0          0x0D
#define BMA400_VAL_ACCEL_ODR_RESERVED1          0x0E
#define BMA400_VAL_ACCEL_ODR_RESERVED2          0x0F

// bandwidth
#define BMA400_VAL_ACCEL_BW_OSR4_AVG1           0x00
#define BMA400_VAL_ACCEL_BW_OSR2_AVG2           0x01
#define BMA400_VAL_ACCEL_BW_NORMAL_AVG4         0x02
#define BMA400_VAL_ACCEL_BW_RES_AVG8            0x03
#define BMA400_VAL_ACCEL_BW_RES_AVG16           0x04
#define BMA400_VAL_ACCEL_BW_RES_AVG32           0x05
#define BMA400_VAL_ACCEL_BW_RES_AVG64           0x06
#define BMA400_VAL_ACCEL_BW_RES_AVG128          0x07



#endif // #ifndef __DPS310_H__