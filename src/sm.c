#include <stdio.h>
#include "sm.h"

void handle(sm_t *sm, uint8_t sig)
{
	sm_op_e status;

	status = sm->state(sm, sig);
	assert_param(status != SM_NOT_HANDLED);

	if(status == SM_TRANSITION)
	{
		status = sm->state(sm, SIG_EXIT);
		assert_param(status == SM_HANDLED);

		sm->state = sm->next_state;

		status = sm->state(sm, SIG_ENTRY);
		assert_param(status == SM_HANDLED);

	}

}
