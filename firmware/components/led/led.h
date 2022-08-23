/**
 * @file led.h
 * @author Jesus Minjares (https://github.com/jminjares4)
 * @brief Custom LED driver header file
 * @version 0.1
 * @date 2022-07-25
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef _LED_H_
#define _LED_H_

#include "driver/gpio.h"

/******************************************************************
 * \enum state_t lcd.h 
 * \brief LED state enumeration
 *******************************************************************/
typedef enum {
    OFF = 0, /*!< LED OFF */
    ON = 1,  /*!< LED ON */
} state_t;

/******************************************************************
 * \struct led_t lcd.h 
 * \brief LED object
 * 
 * ### Example
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~.c
 * typedef struct {
 *      gpio_num_t pin;
 *      state_t state;
 * }led_t;
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *******************************************************************/
typedef struct {
    gpio_num_t pin; /*!< LED pin */
    state_t state;  /*!< LED state */
} led_t;

void led_enable(led_t * const led);

void led_on(led_t * const led);

void led_off(led_t * const led);

void led_toggle(led_t * const led);

#endif