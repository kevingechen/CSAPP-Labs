/* 
 * CS:APP Data Lab 
 * 
 * kevingechen
 * 
 * bits.c - Source file with your solutions to the Lab.
 *          This is the file you will hand in to your instructor.
 *
 * WARNING: Do not include the <stdio.h> header; it confuses the dlc
 * compiler. You can still use printf for debugging without including
 * <stdio.h>, although you might get a compiler warning. In general,
 * it's not good practice to ignore compiler warnings, but in this
 * case it's OK.  
 */

#if 0
/*
 * Instructions to Students:
 *
 * STEP 1: Read the following instructions carefully.
 */

You will provide your solution to the Data Lab by
editing the collection of functions in this source file.

INTEGER CODING RULES:
 
  Replace the "return" statement in each function with one
  or more lines of C code that implements the function. Your code 
  must conform to the following style:
 
  int Funct(arg1, arg2, ...) {
      /* brief description of how your implementation works */
      int var1 = Expr1;
      ...
      int varM = ExprM;

      varJ = ExprJ;
      ...
      varN = ExprN;
      return ExprR;
  }

  Each "Expr" is an expression using ONLY the following:
  1. Integer constants 0 through 255 (0xFF), inclusive. You are
      not allowed to use big constants such as 0xffffffff.
  2. Function arguments and local variables (no global variables).
  3. Unary integer operations ! ~
  4. Binary integer operations & ^ | + << >>
    
  Some of the problems restrict the set of allowed operators even further.
  Each "Expr" may consist of multiple operators. You are not restricted to
  one operator per line.

  You are expressly forbidden to:
  1. Use any control constructs such as if, do, while, for, switch, etc.
  2. Define or use any macros.
  3. Define any additional functions in this file.
  4. Call any functions.
  5. Use any other operations, such as &&, ||, -, or ?:
  6. Use any form of casting.
  7. Use any data type other than int.  This implies that you
     cannot use arrays, structs, or unions.

 
  You may assume that your machine:
  1. Uses 2s complement, 32-bit representations of integers.
  2. Performs right shifts arithmetically.
  3. Has unpredictable behavior when shifting if the shift amount
     is less than 0 or greater than 31.


EXAMPLES OF ACCEPTABLE CODING STYLE:
  /*
   * pow2plus1 - returns 2^x + 1, where 0 <= x <= 31
   */
  int pow2plus1(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     return (1 << x) + 1;
  }

  /*
   * pow2plus4 - returns 2^x + 4, where 0 <= x <= 31
   */
  int pow2plus4(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     int result = (1 << x);
     result += 4;
     return result;
  }

FLOATING POINT CODING RULES

For the problems that require you to implement floating-point operations,
the coding rules are less strict.  You are allowed to use looping and
conditional control.  You are allowed to use both ints and unsigneds.
You can use arbitrary integer and unsigned constants. You can use any arithmetic,
logical, or comparison operations on int or unsigned data.

You are expressly forbidden to:
  1. Define or use any macros.
  2. Define any additional functions in this file.
  3. Call any functions.
  4. Use any form of casting.
  5. Use any data type other than int or unsigned.  This means that you
     cannot use arrays, structs, or unions.
  6. Use any floating point data types, operations, or constants.


NOTES:
  1. Use the dlc (data lab checker) compiler (described in the handout) to 
     check the legality of your solutions.
  2. Each function has a maximum number of operations (integer, logical,
     or comparison) that you are allowed to use for your implementation
     of the function.  The max operator count is checked by dlc.
     Note that assignment ('=') is not counted; you may use as many of
     these as you want without penalty.
  3. Use the btest test harness to check your functions for correctness.
  4. Use the BDD checker to formally verify your functions
  5. The maximum number of ops for each function is given in the
     header comment for each function. If there are any inconsistencies 
     between the maximum ops in the writeup and in this file, consider
     this file the authoritative source.

/*
 * STEP 2: Modify the following functions according the coding rules.
 * 
 *   IMPORTANT. TO AVOID GRADING SURPRISES:
 *   1. Use the dlc compiler to check that your solutions conform
 *      to the coding rules.
 *   2. Use the BDD checker to formally verify that your solutions produce 
 *      the correct answers.
 */


#endif
//1
/* 
 * bitXor - x^y using only ~ and & 
 *   Example: bitXor(4, 5) = 1
 *   Legal ops: ~ &
 *   Max ops: 14
 *   Rating: 1
 */
