/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <string.h>
#include "sdkconfig.h"
#include "esp_log.h"
#include "esp_console.h"
#include "esp_vfs_fat.h"
#include "cmd_system.h"
#include "driver/i2c_master.h"
#include "keypad/keypad.h"
#include "keypad/debugger.h"

static const char *TAG = "keypad_example";

void app_main(void)
{
    keypad_init();

    uint16_t keypad_values[12];
    
    btl_auto_update_fw();
    
    keypad_get_base_values(100);

    printf("\n");

    while (true)
    {
        bool some_pressed = false;
        keypad_get_values(keypad_values);

        for (int i = 0; i < 12; i++)
        {
            if ((int16_t)keypad_base_values[i] - (int16_t)keypad_values[i] > 1000)
            {
                printf("%d ", i);
                some_pressed = true;
            }
        }

        if (some_pressed)
        {
            printf("pressed \n");
        }

        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}






// /* Blink Example

//    This example code is in the Public Domain (or CC0 licensed, at your option.)

//    Unless required by applicable law or agreed to in writing, this
//    software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
//    CONDITIONS OF ANY KIND, either express or implied.
// */
// #include <stdio.h>
// #include "freertos/FreeRTOS.h"
// #include "freertos/task.h"
// #include "driver/gpio.h"
// #include "esp_log.h"
// #include "sdkconfig.h"

// static const char *TAG = "example";

// #define BLINK_GPIO 14


// void app_main(void)
// {
//     gpio_reset_pin(BLINK_GPIO);
//     gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT);

// int state = 0;

// while(true) {

//     gpio_set_level(BLINK_GPIO, state);
//     vTaskDelay(1000 / portTICK_PERIOD_MS);

//     state = (state + 1) % 2;
// }

// }
