/**
 * @file led.c
 * @author Jesus Minjares (https://github.com/jminjares4)
 * @brief Custom LED driver source file
 * @version 0.1
 * @date 2022-07-25
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "led.h"

#define LED_LOW     0  /*!< LED low logic */
#define LED_HIGH    1  /*!< LED high logic */

/**
 * @brief LED enable 
 * 
 * @param led pointer to LED object
 * @note The function assume that the user has initialize the members 
 *       of the object.
 * @return None
 */
void led_enable(led_t * const led){
    /* Select gpio pin */
    esp_rom_gpio_pad_select_gpio(led->pin);
    /* Set direction */
    gpio_set_direction(led->pin, GPIO_MODE_OUTPUT);
    /* Set level */
    gpio_set_level(led->pin, LED_LOW );
    /* Update state */
    led->state = (state_t) OFF;
}

/**
 * @brief Turn on LED object pin
 * 
 * @param led pointer to LED object
 * @note  Function will update the state of LED
 * @return None
 */
void led_on(led_t * const led){
    gpio_set_level(led->pin, LED_HIGH);
    led->state = ON;
}

/**
 * @brief Turn off LED object pin
 * 
 * @param led pointer to LED object
 * @note  Function will update the state of LED
 * @return None
 */
void led_off(led_t * const led){
    gpio_set_level(led->pin, LED_LOW);
    led->state = OFF;
}

/**
 * @brief Toggle LED object pin
 * 
 * @param led pointer to LED object
 * @note  Function will update the state of LED
 * @return None
 */
void led_toggle(led_t * const led){
    switch(led->state){
        case OFF:
            led_on(led);
            break;
        case ON:
            led_off(led);
            break;
        default:
    }
}
