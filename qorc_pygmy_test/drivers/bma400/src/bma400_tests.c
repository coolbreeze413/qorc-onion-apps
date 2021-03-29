
#include <stdint.h>
#include <stdio.h>

#include "Fw_global_config.h"

#include "eoss3_hal_i2c.h"
#include "eoss3_hal_timer.h"

#include "bma400.h"
#include "QL_Trace.h"

#define GRAVITY_EARTH (9.80665f) /* Earth's gravity in m/s^2 */

void set_interface(enum bma400_intf intf, struct bma400_dev *dev);
void delay_ms(uint32_t period);
int8_t i2c_reg_write(uint8_t i2c_addr, uint8_t reg_addr, uint8_t *reg_data, uint16_t length);
int8_t i2c_reg_read(uint8_t i2c_addr, uint8_t reg_addr, uint8_t *reg_data, uint16_t length);
int8_t spi_reg_write(void *intf_ptr, uint8_t cs, uint8_t reg_addr, uint8_t *reg_data, uint16_t length);
int8_t spi_reg_read(void *intf_ptr, uint8_t cs, uint8_t reg_addr, uint8_t *reg_data, uint16_t length);
void print_rslt(int8_t rslt);
float lsb_to_ms2(int16_t val, float g_range, uint8_t bit_width);
float sensor_ticks_to_s(uint32_t sensor_time);


uint8_t bma400_basic_test()
{ 
    struct bma400_dev bma;
    struct bma400_sensor_conf conf;
    struct bma400_sensor_data data;
    int8_t rslt;
    uint8_t n_samples = 200;
    float t, x, y, z;

    set_interface(BMA400_I2C_INTF, &bma);

    rslt = bma400_init(&bma);
    print_rslt(rslt);

    rslt = bma400_soft_reset(&bma);
    print_rslt(rslt);

    /* Select the type of configuration to be modified */
    conf.type = BMA400_ACCEL;

    /* Get the accelerometer configurations which are set in the sensor */
    rslt = bma400_get_sensor_conf(&conf, 1, &bma);
    print_rslt(rslt);

    /* Modify the desired configurations as per macros
     * available in bma400_defs.h file */
    conf.param.accel.odr = BMA400_ODR_100HZ;
    conf.param.accel.range = BMA400_2G_RANGE;
    conf.param.accel.data_src = BMA400_DATA_SRC_ACCEL_FILT_1;

    /* Set the desired configurations to the sensor */
    rslt = bma400_set_sensor_conf(&conf, 1, &bma);
    print_rslt(rslt);

    rslt = bma400_set_power_mode(BMA400_LOW_POWER_MODE, &bma);
    print_rslt(rslt);

    printf("t[s], Ax[m/s2], Ay[m/s2], Az[m/s2]\r\n");

    while (n_samples && (rslt == BMA400_OK))
    {
        bma.delay_ms(10); /* Wait for 10ms as ODR is set to 100Hz */

        rslt = bma400_get_accel_data(BMA400_DATA_SENSOR_TIME, &data, &bma);
        print_rslt(rslt);

        /* 12-bit accelerometer at range 2G */
        x = lsb_to_ms2(data.x, 2, 12);
        y = lsb_to_ms2(data.y, 2, 12);
        z = lsb_to_ms2(data.z, 2, 12);
        t = sensor_ticks_to_s(data.sensortime);

        printf("%.4f, %.2f, %.2f, %.2f\r\n", t, x, y, z);
        n_samples--;
    }

    return 0;
}



