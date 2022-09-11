#ifndef _TIMER_H_
#define _TIMER_H_

/* ESP timer period is set by us */
#define HALF_SECOND 500000
#define ONE_SECOND 1000000
#define FIVE_SECOND 5000000
#define THIRTY_SECOND 30000000
#define ONE_MINUTE 60000000
#define FIVE_MINUTE 300000000
#define TEN_MINUTE 600000000

extern void timer_callback(void *arg);

#endif