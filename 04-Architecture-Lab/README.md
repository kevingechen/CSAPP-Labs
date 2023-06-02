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

# long sum_list(list_ptr ls)
# ls in %rdi
sum_list:
        xorq %rax,%rax         # Set result = 0
        andq %rdi,%rdi         # Set CC
        jmp test

loop:
        mrmovq (%rdi),%r9      # Get *ls
        addq %r9,%rax          # Add to result
        mrmovq 8(%rdi),%rdi     # ls = ls->next
        andq %rdi,%rdi         # Set CC
test:
        jne loop               # Stop when end of linked list 0
        ret

# Stack starts here and grows to lower addresses
        .pos 0x200
stack:
```
We can put the `ys` file in `src/sim/misc` folder to build and test it:
```sh
  04-Architecture-Lab > cd src/sim/misc
  04-Architecture-Lab/src/sim/misc > make clean; make
  04-Architecture-Lab/src/sim/misc > yas sum_list.ys; yis sum_list.yo
    Stopped in 26 steps at PC = 0x13.  Status 'HLT', CC Z=1 S=0 O=0
    Changes to registers:
    %rax:   0x0000000000000000      0x0000000000000cba
    %rsp:   0x0000000000000000      0x0000000000000200
    %r9:    0x0000000000000000      0x0000000000000c00
    
    Changes to memory:
    0x01f0: 0x0000000000000000      0x000000000000005b
    0x01f8: 0x0000000000000000      0x0000000000000013
```
The output of the simulation result shows the return value in `%rax` is the expected
sum of all linked list values.


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

Y86-64 code [`rsum_list.ys`](src/sim/misc/rsum_list.ys):
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
        call rsum_list         # rsum_list(ele1)
        ret

# long rsum_list(list_ptr ls)
# ls in %rdi
rsum_list:
        andq %rdi,%rdi         # Set CC
        jne recursion          # if (ls) goto recursion
        xorq %rax,%rax         # if (!ls) return 0
        ret
recursion:
        mrmovq (%rdi),%r9      # Get *ls
        pushq %r9
        mrmovq 8(%rdi),%rdi    # ls = ls->next
        call rsum_list
        popq %r9
        addq %r9,%rax          # Add current value to recursive result
        ret

# Stack starts here and grows to lower addresses
        .pos 0x200
stack:
```
We can put the `ys` file in `src/sim/misc` folder to build and test it:
```sh
  04-Architecture-Lab > cd src/sim/misc
  04-Architecture-Lab/src/sim/misc > make clean; make
  04-Architecture-Lab/src/sim/misc > yas rsum_list.ys; yis rsum_list.yo
    Stopped in 37 steps at PC = 0x13.  Status 'HLT', CC Z=0 S=0 O=0
    Changes to registers:
    %rax:   0x0000000000000000      0x0000000000000cba
    %rsp:   0x0000000000000000      0x0000000000000200
    %r9:    0x0000000000000000      0x000000000000000a

    Changes to memory:
    0x01c0: 0x0000000000000000      0x0000000000000089
    0x01c8: 0x0000000000000000      0x0000000000000c00
    0x01d0: 0x0000000000000000      0x0000000000000089
    0x01d8: 0x0000000000000000      0x00000000000000b0
    0x01e0: 0x0000000000000000      0x0000000000000089
    0x01e8: 0x0000000000000000      0x000000000000000a
    0x01f0: 0x0000000000000000      0x000000000000005b
    0x01f8: 0x0000000000000000      0x0000000000000013
```
The output at `%rax` is the expected value `0xcba`. A note on this recursion
implementation, before we do the recursion call on `rsum_list(ls->next)`, it
is necessary to push the current value of `*ls` to the stack, and then pop it
once the recursion call returns. This step is required for any local variable
which may be modified during recurssion.

### Copy src to dest and return xor checksum of src
Example C code:
```c
long copy_block(long *src, long *dest, long len)
{
    long result = 0;
    while (len > 0) {
        long val = *src++;
        *dest++ = val;
        result ^= val;
        len--;
    }
    return result;
}
```

```
# Execution begins at address 0
        .pos 0
        irmovq stack, %rsp      # Set up stack pointer
        call main               # Execute main function
        halt                    # Terminate program

        .align 8
# Source block
src:
        .quad 0x00a
        .quad 0x0b0
        .quad 0xc00

# Destination block
dest:
        .quad 0x111
        .quad 0x222
        .quad 0x333

main:
        irmovq src, %rsi
        irmovq dest, %rdi
        irmovq $3, %rcx
        call copy_block
        ret

# long copy_block(long *src, long *dest, long len)
# src in %rsi, dest in %rdi, len in %rcx
copy_block:
        irmovq $8,%r8          # Constant 8
        irmovq $1,%r9          # Constant 1 
        xorq %rax,%rax         # Set result = 0
        andq %rcx,%rcx         # Set CC
        jmp test

loop:
        mrmovq (%rsi),%r10     # %r10 = *src
        addq %r8,%rsi          # src++
        rmmovq %r10,(%rdi)     # *dest = %r10
        addq %r8,%rdi          # dest++
        xorq %r10,%rax         # Xor to result
        subq %r9,%rcx          # len--. Set CC
test:
        jne loop               # Stop when len == 0
        ret

# Stack starts here and grows to lower addresses
        .pos 0x200
stack:
```
By executing the `copy.ys`, we get the following:
```sh
  04-Architecture-Lab > cd src/sim/misc
  04-Architecture-Lab/src/sim/misc > make clean; make
  04-Architecture-Lab/src/sim/misc > yas copy.ys; yis copy.yo

    Stopped in 36 steps at PC = 0x13.  Status 'HLT', CC Z=1 S=0 O=0
    Changes to registers:
    %rax:   0x0000000000000000      0x0000000000000cba
    %rsp:   0x0000000000000000      0x0000000000000200
    %rsi:   0x0000000000000000      0x0000000000000030
    %rdi:   0x0000000000000000      0x0000000000000048
    %r8:    0x0000000000000000      0x0000000000000008
    %r9:    0x0000000000000000      0x0000000000000001
    %r10:   0x0000000000000000      0x0000000000000c00

    Changes to memory:
    0x0030: 0x0000000000000111      0x000000000000000a
    0x0038: 0x0000000000000222      0x00000000000000b0
    0x0040: 0x0000000000000333      0x0000000000000c00
    0x01f0: 0x0000000000000000      0x000000000000006f
    0x01f8: 0x0000000000000000      0x0000000000000013
```
Clearly, the `%rax` value is 0xcba when the program exits. The memory of
`dest`, starting from `0x0030`, is overwritten by the values from `src`.

