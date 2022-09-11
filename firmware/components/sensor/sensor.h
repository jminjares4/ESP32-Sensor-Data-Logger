#ifndef _SENSOR_H_
#define _SENSOR_H_

#include "freertos/queue.h"

/* Queue */
typedef enum
{
   NO_SENSOR = -1,
   BMP180_SENSOR = 0,
   DS3231_SENSOR = 1,
} sensor_event_t;

typedef struct
{
   sensor_event_t event;
   void *data0;
   void *data1;
} data_t;

extern QueueHandle_t pressureSensorQueue;
extern QueueHandle_t realTimeClockQueue;

#define QUEUE_SIZE 10

#endif