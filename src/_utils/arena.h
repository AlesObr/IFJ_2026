/**
 * @file arena.h
 * @brief Region-based memory arena allocator interface.
 * @author Aleš Obr (skolp)
 * @note Standard: ISO C11
 */

#ifndef ARENA_H
#define ARENA_H

#include <stddef.h>
#include <stdint.h>
#include <stdalign.h>

#define ARENA_DEFAULT_BLOCK_SIZE (64 * 1024) // 64 KB

/**
 * @brief Represents a single contiguous block of memory within the arena.
 */
typedef struct ArenaBlock {
	struct ArenaBlock *next;
	size_t capacity;
	size_t used;
	alignas(max_align_t) uint8_t data[];
} ArenaBlock;

/**
 * @brief Represents a memory arena managing linked memory blocks.
 */
typedef struct {
	ArenaBlock *head;
} Arena;

/**
 * @brief Initializes the arena structure.
 * @param arena Pointer to the arena to initialize.
 */
void arena_init(Arena *arena);

/**
 * @brief Allocates memory from the arena with strict alignment.
 * @param arena The arena from which to allocate memory.
 * @param size The size of the memory to allocate in bytes.
 * @return A pointer to the aligned allocated memory (terminates on OOM).
 */
void* arena_alloc(Arena *arena, size_t size);

/**
 * @brief Deallocates all blocks managed by the arena and resets it.
 * @param arena The arena to dispose.
 */
void arena_dispose(Arena *arena);

#endif // ARENA_H
