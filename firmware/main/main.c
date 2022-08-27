#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "freertos/queue.h"

#include "button/button.h"
#include "lcd/esp_lcd.h"
#include "led/led.h"

#include "oled/ssd1306.h"
#include "oled/fonts.h"

#include "bmp180.h"
#include "ds3231.h"
#include <inttypes.h>

#include "esp_idf_version.h"

void lcdTask(void *pvParameters)
{
   /* Create LCD object */
   lcd_t lcd;

   /* Set LCD to custom pinout */
   gpio_num_t data[4] = {19, 18, 17, 16}; /* Data pins */
   gpio_num_t en = 27;                    /* Enable pin */
   gpio_num_t regSel = 26;                /* Register Select pin */

   lcdCtor(&lcd, data, en, regSel);

   /* Initialize LCD */
   lcdInit(&lcd);
   /* Clear previous data */
   lcdClear(&lcd);

   /* Set text */
   lcdSetText(&lcd, "Custom ESP LCD", 0, 0);

   /* Variable */
   int count = 0; /* count */

   while (1)
   {
      /* Set custom text */
      lcdSetText(&lcd, "Count: ", 0, 1);
      lcdSetInt(&lcd, count, 8, 1);
      count++;                               /* Increment count */
      vTaskDelay(1000 / portTICK_PERIOD_MS); /* 1 second delay */
   }
}

void bmp180Task(void *pvParameters)
{

   bmp180_dev_t dev;
   memset(&dev, 0, sizeof(bmp180_dev_t));

   gpio_num_t i2c_sda = 21;
   gpio_num_t i2c_scl = 22;

   ESP_ERROR_CHECK(bmp180_init_desc(&dev, 0, i2c_sda, i2c_scl));
   ESP_ERROR_CHECK(bmp180_init(&dev));

   while (1)
   {
      float temp;
      uint32_t pressure;

      esp_err_t res = bmp180_measure(&dev, &temp, &pressure, BMP180_MODE_STANDARD);
      if (res != ESP_OK)
         printf("Could not measure: %d\n", res);
      else
         /* float is used in printf(). you need non-default configuration in
          * sdkconfig for ESP8266, which is enabled by default for this
          * example. see sdkconfig.defaults.esp8266
          */
         printf("Temperature: %.2f degrees Celsius; Pressure: %" PRIu32 " Pa\n", temp, pressure);

      vTaskDelay(pdMS_TO_TICKS(500));
   }
}

void rtcTask(void *pvParameters)
{

   i2c_dev_t dev;
   memset(&dev, 0, sizeof(i2c_dev_t));

   gpio_num_t i2c_sda = 21;
   gpio_num_t i2c_scl = 22;

   ESP_ERROR_CHECK(ds3231_init_desc(&dev, 0, i2c_sda, i2c_scl));

   // setup datetime: 2016-10-09 13:50:10
   struct tm time = {
       .tm_year = 122, // since 1900 (2016 - 1900)
       .tm_mon = 14,   // 0-based
       .tm_mday = 26,
       .tm_hour = 19,
       .tm_min = 7,
       .tm_sec = 0};
   ESP_ERROR_CHECK(ds3231_set_time(&dev, &time));

   while (1)
   {
      float temp;

      vTaskDelay(1000 / portTICK_PERIOD_MS);

      if (ds3231_get_temp_float(&dev, &temp) != ESP_OK)
      {
         printf("Could not get temperature\n");
         continue;
      }

      if (ds3231_get_time(&dev, &time) != ESP_OK)
      {
         printf("Could not get time\n");
         continue;
      }

      /* float is used in printf(). you need non-default configuration in
       * sdkconfig for ESP8266, which is enabled by default for this
       * example. see sdkconfig.defaults.esp8266
       */
      printf("%04d-%02d-%02d %02d:%02d:%02d, %.2f deg Cel\n", time.tm_year + 1900 /*Add 1900 for better readability*/, time.tm_mon + 1,
             time.tm_mday, time.tm_hour, time.tm_min, time.tm_sec, temp);
   }
}

void app_main(void)
{
   ESP_ERROR_CHECK(i2cdev_init());
   xTaskCreate(&lcdTask, "LCD task", 2048, NULL, 5, NULL);
   xTaskCreate(&bmp180Task, "BMP180 Task", 1920, NULL, 8, NULL);
   xTaskCreate(&rtcTask, "RTC Task", 2048, NULL, 10, NULL);
}