int bitXor(int x, int y) {
  /* x^y = (~x)&y | x&(~y)
   * Let A = (~x)&y, B = x&(~y), then
   * A | B = ~(~(A | B)) = ~((~A) & (~B))*/
  int varA = (~x) & y;
  int varB = (~y) & x;
  return ~((~varA) & (~varB));
}

/* 
 * tmin - return minimum two's complement integer 
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 4
 *   Rating: 1
 */
int tmin(void) {
  
  return 1 << 31;

}

//2
/*
 * isTmax - returns 1 if x is the maximum, two's complement number,
 *     and 0 otherwise 
 *   Legal ops: ! ~ & ^ | +
 *   Max ops: 10
 *   Rating: 1
 */
int isTmax(int x) {
  /* Tmin == Tmax + 1 == ~Tmax && Tmax + 1 != 0 */
  return (!((~x) ^ (x+1))) & (!!(x+1));
}

/* 
 * allOddBits - return 1 if all odd-numbered bits in word set to 1
 *   where bits are numbered from 0 (least significant) to 31 (most significant)
 *   Examples allOddBits(0xFFFFFFFD) = 0, allOddBits(0xAAAAAAAA) = 1
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 12
 *   Rating: 2
 */
int allOddBits(int x) {
  int var1 = x & 0xAA;
  int var2 = (x >> 8) & 0xAA;
  int var3 = (x >> 16) & 0xAA;
  int var4 = (x >> 24) & 0xAA;
  int var = var1 & var2 & var3 & var4;
      
  return !(var ^ 0xAA);
}

/* 
 * negate - return -x 
 *   Example: negate(1) = -1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 5
 *   Rating: 2
 */
int negate(int x) {
  return ~x + 1;
}

//3
/* 
 * isAsciiDigit - return 1 if 0x30 <= x <= 0x39 (ASCII codes for characters '0' to '9')
 *   Example: isAsciiDigit(0x35) = 1.
 *            isAsciiDigit(0x3a) = 0.
 *            isAsciiDigit(0x05) = 0.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 15
 *   Rating: 3
 */
int isAsciiDigit(int x) {
  /* higher 4 bits exactly 0x30
   * and
   * lower 4 bits are 0b0xxx or 0b100x
   */
  int higher4Bits = x & (~ 0x0F);
  int lower4thBit = x & 0x08;
  int lower2ndTo4thBits = x & 0x0E;
  
  return (!(0x30 ^ higher4Bits)) & (!lower4thBit | !(lower2ndTo4thBits ^ 0x08));
}

/* 
 * conditional - same as x ? y : z 
 *   Example: conditional(2,4,5) = 4
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 16
 *   Rating: 3
 */
int conditional(int x, int y, int z) {
  return ((~(!!x) + 1) & y)  | ((~(!x) + 1) & z);
}

/* 
 * isLessOrEqual - if x <= y  then return 1, else return 0 
 *   Example: isLessOrEqual(4,5) = 1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 24
 *   Rating: 3
 */
int isLessOrEqual(int x, int y) {
  int Tmin = 1 << 31;
  int isNonNegativeX = !(Tmin & x);
  int isNegativeX = !isNonNegativeX;
  int isNonNegativeY = !(Tmin & y);
  int isNegativeY = !isNonNegativeY;
  int yMinusX = (~x) + 1 + y;
  int isNonNegativeYMinusX = !(Tmin & yMinusX);

  return (!(isNonNegativeX & isNegativeY)) & ((isNegativeX & isNonNegativeY) | isNonNegativeYMinusX);
}

//4
/* 
 * logicalNeg - implement the ! operator, using all of 
 *              the legal operators except !
 *   Examples: logicalNeg(3) = 0, logicalNeg(0) = 1
 *   Legal ops: ~ & ^ | + << >>
 *   Max ops: 12
 *   Rating: 4 
 */
int logicalNeg(int x) {
  int reverseX = ~x;
  int fold16Bits = reverseX & (reverseX >> 16);
  int fold8Bits = fold16Bits & (fold16Bits >> 8);
  int fold4Bits = fold8Bits & (fold8Bits >> 4);
  int fold2Bits = fold4Bits & (fold4Bits >> 2);
  return fold2Bits & (fold2Bits >> 1) & 1;
}

