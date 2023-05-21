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

### Sum the elements of a linked list
Original C code in `examples.c`:
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

Y86-64 code `sum_list.ys`:
```
# Execution begins at address 0
        .pos 0
        irmovq stack, %rsp      # Set up stack pointer
        call main               # Execute main function
        halt                    # Terminate program

# Linked list of 3 elements
        .align 8
ele1:
        .quad 0x00a
        .quad ele2
ele2:
        .quad 0x0b0
        .quad ele3
ele3:
        .quad 0xc00
        .quad 0


main:
        irmovq ele1, %rdi      # put head of linked list to %rdi
        call sum_list          # sum_list(ele1)
        ret

sum_list:
        irmovq $8,%r8          # Constant 8 for 'next' addressing
        xorq %rax,%rax         # Set result = 0
        andq %rdi,%rdi         # Set CC
        jmp test

loop:
        mrmovq (%rdi),%r9      # Get *ls
        addq %r9,%rax          # Add to result
        addq %r8,%rdi          # ls = ls->next
        mrmovq (%rdi),%rdi     # ls = ls->next
        andq %rdi,%rdi         # Set CC
test:
        jne loop               # Stop when end of linked list 0
        ret

# Stack starts here and grows to lower addresses
        .pos 0x200
stack:
```



### Recursively sum the elements of a linked list
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
