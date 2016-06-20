#include "app_signals.h"

#include "sm.h"

#define DS3231_I2C_ADDRESS	(0x68 << 1)
#define DS3231_TIME_CAL_REG (0x00)
#define DS3231_CONTROL_REG	(0x0e)
#define DS3231_CTRL_INTCN	(1UL << 2)
#define DS3231_CTRL_RS1		(1UL << 3)
#define DS3231_CTRL_RS2		(1UL << 4)
#define DS3231_CTRL_BBSQW	(1UL << 6)

#define SECONDS_FROM_1970_TO_2000 946684800

typedef struct {
	uint8_t sec;
	uint8_t min;
	uint8_t hour;
	uint8_t mday;
	uint8_t mon;
	int16_t year;
	uint8_t wday;
	uint8_t yday;
	uint8_t year_s;
	uint32_t unixtime;
} ts_t;

typedef struct {
	sm_t sm;
	uint8_t xferbuf[8];
	ts_t ts;
} rtc_sm_t;

extern I2C_HandleTypeDef hi2c1;

static sm_op_e rtc_sm_init_state(sm_t *sm, uint8_t sig);
static sm_op_e rtc_sm_idle_state(sm_t *sm, uint8_t sig);
static sm_op_e rtc_sm_set_state(sm_t *sm, uint8_t sig);

void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef *hi2c)
{
	SIG_ACTIVATE(sig_i2c_done);
}

void HAL_I2C_MasterRxCpltCallback(I2C_HandleTypeDef *hi2c)
{
	SIG_ACTIVATE(sig_i2c_done);
}

//void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *hi2c)
//{
//	SIG_ACTIVATE(sig_i2c_done);
//}

static uint8_t bcdtodec(const uint8_t val)
{
	return ((val / 16 * 10) + (val % 16));
}

static uint8_t dectobcd(const uint8_t val)
{
    return ((val / 10 * 16) + (val % 10));
}


static const uint8_t days_in_month[12] = { 31,28,31,30,31,30,31,31,30,31,30,31 };

uint32_t get_unixtime(ts_t *ts)
{
    uint8_t i;
    uint16_t d;
    int16_t y;
    uint32_t rv;

    if (ts->year >= 2000) {
        y = ts->year - 2000;
    } else {
        return 0;
    }

    d = ts->mday - 1;
    for (i=1; i<ts->mon; i++) {
        d += days_in_month[i - 1];
    }
    if (ts->mon > 2 && y % 4 == 0) {
        d++;
    }
    // count leap days
    d += (365 * y + (y + 3) / 4);
    rv = ((d * 24UL + ts->hour) * 60 + ts->min) * 60 + ts->sec + SECONDS_FROM_1970_TO_2000;
    return rv;
}

