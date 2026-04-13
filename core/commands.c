/*
 * MineOS — Command Implementations
 * All shell commands + the dispatcher.
 *
 * Uses ALL five libraries:
 *   math.h     — bounds checks, arithmetic in memstat
 *   string.h   — strcmp for dispatch, strcat for echo, int_to_str for display
 *   memory.h   — memory_used(), memory_free() for memstat
 *   screen.h   — all output via screen_print_*
 *   keyboard.h — keyboard_restore() on exit
 */

#include "commands.h"
#include "fs.h"
#include "scheduler.h"
#include "../libs/math.h"
#include "../libs/string.h"
#include "../libs/memory.h"
#include "../libs/screen.h"
#include "../libs/keyboard.h"

/* ── Command Dispatcher ───────────────────────────────────────────── */
/*
 * Matches tokens[0] against known command names using my_strcmp.
 * Returns 0 on success, -1 to signal shell exit.
 */
int command_dispatch(char *tokens[], int count, int *current_dir) {
    if (count == 0) return 0;

    if (my_strcmp(tokens[0], "echo") == 0)    return cmd_echo(tokens, count);
    if (my_strcmp(tokens[0], "ls") == 0)      return cmd_ls(tokens, count, *current_dir);
    if (my_strcmp(tokens[0], "mkdir") == 0)   return cmd_mkdir(tokens, count, *current_dir);
    if (my_strcmp(tokens[0], "touch") == 0)   return cmd_touch(tokens, count, *current_dir);
    if (my_strcmp(tokens[0], "memstat") == 0) return cmd_memstat();
    if (my_strcmp(tokens[0], "ps") == 0)      return cmd_ps();
    if (my_strcmp(tokens[0], "clear") == 0)   return cmd_clear();
    if (my_strcmp(tokens[0], "help") == 0)    return cmd_help();
    if (my_strcmp(tokens[0], "exit") == 0)    return cmd_exit();

    /* Phase 2 commands — stubs */
    if (my_strcmp(tokens[0], "cat") == 0)     return cmd_cat(tokens, count, *current_dir);
    if (my_strcmp(tokens[0], "write") == 0)   return cmd_write(tokens, count, *current_dir);
    if (my_strcmp(tokens[0], "rm") == 0)      return cmd_rm(tokens, count, *current_dir);
    if (my_strcmp(tokens[0], "cd") == 0)      return cmd_cd(tokens, count, current_dir);
    if (my_strcmp(tokens[0], "pwd") == 0)     return cmd_pwd(*current_dir);
    if (my_strcmp(tokens[0], "save") == 0)    return cmd_save();
    if (my_strcmp(tokens[0], "load") == 0)    return cmd_load();

    /* Unknown command */
    screen_print_color("  Unknown command: '", COLOR_RED);
    screen_print_color(tokens[0], COLOR_RED);
    screen_print_color("'. Type 'help' for available commands.\n", COLOR_RED);
    return 0;
}

/* ══════════════════════════════════════════════════════════════════ */
/*                     PHASE 1 COMMANDS                             */
/* ══════════════════════════════════════════════════════════════════ */

/* ── echo ─────────────────────────────────────────────────────────── */
/*
 * Prints all arguments joined by spaces.
 * Example: echo hello world → "hello world"
 */
int cmd_echo(char *tokens[], int count) {
    int i;

    if (count < 2) {
        screen_print_str("\n");
        return 0;
    }

    screen_print_str("  ");
    for (i = 1; i < count; i = my_add(i, 1)) {
        screen_print_str(tokens[i]);
        if (i < my_sub(count, 1)) {
            screen_print_str(" ");
        }
    }
    screen_print_str("\n");
    return 0;
}

/* ── ls ───────────────────────────────────────────────────────────── */
int cmd_ls(char *tokens[], int count, int current_dir) {
    (void)tokens;
    (void)count;
    fs_list(current_dir);
    return 0;
}

