/**
 * @file button.h
 * @author Jesus Minjares (https://github.com/jminjares4)
 * @brief  Hardware Application Layer for Buttons
 * @version 0.1
 * @date 2022-08-09
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#ifndef _BUTTON_H_
#define _BUTTON_H_

#include "driver/gpio.h"

#define ESP_INTR_FLAG_DEFAULT 0 //default interrupt flag
#define PULL_SEL_EN 1  // enable selected pull selection

/**
 * @struct button_t button.h
 * @brief Custom button_t structure to automate button setup
 * 
 * This structure allows the user to select its gpio pin, and set for pullup or pulldown base on configuration
 * and lastly if the button will be read as input or interrupt.
 * 
 * ### Example
 * ~~~.c
 * typedef struct{
 *      gpio_num_t pin; // gpio pin number
 *      union{
 *          gpio_pullup_t up; //pullup
 *          gpio_pulldown_t down; //pulldown
 *      }pull_sel; //select configuration 
 *      void (*func)(void *arg); //gpio interrupt routine
 * }button_t;
 * ~~~
 */
typedef struct button_t
{
    gpio_num_t pin; /*!< gpio pin number */
    union
    {
        gpio_pullup_t up;
        gpio_pulldown_t down;
    }pull_sel; /*!< gpio configuration for pullup or pulldown */
    void (*func)(void *arg); /*!< gpio interrupt routine */
} button_t;

/**
 * @brief button_t Initialization for buttons
 * @param button pass a button_t by reference
 * @return None
 * @note    buttonInit() is use when the button or input will be read as high or low level
 *          for interrupt intitialization @see buttonInitIRQ()
 */
void button_enable(button_t * const button);

/**
 * @brief buttonRead will read the gpio pin
 * 
 * @param button pass a button_t by reference
 * @return int  return either high or low || 0 or 1
 */
int read_button(button_t * const button);

/**
 * @brief buttonInitIRQ will intialize button for interrupt routine
 * 
 * @param button pass a button_t by referene
 * @return None
 */
void button_enable_irq(button_t * const button);

/**
 * @brief buttonInterrupt will be an general routine for buttons interrupts.
 * 
 * @param arg interrupt argument
 */
extern void button_interrupt(void *arg);

#endif