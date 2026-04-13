/*
 * MineOS — Custom Math Library
 * Replaces <math.h> — all arithmetic operations built from scratch.
 * No standard math functions used anywhere in MineOS.
 */

#ifndef MINEOS_MATH_H
#define MINEOS_MATH_H

/* Basic arithmetic */
int my_add(int a, int b);
int my_sub(int a, int b);
int my_mul(int a, int b);
int my_div(int a, int b);       /* integer division, safe div-by-zero */
int my_mod(int a, int b);       /* modulo, safe div-by-zero */

/* Utility */
int my_abs(int a);               /* absolute value */
int my_max(int a, int b);
int my_min(int a, int b);

/* Bounds & power */
int in_bounds(int val, int low, int high);   /* returns 1 if low <= val <= high */
int my_pow(int base, int exp);               /* iterative exponentiation */

#endif /* MINEOS_MATH_H */