/* ── mkdir ────────────────────────────────────────────────────────── */
/*
 * Creates a new directory in the current directory.
 * Uses: string.c (my_strcmp), math.c (in_bounds), memory.c (inode in RAM),
 *       screen.c (output), fs.c (fs_create).
 */
int cmd_mkdir(char *tokens[], int count, int current_dir) {
    int result;

    if (count < 2) {
        screen_print_color("  Usage: mkdir <name>\n", COLOR_YELLOW);
        return 0;
    }

    result = fs_create(current_dir, tokens[1], INODE_DIR);
    if (result != -1) {
        screen_print_color("  → Directory created: ", COLOR_GREEN);
        screen_print_color(tokens[1], COLOR_GREEN);
        screen_print_str("\n");
    }
    return 0;
}

/* ── touch ────────────────────────────────────────────────────────── */
/*
 * Creates a new empty file in the current directory.
 */
int cmd_touch(char *tokens[], int count, int current_dir) {
    int result;

    if (count < 2) {
        screen_print_color("  Usage: touch <name>\n", COLOR_YELLOW);
        return 0;
    }

    result = fs_create(current_dir, tokens[1], INODE_FILE);
    if (result != -1) {
        screen_print_color("  → File created: ", COLOR_GREEN);
        screen_print_color(tokens[1], COLOR_GREEN);
        screen_print_str("\n");
    }
    return 0;
}

/* ── memstat ──────────────────────────────────────────────────────── */
/*
 * Displays virtual RAM usage statistics.
 * Demonstrates alloc/dealloc working correctly.
 * Uses: math.c (my_add, my_sub), string.c (my_int_to_str), memory.c, screen.c.
 */
int cmd_memstat(void) {
    int used = memory_used();
    int free_bytes = memory_free();
    int total = my_add(used, free_bytes);
    char buf[16];

    screen_print_str("\n");
    screen_print_color("  ╔══════════════════════════════════╗\n", COLOR_CYAN);
    screen_print_color("  ║       VIRTUAL RAM STATUS         ║\n", COLOR_CYAN);
    screen_print_color("  ╠══════════════════════════════════╣\n", COLOR_CYAN);

    screen_print_color("  ║  Total RAM     : ", COLOR_CYAN);
    my_int_to_str(RAM_SIZE, buf);
    screen_print_str(buf);
    screen_print_color(" bytes    ║\n", COLOR_CYAN);

    screen_print_color("  ║  Data Region   : ", COLOR_CYAN);
    my_int_to_str(total, buf);
    screen_print_str(buf);
    screen_print_color(" bytes    ║\n", COLOR_CYAN);

    screen_print_color("  ║  Used          : ", COLOR_CYAN);
    my_int_to_str(used, buf);
    screen_print_color(buf, COLOR_RED);
    screen_print_color(" bytes       ║\n", COLOR_CYAN);

    screen_print_color("  ║  Free          : ", COLOR_CYAN);
    my_int_to_str(free_bytes, buf);
    screen_print_color(buf, COLOR_GREEN);
    screen_print_color(" bytes ║\n", COLOR_CYAN);

    screen_print_color("  ╚══════════════════════════════════╝\n", COLOR_CYAN);
    screen_print_str("\n");

    return 0;
}

/* ── ps ───────────────────────────────────────────────────────────── */
int cmd_ps(void) {
    scheduler_list();
    return 0;
}

/* ── clear ────────────────────────────────────────────────────────── */
int cmd_clear(void) {
    screen_clear();
    return 0;
}

