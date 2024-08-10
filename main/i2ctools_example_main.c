// /*
//  * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
//  *
//  * SPDX-License-Identifier: Apache-2.0
//  */

// #include <stdio.h>
// #include <string.h>
// #include "sdkconfig.h"
// #include "esp_log.h"
// #include "esp_console.h"
// #include "esp_vfs_fat.h"
// #include "cmd_system.h"
// #include "cmd_i2ctools.h"
// #include "driver/i2c_master.h"

// static const char *TAG = "i2c-tools";

// i2c_master_bus_handle_t tool_bus_handle;

// static gpio_num_t i2c_gpio_sda = CONFIG_EXAMPLE_I2C_MASTER_SDA;
// static gpio_num_t i2c_gpio_scl = CONFIG_EXAMPLE_I2C_MASTER_SCL;

// static i2c_port_t i2c_port = I2C_NUM_0;

// i2c_master_dev_handle_t dev_handle;

// uint8_t i2c_data_buffer[64];

// uint64_t keypad_base_values[12];

// void get_keypad_values(uint16_t values[12])
// {
//     i2c_data_buffer[0] = 0x7B;

//     i2c_master_transmit(dev_handle, i2c_data_buffer, 1, -1);
//     i2c_master_receive(dev_handle, i2c_data_buffer, 24, 100);

//     for (int i = 0; i < 12; i++)
//     {
//         values[i] = ((uint16_t)(i2c_data_buffer[i * 2 + 1]) << 8) | (uint16_t)(i2c_data_buffer[i * 2]);
//     }
// }


// void init_keypad() {
//     vTaskDelay(1000 / portTICK_PERIOD_MS);


//     i2c_data_buffer[0] = 0x86;
//     i2c_master_transmit(dev_handle, i2c_data_buffer, 1, -1);
//     i2c_master_receive(dev_handle, i2c_data_buffer, 1, 10000);
//     printf("Got mode: %d\n", i2c_data_buffer[0]);

//     vTaskDelay(100 / portTICK_PERIOD_MS);


//     i2c_data_buffer[0] = 0x9C;
//     i2c_master_transmit(dev_handle, i2c_data_buffer, 1, -1);
//     i2c_master_receive(dev_handle, i2c_data_buffer, 1, 10000);
//     printf("Got calibration: %d\n", i2c_data_buffer[0]);

//     vTaskDelay(100 / portTICK_PERIOD_MS);


//     i2c_data_buffer[0] = 0x24;
//     i2c_master_transmit(dev_handle, i2c_data_buffer, 1, -1);
//     i2c_master_receive(dev_handle, i2c_data_buffer, 1, 10000);
//     printf("Got sensitivity: %d\n", i2c_data_buffer[0]);

//     vTaskDelay(100 / portTICK_PERIOD_MS);


//     i2c_data_buffer[0] = 0x3;
//     i2c_data_buffer[1] = 0b11111100;
//     i2c_data_buffer[2] = 0b00111111;
//     i2c_data_buffer[3] = 0b11111100;
//     i2c_data_buffer[4] = 0b00111111;

//     i2c_master_transmit(dev_handle, i2c_data_buffer, 5, -1);

//     vTaskDelay(100 / portTICK_PERIOD_MS);

//     i2c_data_buffer[0] = 0x3;

//     i2c_master_transmit(dev_handle, i2c_data_buffer, 1, -1);
//     i2c_master_receive(dev_handle, i2c_data_buffer, 4, 10000);
//     printf("Got active channels: %d, %d, %d, %d\n", i2c_data_buffer[0], i2c_data_buffer[1], i2c_data_buffer[2], i2c_data_buffer[3]);
// }

// void i2c_init() {
//     i2c_master_bus_config_t i2c_bus_config = {
//         .clk_source = I2C_CLK_SRC_DEFAULT,
//         .i2c_port = i2c_port,
//         .scl_io_num = 19,
//         .sda_io_num = 18,
//         .glitch_ignore_cnt = 7,
//         .flags.enable_internal_pullup = true,
//     };

//     i2c_device_config_t dev_cfg = {
//         .dev_addr_length = I2C_ADDR_BIT_LEN_7,
//         .device_address = 0x2C,
//         .scl_speed_hz = 100000,
//     };

//     ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_bus_config, &tool_bus_handle));

//     ESP_ERROR_CHECK(i2c_master_bus_add_device(tool_bus_handle, &dev_cfg, &dev_handle));

// }

// void app_main(void)
// {
//     i2c_init();

//     init_keypad();

//     uint16_t keypad_values[12];

//     const int SAMPLES_COUNT = 100;

//     for (int i = 0; i < 12; i++)
//     {
//         keypad_base_values[i] = 0;
//     }

//     vTaskDelay(1000 / portTICK_PERIOD_MS);

//     for (int i = 0; i < SAMPLES_COUNT; i++)
//     {
//         get_keypad_values(keypad_values);
//         for (int i = 0; i < 12; i++)
//         {
//             keypad_base_values[i] += keypad_values[i];
//         }
//         vTaskDelay(10 / portTICK_PERIOD_MS);
//     }

//     ESP_LOGI(TAG, "Base values: ");

//     for (int i = 0; i < 12; i++)
//     {
//         keypad_base_values[i] /= SAMPLES_COUNT;
//         printf("%" PRIu64 " ", keypad_base_values[i]);
//     }

//     printf("\n");

//     while (true)
//     {
//         bool some_pressed = false;
//         get_keypad_values(keypad_values);

//         for (int i = 0; i < 12; i++)
//         {
//             if ((int16_t)keypad_base_values[i] - (int16_t)keypad_values[i] > 1000)
//             {
//                 printf("%d ", i);
//                 some_pressed = true;
//             }
//         }

//         if (some_pressed)
//         {
//             printf("pressed \n");
//         }

//         vTaskDelay(100 / portTICK_PERIOD_MS);
//     }
// }




/* Blink Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "sdkconfig.h"

static const char *TAG = "example";

#define BLINK_GPIO 14


void app_main(void)
{
    gpio_reset_pin(BLINK_GPIO);
    gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT);

int state = 0;

while(true) {

    gpio_set_level(BLINK_GPIO, state);
    vTaskDelay(1000 / portTICK_PERIOD_MS);

    state = (state + 1) % 2;
}

}
