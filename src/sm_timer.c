#include "sm_timer.h"
#include "app_signals.h"

static sm_list_t sm_timers = {0};

void sm_timer_add(sm_timer_t *timer, uint32_t delay, uint8_t sig) {
	timer->expiry = global_time+delay;
	timer->sig = sig;
	/* Move the timer into the right place in the ordered list
       of existing timers. TODO: This is an O(n) operation! */
	sm_list_item_t *it = sm_list_begin(&sm_timers);
	while(it) {
		sm_timer_t *tm = CONTAINER_OF(it, sm_timer_t, item);
		/* If multiple timers expire at the same moment they will be fired
           in the order they were created in (> rather than >=). */
		if(tm->expiry > timer->expiry)
			break;
		it = sm_list_next(it);
	}
	sm_list_insert(&sm_timers, &timer->item, it);
}

void sm_timer_rm(sm_timer_t *timer) {
	sm_list_erase(&sm_timers, &timer->item);
}

int sm_timer_next(void) {
	if(sm_list_empty(&sm_timers))
		return -1;
	uint32_t expiry = CONTAINER_OF(sm_list_begin(&sm_timers),
			sm_timer_t, item)->expiry;
	return (int) (global_time >= expiry ? 0 : expiry - global_time);
}

void sm_timer_fire(void) {
	/* Avoid getting current time if there are no timers anyway. */
	if(sm_list_empty(&sm_timers))
		return;

	while(!sm_list_empty(&sm_timers)) {
		sm_timer_t *tm = CONTAINER_OF(
				sm_list_begin(&sm_timers), sm_timer_t, item);
		if(tm->expiry > global_time)
			break;
		sm_list_erase(&sm_timers, sm_list_begin(&sm_timers));
		SIG_ACTIVATE(tm->sig);
	}
}
