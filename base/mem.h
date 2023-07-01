#ifndef __MEM_H__
#define __MEM_H__ 

#include "defines.h"

#define ARENA_MAX_SIZE      MegaBytes(256)   

typedef struct Arena {
    u8 *memory;
    usize max;
    usize alloc_position;
    usize commit_position;
} Arena;

void arena_init(Arena *arena);
void arena_deinit(Arena *arena);

void *arena_alloc(Arena *arena, usize size);
void *arena_alloc_zero(Arena *arena, usize size);
void *arena_resize(Arena *arena, void *ptr, usize size);

#define POOl_MAX_SIZE       MegaBytes(256)   

typedef struct PoolFreeNode {
    struct PoolFreeNode *next;
} PoolFreeNode;

typedef struct {
    u8 *buffer;
    usize max;
    usize chunk_size;

    PoolFreeNode *head;
} Pool;

void pool_init(Pool *pool, usize chunk_size);
void pool_deinit(Pool *pool);

void *pool_alloc(Pool *pool);
void pool_free(Pool *pool, void *ptr);

#endif /* __MEM_H__ */
