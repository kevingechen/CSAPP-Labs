# Bomb Lab

## Introduction
In this lab, students are given a x64 binary [bomb](src/bomb) and a main file [bomb.c](src/bomb.c). From the
main routine, we can see that there are 6 phases to guess in order to pass the bomb game. If any phase not
matched, the program returns immediately with a BOMB message. The target of Bomb Lab is to crack the 6 phases
by understanding the assembly code complied for binary bomb. We are going to defuse the phases one by one,
with `gdb`.

## Phase 1
Assume you are now in our current directory on a unix shell, let's enter our src folder and launch `gdb`
```sh
  unix > cd src/
  unix > gdb bomb
```
Then in the `gdb` console, we will set a break point at phase\_1 and run program till it.
```sh
  (gdb) b phase_1
  Breakpoint 1 at 0x400ee0
  (gdb) r
  Starting program: /data00/home/chenge.123/projects/mygithub/CSAPP-Labs/02-Bomb-Lab/src/bomb
  Welcome to my fiendish little bomb. You have 6 phases with
  which to blow yourself up. Have a nice day!
  blablabla...
  
  Breakpoint 1, 0x0000000000400ee0 in phase_1 ()
  (gdb) layout asm
```
We now arrive at the first break point, and by inputting random words ended with adn `enter`, we can
start analyzing the assembly code for phase\_1
```
  0x400ee0 <phase_1>      sub    $0x8,%rsp
  0x400ee4 <phase_1+4>    mov    $0x402400,%esi
  0x400ee9 <phase_1+9>    callq  0x401338 <strings_not_equal>
  0x400eee <phase_1+14>   test   %eax,%eax
  0x400ef0 <phase_1+16>   je     0x400ef7 <phase_1+23>
  0x400ef2 <phase_1+18>   callq  0x40143a <explode_bomb>
  0x400ef7 <phase_1+23>   add    $0x8,%rsp
  0x400efb <phase_1+27>   retq
```

