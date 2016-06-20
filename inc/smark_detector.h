#ifndef INC_SMARK_DETECTOR_H_
#define INC_SMARK_DETECTOR_H_

#include <stdint.h>

#define SECONDS_PER_MINUTE 	(60UL)
#define MINUTES_PER_HOUR 	(60UL)

typedef enum {
	symbol_sync_mark = 0,
	symbol_undefined,
	symbol_zero,
	symbol_one
} symbol_t;

typedef struct {
	uint8_t bins[SECONDS_PER_MINUTE];
	uint8_t tick;
	uint8_t max;
	uint8_t max_index;
	uint8_t noise_max;
	uint8_t locked;
} smark_detector_t;

void smark_detector_process_symbol(smark_detector_t *detector, uint8_t symbol);

#endif /* INC_SMARK_DETECTOR_H_ */
