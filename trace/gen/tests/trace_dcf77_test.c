/*
################################################################################
#  THIS FILE IS 100% GENERATED; DO NOT EDIT EXCEPT EXPERIMENTALLY              #
#  Please refer to the README for information about making permanent changes.  #
################################################################################
*/
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

#include "trace_dcf77.h"
#include "config.h"

#define LOG_INFO(M, ...) fprintf(stderr, "[INFO] (%d) " M "\n", __LINE__, ##__VA_ARGS__)

#define BLOCK_SIZE                      (10)
#define ITER_SIZE                       (1)

#define NUMBER_SEED     (0xAAAAAAAAAAAAAAAA)
#define PATTERN_SEED    (0xDE7DBEEFDE7E7DEF)

// Test array for enum 'data'
static const uint8_t _e_data[] = {
    0x03,            // one
    0x02,            // zero
    0x01,            // undefined
    0x00,            // sync_mark
};

static void write_block(FILE *fp, size_t n)
{
    size_t block = BLOCK_SIZE;
    uint8_t tmp[BLOCK_SIZE];

    while ((block == BLOCK_SIZE) && (n > 0))
    {    
        block = trace_dcf77_get(tmp, BLOCK_SIZE);
        fwrite(tmp, 1, block, fp);
        n--;
    }

    fflush(fp);
}

static void update_seed(uint64_t *seed, uint8_t n)
{
    uint8_t i;

    for(i=0; i<8; i++)
    {
        *((uint8_t *)seed + i) ^= *((uint8_t *)seed + (7-i)) | (1 << (n % 8)) | (1<<i);
    }
}

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;
    LOG_INFO("Starting test: trace_dcf77_test.csv version %s", VERSION_STRING);
    LOG_INFO("This test will output '%s' on successful run", "trace_dcf77_test.out");

    FILE *fp;
    uint8_t i;
    uint64_t seed = NUMBER_SEED;

    srand(0xaa);
    fp = fopen("trace_dcf77_test.out", "wb");
    if(fp)
    {
        // Test case 1: 
        //  fill the buffer without overwrite iterating over every message
        for(i=0; i<ITER_SIZE; i++)
        {
            update_seed(&seed, i);
            trace_dcf77_startup();
            update_seed(&seed, i);
            trace_dcf77_rtcepoch(seed & 0xFFFFFFFF);
            update_seed(&seed, i);
            trace_dcf77_status(seed & 0xFFFF, seed & 0xFFFFFFFF, seed & 0xFFFFFFFF);
            update_seed(&seed, i);
            trace_dcf77_data(_e_data[seed % 4], seed & 0xFF, seed & 0xFF, seed & 0xFF);
            update_seed(&seed, i);
            trace_dcf77_locked();
            update_seed(&seed, i);
            trace_dcf77_lost_lock();
            update_seed(&seed, i);
        }
        write_block(fp, 256/BLOCK_SIZE);

        // Test case 2: 
        //  fill the buffer with single message overwriting it

        seed = PATTERN_SEED;
        // startup
        for(i=0; i<16; i++)
        {
            trace_dcf77_startup();
        }
        write_block(fp, 256/BLOCK_SIZE);

        // rtcepoch
        for(i=0; i<11; i++)
        {
            trace_dcf77_rtcepoch(seed & 0xFFFFFFFF);
        }
        write_block(fp, 256/BLOCK_SIZE);

        // status
        for(i=0; i<8; i++)
        {
            trace_dcf77_status(seed & 0xFFFF, seed & 0xFFFFFFFF, seed & 0xFFFFFFFF);
        }
        write_block(fp, 256/BLOCK_SIZE);

        // data
        for(i=0; i<11; i++)
        {
            trace_dcf77_data(_e_data[seed % 4], seed & 0xFF, seed & 0xFF, seed & 0xFF);
        }
        write_block(fp, 256/BLOCK_SIZE);

        // locked
        for(i=0; i<16; i++)
        {
            trace_dcf77_locked();
        }
        write_block(fp, 256/BLOCK_SIZE);

        // lost_lock
        for(i=0; i<16; i++)
        {
            trace_dcf77_lost_lock();
        }
        write_block(fp, 256/BLOCK_SIZE);


        fclose(fp);
    }
    else
    {   
        LOG_INFO("Unable to open output file: %s", strerror(errno));
        return 1;
    }
    return 0;
}
