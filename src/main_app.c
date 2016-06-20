#include "main_app.h"

#include "stm32l0xx_hal.h"
#include "app_signals.h"
#include "sm.h"
#include "trace_dcf77.h"

sm_t *const machines[] = {
		&bin_tick_sm,
		&rtc_sm,
		&uart_trace_sm,
		&led_sm,
};

void main_app(void)
{
	uint8_t i,j;

	trace_dcf77_startup();

	en_sig_reg |=
			SIG_GET_REG(sig_tick)
			| SIG_GET_REG(sig_led_timer)
			| SIG_GET_REG(sig_led_req)
			| SIG_GET_REG(sig_i2c_done)
			| SIG_GET_REG(sig_uart_trace_req)
			| SIG_GET_REG(sig_uart_trace_done);

	for(i=0; i<ARRAY_SIZE(machines); i++)
	{
		handle(machines[i], SIG_ENTRY);
	}

	while(1)
	{
		__disable_irq();
		if(!(signals & en_sig_reg))
		{
			__enable_irq();
			HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_STOPENTRY_WFI);
		} else
		{
			__enable_irq();
		}

		while(signals & en_sig_reg)
		{
			if(SIG_IS_ACTIVE(sig_tick))
			{
				global_time++;
				SIG_ACTIVATE(sig_bin);
				SIG_DEACTIVATE(sig_tick);
				if(sm_timer_next() == 0)
				{
					sm_timer_fire();
				}
			}

			for(i=0; i<ARRAY_SIZE(machines); i++)
			{
				uint16_t active_sigs = machines[i]->reg_sig & signals & en_sig_reg;
				if(active_sigs)
				{
					for(j=0; j<sig_last;j++)
					{
						if(SIG_GET_REG(j) & active_sigs)
						{
							handle(machines[i], SIG_EXTERNAL + j);
						}
					}
				}
			}
		}
	}
}
