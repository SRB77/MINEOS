/*
 * MineOS — Custom Screen Library
 * All terminal rendering via ANSI escape codes.
 * No external rendering libraries used.
 */

#ifndef MINEOS_SCREEN_H
#define MINEOS_SCREEN_H

/* ── ANSI Color Code Constants ────────────────────────────────────── */
#define COLOR_RESET   0
#define COLOR_RED     1
#define COLOR_GREEN   2
#define COLOR_YELLOW  3
#define COLOR_BLUE    4
#define COLOR_CYAN    5
#define COLOR_BOLD    6
#define COLOR_MAGENTA 7
#define COLOR_WHITE   8

/* ── Public API ───────────────────────────────────────────────────── */
void screen_clear(void);                               /* clear entire terminal */
void screen_move_cursor(int row, int col);             /* position cursor at (row, col) */
void screen_print_char(char c);                        /* print single character */
void screen_print_str(const char *s);                  /* print a string */
void screen_print_int(int n);                          /* print integer */
void screen_print_color(const char *s, int color);     /* print string in color */
void screen_draw_line(char c, int length);             /* draw horizontal line */
void screen_boot_banner(void);                         /* MineOS ASCII art + boot sequence */
void screen_newline(void);                             /* print newline */

#endif /* MINEOS_SCREEN_H */
