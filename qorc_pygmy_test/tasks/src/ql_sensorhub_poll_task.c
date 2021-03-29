//
//
// beginning of a very simple FreeRTOS task to poll sensors in the system
// directly from the M4 I2C control
//
//
// our design considerations are:
// 1. accel -> 200Hz/400Hz
// 2. mag -> 50 Hz - 100 Hz max
// 3. gyro -> 200Hz/400Hz
// these come from the bmx160
// 
// 4. smart mode status: doubletap, orientation, activity/inactivity, steps(cnt/det/context) - say every 100ms? or 50ms.
// 5. accel -> 100/200Hz (optional)
// these come from the bma400
//
// 6. temperature + pressure -> min 3.6ms (low precision) - 27.6ms (high precision: default)
// these come from the dps310
//
// 7. hrs + ps + als -> to calculate HR, we should use at least 4-10 seconds of data, calc.
// so the effective poll rate is no less than 1 in 4 seconds. or 0.25 Hz
// note that the hrs calculation is done in a separate task (capture data, fft, etc.) at 400Hz! we can manage 200Hz for now.
//
//
// i2c read speed : 400 kHz = 400,000 clocks per second
// each byte in i2c is 10 cycles assuming ideal behavior -> 400,000 == 40,000B per sec
// so, 40B/msec read in ideal conditions, with ideal clock.
// in actuality we see that we get around 25-30B /msec (actual clock speed is around 370kHz on our system)
// read from i2c + cost of task delay etc. seems to be as expected

// the poll task can run, at the max, at 5ms consistently, but this can be offset by other tasks running in the system.
// so we take the "tick" to be 10ms+ for now. we can change this as we go further depending on the performance we see.
// so tick_ms = 10ms -> the poll loop runs every 10ms -> 100Hz.
// at every tick_ms -> we read the AMG data as it is one read: 1.5ms
// if(bma400_accel_en) -> we read the accel data (0.3ms)
// if(hrs_raw_en) -> we read the hrs, ps, als data (2ms)
// tick_ms_20 -> temperature + pressure also (2ms)
// tick_ms_50 -> bma400 smart mode data (4ms)

// so, at tick_ms % 50 -> bma_smart_cycle
// tick_ms % 20 -> dps_cycle
// tick_ms % 10 -> bmx_amg_cycle, bma_a_cycle, hrs_raw cycle





#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "queue.h"

#include <stdint.h>
#include <stdio.h>

#include "Fw_global_config.h"
#include "QL_Trace.h"

#include "eoss3_hal_timer.h"

#include "bmx160.h"
#include "hx3313.h"
#include "dps310.h"
#include "bma400.h"
#include "ql_sensorhub_poll_task.h"

// memory area and structure of data
#define SENSORHUB_POLL_SRAM_AREA                        (0x2007C000)


struct sensorhub_poll_config* poll_config = (struct sensorhub_poll_config* )0x2007C000;
struct sensorhub_poll_data* poll_data = (struct sensorhub_poll_data* )(0x2007C000 + sizeof(struct sensorhub_poll_config));

// move these defines to the central sensor task configuration
#define SENSORHUB_POLL_TASK_TASK_QUEUE_ITEMS             (100)
#define SENSORHUB_POLL_TASK_TASK_WAIT_TIME               (portMAX_DELAY)
#define SENSORHUB_POLL_TASK_TASK_STACKSIZE               (1024)
#define SENSORHUB_POLL_TASK_TASK_PRIORITY                (tskIDLE_PRIORITY + 16)

// commands
//#define SENSORHUB_POLL_TASK_CMD_START       (0x01)
//#define SENSORHUB_POLL_TASK_CMD_STOP        (0x02)
#define SENSORHUB_POLL_TASK_TIMER_EXPIRED   (0x03)
#define SENSORHUB_POLL_TASK_ERROR           (0x09)

#define SENSORHUB_POLL_TASK_SENSOR_POLL_TIME_DEFAULT_ms     (10)

struct sensorhub_poll_task_msg
{
    uint8_t command;
    // additional stuff later.
};



QueueHandle_t   sensorhub_poll_task_queue;
TaskHandle_t    sensorhub_poll_task;
struct sensorhub_poll_task_msg  sh_poll_task_msg = {0};
uint8_t sensorhub_poll_task_exit = 0;
HAL_Timer_t    sensorhub_poll_timer;
//HAL_TimerCallback_t sensorhub_poll_timer_cb;
void sensorhub_poll_timer_cb(void* cookie);
uint32_t poll_tick = 0;

void sensorhub_poll_task_handler(void *params);
void sensorhub_poll_task_start();
void sensorhub_poll_task_stop();
/*
move it out to sensorhub central file later
*/
void initialize_sensors();



