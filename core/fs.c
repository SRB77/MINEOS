/*
 * MineOS — Virtual File System Implementation
 * Inode table lives in the fixed region of VIRTUAL_RAM.
 *
 * Uses: memory.h (VIRTUAL_RAM, offsets, alloc/dealloc)
 *       string.h (my_strcpy, my_strcmp, my_strlen)
 *       math.h   (my_add, my_mul, in_bounds)
 *       screen.h (screen_print_str, screen_print_color for fs_list)
 */

#include "fs.h"
#include "../libs/math.h"
#include "../libs/string.h"
#include "../libs/screen.h"
#include <stdio.h>   /* allowed: for printf */

/* ── Track next available inode id ────────────────────────────────── */
static int next_inode_id = 0;

/* ── Get Inode by ID ──────────────────────────────────────────────── */
/*
 * Returns a pointer to the inode at the given id in the Inode Table.
 * The Inode Table starts at INODE_TABLE_OFFSET in VIRTUAL_RAM.
 */
Inode* fs_get_inode(int id) {
    if (!in_bounds(id, 0, my_sub(MAX_INODES, 1))) {
        return (Inode *)0;  /* NULL — invalid id */
    }
    return (Inode *)(VIRTUAL_RAM + my_add(INODE_TABLE_OFFSET, my_mul(id, INODE_SIZE)));
}

/* ── Helper: Create a single inode ────────────────────────────────── */
static int create_inode(char *name, int type, int parent_id) {
    Inode *inode;
    int id;
    int i;

    if (next_inode_id >= MAX_INODES) {
        screen_print_color("[fs] Error: inode table full\n", COLOR_RED);
        return -1;
    }

    id = next_inode_id;
    next_inode_id = my_add(next_inode_id, 1);

    inode = fs_get_inode(id);
    inode->id = id;
    my_strcpy(inode->name, name);
    inode->type = type;
    inode->parent_id = parent_id;
    inode->child_count = 0;
    inode->data_offset = -1;
    inode->data_size = 0;
    inode->is_active = 1;

    /* Initialize children array to -1 */
    for (i = 0; i < MAX_CHILDREN; i = my_add(i, 1)) {
        inode->children[i] = -1;
    }

    return id;
}

/* ── Initialize Default Directory Tree ────────────────────────────── */
/*
 * Creates:
 *   /              (inode 0, parent -1)
 *   ├── bin/       (inode 1)
 *   ├── home/      (inode 2)
 *   │   └── chandan/ (inode 3)  ← shell starts here
 *   ├── tmp/       (inode 4)
 *   └── sys/       (inode 5)
 */
void fs_init(void) {
    Inode *root;
    int root_id, bin_id, home_id, user_id, tmp_id, sys_id;

    next_inode_id = 0;

    /* Create root directory */
    root_id = create_inode("/", INODE_DIR, -1);

    /* Create top-level directories */
    bin_id  = create_inode("bin",  INODE_DIR, root_id);
    home_id = create_inode("home", INODE_DIR, root_id);
    user_id = create_inode(CURRENT_USER, INODE_DIR, home_id);
    tmp_id  = create_inode("tmp",  INODE_DIR, root_id);
    sys_id  = create_inode("sys",  INODE_DIR, root_id);

    /* Wire up root's children */
    root = fs_get_inode(root_id);
    root->children[0] = bin_id;
    root->children[1] = home_id;
    root->children[2] = tmp_id;
    root->children[3] = sys_id;
    root->child_count = 4;

    /* Wire up home's children */
    {
        Inode *home = fs_get_inode(home_id);
        home->children[0] = user_id;
        home->child_count = 1;
    }

    /* Suppress unused variable warnings */
    (void)bin_id;
    (void)tmp_id;
    (void)sys_id;
    (void)user_id;
}

/* ── Find Child by Name ───────────────────────────────────────────── */
Inode* fs_find_child(int parent_id, char *name) {
    Inode *parent = fs_get_inode(parent_id);
    Inode *child;
    int i;

    if (parent == (Inode *)0 || parent->type != INODE_DIR) {
        return (Inode *)0;
    }

    for (i = 0; i < parent->child_count; i = my_add(i, 1)) {
        child = fs_get_inode(parent->children[i]);
        if (child != (Inode *)0 && child->is_active && my_strcmp(child->name, name) == 0) {
            return child;
        }
    }

    return (Inode *)0;  /* not found */
}

/* ── Create File or Directory ─────────────────────────────────────── */
/*
 * Creates a new inode as a child of the given parent.
 * Returns the new inode id, or -1 on error.
 */
