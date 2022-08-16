set(device_srcs "devices/button.c"
                "devices/gps.c"
                "devices/led.c"
                "devices/sd_card.c"
)

idf_component_register(SRCS "main.c"
                             ${device_srcs}
                       INCLUDE_DIRS ".")
