#include <cstring> // for memcpy used below

struct Arena {
    void* data = NULL;
    int size_available = 0;
    int prev_offset = 0;
    int curr_offset = 0;
    
    u8 padding[4];
};
// TODO(cogno): pass to s64/u64, an arena of 4gb of data is kind of bad...

struct ArenaHeader {
    ArenaHeader* previous_block = NULL;
};

void printsl_custom(Arena b) {
    if (b.size_available == 0) {
        printsl("Uninitialized Arena Allocator");
        return;
    }
    
    float fill_percentage = 100.0f * b.curr_offset / b.size_available;
    int last_alloc_size = b.curr_offset - b.prev_offset;
    float last_alloc_percentage = 100.0f * last_alloc_size / b.size_available;
    printsl("Arena Allocator of % bytes (%\\% full), last allocation of % bytes (%\\%)", b.size_available, fill_percentage, last_alloc_size, last_alloc_percentage);
}

#define DEFAULT_ALIGNMENT (sizeof(char*))


void arena_reset(Arena* a) {
    a->curr_offset = 0;
    a->prev_offset = 0;
}

// TODO(cogno): test memory alignment at 0, 4 and 8 bytes

ArenaHeader* arena_get_header_before_data(void* arena_data) {
    if(arena_data == NULL) return NULL; // no header before no block duh
    int header_size = sizeof(ArenaHeader) + DEFAULT_ALIGNMENT % sizeof(ArenaHeader);
    u8* header_start = (u8*)arena_data - header_size;
    return (ArenaHeader*)header_start;
}

void* arena_prepare_new_memory_block(Arena* arena, s32 size_requested, ArenaHeader* old_block) {
    int header_size = sizeof(ArenaHeader) + DEFAULT_ALIGNMENT % sizeof(ArenaHeader);
    int alloc_size = size_requested + header_size; // TODO(cogno): default min size
    void* memory = calloc(alloc_size, sizeof(u8)); // TAG: MaybeWeShouldDoThisBetter
    
    auto* head = (ArenaHeader*)memory;
    head->previous_block = old_block;
    
    return (void*)((u8*)memory + header_size);
}

// generic functionality used by Allocator in allocators.h, you can use the functions below for ease of use
void* arena_handle(AllocOp op, void* alloc, s32 old_size, s32 size_requested, void* ptr_request) {
    Arena* allocator = (Arena*)alloc;
    switch(op) {
        case AllocOp::GET_NAME: return (void*)"Arena Allocator";
        case AllocOp::INIT: {
            arena_reset(allocator);
            allocator->data = arena_prepare_new_memory_block(allocator, size_requested, NULL);
            allocator->size_available = size_requested; // because we don't give the user the header, it's for us
            return allocator->data;
        } break;
        case AllocOp::REALLOC: {
            // TODO(cogno): you can now use old_size to simplify this code (I think)
            if((u8*)allocator->data + allocator->prev_offset == ptr_request) {
                // if the alloc to resize is the last one we have then we can do it very easily!
                auto size_the_alloc_has = allocator->curr_offset - allocator->prev_offset;
                auto free_space_left = allocator->size_available - allocator->curr_offset;

                // user wants to shrink or resize (and we have space)
                if(size_requested <= size_the_alloc_has || size_requested <= free_space_left) {
                    allocator->curr_offset = allocator->prev_offset + size_requested;
                    return ptr_request;
                }
            }
        } // there's not enough space, intentionally fall into alloc
        case AllocOp::ALLOC: {
            char* top = (char*)allocator->data + allocator->curr_offset;
            // TODO(cogno): this assumes the initial pointer is aligned, is it so? should we better align this?
            int unaligned_by = allocator->curr_offset % DEFAULT_ALIGNMENT;
            int space_left_in_block = DEFAULT_ALIGNMENT - unaligned_by;
            
            // NOTE(cogno): since the processor retrives data in chunks, if an allocation crosses a word boundary, you will require 1 extra access, which is slow! If we can fit the new allocation in the space remaining we do so, else we align to avoid being slow.
            if(unaligned_by != 0 && space_left_in_block < size_requested) allocator->curr_offset += space_left_in_block;
            
            void* new_memory = (char*)allocator->data + allocator->curr_offset;
            if(allocator->curr_offset + size_requested > allocator->size_available) {
                // resize necessary
                // COPYPASTE(cogno): this is basically equal to AllocOp::INIT, except for the pointer to the old block, easily compressible
                arena_reset(allocator);
                auto* old_header = arena_get_header_before_data(allocator->data);
                new_memory = arena_prepare_new_memory_block(allocator, size_requested, old_header);
                allocator->data = new_memory;
                allocator->size_available = size_requested;
            }
            
            if(op == AllocOp::REALLOC && ptr_request != NULL) {
                // we need to realloc and we have enough space in THIS block
                // API(cogno): min is in math, maybe it's better to have it *everywhere* since it's so common ?
                int amount_to_copy = size_requested < old_size ? size_requested : old_size;
                memcpy(new_memory, ptr_request, amount_to_copy);
            }
            
            allocator->prev_offset = allocator->curr_offset;
            allocator->curr_offset += size_requested;
            return (void*)new_memory;
        } break;
        case AllocOp::FREE_ALL: {
            arena_reset(allocator);
            allocator->size_available = 0;
            // we need to go back every block until no more are left
            ArenaHeader* header_of_this_block = arena_get_header_before_data(allocator->data);
            while(header_of_this_block != NULL) {
                ArenaHeader* header_of_prev_block = header_of_this_block->previous_block;
                free((void*)header_of_this_block); // TAG: MaybeWeShouldDoThisBetter
                header_of_this_block = header_of_prev_block;
            }
            return NULL;
        }; break;
        default: return NULL; // not implemented
    }
}

// NOTE(cogno): we don't have a quick way to make an arena from a pre-existing buffer because the arena can't resize that buffer, you should probably use a bump allocator instead

// will allocate its memory automatically
Arena make_arena_allocator(int min_size) {
    Arena a = {};
    arena_handle(AllocOp::INIT, &a, 0, min_size, NULL);
    return a;
}

void arena_free_all(Arena* a) { arena_handle(AllocOp::FREE_ALL, a, 0, 0, NULL); }
void* arena_alloc(Arena* a, int size) { return arena_handle(AllocOp::ALLOC, a, 0, size, NULL); }
void* arena_realloc(Arena* a, void* to_resize, int new_size) { return arena_handle(AllocOp::ALLOC, a, 0, new_size, to_resize); }
void* arena_realloc(Arena* a, void* to_resize, int new_size, int old_size) { return arena_handle(AllocOp::ALLOC, a, old_size, new_size, to_resize); }
