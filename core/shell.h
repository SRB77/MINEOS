/*
 * MineOS — Shell
 * The main interactive loop: prompt → read → tokenize → dispatch → tick.
 */

#ifndef MINEOS_SHELL_H
#define MINEOS_SHELL_H

/* ── Public API ───────────────────────────────────────────────────── */
void shell_run(void);   /* starts the shell loop (blocks until "exit") */

#endif /* MINEOS_SHELL_H */
