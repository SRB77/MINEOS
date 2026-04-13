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

/* ── Initialize Scheduler ─────────────────────────────────────────── */
/*
 * Phase 1: Just reset the tick counter.
 * Phase 2: Will register default background tasks (e.g., uptime_counter).
 */
void scheduler_init(void) {
    global_ticks = 0;
}

/* ── Spawn Process (Phase 2 Stub) ─────────────────────────────────── */
int scheduler_spawn(char *name) {
    (void)name;
    screen_print_color("[scheduler] spawn: not yet implemented (Phase 2)\n", COLOR_YELLOW);
    return -1;
}

/* ── Kill Process (Phase 2 Stub) ──────────────────────────────────── */
void scheduler_kill(int pid) {
    (void)pid;
    screen_print_color("[scheduler] kill: not yet implemented (Phase 2)\n", COLOR_YELLOW);
}

/* ── Tick ──────────────────────────────────────────────────────────── */
/*
 * Called after every shell command.
 * Phase 1: Just increments the global counter.
 * Phase 2: Will iterate process table, increment ticks for RUNNING,
 *          and trigger uptime messages.
 */
void scheduler_tick(void) {
    global_ticks = my_add(global_ticks, 1);
}

/* ── List Processes (Phase 2 Stub) ────────────────────────────────── */
void scheduler_list(void) {
    char buf[16];

    screen_print_str("┌─────────────────────────────────┐\n");
    screen_print_str("│        PROCESS TABLE            │\n");
    screen_print_str("├─────────────────────────────────┤\n");
    screen_print_str("│  (no active processes — Phase 1)│\n");
    screen_print_str("├─────────────────────────────────┤\n");
    screen_print_str("│  System ticks: ");
    my_int_to_str(global_ticks, buf);
    screen_print_str(buf);
    screen_print_str("                │\n");
    screen_print_str("└─────────────────────────────────┘\n");
}
