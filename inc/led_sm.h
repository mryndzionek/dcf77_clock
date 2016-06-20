#include "app_signals.h"

#ifndef INC_LED_SM_H_
#define INC_LED_SM_H_

typedef struct _led_sm_t led_sm_t;

void led_blink(led_sm_t *self, uint16_t delay);

#endif /* INC_LED_SM_H_ */