/* ── help ─────────────────────────────────────────────────────────── */
int cmd_help(void) {
    screen_print_str("\n");
    screen_print_color("  ╔══════════════════════════════════════════════════════════╗\n", COLOR_CYAN);
    screen_print_color("  ║               MineOS — Command Reference               ║\n", COLOR_CYAN);
    screen_print_color("  ╠══════════════════════════════════════════════════════════╣\n", COLOR_CYAN);
    screen_print_color("  ║  ls              ", COLOR_GREEN);
    screen_print_str("List files and directories             ");
    screen_print_color("║\n", COLOR_CYAN);
    screen_print_color("  ║  mkdir <name>    ", COLOR_GREEN);
    screen_print_str("Create a new directory                 ");
    screen_print_color("║\n", COLOR_CYAN);
    screen_print_color("  ║  touch <name>    ", COLOR_GREEN);
    screen_print_str("Create a new empty file                ");
    screen_print_color("║\n", COLOR_CYAN);
    screen_print_color("  ║  echo <text>     ", COLOR_GREEN);
    screen_print_str("Print text to screen                   ");
    screen_print_color("║\n", COLOR_CYAN);
    screen_print_color("  ║  memstat         ", COLOR_GREEN);
    screen_print_str("Show virtual RAM usage                 ");
    screen_print_color("║\n", COLOR_CYAN);
    screen_print_color("  ║  ps              ", COLOR_GREEN);
    screen_print_str("Show process table                     ");
    screen_print_color("║\n", COLOR_CYAN);
    screen_print_color("  ║  clear           ", COLOR_GREEN);
    screen_print_str("Clear the terminal                     ");
    screen_print_color("║\n", COLOR_CYAN);
    screen_print_color("  ║  help            ", COLOR_GREEN);
    screen_print_str("Show this help message                 ");
    screen_print_color("║\n", COLOR_CYAN);
    screen_print_color("  ║  exit            ", COLOR_GREEN);
    screen_print_str("Shutdown MineOS                        ");
    screen_print_color("║\n", COLOR_CYAN);
    screen_print_color("  ╠══════════════════════════════════════════════════════════╣\n", COLOR_CYAN);
    screen_print_color("  ║  ", COLOR_CYAN);
    screen_print_color("Coming in Phase 2: ", COLOR_YELLOW);
    screen_print_str("cat, write, rm, cd, pwd, save, load");
    screen_print_color("║\n", COLOR_CYAN);
    screen_print_color("  ╚══════════════════════════════════════════════════════════╝\n", COLOR_CYAN);
    screen_print_str("\n");

    return 0;
}

/* ── exit ─────────────────────────────────────────────────────────── */
int cmd_exit(void) {
    screen_print_str("\n");
    screen_print_color("  Shutting down MineOS...\n", COLOR_YELLOW);
    screen_print_color("  Goodbye!\n\n", COLOR_GREEN);
    keyboard_restore();
    return -1;  /* signal shell loop to exit */
}

/* ══════════════════════════════════════════════════════════════════ */
/*                     PHASE 2 STUBS                                */
/* ══════════════════════════════════════════════════════════════════ */

int cmd_write(char *tokens[], int count, int current_dir) {
    (void)tokens; (void)count; (void)current_dir;
    screen_print_color("  write: coming in Phase 2\n", COLOR_YELLOW);
    return 0;
}

int cmd_cat(char *tokens[], int count, int current_dir) {
    (void)tokens; (void)count; (void)current_dir;
    screen_print_color("  cat: coming in Phase 2\n", COLOR_YELLOW);
    return 0;
}

int cmd_rm(char *tokens[], int count, int current_dir) {
    (void)tokens; (void)count; (void)current_dir;
    screen_print_color("  rm: coming in Phase 2\n", COLOR_YELLOW);
    return 0;
}

int cmd_cd(char *tokens[], int count, int *current_dir) {
    (void)tokens; (void)count; (void)current_dir;
    screen_print_color("  cd: coming in Phase 2\n", COLOR_YELLOW);
    return 0;
}

int cmd_pwd(int current_dir) {
    (void)current_dir;
    screen_print_color("  pwd: coming in Phase 2\n", COLOR_YELLOW);
    return 0;
}

int cmd_save(void) {
    screen_print_color("  save: coming in Phase 2\n", COLOR_YELLOW);
    return 0;
}

int cmd_load(void) {
    screen_print_color("  load: coming in Phase 2\n", COLOR_YELLOW);
    return 0;
}
