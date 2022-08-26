set(component_srcs  "button/button.c"
                    "led/led.c"
                    "lcd/esp_lcd.c" 
                    "sdcard/sd_card.c"
)

set(oled_srcs "oled/fonts.c"
              "oled/ssd1306.c")

idf_component_register(SRCS "${component_srcs}"
                            "${oled_srcs}"
                       PRIV_REQUIRES driver
                       INCLUDE_DIRS ".")