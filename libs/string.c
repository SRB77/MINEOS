/*
 * MineOS — Custom String Library Implementation
 * All string operations built from scratch.
 * No <string.h> used anywhere.
 *
 * Uses: math.h (my_abs for int_to_str with negative numbers)
 */

#include "string.h"
#include "math.h"

/* ── Basic String Operations ──────────────────────────────────────── */

int my_strlen(const char *s) {
    int len = 0;
    while (s[len] != '\0') {
        len = my_add(len, 1);
    }
    return len;
}

void my_strcpy(char *dst, const char *src) {
    int i = 0;
    while (src[i] != '\0') {
        dst[i] = src[i];
        i = my_add(i, 1);
    }
    dst[i] = '\0';
}

int my_strcmp(const char *a, const char *b) {
    int i = 0;
    while (a[i] != '\0' && b[i] != '\0') {
        if (a[i] != b[i]) {
            return my_sub((int)a[i], (int)b[i]);
        }
        i = my_add(i, 1);
    }
    return my_sub((int)a[i], (int)b[i]);
}

void my_strcat(char *dst, const char *src) {
    int dst_len = my_strlen(dst);
    int i = 0;
    while (src[i] != '\0') {
        dst[my_add(dst_len, i)] = src[i];
        i = my_add(i, 1);
    }
    dst[my_add(dst_len, i)] = '\0';
}

char* my_strchr(const char *s, char c) {
    int i = 0;
    while (s[i] != '\0') {
        if (s[i] == c) {
            return (char *)&s[i];
        }
        i = my_add(i, 1);
    }
    /* also check the null terminator if searching for '\0' */
    if (c == '\0') {
        return (char *)&s[i];
    }
    return (char *)0; /* NULL — not found */
}

/* ── Tokenizer ────────────────────────────────────────────────────── */

/*
 * Splits input string by delimiter character.
 * Modifies input in place (replaces delimiters with '\0').
 * Fills tokens[] with pointers into the modified input.
 * Returns the number of tokens found.
 *
 * Example: my_tokenize("mkdir foo", tokens, ' ')
 *   → tokens[0] = "mkdir", tokens[1] = "foo", returns 2
 */
int my_tokenize(char *input, char *tokens[], char delim) {
    int count = 0;
    int i = 0;
    int in_token = 0;
    int len = my_strlen(input);

    while (i < len) {
        if (input[i] == delim) {
            /* end current token */
            input[i] = '\0';
            in_token = 0;
        } else {
            if (!in_token) {
                /* start of a new token */
                tokens[count] = &input[i];
                count = my_add(count, 1);
                in_token = 1;
            }
        }
        i = my_add(i, 1);
    }

    return count;
}

/* ── Number ↔ String Conversions ──────────────────────────────────── */

/*
 * Converts integer to string representation.
 * Handles negative numbers using my_abs() from math.c.
 * Caller must provide buf with enough space (at least 12 bytes for int).
 */
void my_int_to_str(int n, char *buf) {
    int i = 0;
    int j;
    int is_negative = 0;
    int digit;
    char temp;

    if (n == 0) {
        buf[0] = '0';
        buf[1] = '\0';
        return;
    }

    if (n < 0) {
        is_negative = 1;
        n = my_abs(n);
    }

    /* extract digits in reverse order */
    while (n > 0) {
        digit = my_mod(n, 10);
        buf[i] = (char)my_add(digit, '0');
        n = my_div(n, 10);
        i = my_add(i, 1);
    }

    if (is_negative) {
        buf[i] = '-';
        i = my_add(i, 1);
    }

    buf[i] = '\0';

    /* reverse the string in place */
    j = 0;
    i = my_sub(i, 1);
    while (j < i) {
        temp = buf[j];
        buf[j] = buf[i];
        buf[i] = temp;
        j = my_add(j, 1);
        i = my_sub(i, 1);
    }
}

/*
 * Converts string to integer.
 * Handles optional leading '-' sign.
 * Stops at first non-digit character.
 */
int my_str_to_int(const char *s) {
    int result = 0;
    int i = 0;
    int is_negative = 0;

    /* skip leading whitespace */
    while (s[i] == ' ' || s[i] == '\t') {
        i = my_add(i, 1);
    }

    /* handle sign */
    if (s[i] == '-') {
        is_negative = 1;
        i = my_add(i, 1);
    } else if (s[i] == '+') {
        i = my_add(i, 1);
    }

    /* convert digits */
    while (s[i] >= '0' && s[i] <= '9') {
        result = my_add(my_mul(result, 10), my_sub(s[i], '0'));
        i = my_add(i, 1);
    }

    if (is_negative) {
        result = my_sub(0, result);
    }

    return result;
}

/* ── Whitespace Trimming ──────────────────────────────────────────── */

/*
 * Removes leading and trailing whitespace from string in place.
 */
void my_str_trim(char *s) {
    int start = 0;
    int end;
    int len = my_strlen(s);
    int i;

    if (len == 0) return;

    /* find first non-space character */
    while (s[start] == ' ' || s[start] == '\t' || s[start] == '\n' || s[start] == '\r') {
        start = my_add(start, 1);
    }

    /* find last non-space character */
    end = my_sub(len, 1);
    while (end >= start && (s[end] == ' ' || s[end] == '\t' || s[end] == '\n' || s[end] == '\r')) {
        end = my_sub(end, 1);
    }

    /* shift content to beginning */
    if (start > 0) {
        i = 0;
        while (my_add(start, i) <= end) {
            s[i] = s[my_add(start, i)];
            i = my_add(i, 1);
        }
        s[i] = '\0';
    } else {
        s[my_add(end, 1)] = '\0';
    }
}