uint8_t bma400_step_counter_test()
{
    struct bma400_dev bma;
    struct bma400_int_enable step_int;
    int8_t rslt;
    uint8_t test_dur = 30;
    uint32_t step_count;
    uint8_t activity;

    set_interface(BMA400_I2C_INTF, &bma);

    rslt = bma400_init(&bma);
    print_rslt(rslt);

    rslt = bma400_soft_reset(&bma);
    print_rslt(rslt);

    step_int.type = BMA400_STEP_COUNTER_INT_EN;
    step_int.conf = BMA400_ENABLE;

    rslt = bma400_enable_interrupt(&step_int, 1, &bma);
    print_rslt(rslt);

    rslt = bma400_set_power_mode(BMA400_NORMAL_MODE, &bma);
    print_rslt(rslt);

    printf("Steps counted, Activity classifier\r\n");

    while (test_dur)
    {
        bma.delay_ms(1000);

        rslt = bma400_get_steps_counted(&step_count, &activity, &bma);
        printf("%ld", step_count);

        switch (activity)
        {
            case BMA400_STILL_ACT:
                printf(", Still\r\n");
                break;
            case BMA400_WALK_ACT:
                printf(", Walking\r\n");
                break;
            case BMA400_RUN_ACT:
                printf(", Running\r\n");
                break;
            default:
                printf(", undefined\r\n");
                break;
        }

        test_dur--;
    }

    return 0;
}


uint8_t bma400_tap_detection_test()
{
    struct bma400_dev bma;
    struct bma400_int_enable tap_int[2];
    struct bma400_sensor_conf conf[2];
    int8_t rslt;
    uint32_t poll_period = 5, test_dur_ms = 30000;
    uint16_t int_status;

    set_interface(BMA400_I2C_INTF, &bma);

    rslt = bma400_init(&bma);
    print_rslt(rslt);

    rslt = bma400_soft_reset(&bma);
    print_rslt(rslt);

    conf[0].type = BMA400_ACCEL;
    conf[1].type = BMA400_TAP_INT;

    rslt = bma400_get_sensor_conf(conf, 2, &bma);
    print_rslt(rslt);

    conf[0].param.accel.odr = BMA400_ODR_200HZ;
    conf[0].param.accel.range = BMA400_4G_RANGE;
    conf[0].param.accel.data_src = BMA400_DATA_SRC_ACCEL_FILT_1;
    conf[0].param.accel.filt1_bw = BMA400_ACCEL_FILT1_BW_1;

    conf[1].param.tap.int_chan = BMA400_UNMAP_INT_PIN;
    conf[1].param.tap.axes_sel = BMA400_Z_AXIS_EN_TAP;
    conf[1].param.tap.sensitivity = BMA400_TAP_SENSITIVITY_0;

    rslt = bma400_set_sensor_conf(conf, 2, &bma);
    print_rslt(rslt);

    bma.delay_ms(100);

    tap_int[0].type = BMA400_SINGLE_TAP_INT_EN;
    tap_int[0].conf = BMA400_ENABLE;

    tap_int[1].type = BMA400_DOUBLE_TAP_INT_EN;
    tap_int[1].conf = BMA400_ENABLE;

    rslt = bma400_enable_interrupt(tap_int, 2, &bma);
    print_rslt(rslt);

    bma.delay_ms(100);

    rslt = bma400_set_power_mode(BMA400_NORMAL_MODE, &bma);
    print_rslt(rslt);

    bma.delay_ms(100);

    if (rslt == BMA400_OK)
    {
        printf("Tap configured.\r\n");

        while (test_dur_ms)
        {
            bma.delay_ms(poll_period);

            rslt = bma400_get_interrupt_status(&int_status, &bma);
            print_rslt(rslt);

            if (int_status & BMA400_S_TAP_INT_ASSERTED)
            {
                printf("Single tap detected!\r\n");
            }

            if (int_status & BMA400_D_TAP_INT_ASSERTED)
            {
                printf("Double tap detected!\r\n");
            }

            test_dur_ms -= poll_period;
        }

    }

    return 0;
}


