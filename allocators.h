/*
The start of all allocators we made, with easy ways to use and expand them to fit your needs
*/
ENUM(AllocOp,
    ALLOC,
    REALLOC,
    FREE,
    FREE_ALL,
    INIT,
    DEINIT,
    GET_NAME
);



#include "bump.h"
#include "arena.h"


struct Allocator {
    void* data;
    void* (*handle)(AllocOp op, void* allocator_data, s32 size_requested, void* alloc_to_free);
};

void printsl_custom(Allocator alloc) {
    printsl((const char*)alloc.handle(AllocOp::GET_NAME, &alloc, 0, NULL));
}

Allocator make_allocator(Bump* allocator) {
    Allocator out = {};
    out.data = (void*)allocator;
    out.handle = bump_handle;
    return out;
}




/*
COPYPASTE(cogno): from dynamic_array to have a real usage example

template <typename T>
struct Array {
    s32 size = 0;
    s32 reserved_size = 0;
    T* ptr = NULL;
    Allocator allocator;
    T& operator[](s32 i) { ASSERT_BOUNDS(i, 0, size); return ptr[i]; }
};


template<typename T>
Array<T> array_new(s32 size, Allocator allocator) {
    ASSERT(size >= 0, "cannot create array with negative size %", size);
    Array<T> array;
    array.reserved_size = size;
    array.size = 0;
    array.allocator = allocator;
    array.ptr = allocator.handle(AllocOp::ALLOC, array.allocator.data, size * sizeof(T), NULL);
    return array;
}

template<typename T>
void array_free(Array<T>* array) {
    array->ptr = alloc->handle(AllocOp::FREE, &array->allocator, 0, array->ptr);
    array->size = 0;
    array->reserved_size = 0;
}

template<typename T>
void array_clear(Array<T>* array) {
    array->size = 0;
    // API(cogno): maybe we should add a slow path that sets all to 0? maybe always do it? maybe provide 2 options? I don't know...
}

template<typename T>
void array_resize(Array<T>* array, s32 new_size) {
    array->ptr = (T*)alloc->handle(AllocOp::REALLOC, array->allocator, new_size * sizeof(T), NULL);
    array->reserved_size = new_size;
}

*/