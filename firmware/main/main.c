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
#include <inttypes.h>

#include "esp_idf_version.h"

/* Dummy struct */
typedef struct {
    float temp;
    int pressure;
}temp_t;


/* Global Queue handle */
QueueHandle_t myQueue;

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
   led_t led = {.pin = 15, .state = OFF};

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
   extraLed.pin = 25;
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

   /* Set LCD to custom pinout */
   gpio_num_t data[4] = {19, 18, 17, 16}; /* Data pins */
   gpio_num_t en = 27;        /* Enable pin */
   gpio_num_t regSel = 26;    /* Register Select pin */

   lcdCtor(&lcd,data, en, regSel);

   /* Initialize LCD */
   lcdInit(&lcd);
   /* Clear previous data */
   lcdClear(&lcd);

   /* Set text */
   lcdSetText(&lcd, "Custom ESP LCD", 0, 0);

   /* Variable */
   int count = 0;   /* count */

   while (1)
   {
      /* Set custom text */
      lcdSetText(&lcd, "Count: ", 0, 1);
      lcdSetInt(&lcd, count, 8, 1);
      count++; /* Increment count */
      vTaskDelay(1000 / portTICK_PERIOD_MS); /* 1 second delay */
   }
}

void bmp180Task(void *pvParameters){

   ESP_ERROR_CHECK(i2cdev_init());

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
        else{
            /* float is used in printf(). you need non-default configuration in
             * sdkconfig for ESP8266, which is enabled by default for this
             * example. see sdkconfig.defaults.esp8266
             */
            printf("Temperature: %.2f degrees Celsius; Pressure: %" PRIu32 " Pa\n", temp, pressure);
            temp_t data = {.temp = temp, .pressure= pressure};
            xQueueSendToBack(myQueue, &data, 0);
        }

        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

void ssd1306Task(void *pvParameters){
   
   SSD1306_Init();
   
   printf("SSD1306 Initialized...\n");

   char str[16];

   temp_t dummy_data = {.temp = 0.0, .pressure = 0};

   while(1){
      if(xQueueReceive(myQueue, &dummy_data, (TickType_t)100) == pdPASS){

         SSD1306_GotoXY(10,10);
         SSD1306_Clear();
         vTaskDelay(10);
         float temp_f = (dummy_data.temp * 9/5) + 32;
         sprintf(str,"T:%.2f F", temp_f);
         SSD1306_Puts(str,&Font_11x18,1);
         SSD1306_GotoXY(10,40);
         sprintf(str,"P:%d Pa",dummy_data.pressure);
         SSD1306_Puts(str,&Font_11x18,1);
         SSD1306_UpdateScreen();
      }
   }

}

void app_main(void)
{
   myQueue = xQueueCreate(2, sizeof(temp_t));
   
   xTaskCreate(&task_one, "task 1", 1024, NULL, tskIDLE_PRIORITY, NULL);
   xTaskCreate(&task_two, "task 2", 1024, NULL, tskIDLE_PRIORITY, NULL);
   xTaskCreate(&ledTask, "LED task", 1024, NULL, 7, NULL);
   xTaskCreate(&buttonTask, "Button task", 1024, NULL, tskIDLE_PRIORITY, NULL);
   xTaskCreate(&lcdTask, "LCD task", 2048, NULL, 5, NULL);
   xTaskCreate(&bmp180Task, "BMP180 Task", 2048, NULL, 5, NULL);
   xTaskCreate(&ssd1306Task, "OLED Task", 2048, NULL, 3, NULL);

}