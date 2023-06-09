# Cache Lab

## Introduction
In this lab, we are going to work on two files:
```
csim.c       A Least-Recently Used(LRU) cache simulator
trans.c      A matrix transpose function to optimize cache behavior
```

## Task 1
In this task, we will implement an LRU cache simulator from scratch.  The input
is a polished version of valgrind trace file, having the format:
```
I 0400d7d4,8
 M 0421c7f0,4
 L 04f6b868,8
 S 7ff0005c8,8
```
Each line denotes one or two memory accesses. The format of each line is
```
[space]operation address,size
```
