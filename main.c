/*
 * MineOS — Entry Point
 * Boot sequence: init RAM → init filesystem → init keyboard → boot banner → shell
 *
 * This is the thin entry point. All real work is done in the libraries and core modules.
 *
 * Build: make
 * Run:   ./mine
 */

#include "libs/math.h"
#include "libs/string.h"
#include "libs/memory.h"
#include "libs/screen.h"
#include "libs/keyboard.h"
#include "core/fs.h"
#include "core/shell.h"
#include "core/scheduler.h"

int main(void) {
    /* 1. Initialize virtual RAM (memory layout + first free block) */
    memory_init();

    /* 2. Initialize file system (default directory tree) */
    fs_init();

    /* 3. Initialize keyboard (switch to raw mode) */
    keyboard_init();

    /* 4. Initialize scheduler (minimal for Phase 1) */
    scheduler_init();

    /* 5. Clear screen and display boot banner */
    screen_boot_banner();

    /* 6. Enter the shell loop — blocks until user types "exit" */
    shell_run();

    /* 7. Cleanup — restore terminal to normal mode */
    keyboard_restore();

    return 0;
    
}
