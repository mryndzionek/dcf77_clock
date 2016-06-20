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

#include "trace_dcf77_trace.h"
#include "config.h"

#define LOG_INFO(M, ...) fprintf(stderr, "[INFO] (%d) " M "\n", __LINE__, ##__VA_ARGS__)

#define BLOCK_SIZE                      (10)
#define ITER_SIZE                       (25)

#define NUMBER_SEED     (0xAAAAAAAAAAAAAAAA)
#define PATTERN_SEED    (0xDE7DBEEFDE7E7DEF)

static void write_block(FILE *fp, size_t n)
{
    size_t block = BLOCK_SIZE;
    uint8_t tmp[BLOCK_SIZE];

    while ((block == BLOCK_SIZE) && (n > 0))
    {    
        block = trace_dcf77_trace_get(tmp, BLOCK_SIZE);
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
    LOG_INFO("Starting test: trace_dcf77_trace_test.csv version %s", VERSION_STRING);
    LOG_INFO("This test will output '%s' on successful run", "trace_dcf77_trace_test.out");

    FILE *fp;
    uint8_t i;
    uint64_t seed = NUMBER_SEED;

    srand(0xaa);
    fp = fopen("trace_dcf77_trace_test.out", "wb");
    if(fp)
    {
        // Test case 1: 
        //  fill the buffer without overwrite iterating over every message
        for(i=0; i<ITER_SIZE; i++)
        {
            update_seed(&seed, i);
            trace_dcf77_trace_status(seed & 0xFFFF);
            update_seed(&seed, i);
        }
        write_block(fp, 512/BLOCK_SIZE);

        // Test case 2: 
        //  fill the buffer with single message overwriting it

        seed = PATTERN_SEED;
        // status
        for(i=0; i<26; i++)
        {
            trace_dcf77_trace_status(seed & 0xFFFF);
        }
        write_block(fp, 512/BLOCK_SIZE);


        fclose(fp);
    }
    else
    {   
        LOG_INFO("Unable to open output file: %s", strerror(errno));
        return 1;
    }
    return 0;
}