QL_Status sensorhub_poll_task_init(void)
{
    HAL_StatusTypeDef halret;
    
    memset(0x2007C000, 0, sizeof(struct sensorhub_poll_config) + sizeof(struct sensorhub_poll_data));
    
	sensorhub_poll_task_queue = xQueueCreate(SENSORHUB_POLL_TASK_TASK_QUEUE_ITEMS, 
                                             sizeof(struct sensorhub_poll_task_msg));
    
    if(sensorhub_poll_task_queue == NULL)
    {
        return QL_STATUS_ERROR;
    }


	xTaskCreate(sensorhub_poll_task_handler, 
                "sh_poll", 
                SENSORHUB_POLL_TASK_TASK_STACKSIZE, 
                NULL, 
                SENSORHUB_POLL_TASK_TASK_PRIORITY,
                &sensorhub_poll_task);
    
    if(sensorhub_poll_task == NULL)
    {
        return QL_STATUS_ERROR;
    }
    
    // create a timer for the sensor poll task:
    halret = HAL_TimerCreate(&sensorhub_poll_timer,
                             (SENSORHUB_POLL_TASK_SENSOR_POLL_TIME_DEFAULT_ms*1000),
                             HAL_TIMER_PERIODIC, 
                             sensorhub_poll_timer_cb,
                             NULL);
    QL_ASSERT(halret == HAL_OK);
    
    initialize_sensors();
    
    printf("sensorhub_poll_config A : [0x%08x]\r\n", poll_config);
    printf("sensorhub_poll_config S : [%d B]\r\n", sizeof(struct sensorhub_poll_config));
    printf("sensorhub_poll_data   A : [0x%08x]\r\n", poll_data);
    printf("sensorhub_poll_data   S : [%d B]\r\n", sizeof(struct sensorhub_poll_data));
    
    return QL_STATUS_OK;
}


void sensorhub_poll_task_handler(void *params)
{
    BaseType_t qret;
    uint32_t index_for_test = 0;
    poll_tick = 0;
    
    sensorhub_poll_task_start();
    
    while (sensorhub_poll_task_exit == 0)
    {
        qret = xQueueReceive(sensorhub_poll_task_queue, 
                             &sh_poll_task_msg, 
                             SENSORHUB_POLL_TASK_TASK_WAIT_TIME);
        
        QL_ASSERT(qret == pdTRUE);
        
        switch(sh_poll_task_msg.command)
        {   
            case SENSORHUB_POLL_TASK_TIMER_EXPIRED:
            {
                // poll sensors and store into RAM
                if((poll_tick % 10) == 0) 
                {
                    // 10ms cycle -> bmx agmht
                    bmx160_read_sensorpoll_data(&(poll_data->bmx160r_data), 
                                                        &(poll_data->bmx160_data));
                }
                
                if((poll_tick % 1000) == 0) 
                {
                    // 1s cycle -> dps temp + press
                    dps310_read_sensorpoll_data(&(poll_data->dps310r_data), &(poll_data->dps310_data));
                }
                
                if((poll_tick % 50) == 0) 
                {
                    // 50ms cycle -> bma smart TODO
                }
                
                if((poll_tick % 50000) == 0)
                {
                    // 10 seconds
                    // reset poll_tick so we don't have to worry about overflow problems and stuff.
                    poll_tick = 0;
                }
                
                poll_tick += SENSORHUB_POLL_TASK_SENSOR_POLL_TIME_DEFAULT_ms;
                index_for_test++;
                if(index_for_test == 201) {                  
                  index_for_test = 0;
                  printf("%u\r\n", poll_tick);
                  //sensorhub_poll_task_stop();
                }
            }
            break;
            
            case SENSORHUB_POLL_TASK_ERROR:
            {
                // for future use.
                // any sensor error/interrupts etc.
                //halret = HAL_TimerStop(sensorhub_poll_timer);
            }
            break;
            
            default:
            {
            }
            break;
        }
    }
}


void sensorhub_poll_timer_cb(void* cookie)
{
    // send msg to sensorhub poll task
    struct sensorhub_poll_task_msg  poll_timer_expired_msg = {0};
    poll_timer_expired_msg.command = SENSORHUB_POLL_TASK_TIMER_EXPIRED;
    
    // delay should be lesser ticks? this needs to go quickly.
    if (xQueueSend(sensorhub_poll_task_queue, &poll_timer_expired_msg, portMAX_DELAY) != pdPASS)
    {
        printf("sensorhub_poll_timer_cb failed\r\n");
    }
}

// wrapper util functions:
void sensorhub_poll_task_start()
{
    HAL_StatusTypeDef halret;
    halret = HAL_TimerStart(sensorhub_poll_timer);
    printf("poll_task_start\r\n");
}

void sensorhub_poll_task_stop()
{
    int i;
    HAL_StatusTypeDef halret;
    halret = HAL_TimerStop(sensorhub_poll_timer);
    taskYIELD();
    printf("poll_task_stop\r\n");
    
    //printf("accel: [%d, %u, 0x%04x, 0x%04x, 0x%04x]\r\n", 0, accel_data_arr[0].ts, (uint16_t)accel_data_arr[0].xr, (uint16_t)accel_data_arr[0].yr, (uint16_t)accel_data_arr[0].zr);
    for(i=1; i<20; i++)
    {       
        //printf("accel: [%d, %u, 0x%04x, 0x%04x, 0x%04x]\r\n", i, (accel_data_arr[i].ts - accel_data_arr[i-1].ts), (uint16_t)accel_data_arr[i].xr, (uint16_t)accel_data_arr[i].yr, (uint16_t)accel_data_arr[i].zr);
        //printf("gyro: [%d, %u, 0x%04x, 0x%04x, 0x%04x]\r\n", i, (accel_data_arr[i].ts - accel_data_arr[i-1].ts), (uint16_t)gyro_data_arr[i].xr, (uint16_t)gyro_data_arr[i].yr, (uint16_t)gyro_data_arr[i].zr);
        //printf("accel: [%f, %f, %f]\r\n", accel_data_arr[i].x, accel_data_arr[i].y, accel_data_arr[i].z);
    }
}


