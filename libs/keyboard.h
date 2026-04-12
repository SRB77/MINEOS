/*
 * MineOS — Custom Keyboard Library
 * Handles terminal input — raw mode for character-by-character reading.
 *
 * Uses <termios.h> for raw mode — this is acceptable per the project guide
 * since <stdio.h> is allowed for terminal I/O simulation.
 */

#ifndef MINEOS_KEYBOARD_H
#define MINEOS_KEYBOARD_H

/* ── Public API ───────────────────────────────────────────────────── */
void keyboard_init(void);           /* switch terminal to raw mode */
void keyboard_restore(void);        /* restore terminal to normal mode */
int  key_pressed(void);             /* non-blocking: 1 if key available, 0 if not */
char read_char(void);               /* read one character (raw mode) */
void read_line(char *buf, int max_len);  /* read full line with backspace support */

#endif /* MINEOS_KEYBOARD_H */
