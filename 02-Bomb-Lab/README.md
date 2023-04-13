# Bomb Lab

## Introduction
In this lab, students are given a x64 binary [bomb](src/bomb) and a main file [bomb.c](src/bomb.c). From the
main routine, we can see that there are 6 phases to guess in order to pass the bomb game. If any phase not
matched, the program returns immediately with a BOMB message. The target of Bomb Lab is to crack the 6 phases
by understanding the assembly code complied for binary bomb. To obtain the assembly code for bomb, you can
simply enter current directory and execute
```sh
  unix > objdump -d src/bomb > src/bomb.a
```
then you can browse the assembly program in `src/bomb.a`.
Next, We are going to defuse the phases one by one, with `gdb`.


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
  0x401338:    push   %r12
  0x40133a:    push   %rbp
  0x40133b:    push   %rbx
  0x40133c:    mov    %rdi,%rbx
  0x40133f:    mov    %rsi,%rbp
  0x401342:    callq  40131b <string_length>
  0x401347:    mov    %eax,%r12d
  0x40134a:    mov    %rbp,%rdi
  0x40134d:    callq  40131b <string_length>
  0x401352:    mov    $0x1,%edx
  0x401357:    cmp    %eax,%r12d
  0x40135a:    jne    40139b <strings_not_equal+0x63>
  0x40135c:    movzbl (%rbx),%eax
  0x40135f:    test   %al,%al
  0x401361:    je     401388 <strings_not_equal+0x50>
  0x401363:    cmp    0x0(%rbp),%al
  0x401366:    je     401372 <strings_not_equal+0x3a>
  0x401368:    jmp    40138f <strings_not_equal+0x57>
  0x40136a:    cmp    0x0(%rbp),%al
  0x40136d:    nopl   (%rax)
  0x401370:    jne    401396 <strings_not_equal+0x5e>
  0x401372:    add    $0x1,%rbx
  0x401376:    add    $0x1,%rbp
  0x40137a:    movzbl (%rbx),%eax
  0x40137d:    test   %al,%al
  0x40137f:    jne    40136a <strings_not_equal+0x32>
  0x401381:    mov    $0x0,%edx
  0x401386:    jmp    40139b <strings_not_equal+0x63>
  0x401388:    mov    $0x0,%edx
  0x40138d:    jmp    40139b <strings_not_equal+0x63>
  0x40138f:    mov    $0x1,%edx
  0x401394:    jmp    40139b <strings_not_equal+0x63>
  0x401396:    mov    $0x1,%edx
  0x40139b:    mov    %edx,%eax
  0x40139d:    pop    %rbx
  0x40139e:    pop    %rbp
  0x40139f:    pop    %r12
  0x4013a1:    retq
```
According to the assembly code for phase\_1, we can read the string from address 0x402400
```
  (gdb) x/s 0x402400
  0x402400:       "Border relations with Canada have never been better."
```
We get phase\_1 !!


## Phase 2

To accelerate the procedure, we can put all previous defused phases into a file `src/solution.txt` such that
we can run directly to later phase breakpoint by executing

We continue to set a break point at phase\_2 
```
  (gdb) b phase_2
  (gdb) r solution.txt
  (gdb) layout asm
