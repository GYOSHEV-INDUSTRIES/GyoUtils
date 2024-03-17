#pragma once
#define GYO_ALLOCATORS
/*
In this file:
- The start of all allocators we made, with easy ways to use and expand them to fit your needs
- default allocator, useful for generic operations
- temporary arena allocator, useful for temporary quick operations to be done and forget about
*/


#ifndef GYOFIRST
    #include "first.h"
#endif

ENUM(AllocOp,
    ALLOC,
    REALLOC,
    FREE,
    FREE_ALL,
    INIT,
    DEINIT,
    GET_NAME
);


#ifndef GYO_BUMP
    #include "bump.h"
#endif
#ifndef GYO_ARENA
    #include "arena.h"
#endif

// if you want to create your custom allocator compatible with gyoutils simply implement the handle_function (look at other allocator files for more info)
struct Allocator {
    void* data = NULL;
    void* (*handle)(AllocOp op, void* allocator_data, s32 old_size, s32 size_requested, void* ptr_request) = NULL;
};
// TODO(cogno): pass to s64/u64, an allocator of 4gb of data is kind of bad...

void printsl_custom(Allocator alloc) {
    if(alloc.handle == NULL) printsl("Null Allocator");
    else printsl((const char*)alloc.handle(AllocOp::GET_NAME, &alloc, 0, 0, NULL));
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

void* default_handle(AllocOp op, void* allocator_data, s32 old_size, s32 size_requested, void* ptr_request) {
    switch (op) {
        case AllocOp::GET_NAME: return (void*)"Default Allocator";
        case AllocOp::ALLOC: return calloc(size_requested, sizeof(u8));
        case AllocOp::REALLOC: return realloc(ptr_request, size_requested);
        case AllocOp::FREE: free(ptr_request); return NULL;
        // API(cogno): maybe we can make a FREE_ALL if we track each allocation (we can make each block have a header or we can make a list of each allocation on the side..., I would go with the headers...)
        default: return NULL;
    }
}

void* mem_alloc(Allocator alloc, s32 size) {
    return alloc.handle(AllocOp::ALLOC, alloc.data, 0, size, NULL);
}

void* mem_realloc(Allocator alloc, s32 old_size, s32 new_size, void* to_realloc) {
    return alloc.handle(AllocOp::REALLOC, alloc.data, old_size, new_size, to_realloc);
}

void* mem_free(Allocator alloc, void* to_free) {
    return alloc.handle(AllocOp::FREE, alloc.data, 0, 0, to_free);
}

// TODO(cogno): Stack Allocator

// API(cogno): what if instead of a single arena as default we make a stack of allocators so you can push/pop temporary allocators of different kinds for different functions? This seems interesting...

Arena temporary_arena; // even though it's empty it auto setups itself when used
Allocator default_allocator   = {NULL, default_handle}; // points to default handle
Allocator temporary_allocator = {(void*)&temporary_arena, arena_handle}; // points to temporary_arena
