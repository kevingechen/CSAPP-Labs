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

typedef struct cache_block_node {
    unsigned int valid_bit;
    unsigned long tag;
    struct cache_block_node *prev;
    struct cache_block_node *next;
} cache_block;

typedef struct {
    unsigned int s;
    unsigned int E;
    unsigned int b;
    cache_block *lru_block;
} cache_set;

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
        mem_access_node* current = (mem_access_node*) malloc(sizeof(mem_access_node));
        memset(current, 0, sizeof(mem_access_node));  // Initialize addr with zeros
        sscanf(line, " %c %[^,],%s", &current->type, current->addr, current->bsize);
        if (NULL == head) {
            head = current;
            tail = current;
        } else {
            tail->next = current;
            tail = current;
        }
    }

    fclose(in_file);
    return head;
}

void printMemAccessList(mem_access_node *head) {
    mem_access_node* current = head;
    printf("\nThe sequence of memory access:\n");
    while (current) {
        printf("\ttype=%c, addr=%s, bsize=%s, cb_len=%d\n",
                current->type, current->addr, current->bsize, current->cb_len);
        current = current->next;
    }
}

/**
 * Function to free the memory occupied by the linked list Function
 * to free the memory occupied by the linked list
 */
void freeMemAccessList(mem_access_node* head) {
    mem_access_node* current = head;
    while (current != NULL) {
        mem_access_node* temp = current;
        current = current->next;
        free(temp);
    }
}

/**
 * Initialize LRU blocks by creating
 * a doubly linked list of cache_block.
 * Return the tail (least recently used block pointer)
 */
cache_block* initLruBlocks(unsigned int E) {
    cache_block* lru_block = NULL;
    cache_block* current = NULL;
    for (unsigned int i = 0; i < E; i++) {
        lru_block = (cache_block*) malloc(sizeof(cache_block));
        memset(lru_block, 0, sizeof(cache_block));
        if (NULL == current) {
            current = lru_block;
        } else {
            current->next = lru_block;
            lru_block->prev = current;
            current = lru_block;
        }
    }

    return lru_block;
}

/**
 * Initialize LRU cache by returning an array of cache set
 */
cache_set* initLruCache(unsigned int S, unsigned int E) {
    cache_set *lru_cache = (cache_set *) malloc(S * sizeof(cache_set));
    memset(lru_cache, 0, S * sizeof(cache_set));
    for (unsigned int i = 0; i < S; i++) {
        lru_cache[i].E = E;
        cache_block *lru_block = initLruBlocks(E);
        lru_cache[i].lru_block = lru_block;
    }

    return lru_cache;
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

    // parse trace file
    mem_access_node *mlist = readMemAccessHistoryFromTraceFile(trace_file);

    // initiate lru_cache
    // unsigned int S = 1 << s;
    // cache_set* lru_cache = initLruCache(S, E);
    // evaluate cache actions
    // simulateCacheBehaviors(mlist, lru_cache, s, E, b);

    if (help_flag)
        printf("Usage: %s [-hv] -s <s> -E <E> -b <b> -t <tracefile>\n", argv[0]);
    if (verbose_flag) {
        // print detailed cache actions for each memory access
        printf("verbose_flag = %d\n", verbose_flag);
        printMemAccessList(mlist);
        printf("s = %d, E = %d, b = %d, trace_file = %s\n", s, E, b, trace_file);
        printf("hit_count = %d, miss_count = %d, eviction_count = %d\n", hit_count, miss_count, eviction_count);
    }

	freeMemAccessList(mlist);
    // calculate statistics
    // printSummary(hit_count, miss_count, eviction_count);
    return 0;
}