uint8_t bma400_combo_1_test()
{
    // accel data -> filt1 or TAP -> filt1
    // filt2 for rest of the interrupt engine events
    
    // accel data is enabled -> filt_1 only. (others are limited to 100Hz)
    // tap detection is enabled (double tap only) -> filt_1 at 200Hz only
    // activity - filt1/filt2 + thresholds, separate from stepdet/stepcount!
    // step detect, step count -> which source is used? or independent mostly.
    // orientation change -> filt2 or filt_lp
    // generic int1 + int 2: anymotion/nomotion     
    // ensure the interrupt engine is not overrun! another interrupt here.
    // wakeup interrupt?
  
    // bma400_get_sensor_conf by specifying the types
    // modify settings, bma400_set_sensor_conf
    // bma400_enable_interrupt
    // bma400_set_power_mode
    struct bma400_dev bma;
    struct bma400_int_enable latch_int;    
    struct bma400_int_enable tap_int[1];
    struct bma400_int_enable step_int;
    struct bma400_int_enable gen1_int;
    struct bma400_int_enable orient_int;
    
    struct bma400_sensor_conf conf[7];
    int8_t rslt;
    uint32_t poll_period = 500, test_dur_ms = 1800000;
    uint16_t int_status;
    uint32_t step_count;
    uint8_t activity;
    struct bma400_sensor_data data;
    float t, x, y, z;


    set_interface(BMA400_I2C_INTF, &bma);

    rslt = bma400_init(&bma);
    print_rslt(rslt);

    rslt = bma400_soft_reset(&bma);
    print_rslt(rslt);

    conf[0].type = BMA400_ACCEL;
    conf[1].type = BMA400_TAP_INT;
    conf[2].type = BMA400_STEP_COUNTER_INT;
    conf[3].type = BMA400_GEN1_INT; // activity detection in this case
    conf[4].type = BMA400_ORIENT_CHANGE_INT;
    //conf[2].type = BMA400_ACTIVITY_CHANGE_INT;
    
    //conf[4].type = BMA400_GEN2_INT;
    

    rslt = bma400_get_sensor_conf(conf, 5, &bma);
    print_rslt(rslt);

    conf[0].param.accel.odr = BMA400_ODR_200HZ;
    conf[0].param.accel.range = BMA400_4G_RANGE;
    conf[0].param.accel.data_src = BMA400_DATA_SRC_ACCEL_FILT_1;
    conf[0].param.accel.filt1_bw = BMA400_ACCEL_FILT1_BW_1;

    conf[1].param.tap.int_chan = BMA400_UNMAP_INT_PIN; // no interrupt pin mapped
    conf[1].param.tap.axes_sel = BMA400_Z_AXIS_EN_TAP;
    conf[1].param.tap.sensitivity = BMA400_TAP_SENSITIVITY_0;
    
    conf[2].param.step_cnt.int_chan = BMA400_UNMAP_INT_PIN; // no interrupt pin mapped
    
    conf[3].param.gen_int.int_chan = BMA400_UNMAP_INT_PIN; // no interrupt pin mapped
    // enable X,Y,Z axis. data src = acc_filt_2(100Hz), reference update = every time. hysterisis = 48mg
    conf[3].param.gen_int.axes_sel = BMA400_XYZ_AXIS_EN;
    conf[3].param.gen_int.data_src = BMA400_DATA_SRC_ACC_FILT2;
    conf[3].param.gen_int.ref_update = BMA400_EVERY_TIME_UPDATE;
    conf[3].param.gen_int.hysteresis = BMA400_HYST_48_MG;
    
    // configure criterion to "inactivity" and combination of axes = AND
    conf[3].param.gen_int.criterion_sel = BMA400_ACTIVITY_INT;//BMA400_INACTIVITY_INT;
    conf[3].param.gen_int.evaluate_axes = BMA400_ANY_AXES_INT;//BMA400_ALL_AXES_INT;
    
    // set threshold to 8mg/LSB
    conf[3].param.gen_int.gen_int_thres = 0x5; // ??
    
    // set duration to 15 odr ticks
    conf[3].param.gen_int.gen_int_dur = 0x000f;
    
    
    conf[4].param.orient.int_chan = BMA400_UNMAP_INT_PIN; // no interrupt pin mapped
    conf[4].param.orient.axes_sel = BMA400_X_AXIS_EN;
    conf[4].param.orient.data_src = BMA400_DATA_SRC_ACC_FILT2;
    conf[4].param.orient.ref_update = BMA400_ORIENT_REFU_ACC_FILT_2;
    conf[4].param.orient.stability_mode = BMA400_STABILITY_ACC_FILT_2;
      
    conf[4].param.orient.orient_thres = 0x5;
      
    conf[4].param.orient.stability_thres = 0x5;
      
    conf[4].param.orient.orient_int_dur = 0x5;
    

    rslt = bma400_set_sensor_conf(conf, 5, &bma);
    print_rslt(rslt);

    bma.delay_ms(100);

    
    //tap_int[0].type = BMA400_SINGLE_TAP_INT_EN;
    //tap_int[0].conf = BMA400_ENABLE;

    tap_int[0].type = BMA400_DOUBLE_TAP_INT_EN;
    tap_int[0].conf = BMA400_ENABLE;

    rslt = bma400_enable_interrupt(tap_int, 1, &bma);
    print_rslt(rslt);
    
    
    step_int.type = BMA400_STEP_COUNTER_INT_EN;
    step_int.conf = BMA400_ENABLE;

    rslt = bma400_enable_interrupt(&step_int, 1, &bma);
    print_rslt(rslt);
    
    
    gen1_int.type = BMA400_GEN1_INT_EN;
    gen1_int.conf = BMA400_ENABLE;

    rslt = bma400_enable_interrupt(&gen1_int, 1, &bma);
    print_rslt(rslt);
    
    
    orient_int.type = BMA400_ORIENT_CHANGE_INT_EN;
    orient_int.conf = BMA400_ENABLE;

    rslt = bma400_enable_interrupt(&orient_int, 1, &bma);
    print_rslt(rslt);
    
    
    // latch everything, as we are polling.
    latch_int.type = BMA400_LATCH_INT_EN;
    latch_int.conf = BMA400_ENABLE;

    rslt = bma400_enable_interrupt(&latch_int, 1, &bma);
    print_rslt(rslt);
    

    bma.delay_ms(100);

    rslt = bma400_set_power_mode(BMA400_NORMAL_MODE, &bma);
    print_rslt(rslt);

    bma.delay_ms(100);

    if (rslt == BMA400_OK)
    {
        printf("Step, Tap, Orient, Activity, Accel configured.\r\n");

        while (test_dur_ms)
        {
            bma.delay_ms(poll_period);

            rslt = bma400_get_interrupt_status(&int_status, &bma);
            print_rslt(rslt);

            //if (int_status & BMA400_S_TAP_INT_ASSERTED)
            //{
            //    printf("Single tap detected!\r\n");
            //}

            if (int_status & BMA400_D_TAP_INT_ASSERTED)
            {
                printf("Double tap detected!\r\n");
            }
            
            if(int_status & BMA400_GEN1_INT_ASSERTED)
            {
                printf("Motion detected!\r\n");
            }
            
            if(int_status & BMA400_ORIENT_CH_INT_ASSERTED)
            {
                printf("orient change detected!\r\n");
            }
            
            if(int_status & BMA400_INT_OVERRUN_ASSERTED)
            {
                printf("int-eng-overrun detected!\r\n");
            }
            
            rslt = bma400_get_steps_counted(&step_count, &activity, &bma);
            printf("%ld", step_count);

            switch (activity)
            {
                case BMA400_STILL_ACT:
                    printf(", Still\r\n");
                    break;
                case BMA400_WALK_ACT:
                    printf(", Walking\r\n");
                    break;
                case BMA400_RUN_ACT:
                    printf(", Running\r\n");
                    break;
                default:
                    printf(", undefined\r\n");
                    break;
            }
            
            
            rslt = bma400_get_accel_data(BMA400_DATA_SENSOR_TIME, &data, &bma);
            print_rslt(rslt);

            /* 12-bit accelerometer at range 2G */
            x = lsb_to_ms2(data.x, 4, 12);
            y = lsb_to_ms2(data.y, 4, 12);
            z = lsb_to_ms2(data.z, 4, 12);
            t = sensor_ticks_to_s(data.sensortime);

            printf("%.4f, %.2f, %.2f, %.2f\r\n", t, x, y, z);


            test_dur_ms -= poll_period;
        }

    }

    return 0;
}



