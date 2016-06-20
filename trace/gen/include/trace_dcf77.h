/*
################################################################################
#  THIS FILE IS 100% GENERATED; DO NOT EDIT EXCEPT EXPERIMENTALLY              #
#  Please refer to the README for information about making permanent changes.  #
################################################################################
*/
#ifndef TRACE_DCF77_H_INCLUDED
#define TRACE_DCF77_H_INCLUDED

#include <stdint.h>
#include <stddef.h>

void trace_dcf77_startup();
void trace_dcf77_rtcepoch(uint32_t ts);
void trace_dcf77_status(uint16_t phase, uint32_t max, uint32_t noise_max);
void trace_dcf77_data(uint8_t data, uint8_t s_index, uint8_t s_max, uint8_t s_noise);
void trace_dcf77_locked();
void trace_dcf77_lost_lock();
size_t trace_dcf77_get(uint8_t *data, size_t bytes);

#endif // TRACE_DCF77_H_INCLUDED
