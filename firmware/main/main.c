#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

#include "devices/button.h"
#include "devices/led.h"
#include "esp_idf_version.h"

#include "esp_log.h"

#define ONBOARD_LED 2
#define EXTERNAL_LED 4

void task_one(void *pvParameters)
{

#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 0, 0)
   esp_rom_gpio_pad_select_gpio(ONBOARD_LED);
#else
   gpio_pad_select_gpio(ONBOARD_LED);
#endif
   gpio_set_direction(ONBOARD_LED, GPIO_MODE_OUTPUT);

   while (1)
   {
      gpio_set_level(ONBOARD_LED, 0);
      vTaskDelay(1000 / portTICK_PERIOD_MS);
      gpio_set_level(ONBOARD_LED, 1);
      vTaskDelay(1000 / portTICK_PERIOD_MS);
      printf("Task 1\n");
   }
}
void task_two(void *pvParameters)
{

#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 0, 0)
   esp_rom_gpio_pad_select_gpio(EXTERNAL_LED);
#else
   gpio_pad_select_gpio(EXTERNAL_LED);
#endif
   gpio_set_direction(EXTERNAL_LED, GPIO_MODE_OUTPUT);

   while (1)
   {
      gpio_set_level(EXTERNAL_LED, 0);
      vTaskDelay(500 / portTICK_PERIOD_MS);
      gpio_set_level(EXTERNAL_LED, 1);
      vTaskDelay(500 / portTICK_PERIOD_MS);
      printf("Task 2\n");
   }
}

void ledTask(void *pvParameters)
{
   led_t led = {.pin = 16, .state = OFF};

   led_enable(&led);

   led_off(&led);

   while (1)
   {
      led_toggle(&led);
      if (led.state)
      {
         printf("LED is on!\n");
      }
      else
      {
         printf("LED is off...\n");
      }
      vTaskDelay(100 / portTICK_PERIOD_MS);
   }
}

void buttonTask(void *pvParameters)
{
   /* Set button configuration */
   button_t button;
   button.pin = 23;
   button.pull_sel.up = 1;
   button.func = NULL;
   /* Initialize button */
   button_enable(&button);

   /* Set led configuration */
   led_t extraLed;
   extraLed.pin = 22;
   extraLed.state = OFF;
   /* Initialize led */
   led_enable(&extraLed);

   while (1)
   {
      /* Debounce button */
      vTaskDelay(10 / portTICK_PERIOD_MS);
      /* Check if button is pressed
         True:    led on
         False:   led off
      */
      !read_button(&button) ? led_on(&extraLed) : led_off(&extraLed);
      vTaskDelay(100 / portTICK_PERIOD_MS); /* 100 ms delay*/
   }
}

void app_main(void)
{
   xTaskCreate(&task_one, "task 1", 1024, NULL, tskIDLE_PRIORITY, NULL);
   xTaskCreate(&task_two, "task 2", 1024, NULL, tskIDLE_PRIORITY, NULL);
   xTaskCreate(&ledTask, "LED task", 1024, NULL, 7, NULL);
   xTaskCreate(&buttonTask, "Button task", 1024, NULL, tskIDLE_PRIORITY, NULL);
}
