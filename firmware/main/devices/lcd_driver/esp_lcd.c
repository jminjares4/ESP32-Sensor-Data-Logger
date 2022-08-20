/**
 * @file esp_lcd.c
 * @author Jesus Minjares (https://github.com/jminjares4)
 * @brief Liquid Crystal Display source file
 * @version 0.1
 * @date 2022-08-15
 *
 * @copyright Copyright (c) 2022
 *
 */

#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_idf_version.h"
#include "esp_lcd.h"

#define LCD_DATA 0        /*!< LCD data */
#define LCD_CMD 1         /*!< LCD command */
#define GPIO_STATE_LOW 0  /*!< Logic low */
#define GPIO_STATE_HIGH 1 /*!< Logic high */

/* Default pinout  */
#define DATA_0_PIN 19          /*!< DATA 0 */
#define DATA_1_PIN 18          /*!< DATA 0 */
#define DATA_2_PIN 17          /*!< DATA 0 */
#define DATA_3_PIN 16          /*!< DATA 0 */
#define ENABLE_PIN 22          /*!< Enable  */
#define REGISTER_SELECT_PIN 23 /*!< Register Select  */

/**
 * @brief Trigger LCD enable pin
 *
 * @param lcd   pointer to LCD object
 * @return None
 */
static void lcdTriggerEN(lcd_t *const lcd)
{
    gpio_set_level(lcd->en, GPIO_STATE_LOW);
    vTaskDelay(1 / portTICK_PERIOD_MS);
    gpio_set_level(lcd->en, GPIO_STATE_HIGH);
    vTaskDelay(1 / portTICK_PERIOD_MS);
    gpio_set_level(lcd->en, GPIO_STATE_LOW);
    vTaskDelay(1 / portTICK_PERIOD_MS);
}

/**
 * @brief Transmitt lower nibble
 *
 * @param lcd   pointer to LCD object
 * @param x     bits
 * @return None
 */
static void lownibble(lcd_t *const lcd, unsigned char x)
{
    uint8_t i, val = 0x01;
    for (i = 0; i < LCD_DATA_LINE; i++)
    {
        /* check if x is high for every bit */
        gpio_set_level(lcd->data[i], (x >> i) & val);
    }
}

/**
 * @brief Write command to LCD object
 *
 * @param lcd       pointer to LCD object
 * @param cmd       LCD command
 * @param lcd_opt   0: data , 1: command
 * @return None
 */
static void lcdWriteCmd(lcd_t *const lcd, unsigned char cmd, uint8_t lcd_opt)
{
    /* CMD: 1, DATA: 0 */
    (lcd_opt == LCD_CMD) ? gpio_set_level(lcd->regSel, GPIO_STATE_LOW) : gpio_set_level(lcd->regSel, GPIO_STATE_HIGH);

    /* upper bits */
    lownibble(lcd, cmd >> 4);
    lcdTriggerEN(lcd);

    /* lower bits */
    lownibble(lcd, cmd);
    lcdTriggerEN(lcd);

    /* CMD : 1, DATA: 0 */
    if (lcd_opt == LCD_CMD)
    {
        vTaskDelay(10 / portTICK_PERIOD_MS); /* 10 ms delay */
    }
}

/**
 * @brief Initialize LCD object
 *
 * @param lcd   pointer to LCD object
 * @note  Must constructor LCD object. @see lcd_ctor() and @see lcd_default()
 * @return None
 */
