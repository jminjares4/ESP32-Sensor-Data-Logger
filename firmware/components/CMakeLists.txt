set(component_srcs  "button/button.c"
                    "led/led.c"
                    "lcd/esp_lcd.c" 
                    "sdcard/sd_card.c"
)

idf_component_register(SRCS "${component_srcs}"
                       PRIV_REQUIRES driver
                       INCLUDE_DIRS ".")