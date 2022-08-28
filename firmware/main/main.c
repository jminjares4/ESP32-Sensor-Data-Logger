#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "freertos/queue.h"

/* Drivers */
#include "button/button.h"
#include "lcd/esp_lcd.h"
#include "led/led.h"

/* I2C drivers */
#include "bmp180.h"
#include "ds3231.h"
#include <inttypes.h>

/* SD Card */
#include <sys/unistd.h>
#include <sys/stat.h>
#include "esp_vfs_fat.h"
#include "sdmmc_cmd.h"

static const char *TAG = "example";

#define MOUNT_POINT "/sdcard"

// Pin assignments can be set in menuconfig, see "SD SPI Example Configuration" menu.
// You can also change the pin assignments here by changing the following 4 lines.
#define PIN_NUM_MISO  19
#define PIN_NUM_MOSI  23
#define PIN_NUM_CLK   18
#define PIN_NUM_CS    13

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

void sdcardTask(void * pvParameters){
   esp_err_t ret;

    // Options for mounting the filesystem.
    // If format_if_mount_failed is set to true, SD card will be partitioned and
    // formatted in case when mounting fails.
    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
#ifdef CONFIG_EXAMPLE_FORMAT_IF_MOUNT_FAILED
        .format_if_mount_failed = true,
#else
        .format_if_mount_failed = false,
#endif // EXAMPLE_FORMAT_IF_MOUNT_FAILED
        .max_files = 5,
        .allocation_unit_size = 16 * 1024
    };
    sdmmc_card_t *card;
    const char mount_point[] = MOUNT_POINT;
    ESP_LOGI(TAG, "Initializing SD card");

    // Use settings defined above to initialize SD card and mount FAT filesystem.
    // Note: esp_vfs_fat_sdmmc/sdspi_mount is all-in-one convenience functions.
    // Please check its source code and implement error recovery when developing
    // production applications.
    ESP_LOGI(TAG, "Using SPI peripheral");

    sdmmc_host_t host = SDSPI_HOST_DEFAULT();
    spi_bus_config_t bus_cfg = {
        .mosi_io_num = PIN_NUM_MOSI,
        .miso_io_num = PIN_NUM_MISO,
        .sclk_io_num = PIN_NUM_CLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 4000,
    };
    ret = spi_bus_initialize(host.slot, &bus_cfg, SDSPI_DEFAULT_DMA);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize bus.");
        return;
    }

    // This initializes the slot without card detect (CD) and write protect (WP) signals.
    // Modify slot_config.gpio_cd and slot_config.gpio_wp if your board has these signals.
    sdspi_device_config_t slot_config = SDSPI_DEVICE_CONFIG_DEFAULT();
    slot_config.gpio_cs = PIN_NUM_CS;
    slot_config.host_id = host.slot;

    ESP_LOGI(TAG, "Mounting filesystem");
    ret = esp_vfs_fat_sdspi_mount(mount_point, &host, &slot_config, &mount_config, &card);

    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            ESP_LOGE(TAG, "Failed to mount filesystem. "
                     "If you want the card to be formatted, set the CONFIG_EXAMPLE_FORMAT_IF_MOUNT_FAILED menuconfig option.");
        } else {
            ESP_LOGE(TAG, "Failed to initialize the card (%s). "
                     "Make sure SD card lines have pull-up resistors in place.", esp_err_to_name(ret));
        }
        return;
    }
    ESP_LOGI(TAG, "Filesystem mounted");

    // Card has been initialized, print its properties
    sdmmc_card_print_info(stdout, card);

    // Use POSIX and C standard library functions to work with files.

    // First create a file.
    const char *file_hello = MOUNT_POINT"/test.txt";

    ESP_LOGI(TAG, "Opening file %s", file_hello);
    FILE *f = fopen(file_hello, "w");
    if (f == NULL) {
        ESP_LOGE(TAG, "Failed to open file for writing");
        return;
    }
    fprintf(f, "Hello %s!\n", card->cid.name);
    printf("Hello  %s!\n", card->cid.name);
    fclose(f);
    ESP_LOGI(TAG, "File written");

    const char *file_foo = MOUNT_POINT"/foo.txt";

    // Check if destination file exists before renaming
    struct stat st;
    if (stat(file_foo, &st) == 0) {
        // Delete it if it exists
        unlink(file_foo);
    }

    // Rename original file
    ESP_LOGI(TAG, "Renaming file %s to %s", file_hello, file_foo);
    if (rename(file_hello, file_foo) != 0) {
        ESP_LOGE(TAG, "Rename failed");
        return;
    }

    // Open renamed file for reading
    ESP_LOGI(TAG, "Reading file %s", file_foo);
    f = fopen(file_foo, "r");
    if (f == NULL) {
        ESP_LOGE(TAG, "Failed to open file for reading");
        return;
    }

    // Read a line from file
    char line[64];
    fgets(line, sizeof(line), f);
    fclose(f);

    // Strip newline
    char *pos = strchr(line, '\n');
    if (pos) {
        *pos = '\0';
    }
    ESP_LOGI(TAG, "Read from file: '%s'", line);
    printf("Data: %s\n", line);

    // All done, unmount partition and disable SPI peripheral
    esp_vfs_fat_sdcard_unmount(mount_point, card);
    ESP_LOGI(TAG, "Card unmounted");

    //deinitialize the bus after all devices are removed
    spi_bus_free(host.slot);

}

void app_main(void)
{
   /* Create mutex for i2c devices */
   ESP_ERROR_CHECK(i2cdev_init());

   xTaskCreate(&sdcardTask, "SDCARD Task", 2048, NULL, 5, NULL);
   xTaskCreate(&lcdTask, "LCD task", 2048, NULL, 3, NULL);
   xTaskCreate(&bmp180Task, "BMP180 Task", 1920, NULL, 4, NULL);
   xTaskCreate(&rtcTask, "RTC Task", 2048, NULL, 4, NULL);
}