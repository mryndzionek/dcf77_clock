#include "app_signals.h"
#include "sm.h"
#include "trace_dcf77.h"
#include "smark_detector.h"
#include "led_sm.h"

#define BINS_PER_10MS					(TICKS_PER_SEC / 100)
#define BINS_PER_50MS					(5 * BINS_PER_10MS)
#define BINS_PER_100MS					(10 * BINS_PER_10MS)
#define BINS_PER_200MS					(20 * BINS_PER_10MS)
#define TICKS_TO_DRIFT_ONE_TICK     	(300000UL)
#define _NN								(TICKS_TO_DRIFT_ONE_TICK / TICKS_PER_SEC)

extern led_sm_t led_sm;

typedef struct {
	sm_t sm;
	uint16_t _bin_index;
	uint16_t _bins[TICKS_PER_SEC];
	uint16_t max_index;
	int32_t max;
	int32_t noise_max;
	smark_detector_t detector;
} bin_tick_sm_t;

static sm_op_e bin_tick_sm_collecting_state(sm_t *sm, uint8_t sig);

static uint16_t wrap(const uint16_t value) {
	// faster modulo function which avoids division
	uint16_t result = value;
	while (result >= TICKS_PER_SEC) {
		result-= TICKS_PER_SEC;
	}
	return result;
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if(GPIO_Pin == RTC_SQW_INT_Pin)
	{
		SIG_ACTIVATE(sig_tick);
	}
}

__attribute__((always_inline))
static void phase_binning(bin_tick_sm_t *self, uint8_t sample)
{
	static int32_t integral = 0;
	static int32_t running_max = 0;
	static uint16_t running_max_index = 0;
	static int32_t running_noise_max = 0;

	if(sample)
	{
		if(self->_bins[self->_bin_index] < _NN)
		{
			self->_bins[self->_bin_index]++;
		}
	} else
	{
		if(self->_bins[self->_bin_index] > 0)
		{
			self->_bins[self->_bin_index]--;
		}
	}

	const uint16_t ck_start_tick  = wrap(self->_bin_index+((10-2)*BINS_PER_100MS));
	const uint16_t ck_middle_tick = wrap(self->_bin_index+((10-1)*BINS_PER_100MS));

	if (integral > running_max) {
		running_max = integral;
		running_max_index = ck_start_tick;
	}
	if (self->_bin_index == wrap(self->max_index + 2*BINS_PER_200MS)) {
		running_noise_max = integral;
	}

	if (self->_bin_index == 0) {
		// one period has passed, flush the result

		self->max = running_max;
		self->max_index = running_max_index;
		self->noise_max = running_noise_max;

		trace_dcf77_status(self->max_index, self->max, self->noise_max);
		SIG_ACTIVATE(sig_uart_trace_req);

		// reset running_max for next period
		running_max = 0;

		// no need to reset the noise_max as this is
		// not computed by comparison against the integral
		// but by index lookup
	}

	integral -= (int32_t)self->_bins[ck_start_tick]*2;
	integral += (int32_t)self->_bins[ck_middle_tick];
	integral += (int32_t)self->_bins[self->_bin_index];
}

__attribute__((always_inline))
static void decode_200ms(bin_tick_sm_t *self, const uint16_t bins_to_go, uint8_t sample)
{
	static uint16_t count = 0;
	static uint8_t decoded_data = 0;

	// will be called for each bin during the "interesting" 200 ms
	count += sample;
	if (bins_to_go > BINS_PER_100MS) {
		if (bins_to_go == BINS_PER_100MS + 1) {
			decoded_data = count > BINS_PER_50MS? 2: 0;
			count = 0;
		}
	} else {
		if (bins_to_go == 0) {
			decoded_data += count > BINS_PER_50MS? 1: 0;
			count = 0;

			smark_detector_process_symbol(&self->detector, decoded_data);

			trace_dcf77_data(decoded_data, self->detector.max_index, self->detector.max, self->detector.noise_max);
			SIG_ACTIVATE(sig_uart_trace_req);
		}
	}
}

__attribute__((always_inline))
static void detector_stage_2(bin_tick_sm_t *self, uint8_t sample)
{
	static uint16_t bins_to_go = 0;

	if (bins_to_go == 0) {
		if (wrap((TICKS_PER_SEC + self->_bin_index - self->max_index)) <= BINS_PER_100MS ||   // current_bin at most 100ms after phase_bin
				wrap((TICKS_PER_SEC + self->max_index - self->_bin_index)) <= BINS_PER_10MS ) {  // current bin at 1 tick before phase_bin
			// if phase bin varies to much during one period we will always be screwed in may ways...
			// last tick of current second
			//Clock_Controller::flush();
			// start processing of bins

			bins_to_go = BINS_PER_200MS + 2*BINS_PER_10MS;
		}
	}

	if (bins_to_go > 0) {
		--bins_to_go;

		// this will be called for each bin in the "interesting" 200ms
		// this is also a good place for a "monitoring hook"

		decode_200ms(self, bins_to_go, sample);
	}
}

static sm_op_e bin_tick_sm_collecting_state(sm_t *sm, uint8_t sig)
{
	bin_tick_sm_t *self = CONTAINER_OF(sm, bin_tick_sm_t, sm);

	switch(sig)
	{
	case SIG_ENTRY:
		return SM_HANDLED;

	case SIG_EXTERNAL+sig_bin: {

		if(self->max_index == self->_bin_index)
		{
			if(self->detector.locked)
			{
				if(self->detector.tick == self->detector.max_index)
				{
					led_blink(&led_sm, 600);
				} else {
					led_blink(&led_sm, 200);
				}
			} else {
				led_blink(&led_sm, 50);
			}
		}

		uint8_t sample = HAL_GPIO_ReadPin(DCF77_DATA_IN_GPIO_Port,
				DCF77_DATA_IN_Pin);
		phase_binning(self, sample);
		detector_stage_2(self, sample);

		if(self->_bin_index < (TICKS_PER_SEC - 1))
		{
			self->_bin_index++;
		} else {
			self->_bin_index = 0;
		}
		SIG_DEACTIVATE(sig_bin);
	}
	return SM_HANDLED;

	case SIG_EXIT:
		return SM_HANDLED;

	}

	return SM_NOT_HANDLED;
}

bin_tick_sm_t bin_tick_sm = {
		.sm = {
				.state = bin_tick_sm_collecting_state,
				.reg_sig = SIG_GET_REG(sig_bin)
		},
};