void lcdInit(lcd_t *const lcd)
{
    /* 100 ms delay */
    vTaskDelay(100 / portTICK_PERIOD_MS);

    /* set 0x03 to LCD */
    gpio_set_level(lcd->en, GPIO_STATE_LOW);
    gpio_set_level(lcd->regSel, GPIO_STATE_LOW);
    gpio_set_level(lcd->data[0], GPIO_STATE_HIGH);
    gpio_set_level(lcd->data[1], GPIO_STATE_HIGH);
    gpio_set_level(lcd->data[2], GPIO_STATE_LOW);
    gpio_set_level(lcd->data[3], GPIO_STATE_LOW);

    /* Send 0x03 3 times at 10ms then 100 us */
    lcdTriggerEN(lcd);
    vTaskDelay(10 / portTICK_PERIOD_MS);
    lcdTriggerEN(lcd);
    vTaskDelay(10 / portTICK_PERIOD_MS);
    lcdTriggerEN(lcd);
    vTaskDelay(10 / portTICK_PERIOD_MS);

    /* switch to 4-bit mode, 0x02 */
    gpio_set_level(lcd->en, GPIO_STATE_LOW);
    gpio_set_level(lcd->regSel, GPIO_STATE_LOW);
    gpio_set_level(lcd->data[0], GPIO_STATE_LOW);
    gpio_set_level(lcd->data[1], GPIO_STATE_HIGH);
    gpio_set_level(lcd->data[2], GPIO_STATE_LOW);
    gpio_set_level(lcd->data[3], GPIO_STATE_LOW);

    /* Trigger enable */
    lcdTriggerEN(lcd);
    vTaskDelay(10 / portTICK_PERIOD_MS);

    /* Initialize LCD */
    lcdWriteCmd(lcd, 0x28, LCD_CMD); // 4-bit, 2 line, 5x8
    lcdWriteCmd(lcd, 0x08, LCD_CMD); // Instruction Flow
    lcdWriteCmd(lcd, 0x01, LCD_CMD); // Clear LCD
    lcdWriteCmd(lcd, 0x06, LCD_CMD); // Auto-Increment
    lcdWriteCmd(lcd, 0x0C, LCD_CMD); // Display On, No blink
}

/**
 * @brief LCD default constructor
 * @param lcd    pointer to LCD object
 * @note  This function call will use hardcode pins. If you want
 *        to use custom pins @see lcd_ctor
 *
 * @return None
 */
void lcdDefault(lcd_t *const lcd)
{

    /* Default pins */
    gpio_num_t data[LCD_DATA_LINE] = {DATA_0_PIN, DATA_1_PIN, DATA_2_PIN, DATA_3_PIN}; /* Data pins */
    gpio_num_t en = ENABLE_PIN;                                                        /* Enable pin */
    gpio_num_t regSel = REGISTER_SELECT_PIN;                                           /* Register Select pin */

    /* Instantiate lcd object with default pins */
    lcdCtor(lcd, data, en, regSel);
}

/**
 * @brief LCD constructor
 *
 * Detailed description starts here
 * @param lcd       pointer to LCD object
 * @param data      lcd data array
 * @param en        lcd en
 * @param regSel    register select
 * @return          None
 */
void lcdCtor(lcd_t *lcd, gpio_num_t data[LCD_DATA_LINE], gpio_num_t en, gpio_num_t regSel)
{
    /* Map each data pin to LCD object */
    int i;
    for (i = 0; i < LCD_DATA_LINE; i++)
    {
        lcd->data[i] = data[i];
    }

    /* Map enable and register select pin */
    lcd->en = en;
    lcd->regSel = regSel;

#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 0, 0)
    /* Select en and register select pin */
    esp_rom_gpio_pad_select_gpio(lcd->en);
    esp_rom_gpio_pad_select_gpio(lcd->regSel);
#else
    /* Select en and register select pin */
    gpio_pad_select_gpio(lcd->en);
    gpio_pad_select_gpio(lcd->regSel);
#endif

    /* Set en and register select pin as output */
    gpio_set_direction(lcd->en, GPIO_MODE_OUTPUT);
    gpio_set_direction(lcd->regSel, GPIO_MODE_OUTPUT);

    /* Set en and register select pin as low */
    gpio_set_level(lcd->en, GPIO_STATE_LOW);
    gpio_set_level(lcd->regSel, GPIO_STATE_LOW);

    /* Select all data pins */
    for (i = 0; i < LCD_DATA_LINE; i++)
    {
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 0, 0)
        esp_rom_gpio_pad_select_gpio(lcd->data[i]);
#else
        gpio_pad_select_gpio(lcd->data[i]);
#endif
    }
    /* Set all data pins as output */
    for (i = 0; i < LCD_DATA_LINE; i++)
    {
        gpio_set_direction(lcd->data[i], GPIO_MODE_OUTPUT);
    }
    /* Set all data pins output as low */
    for (i = 0; i < LCD_DATA_LINE; i++)
    {
        gpio_set_level(lcd->data[i], GPIO_STATE_LOW);
    }

    lcd->state = (lcd_state_t)LCD_ACTIVE;
}

