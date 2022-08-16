/**
 * @file button.c
 * @author Jesus Minjares (https://github.com/jminjares4)
 * @brief button.h source code
 * @version 0.1
 * @date 2022-08-09
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#include "button.h"
#include <stdio.h>


int gpio_install_count = 0;

/**
 * @brief button_t Initialization for buttons
 * @param button pass a button_t by reference
 * @return None
 * @note    button_enable() is use when the button or input will be read as high or low level
 *          for interrupt intitialization @see button_enable_irq()
 */
void button_enable(button_t * const button)
{
    if (button->func == NULL) //check if button has a routine
    {
        esp_rom_gpio_pad_select_gpio(button->pin); //enable pin
        gpio_set_direction(button->pin, GPIO_MODE_INPUT); //set pin as input
        if (button->pull_sel.up) //check if pull-up is selected
        {
            gpio_set_pull_mode(button->pin, GPIO_PULLUP_ONLY); //enable pull-up
        }
        else //pull-down selected
        {
            gpio_set_pull_mode(button->pin, GPIO_PULLDOWN_ONLY); //enable pull-down
        }
    }else{ //warning
        printf("Changed button->func to NULL\n"); 
        button->func = NULL; //set button to a default routine as NULL
    }
}

/**
 * @brief buttonRead will read the pin pin
 * 
 * @param button pass a button_t by reference
 * @return int  return either high or low || 0 or 1
 */
int read_button(button_t * const button)
{
    return gpio_get_level(button->pin); //get the level of the gpio pin
}

/**
 * @brief buttonInitIRQ will intialize button for interrupt routine
 * 
 * @param button pass a button_t by referene
 * @return None
 */
void button_enable_irq(button_t * const button)
{
    gpio_config_t io_conf; // gpio_config_t to store configuration
    if (button->pull_sel.up)//check if pull-up is selected
    {
        io_conf.intr_type = GPIO_INTR_NEGEDGE; // set as NEGEDGE for interrupt
        io_conf.pull_down_en = 0;              // disable pull-down
        io_conf.pull_up_en = 1;                // enable pull-up
    }
    else  //pulldown selected
    {
        io_conf.intr_type = GPIO_INTR_POSEDGE; // set as NEGEDGE for interrupt
        io_conf.pull_down_en = 1; // enable pull-down
        io_conf.pull_up_en = 0;   // disable pull-up
    }
    io_conf.mode = GPIO_MODE_INPUT;                // set input
    io_conf.pin_bit_mask = (1ULL << button->pin); // set gpio that will be used for input

    gpio_config(&io_conf); //set configuration


    if(gpio_install_count++ == 0)
        gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT); //set default flag for interrupts
    gpio_isr_handler_add(button->pin, button->func, (void *)button->pin); //pass the gpio number, routine and argument for the routine
}