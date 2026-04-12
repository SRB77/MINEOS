/*
 * MineOS — Custom Memory Library
 * Replaces malloc()/free() — custom first-fit allocator on a static 10 MB array.
 * The only standard library usage is the static array declaration itself.
 *
 * Memory Layout:
 *   [0..63]            → RAM Header (metadata)
 *   [64..16447]        → Inode Table (128 slots × 128 bytes)
 *   [16448..17471]     → Process Table (16 slots × 64 bytes)
 *   [17472..RAM_SIZE]  → Data Block Region (managed by alloc/dealloc)
 */

#ifndef MINEOS_MEMORY_H
#define MINEOS_MEMORY_H

/* ── RAM Size ─────────────────────────────────────────────────────── */
#define RAM_SIZE            (10 * 1024 * 1024)   /* 10 MB */

/* ── RAM Header ───────────────────────────────────────────────────── */
#define RAM_HEADER_SIZE     64

/* ── Inode Table Layout ───────────────────────────────────────────── */
#define MAX_INODES          128
#define MAX_NAME_LEN        32
#define MAX_CHILDREN        16
#define INODE_SIZE          128
#define INODE_TABLE_OFFSET  RAM_HEADER_SIZE                         /* 64       */
#define INODE_TABLE_SIZE    (MAX_INODES * INODE_SIZE)               /* 16,384   */

/* ── Process Table Layout ─────────────────────────────────────────── */
#define MAX_PROCESSES       16
#define PROCESS_ENTRY_SIZE  64
#define PROCESS_TABLE_OFFSET (INODE_TABLE_OFFSET + INODE_TABLE_SIZE) /* 16,448  */
#define PROCESS_TABLE_SIZE  (MAX_PROCESSES * PROCESS_ENTRY_SIZE)    /* 1,024    */

/* ── Data Block Region ────────────────────────────────────────────── */
#define DATA_REGION_OFFSET  (PROCESS_TABLE_OFFSET + PROCESS_TABLE_SIZE)  /* 17,472 */
#define DATA_REGION_SIZE    (RAM_SIZE - DATA_REGION_OFFSET)

/* ── Block Header (stored at start of every allocated block) ──────── */
typedef struct {
    int size;          /* size of usable data (excluding this header) */
    int is_free;       /* 1 = free, 0 = in use */
    int next_offset;   /* offset in VIRTUAL_RAM of next BlockHeader (-1 if last) */
} BlockHeader;

#define BLOCK_HEADER_SIZE  ((int)sizeof(BlockHeader))

/* ── Configurable Username ────────────────────────────────────────── */
#define DEFAULT_USERNAME    "chandan"
#define MAX_USERNAME_LEN    32

/* ── Global Virtual RAM ───────────────────────────────────────────── */
extern char VIRTUAL_RAM[];
extern char CURRENT_USER[];

/* ── Public API ───────────────────────────────────────────────────── */
void  memory_init(void);         /* initialize RAM layout + first free block */
void* alloc(int size);           /* allocate 'size' bytes from data region */
void  dealloc(void *ptr);       /* free a previously allocated block */
int   memory_used(void);         /* total bytes currently in use */
int   memory_free(void);         /* total bytes currently free */
void  memory_dump(void);         /* debug: print all block headers */

#endif /* MINEOS_MEMORY_H */
