/*==========================================================
 * Copyright 2020 QuickLogic Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *==========================================================*/

/*==========================================================
 *
 *    File   : bootloader_task.c
 *    Purpose: This file has task to execute flash Update functionality.
 *
 *
 *=========================================================*/
#include "Fw_global_config.h"

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "spi_flash.h"
#include "eoss3_hal_gpio.h"
#include "dbg_uart.h"
#include "flash_defines.h"
#include "bootloader_defines.h"

extern uint32_t image_metadata[FLASH_APP_META_SIZE / 4];
extern int load_m4app(void);
extern int load_usb_flasher(void);
extern int load_appfpga(void);
int check_active_images(void);

#define MAX_BOOTLOADER_WAIT_MSEC (5 * 1000)
#define MIN_USER_BTN_PRESS_WAIT_MSEC (200)
#define USER_BUTTON_GPIO_NUM (0) // PAD 6, GPIO is connected to User Button
#define BLUE_LED_GPIO_NUM (4)    // PAD 18, GPIO is connected to Blue LED
#define GREEN_LED_GPIO_NUM (5)   // PAD 21, GPIO is connected to Green LED
#define RED_LED_GPIO_NUM (6)     // PAD 22, GPIO is connected to Red LED

#define METADATA_UNKNOWN_FLASH_STATE (0x0)
#define METADATA_APPFPGA_IS_FLASHED (0x1)
#define METADATA_M4APP_IS_FLASHED (0x2)
#define METADATA_APPFFE_IS_FLASHED (0x4)
#define METADATA_VALID_IMAGE_IS_FLASHED (METADATA_APPFPGA_IS_FLASHED | METADATA_M4APP_IS_FLASHED) // ffe not supported yet

TaskHandle_t BLTaskHandle;
static int user_button_pressed = 0;
// static int user_btn_on_count = 0;
static int user_btn_on_start = 0;
void check_user_button(void)
{
    uint8_t gpio_value = 1;
    HAL_GPIO_Read(USER_BUTTON_GPIO_NUM, &gpio_value);
    if (gpio_value == 0)
    {
        // if first time get the time stamp
        if (user_btn_on_start == 0)
            user_btn_on_start = xTaskGetTickCount();
        if ((xTaskGetTickCount() - user_btn_on_start) > MIN_USER_BTN_PRESS_WAIT_MSEC)
        {
            user_button_pressed = 1;
        }
    }
    return;
}
/*
 * This will set the red LED state to ON or OFF
 */
void set_boot_error_led(uint8_t value)
{
    // Use the red LED to indicate fatal erro
    HAL_GPIO_Write(RED_LED_GPIO_NUM, value);
}
/*
 * This will set the green LED state to ON or OFF
 */
void set_downloading_led(uint8_t value)
{
    // we use green LED toggle to indicate waiting
    // So, set to constant ON during Flashing using USB
    // as an Acknowledgement of User button press
    HAL_GPIO_Write(GREEN_LED_GPIO_NUM, !value);
    HAL_GPIO_Write(RED_LED_GPIO_NUM, value);
    HAL_GPIO_Write(BLUE_LED_GPIO_NUM, value);
}
/*
 * This will toggle green LED with time passed.
 * The state is maintained internal.
 * Uses Tickcount to change the ON-OFF states.
 */
void toggle_downloading_led(int high_time_msec, int low_time_msec)
{
    static uint8_t led_state = 1;
    static int led_state_count = 0;

    // if the time exceeds the toggle time, change state and note time
    if (led_state == 1 && (xTaskGetTickCount() - led_state_count) > high_time_msec)
    {
        led_state_count = xTaskGetTickCount();
        led_state = 0;
        HAL_GPIO_Write(GREEN_LED_GPIO_NUM, led_state);
        HAL_GPIO_Write(RED_LED_GPIO_NUM, led_state);
        HAL_GPIO_Write(BLUE_LED_GPIO_NUM, led_state);
    }
    if (led_state == 0 && (xTaskGetTickCount() - led_state_count) > low_time_msec)
    {
        led_state_count = xTaskGetTickCount();
        led_state = 1;
        HAL_GPIO_Write(GREEN_LED_GPIO_NUM, !led_state);
        HAL_GPIO_Write(RED_LED_GPIO_NUM, led_state);
        HAL_GPIO_Write(BLUE_LED_GPIO_NUM, led_state);
    }
    return;
}
void set_waiting_led(uint8_t value)
{
    // we use blue LED toggle to indicate waiting
    // So, set to constant ON during Flashing using USB
    // as an Acknowledgement of User button press
    HAL_GPIO_Write(BLUE_LED_GPIO_NUM, value);
}
/*
 * This will toggle blue LED with time passed.
 * The state is maintained internal.
 * Uses Tickcount to change the ON-OFF states.
 */
void toggle_waiting_led(int toggle_time_msec)
{
    static uint8_t led_state = 1;
    static int led_state_count = 0;

    // set the led state first to last state
    HAL_GPIO_Write(BLUE_LED_GPIO_NUM, led_state);

    // if the time exceeds the toggle time, change state and note time
    if ((xTaskGetTickCount() - led_state_count) > toggle_time_msec)
    {
        led_state_count = xTaskGetTickCount();
        if (led_state == 0)
            led_state = 1;
        else
            led_state = 0;
    }
    return;
}
/*
 * This will toggle red LED with time passed.
 * The state is maintained internal.
 * Uses Tickcount to change the ON-OFF states.
 */
void toggle_red_led(int toggle_time_msec)
{
    static uint8_t led_state = 1;
    static int led_state_count = 0;

    // set the led state first to last state
    HAL_GPIO_Write(RED_LED_GPIO_NUM, led_state);

    // if the time exceeds the toggle time, change state and note time
    if ((xTaskGetTickCount() - led_state_count) > toggle_time_msec)
    {
        led_state_count = xTaskGetTickCount();
        if (led_state == 0)
            led_state = 1;
        else
            led_state = 0;
    }
    return;
}


static void BLTaskHandler(void *pvParameters)
{
    int wait_time_msec = 0;

    while (1)
    {
        set_waiting_led(0);
        set_downloading_led(1);
        int error = load_usb_flasher(); // this should never return
        // if USB-Serial FPGA load fails, we are dead in the water.
        while (1)
        {
            // set red LED for error and turn off green LED
            set_boot_error_led(1);
            set_downloading_led(0);
            dbg_str("[FATAL] ERROR loading USB FPGA Image\n\n");
            vTaskDelay(5 * 1000);
        }
    }
}


/*!
 * \fn void BL_Task_Init()
 * \brief  Init function to create BootloaderTask to be called from main()
 *
 */
void BL_Task_Init(void)
{
    xTaskCreate(BLTaskHandler, "BL_Task", 256, NULL, tskIDLE_PRIORITY + 4, &BLTaskHandle);
    configASSERT(BLTaskHandle);
    return;
}
