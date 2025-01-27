/*
In this file:
A Circular buffer allocator.
Controls a fixed buffer to allocate elements inside it in a circular way.
Each allocation will be added on top of the buffer circularly.
Only the *last* allocations can be freed.
In a Stack you can free in the reverse order of what you allocated,
in a Circular you can free in the *same* order of what you allocated.
*/

#pragma once
#define GYO_CIRCULAR

struct Circular {
    void* data = NULL;
    int size_available = 0;
    int top_offset = 0;
    int bot_offset = 0;
    int last_alloc_offset = 0;
};
// TODO(cogno): pass to s64/u64, a bump of 4gb of data is kind of bad...
// TODO(cogno): make circular work when initialized to zero

void printsl_custom(Circular c) {
    if (c.size_available == 0) {
        printsl("Uninitialized Circular Allocator");
        return;
    }
    
    float fill_percentage = 100.0f * (c.top_offset - c.bot_offset) / c.size_available;
    int last_alloc_size = c.top_offset - c.last_alloc_offset;
    float last_alloc_percentage = 100.0f * last_alloc_size / c.size_available;
    printsl("Circular Allocator of % bytes (%\\% full), last allocation of % bytes (%\\%)", c.size_available, fill_percentage, last_alloc_size, last_alloc_percentage);
}

#define GYO_CIRC_DEFAULT_ALIGNMENT (16) // read in bump.h


void circular_reset(Circular* a) {
    a->top_offset = 0;
    a->bot_offset = 0;
    a->last_alloc_offset = 0;
    maybe_remove_all_allocations(a->data);
}

// TODO(cogno): test memory alignment at 0, 4 and 8 bytes

// internal, do not call! (use circular_handle(AllocOp::ALLOC, ...) or mem_alloc(...) instead)
void* _circular_make_allocation(Circular* allocator, s32 size_requested) {
    auto* alloc_start = (char*)allocator->data + allocator->top_offset;
    maybe_add_tracking_info(allocator->data, allocator->size_available, allocator->top_offset, size_requested);
    allocator->last_alloc_offset = allocator->top_offset;
    allocator->top_offset += size_requested;
    return (void*)alloc_start;
}


// generic functionality used by Allocator in allocators.h, you can use the functions below for ease of use
void* circular_handle(AllocOp op, void* alloc, s32 old_size, s32 size_requested, void* to_free) {
    ASSERT(alloc != NULL, "Invalid allocator data given (was NULL)");
    Circular* allocator = (Circular*)alloc;
    switch(op) {
        case AllocOp::GET_NAME: return (void*)"Bump Allocator";
        case AllocOp::INIT: {
            circular_reset(allocator);
            allocator->data = calloc(size_requested, sizeof(u8)); // TAG: MaybeWeShouldDoThisBetter
            allocator->size_available = size_requested;
            return allocator->data;
        } break;
        // NOTE(cogno): we can make REALLOC work only in 1 case: if the last allocation wants more memory (and we have it available) then we can simply increase the size. Is this a good idea? (I think it is because it might screw up deallocations) - Cogno 2024/11/04
        case AllocOp::ALLOC: {
            // TODO(cogno): this assumes the initial pointer is aligned, is it so? should we better align this?
            int unaligned_by = allocator->top_offset % GYO_CIRC_DEFAULT_ALIGNMENT;
            int space_left_in_block = GYO_CIRC_DEFAULT_ALIGNMENT - unaligned_by;
            
            // NOTE(cogno): since the processor retrives data in chunks, if an allocation crosses a word boundary, you will require 1 extra access, which is slow! If we can fit the new allocation in the space remaining we do so, else we align to avoid being slow.
            // if(unaligned_by != 0 && space_left_in_block < size_requested) allocator->top_offset += space_left_in_block;
            // NOTE(cogno): I left this disabled because when we free we want the bot_offset to perfectly mimic the top one, and this complicates things, so I just ignored this for now. Maybe you will be able to do this.
            if(unaligned_by != 0) allocator->top_offset += space_left_in_block;
            
            // case 1. allocation fits on the top side of the circular (aka top > bot and there's enough space)
            s32 old_top = allocator->top_offset;
            if(allocator->top_offset >= allocator->bot_offset) {
                auto size_available = allocator->size_available - allocator->top_offset;
                if (size_requested <= size_available) { // enough space!
                    return _circular_make_allocation(allocator, size_requested);
                }
                // not enough space here, circularly wrap the top!
                allocator->top_offset = 0;
            }

            // case 2. allocation fits on the BOTTOM side of the circular (aka top < bot and there's enough space)
            if(allocator->top_offset < allocator->bot_offset) {
                auto size_available = allocator->bot_offset - allocator->top_offset;
                if (size_requested <= size_available) { // enough space!
                    return _circular_make_allocation(allocator, size_requested);
                }
                // not enough space here too, fallthrough into case 3
            }

            // case 3. allocation never fits (there's not enough *contiguous* space). Since Circular doesn't resize we just error/return NULL
            // TODO(cogno): do we *always* assert or do we return NULL if it's not available? I think we should return NULL and never assert (whatever uses the allocator should check if it got any useful memory!)
            ASSERT_ALWAYS(false, "Not enough space in Circular allocator. Wanted to allocate % bytes, top contains only %, bot only %", size_requested, (allocator->size_available - old_top), (allocator->bot_offset - allocator->top_offset));
            return NULL;
        } break;
        case AllocOp::FREE: {
            // circular can free only the first allocation we are tracking, check if we can do so!
            if((u8*)allocator->data == (u8*)to_free) {
                // we want to free the first allocation, it's time to circularly do so!
                allocator->bot_offset = 0;
                // BUG(cogno): what if we *intentionally* give a wrong ptr to free here? do we fuck everything up?
            }

            // first align so we hit the proper boundary
            int unaligned_by = allocator->bot_offset % GYO_CIRC_DEFAULT_ALIGNMENT;
            int space_left_in_block = GYO_CIRC_DEFAULT_ALIGNMENT - unaligned_by;
            if(unaligned_by != 0) allocator->bot_offset += space_left_in_block;

            // then maybe deallocate
            u8* first_alloc = (u8*)allocator->data + allocator->bot_offset;
            if(first_alloc == (u8*)to_free) {
                allocator->bot_offset += old_size;
                maybe_remove_tracking_info(to_free);
            }

            return NULL;
        } break;
        case AllocOp::FREE_ALL: {
            circular_reset(allocator);
            return NULL;
        } break;
        case AllocOp::DEINIT: {
            circular_reset(allocator);
            allocator->size_available = 0;
            free(allocator->data); // TAG: MaybeWeShouldDoThisBetter
            return NULL;
        } break;
        default: return NULL; // not implemented
    }
}

// will use and control pre-allocated memory for you
Circular make_circular_allocator(void* buffer, int buffer_length) {
    ASSERT(buffer != NULL, "Invalid input buffer given");
    Circular a = {};
    a.data = (char*)buffer;
    a.size_available = buffer_length;
    return a;
}

// will allocate its memory automatically
Circular make_circular_allocator(int min_size) {
    Circular c = {};
    circular_handle(AllocOp::INIT, &c, 0, min_size, NULL);
    return c;
}

