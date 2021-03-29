#ifndef __HX3313_H__
#define __HX3313_H__

#include <stdint.h>

// references:
// http://www.synercontech.com/Public/uploads/file/2019_10/20191019232429_24955.pdf
// https://translate.googleusercontent.com/translate_c?depth=1&rurl=translate.google.com&sl=auto&sp=nmt4&tl=en&u=http://www.synercontech.com/index.php/Product/show/type/10/id/10&usg=ALkJrhieZQv8gM0unY_Ui7O7a6q9VRpyZw

// incomplete information, no sample code
// led doesn't glow
// algorithm to calculate HRM is unknown
// ps en and hrs en does show data in the data registers, varying with light intensity, so something works.

// device address
#define HX3313_DEV_ADDR                         0x44

// registers
#define HX3313_REG_DEVICE_ID                    0x00
#define HX3313_REG_ENABLE                       0x02
#define HX3313_REG_LED_HRS                      0x04
#define HX3313_REG_LED_PS                       0x05

#define HX3313_REG_INTERRUPT_CFG_1              0x06
#define HX3313_REG_INTERRUPT_CFG_2              0x07
#define HX3313_REG_INTERRUPT_CFG_3              0x08

#define HX3313_REG_SLEEP_ENABLE                 0x09

#define HX3313_REG_OFFSET_IDAC_HRS              0x14
#define HX3313_REG_OFFSET_IDAC_PS               0x15

#define HX3313_REG_PS_INTERVAL                  0x16

#define HX3313_REG_HRS_DATA1                    0xA0 // 3B
#define HX3313_REG_ALS_DATA1                    0xA3 // 3B
#define HX3313_REG_PS1_DATA1                    0xA6 // 3B
#define HX3313_REG_ALS_DATA2                    0xA9 // 3B

#define HX3313_REG_LED_DR                       0xC0


// default or common values
#define HX3313_VAL_DEVICE_ID                    0x22


#define HX3313_HRS_ADC_OSR_128                  0x00
#define HX3313_HRS_ADC_OSR_256                  0x01
#define HX3313_HRS_ADC_OSR_512                  0x02
#define HX3313_HRS_ADC_OSR_1024                 0x03

#define HX3313_PS_ADC_OSR_128                   0x00
#define HX3313_PS_ADC_OSR_256                   0x01
#define HX3313_PS_ADC_OSR_512                   0x02
#define HX3313_PS_ADC_OSR_1024                  0x03

#define HX3313_LED_DR_12_5_mA                   0x00
#define HX3313_LED_DR_25_mA                     0x01
#define HX3313_LED_DR_50_mA                     0x02
#define HX3313_LED_DR_100_mA                    0x03


uint8_t hx3313_probe();
uint8_t hx3313_config_led(uint8_t led_current);
uint8_t hx3313_config_ps(uint8_t ps_osr, uint8_t ps_led_on_time, uint8_t ps_phase_offset_idac, uint8_t ps_interval);
uint8_t hx3313_config_hrs(uint8_t hrs_osr, uint8_t hrs_led_on_time, uint8_t hrs_phase_offset_idac);
uint8_t hx3313_enable_ps(uint8_t en_ps);
uint8_t hx3313_enable_hrs(uint8_t en_hrs);
uint8_t hx3313_read_interrupt();
uint8_t hx3313_read_hrs_data();
uint8_t hx3313_read_als_data();
uint8_t hx3313_read_ps1_data();


uint8_t hx3313_ldo_drive_config();
uint8_t hx3313_basic_test(uint32_t num_samples, uint32_t period_ms);


#endif // #ifndef __HX3313_H__