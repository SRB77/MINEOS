/*
 * MineOS — Custom String Library
 * Replaces <string.h> — all string operations built from scratch.
 * No standard string functions used anywhere in MineOS.
 */

#ifndef MINEOS_STRING_H
#define MINEOS_STRING_H

/* Basic string operations */
int   my_strlen(const char *s);
void  my_strcpy(char *dst, const char *src);
int   my_strcmp(const char *a, const char *b);
void  my_strcat(char *dst, const char *src);
char* my_strchr(const char *s, char c);

/* Tokenizer — splits input by delimiter, returns token count */
int   my_tokenize(char *input, char *tokens[], char delim);

/* Number ↔ String conversions */
void  my_int_to_str(int n, char *buf);    /* 42 → "42"  */
int   my_str_to_int(const char *s);       /* "42" → 42   */

/* Whitespace trimming */
void  my_str_trim(char *s);              /* remove leading/trailing spaces */

#endif /* MINEOS_STRING_H */
