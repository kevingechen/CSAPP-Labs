# Architecture Lab

## Introduction

In this lab, we are going to learn about the design and implementation of a pipelined Y86-64 processor,
optimizing both it and a benchmark program to maximize performance. We can make any semantics
preserving transformation to the benchmark program, or to make enhancements to the pipelined processor,
or both.

The lab consists of three tasks, respectively:
+ `Task 1`: write some simple Y86-64 programs and become familiar with the Y86-64 tools.
+ `Task 2`: extend the SEQ simulator with a new instruction.
+ `Task 3`: optimize the Y86-64 benchmark program and the processor design.

## Task 1
In this task, we will work under directory `src/sim/misc`.
We are going to write three Y86-64 programs simulating the example functions in [examples.c](src/sim/misc/examples.c)
```c
/* $begin examples */
/* linked list element */
typedef struct ELE {
    long val;
    struct ELE *next;
} *list_ptr;

/* sum_list - Sum the elements of a linked list */
long sum_list(list_ptr ls)
{
    long val = 0;
    while (ls) {
    val += ls->val;
    ls = ls->next;
    }
    return val;
}
```

```c
/* rsum_list - Recursive version of sum_list */
long rsum_list(list_ptr ls)
{
    if (!ls)
    return 0;
    else {
    long val = ls->val;
    long rest = rsum_list(ls->next);
    return val + rest;
    }
}
```

## Task 2

## Task 3