/**
 * @brief Set text
 *
 * Detailed description starts here
 * @param lcd   pointer to LCD object
 * @param text  string text
 * @param x     location at x-axis
 * @param y     location at y-axis
 * @return      lcd error status @see lcd_err_t
 */
lcd_err_t lcdSetText(lcd_t *const lcd, char *text, int x, int y)
{
    /* Check if lcd is active */
    if (lcd->state == LCD_ACTIVE)
    {
        int i;
        if (x < 16)
        {
            x |= 0x80; // Set LCD for first line write
            switch (y)
            {
            case 1:
                x |= 0x40; // Set LCD for second line write
                break;
            case 2:
                x |= 0x60; // Set LCD for first line write reverse
                break;
            case 3:
                x |= 0x20; // Set LCD for second line write reverse
                break;
            }
            lcdWriteCmd(lcd, x, LCD_CMD);
        }
        i = 0;
        /* Write text */
        while (text[i] != '\0')
        {
            lcdWriteCmd(lcd, text[i], LCD_DATA);
            i++;
        }
    }
    /* return lcd status */
    return lcd->state == LCD_ACTIVE ? LCD_OK : LCD_FAIL;
}

/**
 * @brief Set integer
 *
 * Detailed description starts here
 * @param lcd   pointer to LCD object
 * @param val   integer value to be displayed
 * @param x     location at x-axis
 * @param y     location at y-axis
 * @return      lcd error status @see lcd_err_t
 */
lcd_err_t lcdSetInt(lcd_t *const lcd, int val, int x, int y)
{
    /* Check if lcd is active */
    if (lcd->state == LCD_ACTIVE)
    {
        /* Store integer to buffer */
        char buffer[16];
        sprintf(buffer, "%d", val);
        /* Set integer */
        lcdSetText(lcd, buffer, x, y);
    }
    /* return lcd status */
    return lcd->state == LCD_ACTIVE ? LCD_OK : LCD_FAIL;
}

/**
 * @brief Clear LCD screen
 * Detailed description starts here
 * @param lcd   pointer to LCD object
 * @return      lcd error status @see lcd_err_t 
 */
lcd_err_t lcdClear(lcd_t *const lcd)
{
    /* Check if lcd is active */
    if (lcd->state == LCD_ACTIVE)
    {
        /* Clear LCD screen */
        lcdWriteCmd(lcd, 0x01, LCD_CMD);
    }

    /* return lcd status */
    return lcd->state == LCD_ACTIVE ? LCD_OK : LCD_FAIL;
}

/**
 * @brief Reset pins to default configuration. Freeing GPIO pins.
 * @param lcd   pointer to LCD object
 * @note        This function will set GPIO pins to reset configuration,
 *              disabling the LCD. @see gpio_reset_pin()
 */
void lcdFree(lcd_t *const lcd)
{
    /* Reset data pins to default configuration */
    for (int i = 0; i < LCD_DATA_LINE; i++)
    {
        gpio_reset_pin(lcd->data[i]);
    }
    /* Reset enable pin to default configuration */
    gpio_reset_pin(lcd->en);
    /* Reset register select pin to default configuration */
    gpio_reset_pin(lcd->regSel);

    /* Update gpio pins to no connection */
    for (int i = 0; i < LCD_DATA_LINE; i++)
    {
        lcd->data[i] = GPIO_NUM_NC; /* Set to no connection */
    }

    lcd->en = GPIO_NUM_NC;     /* Set to no connection */
    lcd->regSel = GPIO_NUM_NC; /* Set to no connection */

    lcd->state = (lcd_state_t)LCD_INACTIVE;
}
