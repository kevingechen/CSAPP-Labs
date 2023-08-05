# Cache Lab

## Introduction
In this lab, we are going to work on two files:
```
csim.c       A Least-Recently Used(LRU) cache simulator
trans.c      A matrix transpose function to optimize cache behavior
```

## Task 1
In this task, we will implement an LRU cache simulator from scratch.  The input
is a polished version of valgrind trace file, having the format:
```
I 0400d7d4,8
 M 0421c7f0,4
 L 04f6b868,8
 S 7ff0005c8,8
```
Each line denotes one or two memory accesses. The format of each line is
```
[space]operation address,size
```
The operation field denotes the type of memory access: "I" denotes an instruction load,
"L" a data load, "S" a data store, and "M" a data modification. A data modification is
interpreted as a data load plus store, or simply "M" = "L"+"S". Each memory access may
affect the status of cache data. The following figure show the general organization of
cache memory.

![General Cache Organization](./static/general_cache_org.png)

Cache memory are divided to a number $S$ of sets. Each set consists of $E$ fixed-size
blocks (or 'lines' in the textbook). Each block has a valid bit, a multi-bit tag field
and a data of $B$ bytes holding continuous memory data. An operation on a given address
, either load or store, will overwrite the block data that is mapped by current memory
address. The mapping between memory address and cache block parameters is based on a
segmentation of address bits. As show in the previous figure, the first $t$ bits account
for the tag value for the matched block. Then $s$ bits will determine the set index, and
the last $b$ bits will help locate the concrete byte inside the block data array. From
this segmentation rule, we know that $S = 2^s$ and $B = 2^b$. Since cache is often more
expensive than lower level memory storage, it is impossible to hold all data in the cache
and cache conflicts occur from time to time. Quantitively, cache conflicts stem from the
fact that $E$ (the number of blocks within a set) is usually less than $2^t$.


Each access to a memory address will first traverse the blocks of $s$-th set, locating a
block whose tag equals the tag of current address and whose valid bit is set to 1. If
this block is found, known as "cache hit", then the program will directly operate on it
. If there is no such a block, called "cache miss", then the program will find a non-valid
 block or peek a victim block holding another tag inside the set, to overwrite it by
current tag and data. The latter action is called "cache eviction". As memory accesses run
in order, the cache memory is updated accordingly. For this task, we will implement the
Least-Recently Used (LRU) policy to locate the victim block. In brief, our cache eviction
will execute on the block which is far most unaccessed within the set. And each time a
block is hit or evicted, it becomes the most recently used one. Therefore, we need a
mechanism to correctly update the "freshness" of each block following the trace.

In the solution, I use a data structure of doubly linked list to simulate the LRU policy.

## Task 2
In this task, we are going to optimize a matrix transpose function (e.g. B[M][N] = 
transpose(A[N][M])) in order to minimize the total amount of cache misses.
The performance are evaluated in three cases of different matric sizes, respectively (M=32, N=32), (M=64, N=64) and (M=61, N=67).
