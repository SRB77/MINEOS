/*
 * MineOS — Background Task Scheduler Implementation
 * Phase 1: Minimal stub — tracks a global tick counter.
 * Phase 2: Full process table with background tasks.
 *
 * Uses: memory.h (VIRTUAL_RAM, process table offsets)
 *       string.h (my_strcpy)
 *       math.h   (my_add)
 *       screen.h (screen_print_str, screen_print_color)
 */

#include "scheduler.h"
#include "../libs/memory.h"
#include "../libs/math.h"
#include "../libs/string.h"
#include "../libs/screen.h"

/* ── Global tick counter ──────────────────────────────────────────── */
static int global_ticks = 0;

/* ── Helper: get process slot pointer from VIRTUAL_RAM ────────────── */
static Process* get_process(int slot) {
    if (!in_bounds(slot, 0, my_sub(MAX_PROCESSES, 1))) {
        return (Process *)0;
    }
    return (Process *)(VIRTUAL_RAM + my_add(PROCESS_TABLE_OFFSET, my_mul(slot, PROCESS_ENTRY_SIZE)));
}

/* ── Initialize Scheduler ─────────────────────────────────────────── */
/*
 * Zero out all 16 process slots in VIRTUAL_RAM.
 * Set all PIDs to -1 (empty). Spawn the default "uptime" task.
 */
void scheduler_init(void) {
    int i;
    Process *proc;

    global_ticks = 0;

    /* Zero all process slots and mark as empty */
    for (i = 0; i < MAX_PROCESSES; i = my_add(i, 1)) {
        proc = get_process(i);
        if (proc != (Process *)0) {
            proc->pid = -1;
            proc->name[0] = '\0';
            proc->status = PROC_SLEEPING;
            proc->ticks = 0;
        }
    }

    /* Spawn default background task */
    scheduler_spawn("uptime");
}

/* ── Spawn Process ────────────────────────────────────────────────── */
/*
 * Finds the first empty slot (pid == -1) and registers a new process.
 * Returns the pid (slot index) or -1 if table is full.
 */
int scheduler_spawn(char *name) {
    int i;
    Process *proc;

    for (i = 0; i < MAX_PROCESSES; i = my_add(i, 1)) {
        proc = get_process(i);
        if (proc != (Process *)0 && proc->pid == -1) {
            proc->pid = i;
            my_strcpy(proc->name, name);
            proc->status = PROC_RUNNING;
            proc->ticks = 0;
            return i;
        }
    }

    screen_print_color("[scheduler] Error: process table full\n", COLOR_RED);
    return -1;
}

/* ── Kill Process ─────────────────────────────────────────────────── */
void scheduler_kill(int pid) {
    Process *proc;

    if (!in_bounds(pid, 0, my_sub(MAX_PROCESSES, 1))) {
        screen_print_color("[scheduler] Error: invalid PID\n", COLOR_RED);
        return;
    }

    proc = get_process(pid);
    if (proc == (Process *)0 || proc->pid == -1) {
        screen_print_color("[scheduler] Error: no such process\n", COLOR_RED);
        return;
    }

    screen_print_color("  → Process terminated: ", COLOR_YELLOW);
    screen_print_color(proc->name, COLOR_YELLOW);
    screen_print_str("\n");

    proc->pid = -1;
    proc->status = PROC_SLEEPING;
    proc->name[0] = '\0';
    proc->ticks = 0;
}

/* ── Tick ──────────────────────────────────────────────────────────── */
/*
 * Called after every shell command.
 * Increments global_ticks. For every RUNNING process, increments its ticks.
 * Every 5 ticks, prints a visible uptime message.
 */
void scheduler_tick(void) {
    int i;
    Process *proc;

    global_ticks = my_add(global_ticks, 1);

    /* Increment ticks for all running processes */
    for (i = 0; i < MAX_PROCESSES; i = my_add(i, 1)) {
        proc = get_process(i);
        if (proc != (Process *)0 && proc->pid != -1 && proc->status == PROC_RUNNING) {
            proc->ticks = my_add(proc->ticks, 1);
        }
    }

    /* Print uptime message every 5 ticks */
    if (my_mod(global_ticks, 5) == 0) {
        char buf[16];
        screen_print_color("  [uptime] system tick: ", COLOR_MAGENTA);
        my_int_to_str(global_ticks, buf);
        screen_print_color(buf, COLOR_MAGENTA);
        screen_print_str("\n");
    }
}

/* ── List Processes ───────────────────────────────────────────────── */
/*
 * ps — prints a formatted table of all active processes.
 */
void scheduler_list(void) {
    int i;
    int active_count = 0;
    Process *proc;
    char buf[16];

    screen_print_str("\n");
    screen_print_color("  ╔═════╤══════════════════╤══════════╤════════╗\n", COLOR_CYAN);
    screen_print_color("  ║ PID │ NAME             │ STATUS   │ TICKS  ║\n", COLOR_CYAN);
    screen_print_color("  ╠═════╪══════════════════╪══════════╪════════╣\n", COLOR_CYAN);

    for (i = 0; i < MAX_PROCESSES; i = my_add(i, 1)) {
        proc = get_process(i);
        if (proc != (Process *)0 && proc->pid != -1) {
            active_count = my_add(active_count, 1);

            screen_print_color("  ║ ", COLOR_CYAN);
            my_int_to_str(proc->pid, buf);
            screen_print_str(buf);
            screen_print_str("   │ ");

            screen_print_color(proc->name, COLOR_GREEN);
            /* Pad name to 16 chars */
            {
                int pad = my_sub(16, my_strlen(proc->name));
                int p;
                for (p = 0; p < pad; p = my_add(p, 1)) {
                    screen_print_str(" ");
                }
            }
            screen_print_str(" │ ");

            if (proc->status == PROC_RUNNING) {
                screen_print_color("RUNNING ", COLOR_GREEN);
            } else {
                screen_print_color("SLEEPING", COLOR_YELLOW);
            }
            screen_print_str(" │ ");

            my_int_to_str(proc->ticks, buf);
            screen_print_str(buf);
            /* Pad ticks to 6 chars */
            {
                int pad = my_sub(6, my_strlen(buf));
                int p;
                for (p = 0; p < pad; p = my_add(p, 1)) {
                    screen_print_str(" ");
                }
            }
            screen_print_color("║\n", COLOR_CYAN);
        }
    }

    if (active_count == 0) {
        screen_print_color("  ║       (no active processes)                ║\n", COLOR_YELLOW);
    }

    screen_print_color("  ╠═════╧══════════════════╧══════════╧════════╣\n", COLOR_CYAN);
    screen_print_color("  ║  System ticks: ", COLOR_CYAN);
    my_int_to_str(global_ticks, buf);
    screen_print_str(buf);
    /* Pad to fill row */
    {
        int pad = my_sub(30, my_strlen(buf));
        int p;
        for (p = 0; p < pad; p = my_add(p, 1)) {
            screen_print_str(" ");
        }
    }
    screen_print_color("║\n", COLOR_CYAN);
    screen_print_color("  ╚═══════════════════════════════════════════╝\n", COLOR_CYAN);
    screen_print_str("\n");
}