```
and get the assembly code
```
  0x400efc:   push   %rbp                          # save address of previous stack frame
  0x400efd:   push   %rbx                          # save previous calculation value
  0x400efe:   sub    $0x28,%rsp                    # push down stack for 40 bytes
  0x400f02:   mov    %rsp,%rsi                     # save current stack frame to rsi
  0x400f05:   callq  40145c <read_six_numbers>
  0x400f0a:   cmpl   $0x1,(%rsp)                   # compare the first number with integer 1
  0x400f0e:   je     0x400f30 <phase_2+0x34>       # if equal, jump to instruction at address 0x400f30
  0x400f10:   callq  0x40143a <explode_bomb>       # BOMB if the first number is not equal to 1
  0x400f15:   jmp    0x400f30 <phase_2+0x34>       
  0x400f17:   mov    -0x4(%rbx),%eax               # move previous number to eax
  0x400f1a:   add    %eax,%eax                     # double eax
  0x400f1c:   cmp    %eax,(%rbx)                   # compare eax with current number
  0x400f1e:   je     0x400f25 <phase_2+0x29>       # jump to address 0x400f25 when (current int == 2 * previous int)
  0x400f20:   callq  0x40143a <explode_bomb>       # otherwise trigger BOMB
  0x400f25:   add    $0x4,%rbx                     # let rbx point to current number
  0x400f29:   cmp    %rbp,%rbx                     # check if arriving at end of number array (all 6 numbers evaluated)
  0x400f2c:   jne    0x400f17 <phase_2+0x1b>       # go to evaluate next number if not finished
  0x400f2e:   jmp    0x400f3c <phase_2+0x40>       
  0x400f30:   lea    0x4(%rsp),%rbx                # let rbx point to the second input number
  0x400f35:   lea    0x18(%rsp),%rbp               # let rbp point to end of number array (6 numbers use 24 bytes)
  0x400f3a:   jmp    0x400f17 <phase_2+0x1b>
  0x400f3c:   add    $0x28,%rsp                    # pop up stack for 40 bytes
  0x400f40:   pop    %rbx
  0x400f41:   pop    %rbp
  0x400f42:   retq
```
From the disassembled fraction of phase\_2, we know that it takes an array of 6 numbers.
The first number should be 1, and for the following numbers, each should be the twice of
its previous neighbor. Hence, phase\_2 must be "1 2 4 8 16 32". Let's add it to a second
line in [solution.txt](src/solution.txt)

## Phase 3
Taking the phase\_2 result, we can quickly break at phase\_3 and run directly here
```
  (gdb) b phase_3
  (gdb) r solution.txt
  (gdb) layout asm
```
and get the assembly code

```
  0x400f43:    sub    $0x18,%rsp                      # push down stack 24 bytes
  0x400f47:    lea    0xc(%rsp),%rcx                  # rcx = stack pointer + 12
  0x400f4c:    lea    0x8(%rsp),%rdx                  # rdx = stack pointer + 8
  0x400f51:    mov    $0x4025cf,%esi                  # esi = 0x4025cf ("%d %d")
  0x400f56:    mov    $0x0,%eax                       # eax = 0
  0x400f5b:    callq  0x400bf0 <__isoc99_sscanf@plt>  # call sscanf to parse input phase
  0x400f60:    cmp    $0x1,%eax                       # check if size > 1
  0x400f63:    jg     0x400f6a <phase_3+0x27>         # jump to instruction 0x400f6a if size > 1
  0x400f65:    callq  0x40143a <explode_bomb>
  0x400f6a:    cmpl   $0x7,0x8(%rsp)                  # compare the first input number with 7
  0x400f6f:    ja     0x400fad <phase_3+0x6a>         # first number <= 7, then go to instruction 0x400fad
  0x400f71:    mov    0x8(%rsp),%eax                  # put the first input number to eax
  0x400f75:    jmpq   *0x402470(,%rax,8)              # jump to instrction at (8 * rax + *0x402470)
  0x400f7c:    mov    $0xcf,%eax                      #################################################
  0x400f81:    jmp    0x400fbe <phase_3+0x7b>         #  This part can be intepreted as the following
  0x400f83:    mov    $0x2c3,%eax                     #  pseudo code:
  0x400f88:    jmp    0x400fbe <phase_3+0x7b>         #    eax = switch (first input number) {
  0x400f8a:    mov    $0x100,%eax                     #      0 => 0xcf  
  0x400f8f:    jmp    0x400fbe <phase_3+0x7b>         #      1 => 0x137
  0x400f91:    mov    $0x185,%eax                     #      2 => 0x2c3
  0x400f96:    jmp    0x400fbe <phase_3+0x7b>         #      3 => 0x100
  0x400f98:    mov    $0xce,%eax                      #      4 => 0x185
  0x400f9d:    jmp    0x400fbe <phase_3+0x7b>         #      5 => 0xce
  0x400f9f:    mov    $0x2aa,%eax                     #      6 => 0x2aa
  0x400fa4:    jmp    0x400fbe <phase_3+0x7b>         #      7 => 0x147
  0x400fa6:    mov    $0x147,%eax                     #    }                                      
  0x400fab:    jmp    0x400fbe <phase_3+0x7b>         #                                      
  0x400fad:    callq  0x40143a <explode_bomb>         # 
  0x400fb2:    mov    $0x0,%eax                       #
  0x400fb7:    jmp    0x400fbe <phase_3+0x7b>         #
  0x400fb9:    mov    $0x137,%eax                     ################################################# 
  0x400fbe:    cmp    0xc(%rsp),%eax                  # compare the second number with eax
  0x400fc2:    je     0x400fc9 <phase_3+0x86>         # PASS if the second number == eax
  0x400fc4:    callq  0x40143a <explode_bomb>         # BOMB otherwise
  0x400fc9:    add    $0x18,%rsp                      # pop up the stack 24 bytes
  0x400fcd:    retq
