#include "cachelab.h"
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

unsigned int str2Unsigned(char *str) {
    unsigned int num = 0;
    char *char_ptr = str;
    while ('\0' != *char_ptr) {
        num = num * 10 + ((*char_ptr) - '0');
        char_ptr++;
    }
    return num;
}

int main(int argc, char *argv[])
{
    int opt;
    int help_flag = 0, verbose_flag = 0;
    unsigned int s = 0, E = 0, b = 0;
    unsigned int hit_count = 0, miss_count = 0, eviction_count = 0;
    char *trace_file;

    // read arguments and set flags
    while ((opt = getopt(argc, argv, "hvs:E:b:t:")) != -1) {
        switch (opt) {
            case 'h':
                help_flag = 1;
                break;
            case 'v':
                verbose_flag = 1;
                break;
            case 's':
                s = str2Unsigned(optarg);
                break;
            case 'E':
                E = str2Unsigned(optarg);
                break;
            case 'b':
                b = str2Unsigned(optarg);
                break;
            case 't':
                trace_file = optarg;
                break;
            default:
                fprintf(stderr, "Usage: %s [-hv] -s <s> -E <E> -b <b> -t <tracefile>\n", argv[0]);
                exit(-1);
        }
    }

    if (help_flag)
        printf("Usage: %s [-hv] -s <s> -E <E> -b <b> -t <tracefile>\n", argv[0]);
    if (verbose_flag)
        // print detailed cache actions for each memory access
        printf("verbose_flag = %d\n", verbose_flag);
    printf("s = %d, E = %d, b = %d, trace_file = %s\n", s, E, b, trace_file);
    // parse trace file
    // evaluate cache actions
    // calculate statistics
    printf("hit_count = %d, miss_count = %d, eviction_count = %d\n", hit_count, miss_count, eviction_count);
    // printSummary(hit_count, miss_count, eviction_count);
    return 0;
}
