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
We now arrive at the first break point, and by inputting random words ended with adn `Enter`, we can
start analyzing the assembly code for phase\_1
```
  0x400ee0 <phase_1>      sub    $0x8,%rsp                      # push down the stack 8 bytes
  0x400ee4 <phase_1+4>    mov    $0x402400,%esi                 # move the string data from mem address 0x402400 to register esi
  0x400ee9 <phase_1+9>    callq  0x401338 <strings_not_equal>   # call function strings_not_equal
  0x400eee <phase_1+14>   test   %eax,%eax                      # store the result in register eax
  0x400ef0 <phase_1+16>   je     0x400ef7 <phase_1+23>          # if input string equals target, jump to end of current funciton
  0x400ef2 <phase_1+18>   callq  0x40143a <explode_bomb>        # if not, trigger BOMB
  0x400ef7 <phase_1+23>   add    $0x8,%rsp                      # pop up the stack 8 bytes
  0x400efb <phase_1+27>   retq
```
We can further break at the `strings_not_equal` call
```
  0x401338:   41 54                   push   %r12
  0x40133a:   55                      push   %rbp
  0x40133b:   53                      push   %rbx
  0x40133c:   48 89 fb                mov    %rdi,%rbx
  0x40133f:   48 89 f5                mov    %rsi,%rbp
  0x401342:   e8 d4 ff ff ff          callq  40131b <string_length>
  0x401347:   41 89 c4                mov    %eax,%r12d
  0x40134a:   48 89 ef                mov    %rbp,%rdi
  0x40134d:   e8 c9 ff ff ff          callq  40131b <string_length>
  0x401352:   ba 01 00 00 00          mov    $0x1,%edx
  0x401357:   41 39 c4                cmp    %eax,%r12d
  0x40135a:   75 3f                   jne    40139b <strings_not_equal+0x63>
  0x40135c:   0f b6 03                movzbl (%rbx),%eax
  0x40135f:   84 c0                   test   %al,%al
  0x401361:   74 25                   je     401388 <strings_not_equal+0x50>
  0x401363:   3a 45 00                cmp    0x0(%rbp),%al
  0x401366:   74 0a                   je     401372 <strings_not_equal+0x3a>
  0x401368:   eb 25                   jmp    40138f <strings_not_equal+0x57>
  0x40136a:   3a 45 00                cmp    0x0(%rbp),%al
  0x40136d:   0f 1f 00                nopl   (%rax)
  0x401370:   75 24                   jne    401396 <strings_not_equal+0x5e>
  0x401372:   48 83 c3 01             add    $0x1,%rbx
  0x401376:   48 83 c5 01             add    $0x1,%rbp
  0x40137a:   0f b6 03                movzbl (%rbx),%eax
  0x40137d:   84 c0                   test   %al,%al
  0x40137f:   75 e9                   jne    40136a <strings_not_equal+0x32>
  0x401381:   ba 00 00 00 00          mov    $0x0,%edx
  0x401386:   eb 13                   jmp    40139b <strings_not_equal+0x63>
  0x401388:   ba 00 00 00 00          mov    $0x0,%edx
  0x40138d:   eb 0c                   jmp    40139b <strings_not_equal+0x63>
  0x40138f:   ba 01 00 00 00          mov    $0x1,%edx
  0x401394:   eb 05                   jmp    40139b <strings_not_equal+0x63>
  0x401396:   ba 01 00 00 00          mov    $0x1,%edx
  0x40139b:   89 d0                   mov    %edx,%eax
  0x40139d:   5b                      pop    %rbx
  0x40139e:   5d                      pop    %rbp
  0x40139f:   41 5c                   pop    %r12
  0x4013a1:   c3                      retq
```
According to the assembly code for phase\_1, we can read the string from address 0x402400
```
  (gdb) i/s 0x402400
  0x402400:       "Border relations with Canada have never been better."
```
We get phase\_1 !!


## Phase 2
