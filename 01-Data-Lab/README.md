# CS:APP Data Lab
> Copyright (c) 2002-2018, R. Bryant and D. O'Hallaron

This directory contains the files that you will need to run the CS:APP
Data Lab, which helps develops the student's understanding of bit
representations, two's complement arithmetic, and IEEE floating point.

For fun, we've also provided a new user-level HTTP-based "Beat the
Prof" contest that replaces the old email-based version. The new
contest is completely self-contained and does not require root
password.  The only requirement is that you have a user account on a
Linux machine with an IP address.

System requirements: Uses bison and flex to build dlc.

## 1. Overview

In this lab, students work on a C file, called [bits.c](/src/bits.c), that consists
of a series of programming "puzzles".  Each puzzle is an empty
function body that must be completed so that it implements a specified
mathematical function, such as "absolute value". Students must solve
the non-floating point puzzles using only straight-line C code and a
restricted set of C arithmetic and logical operators. For the
floating-point puzzles they can use conditionals and arbitrary
operators.

Students use the following three tools to check their work.
Instructors use the same tools to assign grades.

1. `dlc`: A "data lab compiler" that checks each function in bits.c for
compliance with the coding guidelines, checking that the students use
less than the maximum number of operators, that they use only
straight-line code, and that they use only legal operators. The
sources and a Linux binary are included with the lab.

2. `btest`: A test harness that checks the functions in bits.c for
correctness. This tool has been significantly improved, now checking
wide swaths around the edge cases for integers and floating point
representations such as 0, Tmin, denorm-norm boundary, and inf.

3. `driver.pl`: A autograding driver program that uses dlc and btest to
check each test function in bits.c for correctness and adherence to
the coding guidelines

The default version of the lab consists of 15 puzzles, in
./src/selections.c, chosen from a set of 73 standard puzzles defined
in the directory ./src/puzzles/. You can customize the lab from term
to term by choosing a different set of puzzles from the standard set
of puzzles.

You can also define new puzzles of your own and add them to the
standard set. See ./src/README for instructions on how to add new
puzzles to the standard set.

> NOTE: If you define new puzzles, please send them to me (Dave
> O'Hallaron, droh@cs.cmu.edu) so that I can add them to the standard
> set of puzzles in the data lab distribution.


## 2. Building the Lab

```
     unix> cd src/
     unix> make clean
     unix> make 
```
The Makefile generates the btest source files, builds the dlc binary
(if necessary), formats the lab writeup, and then copies btest, the
dlc binary, and the driver to the handout directory.  After that, it
builds a tarfile of the handout directory (in ./datalab-handout.tar)
that you can then hand out to students.

Note on Binary Portability: dlc is distributed in the datalab-handout
directory as a binary. Linux binaries are not always portable across
distributions due to different versions of dynamic libraries. You'll
need to be careful to compile dlc on a machine that is compatible with
those that the students will be using.

> Note: Running "make" also automatically generates the solutions to the
> puzzles, which you can find in ./src/bits.c and ./src/bits.c-solution.


## 3. Grading the Lab

There is a handy autograder script that automatically grades your
students' handins.  See ./grade/README for instructions.

