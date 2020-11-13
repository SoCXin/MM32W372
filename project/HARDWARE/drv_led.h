#ifndef __DRV_LED_H__
#define __DRV_LED_H__

#include "sys.h"

/*
LD1 - PA15
LD2 - PC10
LD3 - PC11
LD4 - PC12
*/

#define LD1_OUT PAout(15)
#define LD2_OUT PCout(10)
#define LD3_OUT PCout(11)
#define LD4_OUT PCout(12)

typedef enum {
    ON = 0,
    OFF
}status;

void led_init(void);
void led_blink(uint16_t time);
void led_set(uint8_t num, status sts);
void led_all_off(void);
void led1_toggle(void);


#endif