/* howManyBits - return the minimum number of bits required to represent x in
 *             two's complement
 *  Examples: howManyBits(12) = 5
 *            howManyBits(298) = 10
 *            howManyBits(-5) = 4
 *            howManyBits(0)  = 1
 *            howManyBits(-1) = 1
 *            howManyBits(0x80000000) = 32
 *  Legal ops: ! ~ & ^ | + << >>
 *  Max ops: 90
 *  Rating: 4
 */
int howManyBits(int x) {
  int Tmin = 1 << 31;
  int negOne = ~0;
  int isNonNeg = !(Tmin & x);
  int absoluteX = (isNonNeg + negOne) ^ x;
  int or1Bit = absoluteX | (absoluteX >> 1);
  int or2Bits = or1Bit | (or1Bit >> 2);
  int or4Bits = or2Bits | (or2Bits >> 4);
  int or8Bits = or4Bits | (or4Bits >> 8);
  int upperBound = or8Bits | (or8Bits >> 16);
  /* upperBound is the max value for n-bit two's complement 0b0...011...1*/
  int higher16Bits = upperBound >> 16;
  int lower16Bits = (~(Tmin >> 15)) & upperBound;
  int isLessOrEqualThan16Bits = !higher16Bits;
  int useHigher16BitsMask = isLessOrEqualThan16Bits + negOne;
  int remained16Bits = (useHigher16BitsMask & higher16Bits) |
                        ((~useHigher16BitsMask) & lower16Bits);

  int higher8Bits = remained16Bits >> 8;
  int lower8Bits = remained16Bits & 0xFF;
  int isLessOrEqualThan8Bits = !higher8Bits;
  int useHigher8BitsMask = isLessOrEqualThan8Bits + negOne;
  int remained8Bits = (useHigher8BitsMask & higher8Bits) |
                        ((~useHigher8BitsMask) & lower8Bits);

  int higher4Bits = remained8Bits >> 4;
  int lower4Bits = remained8Bits & 0x0F;
  int isLessOrEqualThan4Bits = !higher4Bits;
  int useHigher4BitsMask = isLessOrEqualThan4Bits + negOne;
  int remained4Bits = (useHigher4BitsMask & higher4Bits) |
                        ((~useHigher4BitsMask) & lower4Bits);

  int higher2Bits = remained4Bits >> 2;
  int lower2Bits = remained4Bits & 0x03;
  int isLessOrEqualThan2Bits = !higher2Bits;
  int useHigher2BitsMask = isLessOrEqualThan2Bits + negOne;
  int remained2Bits = (useHigher2BitsMask & higher2Bits) |
                        ((~useHigher2BitsMask) & lower2Bits);

  int higher1Bit = remained2Bits >> 1;
  int lower1Bit = remained2Bits & 0x01;
  int isLessOrEqualThan1Bit = !higher1Bit;
  int useHigher1BitMask = isLessOrEqualThan1Bit + negOne;
  int remained1Bit = (useHigher1BitMask & higher1Bit) |
                        ((~useHigher1BitMask) & lower1Bit);

  return 1 +
      (useHigher16BitsMask & 16) +
      (useHigher8BitsMask & 8) +
      (useHigher4BitsMask & 4) +
      (useHigher2BitsMask & 2) +
      (useHigher1BitMask & 1) +
      remained1Bit;
}

//float
/* 
 * floatScale2 - Return bit-level equivalent of expression 2*f for
 *   floating point argument f.
 *   Both the argument and result are passed as unsigned int's, but
 *   they are to be interpreted as the bit-level representation of
 *   single-precision floating point values.
 *   When argument is NaN, return argument
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4
 */
unsigned floatScale2(unsigned uf) {
  return 2;
}
/* 
 * floatFloat2Int - Return bit-level equivalent of expression (int) f
 *   for floating point argument f.
 *   Argument is passed as unsigned int, but
 *   it is to be interpreted as the bit-level representation of a
 *   single-precision floating point value.
 *   Anything out of range (including NaN and infinity) should return
 *   0x80000000u.
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4
 */
int floatFloat2Int(unsigned uf) {
  return 2;
}
/* 
 * floatPower2 - Return bit-level equivalent of the expression 2.0^x
 *   (2.0 raised to the power x) for any 32-bit integer x.
 *
 *   The unsigned value that is returned should have the identical bit
 *   representation as the single-precision floating-point number 2.0^x.
 *   If the result is too small to be represented as a denorm, return
 *   0. If too large, return +INF.
 * 
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. Also if, while 
 *   Max ops: 30 
 *   Rating: 4
 */
unsigned floatPower2(int x) {
    return 2;
}
