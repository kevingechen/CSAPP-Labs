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

const char* CacheBehaviorNames[] = {
    "UNKOWN",
    "hit",
    "miss",
    "eviction"
};

typedef struct mem_access {
    char type;
    char addr[18];
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
    unsigned int E;
    cache_block *lru_block;
} cache_set;

typedef struct {
    unsigned int s;
    unsigned int E;
    unsigned int b;
    cache_set *lru_cache_sets;
} cache;

/**
 * Keep the middle s bits of address value
 * and shifted to right end
 * e.g.
 *   given the binary representation of address
 *       10110101100010100100101000000001
 *       |--- tag ----||-- s --||-- b --|
 *   then return 100100101
 */
unsigned long calculateSetIndex(unsigned int s, unsigned int b, unsigned long addr_val) {
    return ((addr_val) >> b) & ((1ul << s) - 1);
}

/**
 * Keep the higher tag bits of address value
 * and shifted to right end
 * e.g.
 *   given the binary representation of address
 *       10110101100010100100101000000001
 *       |--- tag ----||-- s --||-- b --|
 *   then return 10110101100010
 */
unsigned long calculateTag(unsigned int s, unsigned int b, unsigned long addr_val) {
    return addr_val >> (s + b);
}

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
    printf("\nThe sequence of memory access:");
    while (current) {
        printf("\n%c %s,%s", current->type, current->addr, current->bsize);
        for (unsigned int i = 0; i < current->cb_len; i++) {
            printf(" %s", CacheBehaviorNames[current->cb_arr[i]]);
        }
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
    cache_block* mru_block = (cache_block*) malloc(E * sizeof(cache_block));
    cache_block* left = NULL;
    cache_block* right = NULL;
    memset(mru_block, 0, E * sizeof(cache_block));

    for (unsigned int i = 0; i < E; i++) {
        left = mru_block + i;
        right = mru_block + ((i + 1) % E);
        left->next = right;
        right->prev = left;
    }

    return mru_block + (E - 1);
}

/**
 * Initialize LRU cache sets
 */
cache_set* initLruCacheSets(unsigned int S, unsigned int E) {
    cache_set *cache_set_arr = (cache_set *) malloc(S * sizeof(cache_set));
    memset(cache_set_arr, 0, S * sizeof(cache_set));
    for (unsigned int i = 0; i < S; i++) {
        cache_set_arr[i].E = E;
        cache_block *lru_block = initLruBlocks(E);
        cache_set_arr[i].lru_block = lru_block;
    }

    return cache_set_arr;
}

/**
 * Initialize LRU cache
 */
cache* initLruCache(unsigned int s, unsigned int E, unsigned int b) {
    unsigned int S = 1 << s;
    cache* lru_cache = (cache*) malloc(sizeof(cache));
    memset(lru_cache, 0, sizeof(cache));
    lru_cache->s = s;
    lru_cache->E = E;
    lru_cache->b = b;
    lru_cache->lru_cache_sets = initLruCacheSets(S, E); 

    return lru_cache;
}

/**
 * Try to locate the cache block that matches the given tag
 * and valid_bit is set.
 * Return NULL if not found, indicating a MISS
 */
cache_block* lookupTargetCacheBlock(cache_set *lru_cache_set, unsigned long tag) {
    cache_block *target = NULL;
    cache_block *current;
    unsigned int i;
    for (i = 0, current = lru_cache_set->lru_block; i < lru_cache_set->E; i++, current = current->next) {
        if (1 == current->valid_bit && tag == current->tag) {
            target = current;
            break;
        }
    }

    return target;
}

/**
 * Do update a cache block by moving the most recent used
 * block (mru_block) to the head position, which is previous
 * neighbor of the lru_block in acyclic block list
 */
void doUpdateCacheSet(cache_set *lru_cache_set, cache_block *mru_block) {
    if (mru_block == lru_cache_set->lru_block->next) {
        return;
    } else if (mru_block == lru_cache_set->lru_block) {
        lru_cache_set->lru_block = lru_cache_set->lru_block->prev;
        return;
    } else {
        mru_block->prev->next = mru_block->next;
        mru_block->next->prev = mru_block->prev;
        mru_block->next = lru_cache_set->lru_block->next;
        mru_block->prev = lru_cache_set->lru_block;
        lru_cache_set->lru_block->next->prev = mru_block;
        lru_cache_set->lru_block->next = mru_block;
    }
}

/**
 * Given the cache block hitted, do update on cache data by
 * least recently used policy.
 * Return
 *  0: no eviction
 *  1: has eviction
 */
int updateCacheSet(cache_set *lru_cache_set, cache_block *hit_block, unsigned long tag) {
    int has_eviction = 0;
    cache_block *mru_block = hit_block;
    if (NULL == mru_block) {
        mru_block = lru_cache_set->lru_block;
        has_eviction = mru_block->valid_bit;
        mru_block->valid_bit = 1;
        mru_block->tag = tag;
    }
    doUpdateCacheSet(lru_cache_set, mru_block);

    return has_eviction;
}

/**
 * append cache behavior to cb_arr
 */
void appendCacheBehavior(mem_access_node *current, CacheBehavior cb) {
    current->cb_arr[current->cb_len++] = cb;
}

/**
 * Simulate a single memory access operation,
 * appending cache behavior
 */
void simulateMemAccess(mem_access_node *current, cache_set *lru_cache_set, unsigned long tag) {
    // lookup target block in lru_cache
    cache_block *hit_block = lookupTargetCacheBlock(lru_cache_set, tag);
    // append cache behavior
    CacheBehavior cb = NULL == hit_block ? MISS : HIT;
    appendCacheBehavior(current, cb);
    int isEviction = updateCacheSet(lru_cache_set, hit_block, tag);
    if (isEviction != 0) {
        cb = EVICTION;
        appendCacheBehavior(current, cb);
    }
}

/**
 * Go through each memory access operation,
 * and simulate the LRU cache behaviors by
 * filling the cb_arr (cache behavior array)
 */
void simulateCacheBehaviors(mem_access_node* mhead,
                            cache* lru_cache) {
    for (mem_access_node *current = mhead; current != NULL; current = current->next) {
        // calculate set index and tag by current->addr, s and b
        unsigned long addr_val = strtoul(current->addr, NULL, 16);
        unsigned long sindex = calculateSetIndex(lru_cache->s, lru_cache->b, addr_val);
        unsigned long tag = calculateTag(lru_cache->s, lru_cache->b, addr_val);
        // 'M' operation requires one more memory access simulation
        if ('M' == current->type) {
            simulateMemAccess(current, lru_cache->lru_cache_sets + sindex, tag);
        }
        simulateMemAccess(current, lru_cache->lru_cache_sets + sindex, tag);
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

    // parse trace file
    mem_access_node *mhead = readMemAccessHistoryFromTraceFile(trace_file);

    // initiate lru_cache
    cache *lru_cache = initLruCache(s, E, b);
    // evaluate cache actions
    simulateCacheBehaviors(mhead, lru_cache);

    if (help_flag)
        printf("Usage: %s [-hv] -s <s> -E <E> -b <b> -t <tracefile>\n", argv[0]);
    if (verbose_flag) {
        // print detailed cache actions for each memory access
        printMemAccessList(mhead);
    }

	freeMemAccessList(mhead);
    // calculate statistics
    printf("\n\nhit_count = %d, miss_count = %d, eviction_count = %d\n", hit_count, miss_count, eviction_count);
    // printSummary(hit_count, miss_count, eviction_count);
    return 0;
}
