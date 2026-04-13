/*
 * MineOS — Custom Screen Library Implementation
 * All terminal rendering via ANSI escape codes + printf.
 *
 * Uses: string.h (my_strlen, my_int_to_str)
 *       math.h   (my_add for loop counters)
 */

#include "screen.h"
#include "string.h"
#include "math.h"
#include "memory.h"
#include <stdio.h>    /* allowed: for printf */

/* ── ANSI Escape Code Strings ─────────────────────────────────────── */
static const char *ANSI_CODES[] = {
    "\033[0m",    /* COLOR_RESET   = 0 */
    "\033[31m",   /* COLOR_RED     = 1 */
    "\033[32m",   /* COLOR_GREEN   = 2 */
    "\033[33m",   /* COLOR_YELLOW  = 3 */
    "\033[34m",   /* COLOR_BLUE    = 4 */
    "\033[36m",   /* COLOR_CYAN    = 5 */
    "\033[1m",    /* COLOR_BOLD    = 6 */
    "\033[35m",   /* COLOR_MAGENTA = 7 */
    "\033[37m"    /* COLOR_WHITE   = 8 */
};

/* ── Screen Operations ────────────────────────────────────────────── */

void screen_clear(void) {
    printf("\033[2J\033[H");
    fflush(stdout);
}

void screen_move_cursor(int row, int col) {
    printf("\033[%d;%dH", row, col);
    fflush(stdout);
}

void screen_print_char(char c) {
    printf("%c", c);
    fflush(stdout);
}

void screen_print_str(const char *s) {
    printf("%s", s);
    fflush(stdout);
}

void screen_print_int(int n) {
    char buf[16];
    my_int_to_str(n, buf);
    printf("%s", buf);
    fflush(stdout);
}

void screen_print_color(const char *s, int color) {
    if (in_bounds(color, 0, 8)) {
        printf("%s%s%s", ANSI_CODES[color], s, ANSI_CODES[COLOR_RESET]);
    } else {
        printf("%s", s);
    }
    fflush(stdout);
}

void screen_draw_line(char c, int length) {
    int i;
    for (i = 0; i < length; i = my_add(i, 1)) {
        printf("%c", c);
    }
    fflush(stdout);
}

void screen_newline(void) {
    printf("\n");
    fflush(stdout);
}

/* ── Boot Banner ──────────────────────────────────────────────────── */

void screen_boot_banner(void) {
    screen_clear();
    printf("\n");

    /* ASCII art logo in bold green */
    screen_print_color(
        "  ███╗   ███╗██╗███╗   ██╗███████╗ ██████╗ ███████╗\n",
        COLOR_GREEN
    );
    screen_print_color(
        "  ████╗ ████║██║████╗  ██║██╔════╝██╔═══██╗██╔════╝\n",
        COLOR_GREEN
    );
    screen_print_color(
        "  ██╔████╔██║██║██╔██╗ ██║█████╗  ██║   ██║███████╗\n",
        COLOR_GREEN
    );
    screen_print_color(
        "  ██║╚██╔╝██║██║██║╚██╗██║██╔══╝  ██║   ██║╚════██║\n",
        COLOR_GREEN
    );
    screen_print_color(
        "  ██║ ╚═╝ ██║██║██║ ╚████║███████╗╚██████╔╝███████║\n",
        COLOR_GREEN
    );
    screen_print_color(
        "  ╚═╝     ╚═╝╚═╝╚═╝  ╚═══╝╚══════╝ ╚═════╝ ╚══════╝\n",
        COLOR_GREEN
    );

    printf("\n");

    /* Version line */
    screen_print_color(
        "  MineOS v1.0",
        COLOR_BOLD
    );
    screen_print_color(
        " — Built by Execution Unit | Newton School of Technology\n",
        COLOR_CYAN
    );

    printf("\n");

    /* Simulated boot messages */
    screen_print_str("  Initializing virtual RAM");
    screen_draw_line('.', 11);
    screen_print_color(" [  OK  ]\n", COLOR_GREEN);

    screen_print_str("  Mounting file system");
    screen_draw_line('.', 15);
    screen_print_color(" [  OK  ]\n", COLOR_GREEN);

    screen_print_str("  Starting scheduler");
    screen_draw_line('.', 17);
    screen_print_color(" [  OK  ]\n", COLOR_GREEN);

    screen_print_str("  Booting shell");
    screen_draw_line('.', 22);
    screen_print_color(" [  OK  ]\n", COLOR_GREEN);

    printf("\n");
}
