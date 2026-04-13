/*
 * MineOS — Background Task Scheduler
 * Manages a simple process table in VIRTUAL_RAM.
 * Phase 1: Minimal stub — init and tick only.
 */

#ifndef MINEOS_SCHEDULER_H
#define MINEOS_SCHEDULER_H

/* ── Process Status ───────────────────────────────────────────────── */
#define PROC_SLEEPING  0
#define PROC_RUNNING   1

/* ── Process Structure ────────────────────────────────────────────── */
typedef struct {
    int  pid;           /* process ID */
    char name[16];      /* task name */
    int  status;        /* PROC_SLEEPING or PROC_RUNNING */
    int  ticks;         /* how many times this task has been ticked */
} Process;

/* ── Public API ───────────────────────────────────────────────────── */
void scheduler_init(void);              /* register default background tasks */
int  scheduler_spawn(char *name);       /* add new task → returns pid */
void scheduler_kill(int pid);           /* terminate task */
void scheduler_tick(void);              /* called after every shell command */
void scheduler_list(void);              /* ps — render process table */

#endif /* MINEOS_SCHEDULER_H */