int fs_create(int parent_id, char *name, int type) {
    Inode *parent = fs_get_inode(parent_id);
    Inode *existing;
    int new_id;

    if (parent == (Inode *)0 || parent->type != INODE_DIR) {
        screen_print_color("[fs] Error: parent is not a directory\n", COLOR_RED);
        return -1;
    }

    if (parent->child_count >= MAX_CHILDREN) {
        screen_print_color("[fs] Error: directory is full\n", COLOR_RED);
        return -1;
    }

    /* Check if name already exists */
    existing = fs_find_child(parent_id, name);
    if (existing != (Inode *)0) {
        screen_print_color("[fs] Error: '", COLOR_RED);
        screen_print_color(name, COLOR_RED);
        screen_print_color("' already exists\n", COLOR_RED);
        return -1;
    }

    /* Create the inode */
    new_id = create_inode(name, type, parent_id);
    if (new_id == -1) return -1;

    /* Add to parent's children */
    parent->children[parent->child_count] = new_id;
    parent->child_count = my_add(parent->child_count, 1);

    return new_id;
}

/* ── Delete Inode (Phase 2 Stub) ──────────────────────────────────── */
int fs_delete(int inode_id) {
    (void)inode_id;
    screen_print_color("[fs] rm: not yet implemented (Phase 2)\n", COLOR_YELLOW);
    return -1;
}

/* ── Write Data to File (Phase 2 Stub) ────────────────────────────── */
int fs_write(int inode_id, char *data) {
    (void)inode_id;
    (void)data;
    screen_print_color("[fs] write: not yet implemented (Phase 2)\n", COLOR_YELLOW);
    return -1;
}

/* ── Read Data from File (Phase 2 Stub) ───────────────────────────── */
int fs_read(int inode_id, char *buf) {
    (void)inode_id;
    (void)buf;
    screen_print_color("[fs] read: not yet implemented (Phase 2)\n", COLOR_YELLOW);
    return -1;
}

/* ── List Directory Contents ──────────────────────────────────────── */
/*
 * ls — prints children of the given directory inode.
 * Directories shown in blue with trailing /, files in white.
 */
void fs_list(int dir_id) {
    Inode *dir = fs_get_inode(dir_id);
    Inode *child;
    int i;

    if (dir == (Inode *)0 || dir->type != INODE_DIR) {
        screen_print_color("[fs] Error: not a directory\n", COLOR_RED);
        return;
    }

    if (dir->child_count == 0) {
        screen_print_color("  (empty)\n", COLOR_YELLOW);
        return;
    }

    for (i = 0; i < dir->child_count; i = my_add(i, 1)) {
        child = fs_get_inode(dir->children[i]);
        if (child != (Inode *)0 && child->is_active) {
            if (child->type == INODE_DIR) {
                /* Directory: blue with trailing / */
                screen_print_str("  ");
                screen_print_color(child->name, COLOR_BLUE);
                screen_print_color("/", COLOR_BLUE);
                screen_print_str("\n");
            } else {
                /* File: cyan */
                screen_print_str("  ");
                screen_print_color(child->name, COLOR_CYAN);
                screen_print_str("\n");
            }
        }
    }
}

/* ── Build Full Path String ───────────────────────────────────────── */
/*
 * pwd — traverses parent pointers to build the full path.
 * Uses a stack-based approach (builds path in reverse, then reverses).
 */
void fs_get_path(int inode_id, char *buf) {
    /* Stack of inode ids to traverse */
    int stack[MAX_INODES];
    int depth = 0;
    int current = inode_id;
    Inode *node;
    int i;

    /* Walk up to root, collecting inode ids */
    while (current != -1 && in_bounds(current, 0, my_sub(MAX_INODES, 1))) {
        stack[depth] = current;
        depth = my_add(depth, 1);
        node = fs_get_inode(current);
        if (node == (Inode *)0) break;
        current = node->parent_id;
    }

    /* Build path from root down */
    buf[0] = '\0';

    if (depth == 0) {
        my_strcpy(buf, "/");
        return;
    }

    /* Start from the top of the stack (root) */
    for (i = my_sub(depth, 1); i >= 0; i = my_sub(i, 1)) {
        node = fs_get_inode(stack[i]);
        if (node == (Inode *)0) continue;

        if (stack[i] == 0) {
            /* Root node — just add / */
            my_strcat(buf, "/");
        } else {
            my_strcat(buf, node->name);
            if (i > 0) {
                my_strcat(buf, "/");
            }
        }
    }

    /* If path is empty (shouldn't happen), default to / */
    if (my_strlen(buf) == 0) {
        my_strcpy(buf, "/");
    }
}
