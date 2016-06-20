#include "smark_detector.h"

#include "app_signals.h"
#include "stm32l0xx_hal.h"
#include "trace_dcf77.h"

#define LOCK_THRESHOLD			(12)

#define INCREMENT(_ptr, _inc) do { \
		if(*_ptr >= 255 - _inc) *_ptr  = 255; else *_ptr += _inc; \
} while(0)

#define DECREMENT(_ptr, _dec) do { \
		if(*_ptr <= 0 + _dec) *_ptr  = 0; else *_ptr -= _dec; \
} while(0)

static void compute_max_index(smark_detector_t *detector) {
	detector->noise_max = 0;
	detector->max = 0;
	detector->max_index = SECONDS_PER_MINUTE + 1;
	for (uint8_t index = 0; index < SECONDS_PER_MINUTE; ++index) {
		uint8_t bin_data = detector->bins[index];

		if (bin_data >= detector->max) {
			detector->noise_max = detector->max;
			detector->max = bin_data;
			detector->max_index = index;
		} else if (bin_data > detector->noise_max) {
			detector->noise_max = bin_data;
		}
	}
}

void smark_detector_process_symbol(smark_detector_t *detector, uint8_t symbol)
{
	uint8_t *tick = &detector->bins[detector->tick];

	uint8_t *previous_tick = detector->tick > 0 ?
			&detector->bins[detector->tick - 1]:
			&detector->bins[SECONDS_PER_MINUTE - 1];

	uint8_t *previous_21_tick = detector->tick > 20 ?
			&detector->bins[detector->tick - 21]:
			&detector->bins[detector->tick + SECONDS_PER_MINUTE - 21];

	switch(symbol)
	{
	case symbol_sync_mark:
		INCREMENT(tick, 6);
		DECREMENT(previous_tick, 2);
		DECREMENT(previous_21_tick, 2);

		uint8_t *next_tick = detector->tick < SECONDS_PER_MINUTE - 1 ?
				&detector->bins[detector->tick + 1]:
				&detector->bins[0];

		DECREMENT(next_tick, 2);
		break;

	case symbol_undefined:
		DECREMENT(tick, 2);
		DECREMENT(previous_tick, 2);
		DECREMENT(previous_21_tick, 2);
		break;

	case symbol_zero:
		INCREMENT(previous_tick, 1);
		DECREMENT(previous_21_tick, 2);
		DECREMENT(tick, 2);
		break;

	case symbol_one:
		INCREMENT(previous_21_tick, 1);
		DECREMENT(previous_tick, 2);
		DECREMENT(tick, 2);
		break;

	}

	detector->tick = detector->tick < SECONDS_PER_MINUTE - 1 ?
			detector->tick + 1 : 0;

	compute_max_index(detector);
	if(detector->locked)
	{
		if (detector->max - detector->noise_max <= LOCK_THRESHOLD)
		{
			detector->locked = 0;
			trace_dcf77_lost_lock();
			SIG_ACTIVATE(sig_uart_trace_req);
		}
	} else {
		if (detector->max - detector->noise_max > LOCK_THRESHOLD)
		{
			detector->locked = 1;
			trace_dcf77_locked();
			SIG_ACTIVATE(sig_uart_trace_req);
		}

	}
}