void set_interface(enum bma400_intf intf, struct bma400_dev *dev)
{
    switch (intf)
    {
        case BMA400_I2C_INTF:
            dev->intf_ptr = NULL; /* To attach your interface device reference */
            dev->delay_ms = delay_ms;
            dev->dev_id = BMA400_I2C_ADDRESS_SDO_LOW;
            dev->read = (bma400_com_fptr_t)i2c_reg_read;
            dev->write = (bma400_com_fptr_t)i2c_reg_write;
            dev->intf = BMA400_I2C_INTF;
            break;
        case BMA400_SPI_INTF:
            dev->intf_ptr = NULL; /* To attach your interface device reference */
            dev->dev_id = 0; /* Could be used to identify the chip select line. */
            dev->read = (bma400_com_fptr_t)spi_reg_read;
            dev->write = (bma400_com_fptr_t)spi_reg_write;
            dev->intf = BMA400_SPI_INTF;
            break;
        default:
            printf("Interface not supported.\r\n");
    }
}


void delay_ms(uint32_t period)
{
    /* Wait for a period amount of ms*/
    HAL_DelayUSec(period*1000);
}

int8_t i2c_reg_write(uint8_t i2c_addr, uint8_t reg_addr, uint8_t *reg_data, uint16_t length)
{

    /* Write to registers using I2C. Return 0 for a successful execution. */
    HAL_I2C_Write(i2c_addr, reg_addr, reg_data, length);
    return 0;
}

