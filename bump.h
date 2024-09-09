#pragma once
#define GYO_BUMP

struct Bump {
    void* data = NULL;
    int size_available = 0;
    int prev_offset = 0;
    int curr_offset = 0;
    u8 padding[4];
};
// TODO(cogno): pass to s64/u64, a bump of 4gb of data is kind of bad...
// TODO(cogno): make bump work when initialized to zero

TrackingInfo* tracking_infos = NULL;
int* current_tracking_index = NULL;
const int MAX_TRACKING_INFOS = 200000;

void printsl_custom(Bump b) {
    if (b.size_available == 0) {
        printsl("Uninitialized Bump Allocator");
        return;
    }
    
    float fill_percentage = 100.0f * b.curr_offset / b.size_available;
    int last_alloc_size = b.curr_offset - b.prev_offset;
    float last_alloc_percentage = 100.0f * last_alloc_size / b.size_available;
    printsl("Bump Allocator of % bytes (%\\% full), last allocation of % bytes (%\\%)", b.size_available, fill_percentage, last_alloc_size, last_alloc_percentage);
}

#define DEFAULT_ALIGNMENT (sizeof(char*))

// TAG: MaybeWeShouldDoThisBetter
// Using malloc/realloc/free works but makes us very slow. Allocators are to control memory, and malloc is one of them.
// If our allocator goes through malloc why use it? You already have malloc!
// We should replaces these spots to go through the platform api and make our allocators faster, but how? Is it a good idea?


void bump_reset(Bump* a) {
    a->curr_offset = 0;
    a->prev_offset = 0;
    
    if (tracking_infos != NULL) {
        // remove all useless allocations data from the array
        // API(cogno): maybe we can do this better instead of literally loosing them?
        // API(cogno): actually it might be a good idea to loose them, this array becomes a tracker of memory leaks!
        for(int i = *current_tracking_index - 1; i >= 0; i--) {
            auto data = tracking_infos[i];
            if (data.alloc_block != a->data) continue; // keep data on other allocators
            
            // this data is from this allocator, but we just reset it, so we should clear it off!
            // aka move everything from where we are to the end of the array back 1
            for(s32 j = i; j < *current_tracking_index - 1; j++) {
                tracking_infos[j] = tracking_infos[j + 1];
            }
            
            *current_tracking_index = *current_tracking_index - 1; // new free spot!
        }
    }
}

// TODO(cogno): test memory alignment at 0, 4 and 8 bytes

// generic functionality used by Allocator in allocators.h, you can use the functions below for ease of use
void* bump_handle(AllocOp op, void* alloc, s32 old_size, s32 size_requested, void* to_free) {
    Bump* allocator = (Bump*)alloc;
    switch(op) {
        case AllocOp::GET_NAME: return (void*)"Bump Allocator";
        case AllocOp::INIT: {
            bump_reset(allocator);
            allocator->data = calloc(size_requested, sizeof(u8)); // TAG: MaybeWeShouldDoThisBetter
            allocator->size_available = size_requested;
            return allocator->data;
        } break;
        // NOTE(cogno): we can make REALLOC work only in 1 case: if the last allocation wants more memory (and we have it available) then we can simply increase the size. Even though this might seem like a good idea, it's actually a BAD idea. Bump allocators are used to make arrays of FIXED size. If the fixed size array silently grows it's a problem...
        case AllocOp::ALLOC: {
            char* top = (char*)allocator->data + allocator->curr_offset;
            // TODO(cogno): this assumes the initial pointer is aligned, is it so? should we better align this?
            int unaligned_by = allocator->curr_offset % DEFAULT_ALIGNMENT;
            int space_left_in_block = DEFAULT_ALIGNMENT - unaligned_by;
            
            // NOTE(cogno): since the processor retrives data in chunks, if an allocation crosses a word boundary, you will require 1 extra access, which is slow! If we can fit the new allocation in the space remaining we do so, else we align to avoid being slow.
            if(unaligned_by != 0 && space_left_in_block < size_requested) allocator->curr_offset += space_left_in_block;
            
            // bump allocators do NOT resize
            ASSERT(allocator->curr_offset + size_requested <= allocator->size_available, "arena out of memory (currently at %, requested %, available %)", allocator->curr_offset, size_requested, allocator->size_available);
            
            if (tracking_infos != NULL) {
                ASSERT(*current_tracking_index + 1 < MAX_TRACKING_INFOS, "OUT OF MEMORY");
                TrackingInfo t = {};
                t.alloc_block = allocator->data;
                t.block_size = allocator->size_available;
                t.start_offset = allocator->curr_offset;
                t.allocation_size = size_requested;
                t.color = color_lerp_hsv(BLUE, RED, (float)size_requested / 100000000);
                tracking_infos[*current_tracking_index] = t;
                *current_tracking_index = *current_tracking_index + 1;
            }
            
            auto* alloc_start = (char*)allocator->data + allocator->curr_offset;
            allocator->prev_offset = allocator->curr_offset;
            allocator->curr_offset += size_requested;
            return (void*)alloc_start;
        } break;
        case AllocOp::FREE_ALL: {
            bump_reset(allocator);
            allocator->size_available = 0;
            free(allocator->data); // TAG: MaybeWeShouldDoThisBetter
            return NULL;
        }; break;
        default: return NULL; // not implemented
    }
}

// will use and control pre-allocated memory
Bump make_bump_allocator(void* buffer, int buffer_length) {
    Bump a = {};
    a.data = (char*)buffer;
    a.size_available = buffer_length;
    return a;
}

// will allocate its memory automatically
Bump make_bump_allocator(int min_size) {
    Bump b = {};
    bump_handle(AllocOp::INIT, &b, 0, min_size, NULL);
    return b;
}

void  mem_free_all(Bump* a) { bump_handle(AllocOp::FREE_ALL, a, 0, 0, NULL); }
void* mem_alloc(Bump* a, int size) { return bump_handle(AllocOp::ALLOC, a, 0, size, NULL); }
