/*
 * main_app.h
 *
 *  Created on: 26 maj 2016
 *      Author: mr_gogol
 */

#ifndef INC_MAIN_APP_H_
#define INC_MAIN_APP_H_

extern struct bin_tick_sm_t bin_tick_sm;
extern struct rtc_sm_t rtc_sm;
extern struct uart_trace_sm_t uart_trace_sm;
extern struct led_sm_t led_sm;

void main_app(void);

#endif /* INC_MAIN_APP_H_ */
