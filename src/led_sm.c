#include "led_sm.h"

#include "sm.h"
#include "sm_timer.h"

struct _led_sm_t {
	sm_t sm;
	sm_timer_t timer;
	uint16_t delay;
};

static sm_op_e led_sm_off_state(sm_t *sm, uint8_t sig);
static sm_op_e led_sm_on_state(sm_t *sm, uint8_t sig);

static sm_op_e led_sm_off_state(sm_t *sm, uint8_t sig)
{
	led_sm_t *self = CONTAINER_OF(sm, led_sm_t, sm);

	switch(sig)
	{
	case SIG_ENTRY:
		HAL_GPIO_WritePin(LD2_GPIO_Port,
				LD2_Pin, GPIO_PIN_RESET);
		return SM_HANDLED;

	case SIG_EXTERNAL+sig_led_req: {
		SIG_DEACTIVATE(sig_led_req);
		sm_transition(sm, led_sm_on_state);
	}
	break;

	case SIG_EXTERNAL+sig_led_timer: {
		SIG_DEACTIVATE(sig_led_timer);
		return SM_HANDLED;
	}
	break;

	case SIG_EXIT:
		return SM_HANDLED;

	}

	return SM_NOT_HANDLED;
}

static sm_op_e led_sm_on_state(sm_t *sm, uint8_t sig)
{
	led_sm_t *self = CONTAINER_OF(sm, led_sm_t, sm);

	switch(sig)
	{
	case SIG_ENTRY:
		HAL_GPIO_WritePin(LD2_GPIO_Port,
				LD2_Pin, GPIO_PIN_SET);
		sm_timer_add(&self->timer, self->delay, sig_led_timer);
		return SM_HANDLED;

	case SIG_EXTERNAL+sig_led_timer: {
		SIG_DEACTIVATE(sig_led_timer);
		sm_transition(sm, led_sm_off_state);
	}
	break;

	case SIG_EXTERNAL+sig_led_req: {
		SIG_DEACTIVATE(sig_led_req);
		return SM_HANDLED;
	}
	break;

	case SIG_EXIT:
		return SM_HANDLED;

	}

	return SM_NOT_HANDLED;
}

led_sm_t led_sm = {
		.sm = {
				.state = led_sm_off_state,
				.reg_sig = SIG_GET_REG(sig_led_timer) |
				SIG_GET_REG(sig_led_req),
		},
};

void led_blink(led_sm_t *self, uint16_t delay)
{
	self->delay = delay;
	SIG_ACTIVATE(sig_led_req);
}
