/*
 * MineOS — Command Declarations
 * All shell commands and the dispatcher.
 */

#ifndef MINEOS_COMMANDS_H
#define MINEOS_COMMANDS_H

/* ── Command Dispatch ─────────────────────────────────────────────── */
/*
 * Dispatches a tokenized command to the correct handler.
 * Returns 0 on success, -1 to signal shell exit.
 */
int command_dispatch(char *tokens[], int count, int *current_dir);

/* ── Individual Commands ──────────────────────────────────────────── */
int cmd_ls(char *tokens[], int count, int current_dir);
int cmd_mkdir(char *tokens[], int count, int current_dir);
int cmd_touch(char *tokens[], int count, int current_dir);
int cmd_echo(char *tokens[], int count);
int cmd_clear(void);
int cmd_memstat(void);
int cmd_ps(void);
int cmd_help(void);
int cmd_exit(void);

/* Phase 2 commands (declared for forward compatibility) */
int cmd_write(char *tokens[], int count, int current_dir);
int cmd_cat(char *tokens[], int count, int current_dir);
int cmd_rm(char *tokens[], int count, int current_dir);
int cmd_cd(char *tokens[], int count, int *current_dir);
int cmd_pwd(int current_dir);
int cmd_save(void);
int cmd_load(void);

#endif /* MINEOS_COMMANDS_H */
