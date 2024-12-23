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

#define GYO_BUMP_DEFAULT_ALIGNMENT (16) // SIMD is 16-bytes aligned, so vec4 might break if we don't align by 16 at a time
// API(cogno): we should probably have a way to align type-per-type instead of forcing it on everyone, right? Because some stuff might want more than 16-byte alignment and most likely break...

// TAG: MaybeWeShouldDoThisBetter
// Using malloc/realloc/free works but makes us very slow. Allocators are to control memory, and malloc is one of them.
// If our allocator goes through malloc why use it? You already have malloc!
// We should replace these spots to go through the platform api and make our allocators faster, but how? Is it a good idea?


void bump_reset(Bump* a) {
    a->curr_offset = 0;
    a->prev_offset = 0;
    maybe_remove_all_allocations(a->data);
}

// TODO(cogno): test memory alignment at 0, 4 and 8 bytes

inline s32 fbump_header_size() {
    s32 bump_size = sizeof(Bump);
    s32 extra = bump_size % GYO_BUMP_DEFAULT_ALIGNMENT;
    if(extra != 0) bump_size += GYO_BUMP_DEFAULT_ALIGNMENT - extra;
    return bump_size;
}

void* init_fbump(void* mem_block, s32 mem_block_size) {
    s32 bump_size = fbump_header_size();
    if(mem_block == NULL) { // we need a NEW block
        s32 actual_size = mem_block_size + bump_size;
        // TODO(cogno): overflow check

        // bump is floating, aka stored as the header of the block
        mem_block = calloc(actual_size, sizeof(u8)); // TAG: MaybeWeShouldDoThisBetter
    }

    Bump* bump_data = (Bump*)mem_block;
    bump_reset(bump_data);
    bump_data->data = (void*)((u8*)mem_block + bump_size);
    bump_data->size_available = mem_block_size; // don't count yourself
    return mem_block;
}

// generic functionality used by Allocator in allocators.h, you can use the functions below for ease of use
void* fbump_handle(AllocOp op, void* alloc, s32 old_size, s32 size_requested, void* to_free) {
    if(op != AllocOp::INIT) { ASSERT(alloc != NULL, "Invalid allocator data given (was NULL)"); }

    Bump* allocator = (Bump*)alloc;
    switch(op) {
        case AllocOp::GET_NAME: return (void*)"Bump Allocator";
        case AllocOp::INIT: {
            return init_fbump(alloc, size_requested);
        } break;
        // NOTE(cogno): we can make REALLOC work only in 1 case: if the last allocation wants more memory (and we have it available) then we can simply increase the size. Even though this might seem like a good idea, it's actually a BAD idea. Bump allocators are used to make arrays of FIXED size. If the fixed size array silently grows it's a problem...
        case AllocOp::ALLOC: {
            if(size_requested <= 0) return NULL; // obviously, but maybe we should just ASSERT_ALWAYS?
            // TODO(cogno): this assumes the initial pointer is aligned, is it so? should we better align this?
            int unaligned_by = allocator->curr_offset % GYO_BUMP_DEFAULT_ALIGNMENT;
            int space_left_in_block = GYO_BUMP_DEFAULT_ALIGNMENT - unaligned_by;
            
            // NOTE(cogno): since the processor retrives data in chunks, if an allocation crosses a word boundary, you will require 1 extra access, which is slow! If we can fit the new allocation in the space remaining we do so, else we align to avoid being slow.
            // PERF(cogno): does this actually work? experimentally show it!
            // UPDATE(2024/11/16): since stuff wants to be aligned, this might go against it and potentially break (for example simd!)
            if(unaligned_by != 0 && space_left_in_block < size_requested) allocator->curr_offset += space_left_in_block;
            
            // bump allocators do NOT resize
            if(allocator->curr_offset + size_requested > allocator->size_available) return NULL; // no more space in this Bump

            auto* alloc_start = (char*)allocator->data + allocator->curr_offset;
            maybe_add_tracking_info(allocator->data, allocator->size_available, allocator->curr_offset, size_requested);
            allocator->prev_offset = allocator->curr_offset;
            allocator->curr_offset += size_requested;
            return (void*)alloc_start;
        } break;
        case AllocOp::FREE_ALL: {
            bump_reset(allocator);
            return NULL;
        } break;
        case AllocOp::DEINIT: {
            bump_reset(allocator);
            allocator->size_available = 0;
            free(allocator->data); // TAG: MaybeWeShouldDoThisBetter
            return NULL;
        } break;
        default: return NULL; // not implemented
    }
}

