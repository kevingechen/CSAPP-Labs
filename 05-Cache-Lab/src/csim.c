#include "cachelab.h"
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TRACE_LEN 100
#define MAX_BEHAVIOR_LEN_PER_MEM_ACCESS 3

typedef enum {
    UNKOWN = 0,
    HIT,
    MISS,
    EVICTION
} CacheBehavior;

typedef struct mem_access {
    char type;
    char addr[10];
    char  bsize[5];
    unsigned int cb_len;
    CacheBehavior cb_arr[3];
    struct mem_access *next;
} mem_access_node;

/**
 * Parse line by line from trace_file,
 * build a memory access struct for each valid line.
 * A valid line is of pattern " {type} {addr_in_hex},{byte_size}"
 * Return the head pointer of a linked list of mem_access_node
 */
mem_access_node* readMemAccessHistoryFromTraceFile(const char *file_name) {
    mem_access_node* head = NULL;
    mem_access_node* tail = NULL;
    FILE* in_file = fopen(file_name, "r");
    if (in_file == NULL) {
        printf("Failed to open the file.\n");
        return head;
    }

    char line[MAX_TRACE_LEN];
    while (fgets(line, MAX_TRACE_LEN, in_file) != NULL) {
        // Skip lines for trace of instruction type 'I'
        if (line[0] != ' ') continue;
        mem_access_node* current_node = (mem_access_node*) malloc(sizeof(mem_access_node));
        memset(current_node, 0, sizeof(mem_access_node));  // Initialize addr with zeros
        sscanf(line, " %c %[^,],%s", &current_node->type, current_node->addr, current_node->bsize);
        if (NULL == head) {
            head = current_node;
            tail = current_node;
        } else {
            tail->next = current_node;
            tail = current_node;
        }
    }

    fclose(in_file);
    return head;
}

void printMemAccessList(mem_access_node *head) {
    mem_access_node* move = head;
    printf("\nThe sequence of memory access:\n");
    while (move) {
        printf("\ttype=%c, addr=%s, bsize=%s, cb_len=%d\n", move->type, move->addr, move->bsize, move->cb_len);
        move = move->next;
    }
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
                s = atoi(optarg);
                break;
            case 'E':
                E = atoi(optarg);
                break;
            case 'b':
                b = atoi(optarg);
                break;
            case 't':
                trace_file = optarg;
                break;
            default:
                fprintf(stderr, "Usage: %s [-hv] -s <s> -E <E> -b <b> -t <tracefile>\n", argv[0]);
                exit(-1);
        }
    }

    mem_access_node *mlist = readMemAccessHistoryFromTraceFile(trace_file);

    if (help_flag)
        printf("Usage: %s [-hv] -s <s> -E <E> -b <b> -t <tracefile>\n", argv[0]);
    if (verbose_flag) {
        // print detailed cache actions for each memory access
        printf("verbose_flag = %d\n", verbose_flag);
        printMemAccessList(mlist);
        printf("s = %d, E = %d, b = %d, trace_file = %s\n", s, E, b, trace_file);
        printf("hit_count = %d, miss_count = %d, eviction_count = %d\n", hit_count, miss_count, eviction_count);
    }

    // parse trace file
    // evaluate cache actions
    // calculate statistics
    // printSummary(hit_count, miss_count, eviction_count);
    return 0;
}
