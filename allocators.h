#pragma once
#define GYO_ALLOCATORS
/*
In this file:
- The start of all allocators we made, with easy ways to use and expand them to fit your needs
- default allocator used by stuff that allocated. By default a wrapper for malloc/realloc/free, but can obviously be replaced but whatever.
*/

#ifndef GYOFIRST
    #include "first.h"
#endif

// Operations that each allocator supports. We might add/remove/change operations in the future if needed.
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
// To aid in debugging, we can track each allocation, so we can know if there are memory leaks or how they are used.
struct TrackingInfo {
    void* alloc_block;
    int block_size; // size of the whole memory block
    int start_offset;
    int allocation_size;
    // FlBump* owner; // API(cogno): this might be useful if we could have a custom name for the allocator (so we can clearly see "asset allocator" vs "temporary allocator").
};

#define TRACK_MEMORY_ALLOCATIONS true

// NOTE(cogno): we don't hold the data because it would get duplicated between exe and dll (this file is shared!). So we just hold pointers to the data and set them on both exe and dll's side.
TrackingInfo* tracking_infos = NULL;
int* current_tracking_index = NULL;
#define MAX_FL_TRACKING_INFO 200000


inline void maybe_add_tracking_info(void* block_start, int block_size, int alloc_start, int alloc_size) {
    #if TRACK_MEMORY_ALLOCATIONS
    if (tracking_infos != NULL) {
        ASSERT(*current_tracking_index + 1 < MAX_FL_TRACKING_INFO, "OUT OF TRACKING INFO MEMORY");
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


inline void maybe_remove_all_allocations(void* alloc_block_to_remove) {
    #if TRACK_MEMORY_ALLOCATIONS
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

inline void maybe_remove_tracking_info(void* alloc_to_remove) {
    #if TRACK_MEMORY_ALLOCATIONS
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


#ifndef GYO_BUMP
    #include "bump.h"
#endif
#ifndef GYO_ARENA
    #include "arena.h"
#endif
#ifndef GYO_CIRCULAR
    #include "circular.h"
#endif
// TODO(cogno): Stack Allocator

ENUM(AllocatorType,
    DEFAULT,
    BUMP,
    ARENA,
    CIRCULAR
);

// TODO(cogno): make Arena floating (like Bump, the header of the memory is the Arena data)
// TODO(cogno): make Circular floating (like Bump, the header of the memory is the Circular data)

// if you want to create your custom allocator compatible with gyoutils simply implement the handle_function (look at other allocator files for more info)
struct Allocator {
    void* data = NULL;
    AllocatorType type = AllocatorType::DEFAULT;
};
// TODO(cogno): pass to s64/u64, an allocator of 4gb of data is kind of bad...

void printsl_custom(Allocator alloc) { printsl(alloc.type); }

Allocator make_allocator(Arena* allocator) {
    Allocator out = {};
    out.data = (void*)allocator;
    out.type = AllocatorType::ARENA;
    return out;
}

Allocator make_allocator(Circular* allocator) {
    Allocator out = {};
    out.data = (void*)allocator;
    out.type = AllocatorType::CIRCULAR;
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

inline void* mem_alloc(Allocator alloc, s32 size) {
    switch(alloc.type) {
        case AllocatorType::DEFAULT:  return default_handle(AllocOp::ALLOC, alloc.data, 0, size, NULL);
        case AllocatorType::BUMP:     return fbump_handle(AllocOp::ALLOC, alloc.data, 0, size, NULL);
        case AllocatorType::CIRCULAR: return circular_handle(AllocOp::ALLOC, alloc.data, 0, size, NULL);
        case AllocatorType::ARENA:    return arena_handle(AllocOp::ALLOC, alloc.data, 0, size, NULL);
        default: ASSERT(false, "Unkwnown allocator type %", alloc.type); return NULL;
    }
}

inline void* mem_realloc(Allocator alloc, s32 old_size, s32 new_size, void* to_realloc) {
    switch(alloc.type) {
        case AllocatorType::DEFAULT:  return default_handle(AllocOp::REALLOC, alloc.data, old_size, new_size, to_realloc);
        case AllocatorType::BUMP:     return fbump_handle(AllocOp::REALLOC, alloc.data, old_size, new_size, to_realloc);
        case AllocatorType::CIRCULAR: return circular_handle(AllocOp::REALLOC, alloc.data, old_size, new_size, to_realloc);
        case AllocatorType::ARENA:    return arena_handle(AllocOp::REALLOC, alloc.data, old_size, new_size, to_realloc);
        default: ASSERT(false, "Unkwnown allocator type %", alloc.type); return NULL;
    }
}
inline void* mem_free(Allocator alloc, void* to_free, s32 size_to_free) {
    switch(alloc.type) {
        case AllocatorType::DEFAULT:  return default_handle(AllocOp::FREE, alloc.data, size_to_free, 0, to_free);
        case AllocatorType::BUMP:     return fbump_handle(AllocOp::FREE, alloc.data, size_to_free, 0, to_free);
        case AllocatorType::CIRCULAR: return circular_handle(AllocOp::FREE, alloc.data, size_to_free, 0, to_free);
        case AllocatorType::ARENA:    return arena_handle(AllocOp::FREE, alloc.data, size_to_free, 0, to_free);
        default: ASSERT(false, "Unkwnown allocator type %", alloc.type); return NULL;
    }
}
inline void* mem_free_all(Allocator alloc) {
    switch(alloc.type) {
        case AllocatorType::DEFAULT:  return default_handle(AllocOp::FREE_ALL, alloc.data, 0, 0, NULL);
        case AllocatorType::BUMP:     return fbump_handle(AllocOp::FREE_ALL, alloc.data, 0, 0, NULL);
        case AllocatorType::CIRCULAR: return circular_handle(AllocOp::FREE_ALL, alloc.data, 0, 0, NULL);
        case AllocatorType::ARENA:    return arena_handle(AllocOp::FREE_ALL, alloc.data, 0, 0, NULL);
        default: ASSERT(false, "Unkwnown allocator type %", alloc.type); return NULL;
    }
}





// By default everything that allocates will use this.
// If you want to use something else you can overwrite this with another allocator,
// or control on a case per case basis (for example everything uses default allocator
// except this specific array because when I constructed the array I gave it a different one).
Allocator default_allocator = {NULL, AllocatorType::DEFAULT}; // points to default handle

// will allocate a block of memory from a given allocator, and control that
Circular make_circular_allocator(Allocator alloc, int min_size) { return make_circular_allocator(mem_alloc(alloc, min_size), min_size); }


Allocator make_allocator(Allocator parent, Circular* out_allocator, s32 size) {
    ASSERT(out_allocator != NULL, "Invalid input given, cannot build an allocator (was NULL)");
    *out_allocator = make_circular_allocator(parent, size);
    return make_allocator(out_allocator);
}

// Construct an allocator with the given input, initializing it in the process.
// This is just a shortcut so you don't have to make_bump and then make_allocator
Allocator make_allocator(Circular* out_allocator, s32 size) {
    ASSERT(out_allocator != NULL, "Invalid input given, cannot build an allocator (was NULL)");
    *out_allocator = make_circular_allocator(size);
    return make_allocator(out_allocator);
}

Allocator make_fbump_allocator(s32 size) {
    Allocator out = {};
    out.type = AllocatorType::BUMP;
    out.data = fbump_handle(AllocOp::INIT, NULL, 0, size, NULL);
    return out;
}

Allocator make_fbump_allocator(Allocator alloc, s32 size) {
    Allocator out = {};
    out.type = AllocatorType::BUMP;
    s32 alloc_size = fbump_header_size() + size;
    out.data = fbump_handle(AllocOp::INIT, mem_alloc(alloc, alloc_size), 0, size, NULL);
    return out;
}