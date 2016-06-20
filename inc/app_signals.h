#ifndef INC_APP_SIGNALS_H_
#define INC_APP_SIGNALS_H_

#include <stdint.h>

#define TICKS_PER_SEC		(1024)

#define SIG_REG_EMPTY 		(0x0000)

#define SIG_ACTIVATE(_s) do { \
		__disable_irq(); \
		signals |= (1UL << _s) & en_sig_reg; \
		__enable_irq(); \
} while(0)

#define SIG_DEACTIVATE(_s) do { \
		__disable_irq(); \
		signals &= ~(1UL << _s); \
		__enable_irq(); \
} while(0)

#define SIG_IS_ACTIVE(_s) (signals & (1UL << _s))
#define SIG_GET_REG(_s) (1UL << _s)

typedef enum {
	sig_tick = 0,
	sig_bin,
	sig_i2c_done,
	sig_uart_trace_req,
	sig_uart_trace_done,
	sig_led_timer,
	sig_led_req,
	sig_last
} signal_e;

volatile uint16_t signals;
uint16_t en_sig_reg;
uint32_t global_time;

#endif /* INC_APP_SIGNALS_H_ */
