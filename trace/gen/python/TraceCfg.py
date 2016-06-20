################################################################################
#  THIS FILE IS 100% GENERATED; DO NOT EDIT EXCEPT EXPERIMENTALLY              #
#  Please refer to the README for information about making permanent changes.  #
################################################################################
import sys
import struct
import binascii
import TraceDecoder

class TraceCfg(object):

    data_enum = {
        0x03: 'one',
        0x02: 'zero',
        0x01: 'undefined',
        0x00: 'sync_mark',
    };

    cfg = {
        # Message: startup
        0: TraceDecoder.MsgDecoder('INFO', 'STM32L053R8 DCF77 clock', '= '),

        # Message: rtcepoch
        1: TraceDecoder.MsgDecoder('INFO', 'RTC unix epoch {}', '=  I '),

        # Message: status
        2: TraceDecoder.MsgDecoder('DEBUG', 'Phase {}, Max {}, Noise {}', '=  H  I  I '),

        # Message: data
        3: TraceDecoder.MsgDecoder('DEBUG', 'Data: {}, Sync index: {} Sync max: {} Sync noise: {}', '=  B  B  B  B ', { 0: data_enum }),

        # Message: locked
        4: TraceDecoder.MsgDecoder('INFO', 'Locked to sync mark', '= '),

        # Message: lost_lock
        5: TraceDecoder.MsgDecoder('WARN', 'Lost lock to sync mark', '= '),


    };

    crc_decoder = struct.Struct('= B ');