int8_t i2c_reg_read(uint8_t i2c_addr, uint8_t reg_addr, uint8_t *reg_data, uint16_t length)
{

    /* Read from registers using I2C. Return 0 for a successful execution. */
    HAL_I2C_Read(i2c_addr, reg_addr, reg_data, length);    
    return 0;
}

int8_t spi_reg_write(void *intf_ptr, uint8_t cs, uint8_t reg_addr, uint8_t *reg_data, uint16_t length)
{

    /* Write to registers using SPI. Return 0 for a successful execution. */
    return -1;
}

int8_t spi_reg_read(void *intf_ptr, uint8_t cs, uint8_t reg_addr, uint8_t *reg_data, uint16_t length)
{

    /* Read from registers using SPI. Return 0 for a successful execution. */
    return -1;
}

void print_rslt(int8_t rslt)
{
    switch (rslt)
    {
        case BMA400_OK:

            /* Do nothing */
            break;
        case BMA400_E_NULL_PTR:
            printf("Error [%d] : Null pointer\r\n", rslt);
            break;
        case BMA400_E_COM_FAIL:
            printf("Error [%d] : Communication failure\r\n", rslt);
            break;
        case BMA400_E_DEV_NOT_FOUND:
            printf("Error [%d] : Device not found\r\n", rslt);
            break;
        case BMA400_E_INVALID_CONFIG:
            printf("Error [%d] : Invalid configuration\r\n", rslt);
            break;
        case BMA400_W_SELF_TEST_FAIL:
            printf("Warning [%d] : Self test failed\r\n", rslt);
            break;
        default:
            printf("Error [%d] : Unknown error code\r\n", rslt);
            break;
    }
}

float lsb_to_ms2(int16_t val, float g_range, uint8_t bit_width)
{
    float half_scale = (float)(1 << bit_width) / 2.0f;

    return GRAVITY_EARTH * val * g_range / half_scale;
}

float sensor_ticks_to_s(uint32_t sensor_time)
{
    return (float)sensor_time * 0.0000390625f;
}