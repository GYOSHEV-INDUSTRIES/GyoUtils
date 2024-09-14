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

// API(cogno): can we have an allocator type so it's easier to find it?
// API(cogno): can we have an allocator name so it's easier to find it? maybe a name per allocator?
struct TrackingInfo {
    void* alloc_block;
    int block_size; // size of the whole memory block
    int start_offset;
    int allocation_size;
};

#define TRACKING_INFO_ENABLED true // set to false to disable tracking info
TrackingInfo* tracking_infos = NULL;
int* current_tracking_index = NULL;
const int MAX_TRACKING_INFOS = 200000;


inline void maybe_add_tracking_info(void* block_start, int block_size, int alloc_start, int alloc_size) {
    #if TRACKING_INFO_ENABLED
    if (tracking_infos != NULL) {
        ASSERT(*current_tracking_index + 1 < MAX_TRACKING_INFOS, "OUT OF TRACKING INFO MEMORY");
        TrackingInfo t = {};
        t.alloc_block = block_start;
        t.block_size = block_size;
        t.start_offset = alloc_start;
        t.allocation_size = alloc_size;
        tracking_infos[*current_tracking_index] = t;
        *current_tracking_index = *current_tracking_index + 1;
    }
    #endif
}

inline void maybe_remove_tracking_info(void* alloc_to_remove) {
    #if TRACKING_INFO_ENABLED
    if (tracking_infos != NULL) {
        for(int i = *current_tracking_index - 1; i >= 0; i--) {
            auto data = tracking_infos[i];
            void* previous_allocation = (void*)((u8*)data.alloc_block + data.start_offset);
            if (previous_allocation != alloc_to_remove) continue; // keep data on other allocations
            
            // we have found the allocations to remove, do so!
            // aka move everything from where we are to the end of the array back 1
            for(s32 j = i; j < *current_tracking_index - 1; j++) {
                tracking_infos[j] = tracking_infos[j + 1];
            }
            
            *current_tracking_index = *current_tracking_index - 1; // new free spot!
            break; // there cannot be 2 allocations on the same spot, so we're done.
        }
    }
    #endif
}

inline void maybe_remove_all_allocations(void* alloc_block_to_remove) {
    #if TRACKING_INFO_ENABLED
    if (tracking_infos != NULL) {
        // remove all useless allocations data from the array
        // PERF(cogno): I wonder if we can make this faster...
        for(int i = *current_tracking_index - 1; i >= 0; i--) {
            auto data = tracking_infos[i];
            if (data.alloc_block != alloc_block_to_remove) continue; // keep data on other allocators
            
            // this data is from this allocator, but we just reset it, so we should clear it off!
            // aka move everything from where we are to the end of the array back 1
            for(s32 j = i; j < *current_tracking_index - 1; j++) {
                tracking_infos[j] = tracking_infos[j + 1];
            }
            
            *current_tracking_index = *current_tracking_index - 1; // new free spot!
            // we can have multiple allocations in the same block, check if we need to remove others.
        }
    }
    #endif
}

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
        case AllocOp::ALLOC: {
            auto* allocated = calloc(size_requested, sizeof(u8));
            maybe_add_tracking_info(allocated, size_requested, 0, size_requested);
            return allocated;
        }
        case AllocOp::REALLOC: {
            auto* reallocated = realloc(ptr_request, size_requested);
            // TODO(cogno): handle tracking of reallocs, how should we do it?
            return reallocated;
        }
        case AllocOp::FREE: {
            free(ptr_request);
            maybe_remove_tracking_info(ptr_request);
            return NULL;
        }
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

Allocator default_allocator = {NULL, default_handle}; // points to default handle
