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

## Code Injection Attacks

The `ctarget` has a main routine directs to a `test` function of the following
```
1 unsigned getbuf()
2 {
3     char buf[BUFFER_SIZE];
4     Gets(buf);
5     return 1;
6 }
```

### Level 1

### Level 2

### Level 3
