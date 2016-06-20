#include "app_signals.h"

#include "sm.h"
#include "trace_dcf77.h"

#define TRACE_BLOCK_SIZE		(16)

extern UART_HandleTypeDef huart2;

typedef struct {
	sm_t sm;
	uint8_t buff[TRACE_BLOCK_SIZE];
	uint8_t enabled;
} uart_trace_sm_t;

static sm_op_e uart_trace_active(sm_t *sm, uint8_t sig);

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	SIG_ACTIVATE(sig_uart_trace_done);
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	SIG_ACTIVATE(sig_uart_trace_done);
}

static sm_op_e uart_trace_active(sm_t *sm, uint8_t sig)
{
	uart_trace_sm_t *self = CONTAINER_OF(sm, uart_trace_sm_t, sm);
	HAL_StatusTypeDef s;
	size_t remaining;

	switch(sig)
	{
	case SIG_EXIT:
		return SM_HANDLED;

	case SIG_ENTRY:
		return SM_HANDLED;

	case SIG_EXTERNAL+sig_uart_trace_req: {
		if(!self->enabled)
		{
			remaining = trace_dcf77_get(self->buff, TRACE_BLOCK_SIZE);
			if(remaining > 0)
			{
				s = HAL_UART_Transmit_IT(&huart2, self->buff, remaining);
				assert_param(s == HAL_OK);
				self->enabled = 1;
			}
		}
		SIG_DEACTIVATE(sig_uart_trace_req);
	}
	return SM_HANDLED;

	case SIG_EXTERNAL+sig_uart_trace_done: {
		assert_param(self->enabled);
		if(self->enabled)
		{
			remaining = trace_dcf77_get(self->buff, TRACE_BLOCK_SIZE);
			if(remaining > 0)
			{
				s = HAL_UART_Transmit_IT(&huart2, self->buff, remaining);
				assert_param(s == HAL_OK);
			} else {
				self->enabled = 0;
			}
		}
		SIG_DEACTIVATE(sig_uart_trace_done);
	}
	return SM_HANDLED;
	}

	return SM_NOT_HANDLED;
}

uart_trace_sm_t uart_trace_sm = {
		.sm = {
				.state = uart_trace_active,
				.reg_sig = SIG_GET_REG(sig_uart_trace_req)
				| SIG_GET_REG(sig_uart_trace_done)
		},
};