static sm_op_e rtc_sm_init_state(sm_t *sm, uint8_t sig)
{
	rtc_sm_t *self = CONTAINER_OF(sm, rtc_sm_t, sm);
	HAL_StatusTypeDef s;

	switch(sig)
	{
	case SIG_EXIT:
		return SM_HANDLED;

	case SIG_ENTRY:
	case SIG_EXTERNAL+sig_i2c_done:

	sm_sync_begin(sm);

	assert_param(s == HAL_OK);
	// Turn on 1024Hz RTC SQW output
	s = HAL_I2C_Master_Transmit_IT(&hi2c1, DS3231_I2C_ADDRESS, self->xferbuf, 1);
	assert_param(s == HAL_OK);
	sm_sync_sig_wait(sm, sig_i2c_done);

	s = HAL_I2C_Master_Receive_IT(&hi2c1, DS3231_I2C_ADDRESS, &self->xferbuf[1], 1);
	assert_param(s == HAL_OK);
	sm_sync_sig_wait(sm, sig_i2c_done);


	self->xferbuf[1] &= ~(DS3231_CTRL_INTCN | DS3231_CTRL_RS2) ;
	self->xferbuf[1] |=  DS3231_CTRL_BBSQW | DS3231_CTRL_RS1;
	s = HAL_I2C_Master_Transmit_IT(&hi2c1, DS3231_I2C_ADDRESS, self->xferbuf, 2);
	assert_param(s == HAL_OK);
	sm_sync_sig_wait(sm, sig_i2c_done);

	//read the time
	self->xferbuf[0] = DS3231_TIME_CAL_REG;
	s = HAL_I2C_Master_Transmit_IT(&hi2c1, DS3231_I2C_ADDRESS, self->xferbuf, 1);
	assert_param(s == HAL_OK);
	sm_sync_sig_wait(sm, sig_i2c_done);

	s = HAL_I2C_Master_Receive_IT(&hi2c1, DS3231_I2C_ADDRESS, &self->xferbuf[1], 7);
	assert_param(s == HAL_OK);
	sm_sync_sig_wait(sm, sig_i2c_done);

	sm_sync_end(sm);

	self->ts.sec = bcdtodec(self->xferbuf[1]);
	self->ts.min = bcdtodec(self->xferbuf[2]);
	self->ts.hour = bcdtodec(self->xferbuf[3]);
	self->ts.mday = bcdtodec(self->xferbuf[5]);
	self->ts.mon = bcdtodec(self->xferbuf[6] & 0x1F);
	if(((self->xferbuf[6] & 0x80) >> 7) == 1)
	{
		self->ts.year = 2000 + bcdtodec(self->xferbuf[7]);
	} else {
		self->ts.year = 1900 + bcdtodec(self->xferbuf[7]);
	}
	self->ts.wday = bcdtodec(self->xferbuf[4]);
	self->ts.year_s = bcdtodec(self->xferbuf[7]);
	self->ts.unixtime = get_unixtime(&self->ts);
	trace_dcf77_rtcepoch(self->ts.unixtime);
	SIG_ACTIVATE(sig_uart_trace_req);

	en_sig_reg |= SIG_GET_REG(sig_bin);
	SIG_DEACTIVATE(sig_i2c_done);
	sm_transition(sm, rtc_sm_idle_state);

	}

	return SM_NOT_HANDLED;
}

static sm_op_e rtc_sm_idle_state(sm_t *sm, uint8_t sig)
{
	sm_op_e op = SM_NOT_HANDLED;

	switch(sig)
	{
	case SIG_ENTRY:
		return SM_HANDLED;

	case SIG_EXIT:
		return SM_HANDLED;

	}

	return op;
}

static sm_op_e rtc_sm_set_state(sm_t *sm, uint8_t sig)
{
	rtc_sm_t *self = CONTAINER_OF(sm, rtc_sm_t, sm);
	HAL_StatusTypeDef s;
	uint8_t century;

	switch(sig)
	{
	case SIG_EXIT:
		return SM_HANDLED;

	case SIG_ENTRY:
	case SIG_EXTERNAL+sig_i2c_done:

    if (self->ts.year > 2000) {
        century = 0x80;
        self->ts.year_s = self->ts.year - 2000;
    } else {
        century = 0;
        self->ts.year_s = self->ts.year - 1900;
    }

	self->xferbuf[0] = DS3231_TIME_CAL_REG;
    self->xferbuf[1] = dectobcd(self->ts.sec);
    self->xferbuf[2] = dectobcd(self->ts.min);
    self->xferbuf[3] = dectobcd(self->ts.hour);
    self->xferbuf[4] = dectobcd(self->ts.wday);
    self->xferbuf[5] = dectobcd(self->ts.mday);
    self->xferbuf[6] = dectobcd(self->ts.mon) + century;
    self->xferbuf[7] = dectobcd(self->ts.year_s);

	sm_sync_begin(sm);

	//write the time
	s = HAL_I2C_Master_Transmit_IT(&hi2c1, DS3231_I2C_ADDRESS, self->xferbuf, 8);
	assert_param(s == HAL_OK);
	sm_sync_sig_wait(sm, sig_i2c_done);

	sm_sync_end(sm);
	SIG_DEACTIVATE(sig_i2c_done);
	sm_transition(sm, rtc_sm_idle_state);

	}

	return SM_NOT_HANDLED;
}

rtc_sm_t rtc_sm = {
		.sm = {
				.state = rtc_sm_init_state,
				.reg_sig = SIG_GET_REG(sig_i2c_done)
		},
		.xferbuf = {DS3231_CONTROL_REG},
};
