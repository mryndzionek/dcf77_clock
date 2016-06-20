#ifndef INC_SM_H_
#define INC_SM_H_

#include "stm32l0xx_hal.h"

#define __same_type(a, b) __builtin_types_compatible_p(typeof(a), typeof(b))
#define __must_be_array(a) BUILD_BUG_ON_ZERO(__same_type((a), &(a)[0]))
#define ARRAY_SIZE(arr) \
		({typedef char ARRAYSIZE_CANT_BE_USED_ON_POINTERS[sizeof(arr) == sizeof(void*) ? -1 : 1]; \
		sizeof(arr) / sizeof((arr)[0]);})

#define CONTAINER_OF(ptr, type, field) \
		((type *)(((char *)(ptr)) - offsetof(type, field)))

typedef enum {
	SM_HANDLED,
	SM_TRANSITION,
	SM_NOT_HANDLED,
} sm_op_e;

enum {
	SIG_ENTRY = 0,
	SIG_EXIT,
	SIG_EXTERNAL,
};

typedef struct sm sm_t;

struct sm {
	int sync_ctx;
	uint8_t sync_bar;
	uint16_t reg_sig;
	sm_op_e (*state)(sm_t * const sm, uint8_t sig);
	sm_op_e (*next_state)(sm_t * const sm, uint8_t sig);
};

#define sm_transition(_sm, _s) do { \
		_sm->next_state = _s; \
		_sm->sync_ctx = 0; \
		return SM_TRANSITION; \
} while(0)

#define sm_sync_begin(_sm) switch (_sm->sync_ctx) { \
		case 0:

#define sm_sync_sig_wait(_sm, _sig) \
		case __LINE__: _sm->sync_ctx = __LINE__; \
		if(!_sm->sync_bar) {\
			_sm->sync_bar = 1; \
			SIG_DEACTIVATE(_sig); \
			return SM_HANDLED; \
		} \
		_sm->sync_bar = 0; \
		if(sig != (SIG_EXTERNAL+_sig)) {\
			return SM_NOT_HANDLED; \
		}

#define sm_sync_end(_sm)     default: \
		break; \
} do {} while(0)

void handle(sm_t *sm, uint8_t sig);
#endif /* INC_SM_H_ */
