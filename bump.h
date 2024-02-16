
struct Bump {
    void* data;
    int size;
    int prev_offset;
    int curr_offset;
    u8 padding[4];
};

void printsl_custom(Bump b) {
    if (b.size == 0) {
        printsl("Uninitialized bump allocator");
        return;
    }
    
    float fill_percentage = 100.0f * b.curr_offset / b.size;
    int last_alloc_size = b.curr_offset - b.prev_offset;
    float last_alloc_percentage = 100.0f * last_alloc_size / b.size;
    printsl("Bump allocator of % bytes (%\\% full), last allocation of % bytes (%\\%)", b.size, fill_percentage, last_alloc_size, last_alloc_percentage);
}

#define DEFAULT_ALIGNMENT (sizeof(char*))

// TAG: MaybeWeShouldDoThisBetter
// Using malloc/realloc/free works but makes us very slow. Allocators are to control memory, and malloc is one of them.
// If our allocator goes through malloc why use it? You already have malloc!
// We should replaces these spots to go through the platform api and make our allocators faster, but how? Is it a good idea?


void bump_reset(Bump* a) {
    a->curr_offset = 0;
    a->prev_offset = 0;
}

// TODO(cogno): test memory alignment at 0, 4 and 8 bytes

// generic functionality used by Allocator in allocators.h, you can use the functions below for ease of use
void* bump_handle(AllocOp op, Bump* allocator, s32 size_requested, void* to_free) {
    switch(op) {
        case AllocOp::INIT: {
            bump_reset(allocator);
            allocator->data = calloc(size_requested, sizeof(u8));
            allocator->size = size_requested;
            return allocator->data;
        } break;
        case AllocOp::ALLOC: {
            char* top = (char*)allocator->data + allocator->curr_offset;
            // TODO(cogno): this assumes the initial pointer is aligned, is it so? should we better align this?
            int unaligned_by = allocator->curr_offset % DEFAULT_ALIGNMENT;
            int space_left_in_cache_line = DEFAULT_ALIGNMENT - unaligned_by;
            
            // NOTE(cogno): since the processor retrives data in chunks, if an allocation crosses a word boundary, you will require 1 extra access, which is slow! If we can fit the new allocation in the space remaining we do so, else we align to avoid being slow.
            if(space_left_in_cache_line < size_requested) allocator->curr_offset += space_left_in_cache_line;
            
            // bump allocators do NOT resize
            ASSERT(allocator->curr_offset + size_requested <= allocator->size, "arena out of memory (currently at %, requested %, size %)", allocator->curr_offset, size_requested, allocator->size);
            
            auto* alloc_start = (char*)allocator->data + allocator->curr_offset;
            allocator->prev_offset = allocator->curr_offset;
            allocator->curr_offset += size_requested;
            return (void*)alloc_start;
        } break;
        case AllocOp::FREE_ALL: {
            bump_reset(allocator);
            allocator->size = 0;
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
    a.size = buffer_length;
    return a;
}

// will allocate its memory automatically
Bump make_bump_allocator(int min_size) {
    Bump b = {};
    bump_handle(AllocOp::INIT, &b, min_size, NULL);
    return b;
}

// makes a bump allocator and stores it's memory inside the allocation (so you don't have to hold it yourself).
// the entire allocation is AT LEAST min_size + sizeof(Bump), 
Bump* make_bump_allocator_floating(int min_size) {
    void* memory = calloc(min_size + sizeof(Bump), sizeof(u8));
    Bump* allocator = (Bump*)memory;
    allocator->curr_offset = 0;
    allocator->prev_offset = 0;
    allocator->size = min_size;
    allocator->data = (u8*)(allocator + 1);
    return allocator;
}

void bump_free_all(Bump* a) { bump_handle(AllocOp::FREE_ALL, a, 0, NULL); }
void* bump_alloc(Bump* a, int size) { return bump_handle(AllocOp::ALLOC, a, size, NULL); }
