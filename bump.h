
struct Bump {
    void* data = NULL;
    int size_available = 0;
    int prev_offset = 0;
    int curr_offset = 0;
    
    // API(cogno): I don't like this very much.
    // basically: if we want the bump to be used in the array without being annoying
    //  we need to store allocator data inside the allocation itself 
    // (as in, first put the allocator with its data and then the array storage).
    // but when you free an allocator you don't know if it's an allocator that points to a
    // block that can be free or if the allocator itself is the start of the block to free,
    // so we use this flag.
    // to avoid this as of now I see 2 solutions: 
    // 1. every bump allocator is floating.
    // 2. every bump is non floating, and we find another way to make fixed array convenient.
    bool is_floating = false;
    u8 padding[3];
};
// TODO(cogno): pass to s64/u64, a bump of 4gb of data is kind of bad...
// TODO(cogno): remove floating bump allocators
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
            
            auto* alloc_start = (char*)allocator->data + allocator->curr_offset;
            allocator->prev_offset = allocator->curr_offset;
            allocator->curr_offset += size_requested;
            return (void*)alloc_start;
        } break;
        case AllocOp::FREE_ALL: {
            bump_reset(allocator);
            allocator->size_available = 0;
            if(allocator->is_floating) free(alloc);
            else                       free(allocator->data); // TAG: MaybeWeShouldDoThisBetter
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

// makes a bump allocator and stores it's memory inside the allocation (so you don't have to hold it yourself).
// the entire allocation is AT LEAST min_size + sizeof(Bump), 
Bump* make_bump_allocator_floating(int min_size) {
    void* memory = calloc(min_size + sizeof(Bump), sizeof(u8));
    print("made memory of size % at %", min_size + sizeof(Bump), memory);
    Bump* allocator = (Bump*)memory;
    allocator->curr_offset = 0;
    allocator->prev_offset = 0;
    allocator->size_available = min_size;
    allocator->data = (u8*)(allocator + 1);
    allocator->is_floating = true;
    return allocator;
}

void bump_free_all(Bump* a) { bump_handle(AllocOp::FREE_ALL, a, 0, 0, NULL); }
void* bump_alloc(Bump* a, int size) { return bump_handle(AllocOp::ALLOC, a, 0, size, NULL); }
