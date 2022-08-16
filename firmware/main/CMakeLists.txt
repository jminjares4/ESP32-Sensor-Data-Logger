# Set all source drivers 
set(src_driver "driver/src/button.c"
          "driver/src/led.c"
          "driver/src/gps.c"
          "driver/src/sd_card.c"
)

idf_component_register(SRCS "main.c"
                            ${src_driver}
                       INCLUDE_DIRS ".")
