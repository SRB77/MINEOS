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
#include <stdio.h>   /* allowed: fopen/fwrite/fread/fclose for save/load */

/* ── Command Dispatcher ───────────────────────────────────────────── */
/*
 * Matches tokens[0] against known command names using my_strcmp.
 * Returns 0 on success, -1 to signal shell exit.
 */
int command_dispatch(char *tokens[], int count, int *current_dir) {
    if (count == 0) return 0;

    /* ── Input Validation (Phase 2 Security) ─── */
    if (tokens[0] == (char *)0 || my_strlen(tokens[0]) == 0) return 0;
    if (my_strlen(tokens[0]) > 32) {
        screen_print_color("  Error: command name too long\n", COLOR_RED);
        return 0;
    }

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
    if (my_strcmp(tokens[0], "halt") == 0)    return cmd_halt();

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
    screen_print_color("  ║  write <f> <txt> ", COLOR_GREEN);
    screen_print_str("Write content to a file                ");
    screen_print_color("║\n", COLOR_CYAN);
    screen_print_color("  ║  cat <file>      ", COLOR_GREEN);
    screen_print_str("Display file contents                  ");
    screen_print_color("║\n", COLOR_CYAN);
    screen_print_color("  ║  rm <name>       ", COLOR_GREEN);
    screen_print_str("Delete a file or empty directory        ");
    screen_print_color("║\n", COLOR_CYAN);
    screen_print_color("  ║  cd <dir>        ", COLOR_GREEN);
    screen_print_str("Change directory (cd .. to go up)      ");
    screen_print_color("║\n", COLOR_CYAN);
    screen_print_color("  ║  pwd             ", COLOR_GREEN);
    screen_print_str("Print current working directory        ");
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
    screen_print_color("  ║  save            ", COLOR_GREEN);
    screen_print_str("Persist filesystem to disk             ");
    screen_print_color("║\n", COLOR_CYAN);
    screen_print_color("  ║  load            ", COLOR_GREEN);
    screen_print_str("Restore filesystem from disk           ");
    screen_print_color("║\n", COLOR_CYAN);
    screen_print_color("  ║  clear           ", COLOR_GREEN);
    screen_print_str("Clear the terminal                     ");
    screen_print_color("║\n", COLOR_CYAN);
    screen_print_color("  ║  help            ", COLOR_GREEN);
    screen_print_str("Show this help message                 ");
    screen_print_color("║\n", COLOR_CYAN);
    screen_print_color("  ║  halt            ", COLOR_GREEN);
    screen_print_str("Emergency shutdown (wipes RAM)         ");
    screen_print_color("║\n", COLOR_CYAN);
    screen_print_color("  ║  exit            ", COLOR_GREEN);
    screen_print_str("Shutdown MineOS                        ");
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
/*                     PHASE 2 COMMANDS                             */
/* ══════════════════════════════════════════════════════════════════ */

/* ── write ────────────────────────────────────────────────────────── */
/*
 * Write content to a file: write <filename> <content...>
 * Joins all tokens after filename with spaces.
 */
int cmd_write(char *tokens[], int count, int current_dir) {
    Inode *file;
    char content[1024];
    int i;
    char buf[16];

    if (count < 3) {
        screen_print_color("  Usage: write <filename> <content>\n", COLOR_YELLOW);
        return 0;
    }

    file = fs_find_child(current_dir, tokens[1]);
    if (file == (Inode *)0) {
        screen_print_color("  Error: file '", COLOR_RED);
        screen_print_color(tokens[1], COLOR_RED);
        screen_print_color("' not found\n", COLOR_RED);
        return 0;
    }

    if (file->type != INODE_FILE) {
        screen_print_color("  Error: '", COLOR_RED);
        screen_print_color(tokens[1], COLOR_RED);
        screen_print_color("' is a directory, not a file\n", COLOR_RED);
        return 0;
    }

    /* Join tokens[2..count-1] with spaces */
    content[0] = '\0';
    for (i = 2; i < count; i = my_add(i, 1)) {
        my_strcat(content, tokens[i]);
        if (i < my_sub(count, 1)) {
            my_strcat(content, " ");
        }
    }

    if (fs_write(file->id, content) == 0) {
        screen_print_color("  → Wrote ", COLOR_GREEN);
        my_int_to_str(my_strlen(content), buf);
        screen_print_color(buf, COLOR_GREEN);
        screen_print_color(" bytes to ", COLOR_GREEN);
        screen_print_color(tokens[1], COLOR_GREEN);
        screen_print_str("\n");
    }
    return 0;
}

/* ── cat ──────────────────────────────────────────────────────────── */
/*
 * Display file contents: cat <filename>
 */
int cmd_cat(char *tokens[], int count, int current_dir) {
    Inode *file;
    char buf[4096];
    int result;

    if (count < 2) {
        screen_print_color("  Usage: cat <filename>\n", COLOR_YELLOW);
        return 0;
    }

    file = fs_find_child(current_dir, tokens[1]);
    if (file == (Inode *)0) {
        screen_print_color("  Error: file '", COLOR_RED);
        screen_print_color(tokens[1], COLOR_RED);
        screen_print_color("' not found\n", COLOR_RED);
        return 0;
    }

    if (file->type != INODE_FILE) {
        screen_print_color("  Error: '", COLOR_RED);
        screen_print_color(tokens[1], COLOR_RED);
        screen_print_color("' is a directory, not a file\n", COLOR_RED);
        return 0;
    }

    result = fs_read(file->id, buf);
    if (result == 0) {
        screen_print_color("  (empty file)\n", COLOR_YELLOW);
    } else if (result > 0) {
        screen_print_str("  ");
        screen_print_str(buf);
        screen_print_str("\n");
    }

    return 0;
}

/* ── rm ───────────────────────────────────────────────────────────── */
/*
 * Delete a file or empty directory: rm <name>
 */
int cmd_rm(char *tokens[], int count, int current_dir) {
    Inode *target;

    if (count < 2) {
        screen_print_color("  Usage: rm <name>\n", COLOR_YELLOW);
        return 0;
    }

    target = fs_find_child(current_dir, tokens[1]);
    if (target == (Inode *)0) {
        screen_print_color("  Error: '", COLOR_RED);
        screen_print_color(tokens[1], COLOR_RED);
        screen_print_color("' not found\n", COLOR_RED);
        return 0;
    }

    if (fs_delete(target->id) == 0) {
        screen_print_color("  → Removed: ", COLOR_GREEN);
        screen_print_color(tokens[1], COLOR_GREEN);
        screen_print_str("\n");
    }
    return 0;
}

/* ── cd ───────────────────────────────────────────────────────────── */
/*
 * cd          → go to home (/home/chandan, inode 3)
 * cd ..       → go to parent
 * cd /        → go to root
 * cd <dir>    → go to named child directory
 */
int cmd_cd(char *tokens[], int count, int *current_dir) {
    Inode *current;
    Inode *target;

    /* No argument: go home */
    if (count < 2) {
        *current_dir = 3;
        return 0;
    }

    /* cd .. — go to parent */
    if (my_strcmp(tokens[1], "..") == 0) {
        current = fs_get_inode(*current_dir);
        if (current != (Inode *)0 && current->parent_id != -1) {
            *current_dir = current->parent_id;
        } else {
            *current_dir = 0;  /* already at root */
        }
        return 0;
    }

    /* cd / — go to root */
    if (my_strcmp(tokens[1], "/") == 0) {
        *current_dir = 0;
        return 0;
    }

    /* cd <name> — find child directory */
    target = fs_find_child(*current_dir, tokens[1]);
    if (target == (Inode *)0) {
        screen_print_color("  Error: '", COLOR_RED);
        screen_print_color(tokens[1], COLOR_RED);
        screen_print_color("' not found\n", COLOR_RED);
        return 0;
    }

    if (target->type != INODE_DIR) {
        screen_print_color("  Error: '", COLOR_RED);
        screen_print_color(tokens[1], COLOR_RED);
        screen_print_color("' is not a directory\n", COLOR_RED);
        return 0;
    }

    *current_dir = target->id;
    return 0;
}

/* ── pwd ──────────────────────────────────────────────────────────── */
int cmd_pwd(int current_dir) {
    char path[256];

    fs_get_path(current_dir, path);
    screen_print_str("  ");
    screen_print_str(path);
    screen_print_str("\n");
    return 0;
}

/* ── save ─────────────────────────────────────────────────────────── */
/*
 * Persists entire VIRTUAL_RAM to mineos.img on disk.
 * Uses fopen/fwrite/fclose from <stdio.h> (allowed per project rules).
 */
int cmd_save(void) {
    FILE *fp = fopen("mineos.img", "wb");

    if (fp == (FILE *)0) {
        screen_print_color("  Error: could not create save file\n", COLOR_RED);
        return 0;
    }

    fwrite(VIRTUAL_RAM, 1, RAM_SIZE, fp);
    fclose(fp);

    screen_print_color("  → Filesystem saved to mineos.img (", COLOR_GREEN);
    {
        char buf[16];
        my_int_to_str(RAM_SIZE, buf);
        screen_print_color(buf, COLOR_GREEN);
    }
    screen_print_color(" bytes)\n", COLOR_GREEN);
    return 0;
}

/* ── load ─────────────────────────────────────────────────────────── */
/*
 * Restores VIRTUAL_RAM from mineos.img on disk.
 */
int cmd_load(void) {
    FILE *fp = fopen("mineos.img", "rb");

    if (fp == (FILE *)0) {
        screen_print_color("  Error: no save file found (mineos.img)\n", COLOR_RED);
        return 0;
    }

    fread(VIRTUAL_RAM, 1, RAM_SIZE, fp);
    fclose(fp);

    screen_print_color("  → Filesystem restored from mineos.img\n", COLOR_GREEN);
    return 0;
}

/* ── halt ─────────────────────────────────────────────────────────── */
/*
 * Emergency shutdown: wipes RAM, restores terminal, exits shell.
 */
int cmd_halt(void) {
    screen_print_str("\n");
    screen_print_color("  ⚠  EMERGENCY SHUTDOWN INITIATED\n", COLOR_RED);
    screen_print_color("  Wiping virtual RAM...\n", COLOR_RED);
    memory_init();
    screen_print_color("  RAM cleared. System halted.\n", COLOR_RED);
    screen_print_str("\n");
    keyboard_restore();
    return -1;  /* signal shell loop to exit */
}
