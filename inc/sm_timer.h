#ifndef INC_SM_TIMER_H_
#define INC_SM_TIMER_H_

#include "sm_list.h"
#include "sm.h"

typedef struct sm_timer sm_timer_t;

struct sm_timer {
	/* Item in the global list of all timers. */
	sm_list_item_t item;
	/* The deadline when the timer expires. -1 if the timer is not active. */
	int64_t expiry;
	uint8_t sig;
};

#define sm_timer_enabled(tm)  ((tm)->expiry >= 0)
void sm_timer_add(sm_timer_t *timer, uint32_t delay, uint8_t sig);

void sm_timer_rm(sm_timer_t *timer);

int sm_timer_next(void);

void sm_timer_fire(void);

#endif /* INC_SM_TIMER_H_ */