void initialize_sensors()
{
    float temp;
    // BMX160
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
    
    // DPS310
    dps310_probe();
    dps310_soft_reset();
    dps310_set_temp_sensor(DPS310_VAL_TEMP_SENSOR_AUTO);
    //initial reading of temperature so that we have a reading for pressure compensation later.
    temp = dps310_measure_single_temp(0x7);
    printf("temp_auto: [%f]\r\n", temp);
    dps310_start_continuous_measurement_both(0x0, 0x3, 
                                             0x0, 0x3);
    // take note of the measurement time reported by this function, polling should be greater than that!
    
    // HX3313
    hx3313_probe();
    hx3313_config_led(HX3313_LED_DR_12_5_mA);
    hx3313_config_ps(HX3313_PS_ADC_OSR_128, 0x20, 0x00, 0x40);
    hx3313_config_hrs(HX3313_HRS_ADC_OSR_128, 0x10, 0x00);
    
    hx3313_enable_ps(1);
    hx3313_enable_hrs(1);
    
    {
    // BMA400
    struct bma400_dev bma;
    struct bma400_int_enable latch_int;    
    struct bma400_int_enable tap_int[1];
    struct bma400_int_enable step_int;
    struct bma400_int_enable gen1_int;
    struct bma400_int_enable orient_int;
    
    struct bma400_sensor_conf conf[7];
    int8_t rslt;
    //uint32_t poll_period = 500, test_dur_ms = 1800000;
    uint16_t int_status;
    uint32_t step_count;
    uint8_t activity;
    struct bma400_sensor_data data;
    float t, x, y, z;

    set_interface(BMA400_I2C_INTF, &bma);

    rslt = bma400_init(&bma);
    print_rslt(rslt);
    printf("bma400_probe: [0x%02x]\r\n", bma.chip_id);

    rslt = bma400_soft_reset(&bma);
    print_rslt(rslt);

    conf[0].type = BMA400_ACCEL;
    conf[1].type = BMA400_TAP_INT;
    conf[2].type = BMA400_STEP_COUNTER_INT;
    conf[3].type = BMA400_GEN1_INT; // activity detection in this case
    conf[4].type = BMA400_ORIENT_CHANGE_INT;   
    //conf[5].type = BMA400_GEN2_INT; // future use
    

    rslt = bma400_get_sensor_conf(conf, 5, &bma);
    print_rslt(rslt);

    conf[0].param.accel.odr = BMA400_ODR_200HZ;
    conf[0].param.accel.range = BMA400_4G_RANGE;
    conf[0].param.accel.data_src = BMA400_DATA_SRC_ACCEL_FILT_1;
    conf[0].param.accel.filt1_bw = BMA400_ACCEL_FILT1_BW_1;

    conf[1].param.tap.int_chan = BMA400_UNMAP_INT_PIN; // no interrupt pin mapped
    conf[1].param.tap.axes_sel = BMA400_Z_AXIS_EN_TAP;
    conf[1].param.tap.sensitivity = BMA400_TAP_SENSITIVITY_0; // most sensitive
    
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
    conf[3].param.gen_int.gen_int_thres = 0x5; // higher threshold, lower false detection
    
    // set duration to 15 odr ticks
    conf[3].param.gen_int.gen_int_dur = 0x000f; // can be increased if motion duration too short, and triggers often
    
    
    conf[4].param.orient.int_chan = BMA400_UNMAP_INT_PIN; // no interrupt pin mapped
    conf[4].param.orient.axes_sel = BMA400_X_AXIS_EN; // only rotation around the X axis will be detected
    conf[4].param.orient.data_src = BMA400_DATA_SRC_ACC_FILT2; // filt2 at 100Hz
    conf[4].param.orient.ref_update = BMA400_ORIENT_REFU_ACC_FILT_2;
    conf[4].param.orient.stability_mode = BMA400_STABILITY_ACC_FILT_2;
      
    conf[4].param.orient.orient_thres = 0x5; // higher value, lower sensitivity
      
    conf[4].param.orient.stability_thres = 0x5; // higher value, so stability is considered
      
    conf[4].param.orient.orient_int_dur = 0x5; // higher value so that a larger orient change is considered
    

    rslt = bma400_set_sensor_conf(conf, 5, &bma);
    print_rslt(rslt);

    bma.delay_ms(100);

    
    //tap_int[0].type = BMA400_SINGLE_TAP_INT_EN; // we dont want this, but if needed can enable.
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
    
    
    // latch everything, as we are gonna be polling.
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
        // nada
    }
    }
    
}
