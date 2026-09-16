/**
 * @project IFJ26 Compiler
 * @file arena.c
 * @brief Region-based memory arena allocator implementation.
 * @author Aleš Obr (xobrale00)
 * @note Standard: ISO C11
 */

#include "arena.h"
#include <stdlib.h>
#include <stdio.h>

/**
 * @brief Internal helper to allocate a new memory block via malloc.
 */
static ArenaBlock* arena_create_block(size_t capacity) {
	ArenaBlock *block = malloc(sizeof(ArenaBlock) + capacity);
	if (!block) {
		fprintf(stderr, "Arena allocator: Out of memory (allocation failed).\n");
		exit(99);
	}
	block->next = NULL;
	block->capacity = capacity;
	block->used = 0;
	return block;
}

void arena_init(Arena *arena) {
	if (arena) {
		arena->head = NULL;
	}
}

void* arena_alloc(Arena *arena, size_t size) {
	if (!arena || size == 0) {
		return NULL;
	}

	size_t align = alignof(max_align_t);
	size_t aligned_size = (size + align - 1) & ~(align - 1);

	if (aligned_size < size) {
		fprintf(stderr, "Arena allocator: Allocation size overflow.\n");
		exit(99);
	}

	if (!arena->head) {
		size_t initial_cap = (aligned_size > ARENA_DEFAULT_BLOCK_SIZE)
							 ? aligned_size
							 : ARENA_DEFAULT_BLOCK_SIZE;
		arena->head = arena_create_block(initial_cap);
	}

	if (arena->head->used + aligned_size > arena->head->capacity) {
		size_t next_capacity = (aligned_size > ARENA_DEFAULT_BLOCK_SIZE)
							   ? aligned_size
							   : ARENA_DEFAULT_BLOCK_SIZE;

		ArenaBlock *new_block = arena_create_block(next_capacity);
		new_block->next = arena->head;
		arena->head = new_block;
	}

	void *ptr = &arena->head->data[arena->head->used];
	arena->head->used += aligned_size;
	return ptr;
}

void arena_dispose(Arena *arena) {
	if (!arena) {
		return;
	}

	ArenaBlock *curr = arena->head;
	while (curr) {
		ArenaBlock *next = curr->next;
		free(curr);
		curr = next;
	}
	arena->head = NULL;
}
