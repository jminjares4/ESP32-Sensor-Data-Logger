#ifndef _SD_CARD_H_
#define _SD_CARD_H_

// Pin assignments can be set in menuconfig, see "SD SPI Example Configuration" menu.
// You can also change the pin assignments here by changing the following 4 lines.
#define PIN_NUM_MISO 19
#define PIN_NUM_MOSI 23
#define PIN_NUM_CLK 18
#define PIN_NUM_CS 13

const char *SD_CARD_TAG = "SDCARD";

#define MOUNT_POINT "/sdcard"

#endif