## Task 2
Computations in sequential implementation of Y86-64 instruction `iaddq` :

| **Stage** | **iaddq V, rB**|
| :---      | :---           |
| Fetch     | $icode:ifun \leftarrow M_1[PC]$ |
| Fetch     | $rA:rB \leftarrow M_1[PC+1]$    |
| Fetch     | $valC \leftarrow M_8[PC+2]$     |
| Fetch     | $valP \leftarrow PC+10$         |
| Decode    | $valB \leftarrow R[rB]$ |
| Execute   | $valE \leftarrow valB + valC$ |
| Execute   | $Set CC$ |
| Memory    | -  |
| Write back| $R[rB] \leftarrow valE$ |
| PC update | $PC \leftarrow valP$ |

Now we can update the file [`seq-full.hcl`](src/sim/seq/seq-full.hcl) to implement
`iaddq`, by specifying all the sequential module related to this command. A diff file
compared with the original one is shown in [`seq-full.diff`](src/sim/seq/seq-full.diff),
which illustrates the modification required:
```diff
 bool instr_valid = icode in 
 	{ INOP, IHALT, IRRMOVQ, IIRMOVQ, IRMMOVQ, IMRMOVQ,
-	       IOPQ, IJXX, ICALL, IRET, IPUSHQ, IPOPQ };
+	       IOPQ, IJXX, ICALL, IRET, IPUSHQ, IPOPQ, IIADDQ };
 
 # Does fetched instruction require a regid byte?
 bool need_regids =
 	icode in { IRRMOVQ, IOPQ, IPUSHQ, IPOPQ, 
-		     IIRMOVQ, IRMMOVQ, IMRMOVQ };
+		     IIRMOVQ, IRMMOVQ, IMRMOVQ, IIADDQ };
 
 # Does fetched instruction require a constant word?
 bool need_valC =
-	icode in { IIRMOVQ, IRMMOVQ, IMRMOVQ, IJXX, ICALL };
+	icode in { IIRMOVQ, IRMMOVQ, IMRMOVQ, IJXX, ICALL, IIADDQ };
 
 ################ Decode Stage    ###################################
 
 ## What register should be used as the A source?
 word srcA = [
-	icode in { IRRMOVQ, IRMMOVQ, IOPQ, IPUSHQ  } : rA;
+	icode in { IRRMOVQ, IRMMOVQ, IOPQ, IPUSHQ, IIADDQ  } : rA;
 	icode in { IPOPQ, IRET } : RRSP;
 	1 : RNONE; # Don't need register
 ];
 
 ## What register should be used as the B source?
 word srcB = [
-	icode in { IOPQ, IRMMOVQ, IMRMOVQ  } : rB;
+	icode in { IOPQ, IRMMOVQ, IMRMOVQ, IIADDQ  } : rB;
 	icode in { IPUSHQ, IPOPQ, ICALL, IRET } : RRSP;
 	1 : RNONE;  # Don't need register
 ];

 ## What register should be used as the E destination?
 word dstE = [
 	icode in { IRRMOVQ } && Cnd : rB;
-	icode in { IIRMOVQ, IOPQ} : rB;
+	icode in { IIRMOVQ, IOPQ, IIADDQ} : rB;
 	icode in { IPUSHQ, IPOPQ, ICALL, IRET } : RRSP;
 	1 : RNONE;  # Don't write any register
 ];

 ## Select input A to ALU
 word aluA = [
 	icode in { IRRMOVQ, IOPQ } : valA;
-	icode in { IIRMOVQ, IRMMOVQ, IMRMOVQ } : valC;
+	icode in { IIRMOVQ, IRMMOVQ, IMRMOVQ, IIADDQ } : valC;
 	icode in { ICALL, IPUSHQ } : -8;
 	icode in { IRET, IPOPQ } : 8;
 	# Other instructions don't need ALU

 ## Select input B to ALU
 word aluB = [
 	icode in { IRMMOVQ, IMRMOVQ, IOPQ, ICALL, 
-		      IPUSHQ, IRET, IPOPQ } : valB;
+		      IPUSHQ, IRET, IPOPQ, IIADDQ } : valB;
 	icode in { IRRMOVQ, IIRMOVQ } : 0;
 	# Other instructions don't need ALU
 ];

...

 
 ## Should the condition codes be updated?
-bool set_cc = icode in { IOPQ };
+bool set_cc = icode in { IOPQ, IIADDQ };
 
 ################ Memory Stage    ###################################
...
```

## Task 3
