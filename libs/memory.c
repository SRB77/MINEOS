/*
 * MineOS — Custom Memory Library Implementation
 * First-fit allocator on a 10 MB static array.
 *
 * Uses: math.h (for offset calculations, bounds checks)
 *       string.h (for int display in memory_dump)
 *
 * The Data Block Region starts at DATA_REGION_OFFSET.
 * alloc/dealloc only manage this region — they never touch the
 * Inode Table or Process Table (those are fixed regions).
 */

#include "memory.h"
#include "math.h"
#include "string.h"
#include <stdio.h>   /* allowed: for printf only */

/* ── Global Virtual RAM ───────────────────────────────────────────── */
char VIRTUAL_RAM[RAM_SIZE];
char CURRENT_USER[MAX_USERNAME_LEN] = DEFAULT_USERNAME;

/* ── Helper: get BlockHeader pointer at given RAM offset ──────────── */
static BlockHeader* get_header_at(int offset) {
    return (BlockHeader *)(VIRTUAL_RAM + offset);
}

/* ── Initialize Memory Layout ─────────────────────────────────────── */
/*
 * Sets up the RAM:
 *   1. Zero out the entire RAM
 *   2. Place a single free BlockHeader at DATA_REGION_OFFSET
 *      covering the entire data region
 */
void memory_init(void) {
    int i;
    BlockHeader *first;

    /* Zero out all of virtual RAM */
    for (i = 0; i < RAM_SIZE; i = my_add(i, 1)) {
        VIRTUAL_RAM[i] = 0;
    }

    /* Place the first (and only) free block header at the start of data region */
    first = get_header_at(DATA_REGION_OFFSET);
    first->size = my_sub(DATA_REGION_SIZE, BLOCK_HEADER_SIZE);
    first->is_free = 1;
    first->next_offset = -1;   /* last block */
}

/* ── Allocate Memory (First-Fit) ──────────────────────────────────── */
/*
 * Walks the block chain from DATA_REGION_OFFSET.
 * Finds the first free block with enough space.
 * Splits the block if there's enough room for another block.
 * Returns pointer to usable memory (after the header).
 * Returns NULL (0) if no space available.
 */
void* alloc(int size) {
    int offset = DATA_REGION_OFFSET;
    BlockHeader *block;
    int remaining;
    int min_split_size;
    BlockHeader *new_block;
    int new_offset;

    if (size <= 0) return (void *)0;

    while (offset != -1 && in_bounds(offset, DATA_REGION_OFFSET, my_sub(RAM_SIZE, 1))) {
        block = get_header_at(offset);

        if (block->is_free && block->size >= size) {
            /* Found a suitable free block */

            /* Check if we can split: need room for new header + at least 1 byte */
            min_split_size = my_add(BLOCK_HEADER_SIZE, 1);
            remaining = my_sub(block->size, size);

            if (remaining >= min_split_size) {
                /* Split: create a new free block after the allocated portion */
                new_offset = my_add(my_add(offset, BLOCK_HEADER_SIZE), size);
                new_block = get_header_at(new_offset);
                new_block->size = my_sub(remaining, BLOCK_HEADER_SIZE);
                new_block->is_free = 1;
                new_block->next_offset = block->next_offset;

                /* Update current block */
                block->size = size;
                block->next_offset = new_offset;
            }

            /* Mark block as used */
            block->is_free = 0;

            /* Return pointer to usable memory (just after the header) */
            return (void *)(VIRTUAL_RAM + my_add(offset, BLOCK_HEADER_SIZE));
        }

        offset = block->next_offset;
    }

    /* No suitable block found */
    printf("[memory] Error: out of memory (requested %d bytes)\n", size);
    return (void *)0;
}

/* ── Free Memory ──────────────────────────────────────────────────── */
/*
 * Marks the block as free.
 * Attempts to coalesce with the next block if it's also free.
 */
void dealloc(void *ptr) {
    int ptr_offset;
    int header_offset;
    BlockHeader *block;
    BlockHeader *next;

    if (ptr == (void *)0) return;

    /* Calculate the offset of this pointer in VIRTUAL_RAM */
    ptr_offset = (int)((char *)ptr - VIRTUAL_RAM);
    header_offset = my_sub(ptr_offset, BLOCK_HEADER_SIZE);

    /* Validate the offset is within the data region */
    if (!in_bounds(header_offset, DATA_REGION_OFFSET, my_sub(RAM_SIZE, BLOCK_HEADER_SIZE))) {
        printf("[memory] Error: invalid dealloc — pointer outside data region\n");
        return;
    }

    block = get_header_at(header_offset);

    /* Mark as free */
    block->is_free = 1;

    /* Coalesce with next block if it's also free */
    if (block->next_offset != -1) {
        next = get_header_at(block->next_offset);
        if (next->is_free) {
            /* Merge: absorb next block's header + data into this block */
            block->size = my_add(my_add(block->size, BLOCK_HEADER_SIZE), next->size);
            block->next_offset = next->next_offset;
        }
    }
}

/* ── Memory Statistics ────────────────────────────────────────────── */

int memory_used(void) {
    int offset = DATA_REGION_OFFSET;
    int used = 0;
    BlockHeader *block;

    while (offset != -1 && in_bounds(offset, DATA_REGION_OFFSET, my_sub(RAM_SIZE, 1))) {
        block = get_header_at(offset);
        if (!block->is_free) {
            used = my_add(used, block->size);
        }
        offset = block->next_offset;
    }

    return used;
}

int memory_free(void) {
    int offset = DATA_REGION_OFFSET;
    int free_bytes = 0;
    BlockHeader *block;

    while (offset != -1 && in_bounds(offset, DATA_REGION_OFFSET, my_sub(RAM_SIZE, 1))) {
        block = get_header_at(offset);
        if (block->is_free) {
            free_bytes = my_add(free_bytes, block->size);
        }
        offset = block->next_offset;
    }

    return free_bytes;
}

/* ── Debug Dump ───────────────────────────────────────────────────── */
/*
 * Prints all block headers in the data region chain.
 * Useful for debugging the allocator.
 */
void memory_dump(void) {
    int offset = DATA_REGION_OFFSET;
    int block_num = 0;
    BlockHeader *block;
    char num_buf[16];

    printf("┌──────────────────────────────────────────────┐\n");
    printf("│           MEMORY BLOCK DUMP                  │\n");
    printf("├──────────────────────────────────────────────┤\n");

    while (offset != -1 && in_bounds(offset, DATA_REGION_OFFSET, my_sub(RAM_SIZE, 1))) {
        block = get_header_at(offset);

        my_int_to_str(block_num, num_buf);
        printf("│ Block %-4s | ", num_buf);

        my_int_to_str(block->size, num_buf);
        printf("Size: %-8s | ", num_buf);

        printf("Status: %-5s | ", block->is_free ? "FREE" : "USED");

        my_int_to_str(offset, num_buf);
        printf("@%-8s│\n", num_buf);

        offset = block->next_offset;
        block_num = my_add(block_num, 1);
    }

    printf("└──────────────────────────────────────────────┘\n");

    my_int_to_str(memory_used(), num_buf);
    printf("  Used: %s bytes  |  ", num_buf);
    my_int_to_str(memory_free(), num_buf);
    printf("Free: %s bytes\n", num_buf);
}
