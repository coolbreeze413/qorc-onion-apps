#include <stdint.h>
#include <stdio.h>

#include "Fw_global_config.h"

#include "hx3313.h"
#include "eoss3_hal_pad_config.h"
#include "eoss3_hal_gpio.h"
#include "QL_Trace.h"

uint8_t hx3313_ldo_drive_config()
{
    PadConfig xPadConf;
  
    // GPIO CONFIG for PAD_9 as GPIO (GPIO_1 from IOMux), pulldown.
    xPadConf.ucPin      = PAD_9;
	xPadConf.ucFunc     = PAD9_FUNC_SEL_GPIO_1;
	xPadConf.ucCtrl     = PAD_CTRL_SRC_A0;
	xPadConf.ucMode     = PAD_MODE_OUTPUT_EN;
	xPadConf.ucPull     = PAD_PULLDOWN;
	xPadConf.ucDrv      = PAD_DRV_STRENGHT_4MA;
	xPadConf.ucSpeed    = PAD_SLEW_RATE_SLOW;
	xPadConf.ucSmtTrg   = PAD_SMT_TRIG_DIS;
	
    HAL_PAD_Config(&xPadConf);
    
    // set LDO_EN for HX3313 3.3V drive.
    HAL_GPIO_Write(GPIO_1, 1);
    
    return QL_STATUS_OK;
}


uint8_t hx3313_basic_test(uint32_t num_samples, uint32_t period_ms)
{
    uint32_t read_count = 0;    
    
    hx3313_probe();
    hx3313_config_led(HX3313_LED_DR_12_5_mA);
    hx3313_config_ps(HX3313_PS_ADC_OSR_128, 0x20, 0x00, 0x40);
    hx3313_config_hrs(HX3313_HRS_ADC_OSR_128, 0x10, 0x00);
    
    hx3313_enable_ps(1);
    hx3313_enable_hrs(1);
    
    while(read_count < num_samples)
    {
        HAL_DelayUSec(200*1000);
        hx3313_read_interrupt();
        hx3313_read_hrs_data();
        hx3313_read_als_data();
        hx3313_read_ps1_data();     
        
        printf("\r\n");
        
        read_count++;
    }

    
    return QL_STATUS_OK;
}