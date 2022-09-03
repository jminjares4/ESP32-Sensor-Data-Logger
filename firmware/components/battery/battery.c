/**
 * @file battery.c
 * @author Jesus Minjares (https://github.com/jminjares4)
 * @brief Battery driver
 * @version 0.1
 * @date 2022-09-01
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "battery.h"
#include "esp_idf_version.h"

#define ADC_CHANNEL_DEFAULT ADC1_CHANNEL_7       /*!< ADC channel default */
#define ADC_WIDTH_DEFAULT ADC_WIDTH_BIT_12       /*!< ADC width default  */
#define ADC_ATTENUATION_DEFAULT ADC_ATTEN_DB_2_5 /*!< ADC attentuation default */
#define BATTERY_ENABLE_DEFAULT 26                /*!< Battery gpio default  */

/* Voltage divider */
#define R2 100
#define R3 10

/* Battery max and min voltage */
#define V_MAX 4200
#define V_MIN 3300

/**
 * @brief Voltage output from divider
 *
 * @param vin   voltage input
 * @return int  voltage
 */
static int voltage_divider(int vin)
{
    return (vin * R3) / (R2 + R3);
}

/**
 * @brief Voltage to adc reading
 *
 * @param value     voltage
 * @param battery   pointer to a const battery object
 * @return int      adc value
 */
static int voltage_to_adc(int value, battery_t *const battery)
{
    /* Get voltage reference from attenuation */
    int vref;
    switch (battery->attenuation)
    {
    case ADC_ATTEN_DB_0:
        vref = 800;
        break;
    case ADC_ATTEN_DB_2_5:
        vref = 1100;
        break;
    case ADC_ATTEN_DB_6:
        vref = 1350;
        break;
    case ADC_ATTEN_DB_11:
        vref = 2600;
        break;
    default:
        vref = 1100;
    }
    /* Get bit resolution from width */
    int bit_res;
    switch (battery->width)
    {
    case ADC_WIDTH_BIT_9:
        bit_res = 512;
        break;
    case ADC_WIDTH_BIT_10:
        bit_res = 1024;
        break;
    case ADC_WIDTH_BIT_11:
        bit_res = 2048;
        break;
    case ADC_WIDTH_BIT_12:
        bit_res = 4096;
        break;
    default:
        bit_res = 4096;
    }

    /* Get conversion: raw adc to voltage */
    return ((vref * value) / bit_res);
}

/**
 * @brief Battery percentage
 *
 * @param battery   pointer to a const battery object
 * @return int percent value
 */
int battery_percentage(battery_t *const battery)
{
    /* Convert battery voltage min to adc */
    int battery_min = voltage_to_adc(voltage_divider(V_MIN), battery);
    /* Convert battery voltage max to adc */
    int battery_max = voltage_to_adc(voltage_divider(V_MAX), battery);

    /* Get battery percentage */
    int battery_percentage = 100 * (battery->value - battery_min) / (battery_max - battery_min);

    /* Set boundaries */
    if (battery_percentage < 0)
        battery_percentage = 0;
    if (battery_percentage > 100)
        battery_percentage = 100;

    return battery_percentage;
}

/**
 * @brief Battery object constructor
 *
 * @param battery pointer to a const battery object
 * @param ch      adc channel
 * @param width   adc width
 * @param atten   adc attentuation
 * @param enable  battery gpio
 * @warning Uses ADC1, please @see battery_t
 */
void battery_ctor(battery_t *const battery, adc1_channel_t ch,
                  adc_bits_width_t width, adc_atten_t atten,
                  gpio_num_t enable)
{
    /* Set battery configuration */
    battery->adc_ch = ch;
    battery->width = width;
    battery->attenuation = atten;
    battery->enable = enable;
}

/**
 * @brief Set battery to default configuration
 *
 * @param battery pointer to a const battery object
 * @note  Please @see battery_ctor()
 * @warning Uses ADC1, please @see battery_t
 */
void battery_default(battery_t *const battery)
{
    battery_ctor(battery, ADC_CHANNEL_DEFAULT, ADC_WIDTH_DEFAULT,
                 ADC_ATTENUATION_DEFAULT, BATTERY_ENABLE_DEFAULT);
}

/**
 * @brief Enable battery reading
 *
 * @param battery pointer to const battery object
 */
void battery_enable(battery_t *const battery)
{
    gpio_set_level(battery->enable, 1);
}

/**
 * @brief Disable battery reading
 *
 * @param battery pointer to const battery object
 */
void battery_disable(battery_t *const battery)
{
    gpio_set_level(battery->enable, 0);
}

/**
 * @brief Initialize battery configuration: adc and gpio
 *
 * @param battery   pointer to a const battery object
 * @return esp_err_t status
 * @note    For esp_err_t @see esp_err.h
 * @warning Please use battery_ctor() or battery_default() prior of this function.
 */
esp_err_t battery_init(battery_t *const battery)
{
    /* Check esp-idf version */
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 0, 0)
    /* Select gpio pad */
    esp_rom_gpio_pad_select_gpio(battery->enable);
#else
    /* Select gpio pad */
    gpio_pad_select_gpio(battery->enable);
#endif
    /* Set gpio direction */
    gpio_set_direction(battery->enable, GPIO_MODE_OUTPUT);
    /* Set gpio low */
    gpio_set_level(battery->enable, 0);

    /* Configure adc1 width*/
    esp_err_t error = adc1_config_width(battery->width);

    /* Check if error */
    if (error != ESP_OK)
    {
        return error;
    }

    /* Configure adc channel attenuation */
    error = adc1_config_channel_atten(battery->adc_ch, battery->attenuation);

    /* Check if error */
    if (error != ESP_OK)
    {
        return error;
    }

    return ESP_OK;
}

/**
 * @brief Get battery voltage reading
 *
 * @param battery pointer to a const battery object
 * @return int    voltage reading
 * @note   reading will be stored in battery.value
 */
int battery_read(battery_t *const battery)
{
    return battery->value = adc1_get_raw(battery->adc_ch);
}