```
For the following part, we know phase\_3 calls sscanf to load input phase by format
```
  0x400f51:    mov    $0x4025cf,%esi                 
  0x400f56:    mov    $0x0,%eax                     
  0x400f5b:    callq  0x400bf0 <__isoc99_sscanf@plt>
```
And by displaying the string stored at `0x4025cf`, we know the input phase should be two
decimal numbers
```
  (gdb) x/s 0x4025cf
  0x4025cf:       "%d %d"
```
In phase\_3, there is a specific address jumping instrction of `jumpq`, which will jump
directly to instruction at address `(\*0x402470) +  8 \* rax`
```
  0x400f75:    jmpq   *0x402470(,%rax,8)*
```
And when we look at the value stored at `0x402470`, it turns out to be `0x400f7c`, which
is the next instruction of `jumpq`
```
  (gdb) x/gx 0x402470
  0x402470:       0x0000000000400f7c
```
Therefore, the remaining part of phase\_3 can be interpreted as a switch branch based
on the first input number. If the second number matches the mapping relationship, we
can pass phase\_3.
```
0 207
1 311
2 707
3 256
4 389
5 206 
6 682
7 327
```
We can append any pair of the map into `solution.txt`.

# Phase 4
Let's continue to crack `phase_4`
```
  (gdb) b phase_4
  (gdb) r solution.txt
  (gdb) layout asm
```

```
  0x40100c:	   sub    $0x18,%rsp                       # push down stack 24 bytes
  0x401010:	   lea    0xc(%rsp),%rcx
  0x401015:	   lea    0x8(%rsp),%rdx
  0x40101a:	   mov    $0x4025cf,%esi                   # string format ("%d %d")
  0x40101f:	   mov    $0x0,%eax
  0x401024:	   callq  0x400bf0 <__isoc99_sscanf@plt>   # call sscanf to parse input phase
  0x401029:	   cmp    $0x2,%eax                        # check if input size equals 2
  0x40102c:	   jne    0x401035 <phase_4+0x29>          # BOMB if not
  0x40102e:	   cmpl   $0xe,0x8(%rsp)                   # compare the first input number with 14
  0x401033:	   jbe    0x40103a <phase_4+0x2e>          # go to 0x40103a if 14 <= n1
  0x401035:	   callq  0x40143a <explode_bomb>
  0x40103a:	   mov    $0xe,%edx                        # edx = 14
  0x40103f:	   mov    $0x0,%esi                        # esi = 0
  0x401044:	   mov    0x8(%rsp),%edi
  0x401048:	   callq  0x400fce <func4>                 # call func4
  0x40104d:	   test   %eax,%eax
  0x40104f:	   jne    0x401058 <phase_4+0x4c>
  0x401051:	   cmpl   $0x0,0xc(%rsp)
  0x401056:	   je     0x40105d <phase_4+0x51>
  0x401058:	   callq  0x40143a <explode_bomb>
  0x40105d:	   add    $0x18,%rsp
  0x401061:	   retq   
```
