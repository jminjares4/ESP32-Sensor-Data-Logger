/**
 * @file esp_lcd.h
 * @author Jesus Minjares (https://github.com/jminjares4)
 * @brief Liquid Crystal Display header file
 * @version 0.1
 * @date 2022-08-15
 *
 * @copyright Copyright (c) 2022
 *
 */
#ifndef _ESP_LCD_H_
#define _ESP_LCD_H_

#include "driver/gpio.h"

/* LCD Error */
typedef int lcd_err_t;      /*!< LCD error type */

#define LCD_FAIL            -1  /*!< LCD fail error */
#define LCD_OK               0  /*!< LCD success    */


#define LCD_DATA_LINE 4 /*!< 4-Bit data line */

typedef enum {
    LCD_INACTIVE = 0,   /*!< LCD inactive */
    LCD_ACTIVE = 1,     /*!< LCD active   */
}lcd_state_t;

typedef struct
{
    gpio_num_t data[LCD_DATA_LINE]; /*!< LCD data line  */
    gpio_num_t en;                  /*!< LCD enable pin */
    gpio_num_t regSel;              /*!< LCD register select */
    lcd_state_t state;              /*!< LCD state  */
} lcd_t;

void lcdDefault(lcd_t *const lcd);

void lcdInit(lcd_t *const lcd);

void lcdCtor(lcd_t *lcd, gpio_num_t data[LCD_DATA_LINE], gpio_num_t en, gpio_num_t regSel);

lcd_err_t lcdSetText(lcd_t *const lcd, char *text, int x, int y);

lcd_err_t lcdSetInt(lcd_t *const lcd, int val, int x, int y);

lcd_err_t lcdClear(lcd_t *const lcd);

void lcdFree(lcd_t * const lcd);

#endif
