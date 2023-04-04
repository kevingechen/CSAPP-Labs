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
  0x401338:             push   %r12
  0x40133a:             push   %rbp
  0x40133b:             push   %rbx
  0x40133c:             mov    %rdi,%rbx
  0x40133f:             mov    %rsi,%rbp
  0x401342:             callq  40131b <string_length>
  0x401347:             mov    %eax,%r12d
  0x40134a:             mov    %rbp,%rdi
  0x40134d:             callq  40131b <string_length>
  0x401352:             mov    $0x1,%edx
  0x401357:             cmp    %eax,%r12d
  0x40135a:             jne    40139b <strings_not_equal+0x63>
  0x40135c:             movzbl (%rbx),%eax
  0x40135f:             test   %al,%al
  0x401361:             je     401388 <strings_not_equal+0x50>
  0x401363:             cmp    0x0(%rbp),%al
  0x401366:             je     401372 <strings_not_equal+0x3a>
  0x401368:             jmp    40138f <strings_not_equal+0x57>
  0x40136a:             cmp    0x0(%rbp),%al
  0x40136d:             nopl   (%rax)
  0x401370:             jne    401396 <strings_not_equal+0x5e>
  0x401372:             add    $0x1,%rbx
  0x401376:             add    $0x1,%rbp
  0x40137a:             movzbl (%rbx),%eax
  0x40137d:             test   %al,%al
  0x40137f:             jne    40136a <strings_not_equal+0x32>
  0x401381:             mov    $0x0,%edx
  0x401386:             jmp    40139b <strings_not_equal+0x63>
  0x401388:             mov    $0x0,%edx
  0x40138d:             jmp    40139b <strings_not_equal+0x63>
  0x40138f:             mov    $0x1,%edx
  0x401394:             jmp    40139b <strings_not_equal+0x63>
  0x401396:             mov    $0x1,%edx
  0x40139b:             mov    %edx,%eax
  0x40139d:             pop    %rbx
  0x40139e:             pop    %rbp
  0x40139f:             pop    %r12
  0x4013a1:             retq
```
According to the assembly code for phase\_1, we can read the string from address 0x402400
```
  (gdb) i/s 0x402400
  0x402400:       "Border relations with Canada have never been better."
```
We get phase\_1 !!


## Phase 2

We continue to set a break point at phase\_2 
```
  (gdb) b phase_2
  (gdb) r
  (gdb) layout asm
```
and get the assembly code
```
  0x400efc:             push   %rbp
  0x400efd:             push   %rbx
  0x400efe:             sub    $0x28,%rsp
  0x400f02:             mov    %rsp,%rsi
  0x400f05:             callq  40145c <read_six_numbers>
  0x400f0a:             cmpl   $0x1,(%rsp)
  0x400f0e:             je     400f30 <phase_2+0x34>
  0x400f10:             callq  40143a <explode_bomb>
  0x400f15:             jmp    400f30 <phase_2+0x34>
  0x400f17:             mov    -0x4(%rbx),%eax
  0x400f1a:             add    %eax,%eax
  0x400f1c:             cmp    %eax,(%rbx)
  0x400f1e:             je     400f25 <phase_2+0x29>
  0x400f20:             callq  40143a <explode_bomb>
  0x400f25:             add    $0x4,%rbx
  0x400f29:             cmp    %rbp,%rbx
  0x400f2c:             jne    400f17 <phase_2+0x1b>
  0x400f2e:             jmp    400f3c <phase_2+0x40>
  0x400f30:             lea    0x4(%rsp),%rbx
  0x400f35:             lea    0x18(%rsp),%rbp
  0x400f3a:             jmp    400f17 <phase_2+0x1b>
  0x400f3c:             add    $0x28,%rsp
  0x400f40:             pop    %rbx
  0x400f41:             pop    %rbp
  0x400f42:             retq
```
