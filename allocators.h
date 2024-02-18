/*
In this file:
- The start of all allocators we made, with easy ways to use and expand them to fit your needs
- default and temporary arena allocators, useful for generic operations
*/

ENUM(AllocOp,
    ALLOC,
    REALLOC,
    FREE,
    FREE_ALL,
    INIT,
    DEINIT,
    GET_NAME
);


#include "bump.h"
#include "arena.h"

// if you want to create your custom allocator compatible with gyoutils simply implement the handle_function (look at other allocator files for more info)
struct Allocator {
    void* data;
    void* (*handle)(AllocOp op, void* allocator_data, s32 size_requested, void* alloc_to_free);
};

void printsl_custom(Allocator alloc) {
    printsl((const char*)alloc.handle(AllocOp::GET_NAME, &alloc, 0, NULL));
}

Allocator make_allocator(Bump* allocator) {
    Allocator out = {};
    out.data = (void*)allocator;
    out.handle = bump_handle;
    return out;
}

Allocator make_allocator(Arena* allocator) {
    Allocator out = {};
    out.data = (void*)allocator;
    out.handle = arena_handle;
    return out;
}


Arena default_arena;   // even though they are empty they auto setup themselves when used
Arena temporary_arena; // even though they are empty they auto setup themselves when used
Allocator default_allocator   = {(void*)&default_arena,   arena_handle}; // points to default arena
Allocator temporary_allocator = {(void*)&temporary_arena, arena_handle}; // points to temporary_arena
