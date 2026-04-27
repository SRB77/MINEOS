/*
 * MineOS — Shell Implementation
 * Interactive command loop — the heart of MineOS.
 *
 * Flow: prompt → read → tokenize → dispatch → tick → repeat
 *
 * Uses ALL five libraries:
 *   keyboard.h — read_line() for input
 *   string.h   — my_tokenize() for parsing, my_str_trim() for cleanup
 *   screen.h   — screen_print_color() for the prompt
 *   math.h     — indirectly through all other modules
 *   memory.h   — CURRENT_USER for prompt display
 */

#include "shell.h"
#include "../libs/keyboard.h"
#include "../libs/math.h"
#include "../libs/memory.h"
#include "../libs/screen.h"
#include "../libs/string.h"
#include "commands.h"
#include "fs.h"
#include "scheduler.h"

/* ── Shell Main Loop ──────────────────────────────────────────────── */
void shell_run(void) {
  int current_dir = 3; /* start at /home/chandan (inode 3) */
  char input[256];
  char *tokens[16];
  int token_count;
  int result;
  char path[256];

  while (1) {
    /* 1. Build current path */
    fs_get_path(current_dir, path);

    /* 2. Print colored prompt: [chandan@mine /path]$ */
    screen_print_color("[", COLOR_BOLD);
    screen_print_color(CURRENT_USER, COLOR_GREEN);
    screen_print_color("@mine ", COLOR_GREEN);
    screen_print_color(path, COLOR_CYAN);
    screen_print_color("]$ ", COLOR_BOLD);

    /* 3. Read input line */
    read_line(input, 256);

    /* 4. Trim whitespace */
    my_str_trim(input);

    /* 5. Skip empty input */
    if (my_strlen(input) == 0)
      continue;

    /* 6. Tokenize */
    token_count = my_tokenize(input, tokens, ' ');
    if (token_count == 0)
      continue;

    /* 7. Dispatch command */
    result = command_dispatch(tokens, token_count, &current_dir);

    /* 8. Tick scheduler */
    scheduler_tick();

    /* 9. Check for exit signal */
    if (result == -1)
      break;
  }
}
