/*
 * MineOS — Custom Keyboard Library Implementation
 * Raw terminal input using <termios.h>.
 *
 * For Phase 1, read_line() uses a simple fgets-based approach
 * wrapped to provide consistent API. We keep raw mode functions
 * ready for Phase 2 enhancement.
 *
 * Uses: math.h (my_sub for index calculations)
 */

#include "keyboard.h"
#include "math.h"
#include <stdio.h>       /* allowed: for terminal I/O */
#include <termios.h>     /* for raw mode — acceptable per project guide */
#include <unistd.h>      /* for read(), STDIN_FILENO */

/* ── Terminal State ───────────────────────────────────────────────── */
static struct termios original_termios;
static int raw_mode_active = 0;

/* ── Switch to Raw Mode ───────────────────────────────────────────── */
void keyboard_init(void) {
    struct termios raw;

    /* Save original terminal settings */
    tcgetattr(STDIN_FILENO, &original_termios);

    raw = original_termios;

    /* Disable canonical mode (line buffering) and echo */
    raw.c_lflag &= ~(ICANON | ECHO);

    /* Set minimum characters for read and timeout */
    raw.c_cc[VMIN] = 1;    /* read returns after 1 char */
    raw.c_cc[VTIME] = 0;   /* no timeout */

    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
    raw_mode_active = 1;
}

/* ── Restore Normal Mode ──────────────────────────────────────────── */
void keyboard_restore(void) {
    if (raw_mode_active) {
        tcsetattr(STDIN_FILENO, TCSAFLUSH, &original_termios);
        raw_mode_active = 0;
    }
}

/* ── Non-Blocking Key Check ───────────────────────────────────────── */
/*
 * Returns 1 if a key is available, 0 if not.
 * Uses a brief timeout approach.
 */
int key_pressed(void) {
    struct termios tmp;
    int result;
    char c;

    tcgetattr(STDIN_FILENO, &tmp);
    tmp.c_cc[VMIN] = 0;     /* don't block */
    tmp.c_cc[VTIME] = 0;    /* no timeout */
    tcsetattr(STDIN_FILENO, TCSANOW, &tmp);

    result = (int)read(STDIN_FILENO, &c, 1);

    /* Restore blocking read */
    tmp.c_cc[VMIN] = 1;
    tcsetattr(STDIN_FILENO, TCSANOW, &tmp);

    return (result > 0) ? 1 : 0;
}

/* ── Read Single Character ────────────────────────────────────────── */
char read_char(void) {
    char c = 0;
    read(STDIN_FILENO, &c, 1);
    return c;
}

/* ── Read Full Line (with backspace support) ──────────────────────── */
/*
 * Reads characters one at a time in raw mode.
 * Handles:
 *   - Printable chars: append to buffer + echo
 *   - Backspace (127 or 8): erase last char from buffer + screen
 *   - Enter ('\r' or '\n'): null-terminate and return
 *   - Buffer full: stop accepting input (beep)
 */
void read_line(char *buf, int max_len) {
    int pos = 0;
    char c;
    int limit = my_sub(max_len, 1);  /* leave room for null terminator */

    while (1) {
        c = read_char();

        if (c == '\r' || c == '\n') {
            /* Enter pressed — finish */
            buf[pos] = '\0';
            printf("\n");
            fflush(stdout);
            return;
        }
        else if (c == 127 || c == 8) {
            /* Backspace — erase last character */
            if (pos > 0) {
                pos = my_sub(pos, 1);
                /* Erase character on screen: move back, overwrite with space, move back */
                printf("\b \b");
                fflush(stdout);
            }
        }
        else if (c >= 32 && c < 127) {
            /* Printable character */
            if (pos < limit) {
                buf[pos] = c;
                pos = my_add(pos, 1);
                /* Echo character to screen */
                printf("%c", c);
                fflush(stdout);
            }
            /* else: buffer full, silently ignore */
        }
        /* else: ignore control characters */
    }
}
