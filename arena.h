
struct Arena {
    void* data;
    int size_available = 0;
    int prev_offset = 0;
    int curr_offset = 0;
    
    u8 padding[4];
};

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

// generic functionality used by Allocator in allocators.h, you can use the functions below for ease of use
void* arena_handle(AllocOp op, void* alloc, s32 size_requested, void* ptr_request) {
    Arena* allocator = (Arena*)alloc;
    switch(op) {
        case AllocOp::GET_NAME: return (void*)"Arena Allocator";
        case AllocOp::INIT: {
            arena_reset(allocator);
            int header_size = sizeof(ArenaHeader) + DEFAULT_ALIGNMENT % sizeof(ArenaHeader);
            int alloc_size = size_requested + header_size; // TODO(cogno): default min size
            void* memory = calloc(alloc_size, sizeof(u8)); // TAG: MaybeWeShouldDoThisBetter
            
            // hide and initialize the header
            auto* head = (ArenaHeader*)memory;
            *head = {}; // maybe we don't have to do this because we memset to 0 anyway...
            
            allocator->data = (void*)((u8*)memory + header_size);
            allocator->size_available = size_requested; // because we don't give the user the header, it's for us
            return allocator->data;
        } break;
        case AllocOp::REALLOC: {
            
            if((u8*)allocator->data + allocator->prev_offset == ptr_request) {
                // if the alloc to resize is the last one we have then we can do it very easily!
                auto size_the_alloc_has = allocator->curr_offset - allocator->prev_offset;

                if(size_requested <= size_the_alloc_has) {
                    // user wants to shrink, an arena should only grow but we can allow it without loss of generality
                    allocator->curr_offset = allocator->prev_offset + size_requested;
                    return ptr_request;
                }
                
                // user wants to grow, we must be careful
                auto free_space_left = allocator->size_available - allocator->curr_offset;
                if(size_requested <= free_space_left) {
                    // we have enough space, no need to realloc!
                    allocator->curr_offset = allocator->prev_offset + size_requested;
                    return ptr_request;
                }
                
                // not enough space, resize necessary
                // 1. make enough space for the whole allocator
                // BUG(cogno): this is MEGA wrong! if the memory gets moved EVERY pointer this allocator has ever returned is FUCKED!!!
                allocator->data = realloc(allocator->data, allocator->size_available * 2); // TAG: MaybeWeShouldDoThisBetter
                allocator->size_available *= 2;
                
                // 2. tell him that we did it
                allocator->curr_offset = allocator->prev_offset + size_requested;
                return ptr_request;
            }
            
            
            
            // there's not enough space
            // 2. prepare the new space
            
            // 3. copy from the old 
        } break;
        case AllocOp::ALLOC: {
            char* top = (char*)allocator->data + allocator->curr_offset;
            // TODO(cogno): this assumes the initial pointer is aligned, is it so? should we better align this?
            int unaligned_by = allocator->curr_offset % DEFAULT_ALIGNMENT;
            int space_left_in_block = DEFAULT_ALIGNMENT - unaligned_by;
            
            // NOTE(cogno): since the processor retrives data in chunks, if an allocation crosses a word boundary, you will require 1 extra access, which is slow! If we can fit the new allocation in the space remaining we do so, else we align to avoid being slow.
            if(unaligned_by != 0 && space_left_in_block < size_requested) allocator->curr_offset += space_left_in_block;
            
            if(allocator->curr_offset + size_requested > allocator->size_available) {
                // resize necessary
                // TODO(cogno): this
            }
            
            auto* alloc_start = (char*)allocator->data + allocator->curr_offset;
            allocator->prev_offset = allocator->curr_offset;
            allocator->curr_offset += size_requested;
            return (void*)alloc_start;
        } break;
        case AllocOp::FREE_ALL: {
            arena_reset(allocator);
            allocator->size_available = 0;
            free(allocator->data); // TAG: MaybeWeShouldDoThisBetter
            return NULL;
        }; break;
        default: return NULL; // not implemented
    }
}

// NOTE(cogno): we don't have a quick way to make an arena from a pre-existing buffer because the arena can't resize that buffer, you should probably use a bump allocator instead

// will allocate its memory automatically
Arena make_arena_allocator(int min_size) {
    Arena a = {};
    arena_handle(AllocOp::INIT, &a, min_size, NULL);
    return a;
}

void arena_free_all(Arena* a) { arena_handle(AllocOp::FREE_ALL, a, 0, NULL); }
void* arena_alloc(Arena* a, int size) { return arena_handle(AllocOp::ALLOC, a, size, NULL); }
void* arena_realloc(Arena* a, void* to_resize, int new_size) { return arena_handle(AllocOp::ALLOC, a, new_size, to_resize); }
