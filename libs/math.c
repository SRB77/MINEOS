/*
 * MineOS — Custom Math Library Implementation
 * All arithmetic operations built from scratch.
 * No <math.h> used anywhere.
 */

#include "math.h"
#include <stdio.h>   /* allowed: for error messages only */

/* ── Basic Arithmetic ─────────────────────────────────────────────── */

int my_add(int a, int b) {
    return a + b;
}

int my_sub(int a, int b) {
    return a - b;
}

int my_mul(int a, int b) {
    return a * b;
}

int my_div(int a, int b) {
    if (b == 0) {
        printf("[math] Error: division by zero\n");
        return 0;
    }
    return a / b;
}

int my_mod(int a, int b) {
    if (b == 0) {
        printf("[math] Error: modulo by zero\n");
        return 0;
    }
    return a - my_mul(my_div(a, b), b);
}

/* ── Utility ──────────────────────────────────────────────────────── */

int my_abs(int a) {
    if (a < 0) return -a;
    return a;
}

int my_max(int a, int b) {
    if (a >= b) return a;
    return b;
}

int my_min(int a, int b) {
    if (a <= b) return a;
    return b;
}

/* ── Bounds & Power ───────────────────────────────────────────────── */

int in_bounds(int val, int low, int high) {
    if (val >= low && val <= high) return 1;
    return 0;
}

int my_pow(int base, int exp) {
    int result = 1;
    int i;

    if (exp < 0) {
        /* integer-only: negative exponent → 0 (truncation) */
        return 0;
    }

    for (i = 0; i < exp; i++) {
        result = my_mul(result, base);
    }

    return result;
}
