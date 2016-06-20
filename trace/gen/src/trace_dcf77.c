/*
################################################################################
#  THIS FILE IS 100% GENERATED; DO NOT EDIT EXCEPT EXPERIMENTALLY              #
#  Please refer to the README for information about making permanent changes.  #
################################################################################
*/
#include "trace_dcf77.h"

#include "rb.h"
#include "trace_platform.h"

#define END_FLAG           (0x7E)
#define ESCAPE_BYTE        (0x7D)

#define ENCODE_NUM_1(_b, _v) \
    if (((_v & 0xFF) ^ ESCAPE_BYTE) && ((_v & 0xFF) ^ END_FLAG)) { \
        *(_b++) = _v; \
     } else { \
        *(_b++) = ESCAPE_BYTE; \
        *(_b++) = _v ^ 0x20; \
    }

#define ENCODE_NUM_2(_b, _v) \
    ENCODE_NUM_1(_b, _v); \
    _v >>= 8; \
    ENCODE_NUM_1(_b, (_v));

#define ENCODE_NUM_4(_b, _v) \
    ENCODE_NUM_2(_b, (_v)); \
    _v >>= 8; \
    ENCODE_NUM_2(_b, (_v));

#define ENCODE_NUM_8(_b, _v) \
    ENCODE_NUM_4(_b, (_v)); \
    _v >>= 8; \
    ENCODE_NUM_4(_b, (_v));

#define crc_default_init()
#define crc_default_deinit()
#define crc_default_setup uint8_t crc = 0
#define crc_default_update(_data) (crc += (_data))
#define crc_default_finish() (crc = ~crc)
#define crc_default_get() (crc)

static rb_t _rb = { .data_ = (uint8_t[128]){0}, .capacity_ = 128};

static uint8_t trace_dcf77_counter;

static uint8_t _trace_dcf77_crc(uint8_t *data, uint8_t len)
{
    uint8_t i;
    crc_default_setup;

    for(i = 0; i < len; i++)
    {
        if(data[i] == ESCAPE_BYTE)
        {
            crc_default_update(data[++i] ^ 0x20);
        }
        else
        {
            crc_default_update(data[i]);
        }
    }
    return crc_default_get();
}

static uint8_t *trace_dcf77_add_header(uint8_t *b, uint8_t uid)
{
    uint32_t ts = GET_TIMESTAMP;

    ENCODE_NUM_1(b, trace_dcf77_counter);
    trace_dcf77_counter++;
    ENCODE_NUM_1(b, uid);
    ENCODE_NUM_4(b, ts);

    return b;
}

static void _trace_dcf77_no_args(uint8_t uid)
{
    uint8_t tmp[15], *b = tmp;
    uint8_t crc = 0;

    b = trace_dcf77_add_header(b, uid);
    crc_default_init();
    crc = _trace_dcf77_crc(tmp, b-tmp);
    crc_default_finish();
    crc_default_deinit();
    ENCODE_NUM_1(b, crc);
    *(b++) = END_FLAG;
    (void)rb_write(&_rb, tmp, b-tmp);
}

static void _trace_dcf77_locked_alias(uint8_t uid)
{
    uint8_t tmp[15], *b = tmp;
    uint8_t crc = 0;

    crc_default_init();

    b = trace_dcf77_add_header(b, uid);
    // Here we have at most 3 bytes left in the buffer
    crc += _trace_dcf77_crc(tmp, b-tmp);
    crc_default_finish();
    crc_default_deinit();
    ENCODE_NUM_1(b, crc);
    *(b++) = END_FLAG;
    (void)rb_write(&_rb, tmp, b-tmp);
}

void trace_dcf77_startup()
{
#ifdef TRACE_DCF77_ENABLE
    _trace_dcf77_no_args(0);
#endif
}

void trace_dcf77_rtcepoch(uint32_t ts)
{
#ifdef TRACE_DCF77_ENABLE
    uint8_t tmp[23], *b = tmp;
    uint8_t crc = 0;

    crc_default_init();

    b = trace_dcf77_add_header(b, 1);
    // Here we have at most 11 bytes left in the buffer
    ENCODE_NUM_4(b, ts);
    crc += _trace_dcf77_crc(tmp, b-tmp);
    crc_default_finish();
    crc_default_deinit();
    ENCODE_NUM_1(b, crc);
    *(b++) = END_FLAG;
    (void)rb_write(&_rb, tmp, b-tmp);
#endif
}

void trace_dcf77_status(uint16_t phase, uint32_t max, uint32_t noise_max)
{
#ifdef TRACE_DCF77_ENABLE
    uint8_t tmp[35], *b = tmp;
    uint8_t crc = 0;

    crc_default_init();

    b = trace_dcf77_add_header(b, 2);
    // Here we have at most 23 bytes left in the buffer
    ENCODE_NUM_2(b, phase);
    ENCODE_NUM_4(b, max);
    ENCODE_NUM_4(b, noise_max);
    crc += _trace_dcf77_crc(tmp, b-tmp);
    crc_default_finish();
    crc_default_deinit();
    ENCODE_NUM_1(b, crc);
    *(b++) = END_FLAG;
    (void)rb_write(&_rb, tmp, b-tmp);
#endif
}

void trace_dcf77_data(uint8_t data, uint8_t s_index, uint8_t s_max, uint8_t s_noise)
{
#ifdef TRACE_DCF77_ENABLE
    uint8_t tmp[23], *b = tmp;
    uint8_t crc = 0;

    crc_default_init();

    b = trace_dcf77_add_header(b, 3);
    // Here we have at most 11 bytes left in the buffer
    ENCODE_NUM_1(b, data);
    ENCODE_NUM_1(b, s_index);
    ENCODE_NUM_1(b, s_max);
    ENCODE_NUM_1(b, s_noise);
    crc += _trace_dcf77_crc(tmp, b-tmp);
    crc_default_finish();
    crc_default_deinit();
    ENCODE_NUM_1(b, crc);
    *(b++) = END_FLAG;
    (void)rb_write(&_rb, tmp, b-tmp);
#endif
}

void trace_dcf77_locked()
{
#ifdef TRACE_DCF77_ENABLE
    _trace_dcf77_locked_alias(4);
#endif
}

void trace_dcf77_lost_lock()
{
#ifdef TRACE_DCF77_ENABLE
    _trace_dcf77_locked_alias(5);
#endif
}

size_t trace_dcf77_get(uint8_t *data, size_t bytes)
{
    return rb_read(&_rb, data, bytes);
}
