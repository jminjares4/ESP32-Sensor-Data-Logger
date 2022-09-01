/**
 * @file battery.h
 * @author Jesus Minjares (https://github.com/jminjares4)
 * @brief Battery driver
 * @version 0.1
 * @date 2022-09-01
 *
 * @copyright Copyright (c) 2022
 *
 */
#ifndef _BATTERY_H_
#define _BATTERY_H_

#include "driver/adc.h"
#include "driver/gpio.h"
#include "esp_err.h"

/******************************************************************
 * \struct battery_t battery.h
 * \brief Custom battery_t object
 *
 * ### Example
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~.c
 * typedef struct {
 *      adc1_channel_t adc_ch;
 *      adc_bits_width_t width;
 *      adc_attent_t attentuation;
 *      gpio_num_t enable;
 *      int value;
 * }battery_t;
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *******************************************************************/
typedef struct
{
    adc1_channel_t adc_ch;   /*!< ADC channel */
    adc_bits_width_t width;  /*!< ADC width */
    adc_atten_t attenuation; /*!< ADC attenuation */
    gpio_num_t enable;       /*!< Battery enable pin */
    int value;               /*!< Battery adc value */
} battery_t;

void battery_ctor(battery_t *const battery, adc1_channel_t ch,
                  adc_bits_width_t width, adc_atten_t atten,
                  gpio_num_t enable);

void battery_default(battery_t *const battery);

void battery_enable(battery_t *const battery);

void battery_disable(battery_t *const battery);

esp_err_t battery_init(battery_t *const battery);

int battery_read(battery_t *const battery);

int battery_percentage(battery_t *const battery);

#endif