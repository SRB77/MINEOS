/*
 * MineOS — Virtual File System
 * Inode-based file system stored in the fixed Inode Table region of VIRTUAL_RAM.
 * Each file and directory is represented by an Inode struct.
 */

#ifndef MINEOS_FS_H
#define MINEOS_FS_H

#include "../libs/memory.h"

/* ── Inode Types ──────────────────────────────────────────────────── */
#define INODE_FILE  0
#define INODE_DIR   1

/* ── Inode Structure ──────────────────────────────────────────────── */
/*
 * Each inode occupies exactly INODE_SIZE (128) bytes in the Inode Table.
 * Fields are laid out to fit within this size.
 */
typedef struct {
    int  id;                        /* unique inode number (0 = root) */
    char name[MAX_NAME_LEN];       /* file or directory name (32 bytes) */
    int  type;                      /* INODE_FILE = 0, INODE_DIR = 1 */
    int  parent_id;                 /* inode id of parent (-1 for root) */
    int  children[MAX_CHILDREN];    /* inode ids of children (dirs only, 16 slots) */
    int  child_count;               /* number of active children */
    int  data_offset;               /* offset in data region (-1 if empty) */
    int  data_size;                 /* size of content in bytes */
    int  is_active;                 /* 1 = in use, 0 = deleted/free */
} Inode;

/* ── Public API ───────────────────────────────────────────────────── */
void   fs_init(void);                                   /* create default directory tree */
Inode* fs_get_inode(int id);                            /* get inode by id */
Inode* fs_find_child(int parent_id, char *name);        /* find child by name */
int    fs_create(int parent_id, char *name, int type);  /* create file or directory */
int    fs_delete(int inode_id);                         /* delete inode (Phase 2) */
int    fs_write(int inode_id, char *data);              /* write data to file (Phase 2) */
int    fs_read(int inode_id, char *buf);                /* read data from file (Phase 2) */
void   fs_list(int dir_id);                             /* ls — list directory contents */
void   fs_get_path(int inode_id, char *buf);            /* pwd — build full path string */

#endif /* MINEOS_FS_H */
