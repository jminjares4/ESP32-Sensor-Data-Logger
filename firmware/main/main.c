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
#include "inc/led.h"


#define ONBOARD_LED  2
#define EXTERNAL_LED 4

void task_one(void *pvParameters){
   esp_rom_gpio_pad_select_gpio(ONBOARD_LED);
   gpio_set_direction(ONBOARD_LED, GPIO_MODE_OUTPUT);

   while(1){
      gpio_set_level(ONBOARD_LED, 0);
      vTaskDelay(1000 / portTICK_PERIOD_MS);
      gpio_set_level(ONBOARD_LED, 1);
      vTaskDelay(1000 / portTICK_PERIOD_MS);
      esp_rom_printf("Jesus Minjares\n");
   }

}
void task_two(void *pvParameters){
   esp_rom_gpio_pad_select_gpio(EXTERNAL_LED);
   gpio_set_direction(EXTERNAL_LED, GPIO_MODE_OUTPUT);

   while(1){
      gpio_set_level(EXTERNAL_LED, 0);
      vTaskDelay(500 / portTICK_PERIOD_MS);
      gpio_set_level(EXTERNAL_LED, 1);
      vTaskDelay(500 / portTICK_PERIOD_MS);
      esp_rom_printf("Jorge Minjares\n");
   }

}


void ledTask( void *pvParameters){
   led_t led = {.state = OFF, .pin = 16};

   led_enable(&led);

   while(1){
      led_toggle(&led);
      if(led.state){
         printf("LED is on!\n");
      }else{
         printf("LED is off...\n");
      }
      vTaskDelay( 1000 / portTICK_PERIOD_MS);
   }

}

void app_main(void)
{
   xTaskCreate(&task_one, "task 1",  1024, NULL, tskIDLE_PRIORITY, NULL);  
   xTaskCreate(&task_two, "task 2",  1024, NULL, tskIDLE_PRIORITY, NULL);
   xTaskCreate(&ledTask, "LED task", 1024, NULL, tskIDLE_PRIORITY, NULL);

}