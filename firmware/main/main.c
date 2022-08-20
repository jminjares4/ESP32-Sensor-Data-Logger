#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

#include "devices/button.h"
#include "devices/led.h"
#include "devices/lcd_driver/esp_lcd.h"
#include "esp_idf_version.h"

#define ONBOARD_LED 2
#define EXTERNAL_LED 4

void task_one(void *pvParameters)
{
   /* Select GPIO pad */
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 0, 0)
   esp_rom_gpio_pad_select_gpio(ONBOARD_LED);
#else
   gpio_pad_select_gpio(ONBOARD_LED);
#endif
   /* Set direction as output */
   gpio_set_direction(ONBOARD_LED, GPIO_MODE_OUTPUT);

   while (1)
   {
      /* Toggle onboard led every second */
      gpio_set_level(ONBOARD_LED, 0);
      vTaskDelay(1000 / portTICK_PERIOD_MS);
      gpio_set_level(ONBOARD_LED, 1);
      vTaskDelay(1000 / portTICK_PERIOD_MS);
      /* Print task 1 */
      printf("Task 1\n");
   }
}
void task_two(void *pvParameters)
{
   /* Select GPIO pad */

#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 0, 0)
   esp_rom_gpio_pad_select_gpio(EXTERNAL_LED);
#else
   gpio_pad_select_gpio(EXTERNAL_LED);
#endif
   /* Set direction as output */
   gpio_set_direction(EXTERNAL_LED, GPIO_MODE_OUTPUT);

   while (1)
   {
      /* Toggle external led every 1/2 second */
      gpio_set_level(EXTERNAL_LED, 0);
      vTaskDelay(500 / portTICK_PERIOD_MS);
      gpio_set_level(EXTERNAL_LED, 1);
      vTaskDelay(500 / portTICK_PERIOD_MS);
      /* Print task 2 */
      printf("Task 2\n");
   }
}
void ledTask(void *pvParameters)
{
   /* Set led */
   led_t led = {.pin = 13, .state = OFF};

   /* Enable led */
   led_enable(&led);

   /* Turn off led */
   led_off(&led);

   while (1)
   {
      /* Toggle led */
      led_toggle(&led);
      /* Print state */
      led.state ? printf("LED is on!\n") : printf("LED is off...\n");
      /* 100 ms delay */
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

void lcdTask(void *pvParameters)
{
   /* Create LCD object */
   lcd_t lcd;
   /* Set LCD to default pinout */
   lcdDefault(&lcd);
   /* Initialize LCD */
   lcdInit(&lcd);
   /* Clear previous data */
   lcdClear(&lcd);

   /* Set text */
   lcdSetText(&lcd, "ESP LCD", 3, 0);

   /* Variables */
   char buffer[16]; /* character buffer */
   int count = 0;   /* count */

   while (1)
   {
      /* Set custom text */
      sprintf(buffer, "Count: %d", count);
      lcdSetText(&lcd, buffer, 0, 1);        /* Set text to second row */
      count++; /* Increment count */
      vTaskDelay(1000 / portTICK_PERIOD_MS); /* 1 second delay */
   }
}

void app_main(void)
{
   xTaskCreate(&task_one, "task 1", 1024, NULL, tskIDLE_PRIORITY, NULL);
   xTaskCreate(&task_two, "task 2", 1024, NULL, tskIDLE_PRIORITY, NULL);
   xTaskCreate(&ledTask, "LED task", 1024, NULL, 7, NULL);
   xTaskCreate(&buttonTask, "Button task", 1024, NULL, tskIDLE_PRIORITY, NULL);
   xTaskCreate(&lcdTask, "LCD task", 2048, NULL, 5, NULL);
}