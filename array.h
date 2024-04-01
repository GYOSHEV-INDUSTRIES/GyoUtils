#pragma once

/*
In this file:
- Array, a simple replacement to std::vector
- Range macro to also simplify basic for cycle syntax
*/

#ifndef GYOFIRST
    #include "first.h"
#endif

#ifndef GYO_ALLOCATORS
    #include "allocators.h"
#endif

#define GYO_ARRAY

template <typename T>
struct Array {
    s32 size = 0;
    s32 reserved_size = 0;
    T* ptr = NULL;
    Allocator alloc;
    T& operator[](s32 i) { ASSERT_BOUNDS(i, 0, size); return ptr[i]; }
};

// TODO(cogno): pass to s64/u64, an array of 4gb of data is kind of bad...

template <typename T>
void printsl_custom(Array<T> arr) {
    printsl("[");
    For(arr) {
        if (it_index != 0) printsl(",");
        printsl(it);
    }
    printsl("]");
}

// uses the custom given allocator
template<typename T>
Array<T> array_new(s32 size, Allocator alloc) {
    ASSERT(size >= 0, "cannot create array with negative size %", size);
    Array<T> array;
    array.reserved_size = size;
    array.size = 0;
    array.alloc = alloc;
    array.ptr = (T*)array.alloc.handle(AllocOp::ALLOC, array.alloc.data, 0, size * sizeof(T), NULL);
    return array;
}

// defaults to using the default allocator
template<typename T>
Array<T> array_new(s32 size) { return array_new<T>(size, default_allocator); }

// makes an array which uses the given arena
template<typename T>
Array<T> array_new(s32 size, Arena* alloc) { return array_new<T>(size, make_allocator(alloc)); }

// quicker way to make a fixed-size array
template<typename T>
Array<T> make_fixed_array(s32 size, Bump* alloc) { return array_new<T>(size, make_allocator(alloc)); }


// TODO(cogno): we can use this! We just have to store the allocators data somewhere. But that's it! Just make a global allocator storage for these allocators. Call it allocator_storage!
#if 0
API(cogno): UNSAFE EXPERIMENTAL API, DO NOT USE IF YOU DON'T KNOW WHAT YOU'RE DOING!
// these 2 functions are used to make in a much more convenient way a fixed-size array,
// they will create a bump allocator which controls both itself and the array memory.
// The problem is that when you deallocate the array, you also should deallocate the hidden
// bump allocator. If you don't you have a memory leak. If we make it so array_free also
// deallocs the array allocator, you risk undefined behaviours (we don't know if the given
// allocator holds only this array or also something else!). So we created the new
// array_free_all(...) function, which should be used instead of array_free(...), but only for
// fixed size arrays (and arrays which create their hidden allocators).
// since we've realized that we're never using these anyway we might remove these and keep the 
// more verbose but actually much more useful functions which require an allocator as input.
//                                  - Cogno 2023/02/16
template<typename T>
Array<T> make_fixed_array(s32 size) {
    ASSERT(size >= 0, "cannot create array with negative size %", size);
    Array<T> array;
    array.reserved_size = size;
    array.size = 0;
    array.alloc = make_allocator(make_bump_allocator_floating(size));
    array.ptr = (T*)array.alloc.handle(AllocOp::ALLOC, array.alloc.data, size, NULL);
    return array;
}

// will free the complete allocator (with everything inside it, including the array)
template<typename T>
void array_free_all(Array<T>* array) {
    if(array->alloc.handle != NULL) array->ptr = (T*)array->alloc.handle(AllocOp::FREE_ALL, array->alloc.data, 0, array->ptr);
    array->size = 0;
    array->reserved_size = 0;
}
#endif

// will free from the allocator only the space used by the array
template<typename T>
void array_free(Array<T>* array) {
    if(array->alloc.handle != NULL) array->ptr = (T*)array->alloc.handle(AllocOp::FREE, array->alloc.data, 0, 0, array->ptr);
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
    if(array->alloc.handle == NULL) array->alloc = default_allocator;
    array->ptr = (T*)array->alloc.handle(AllocOp::REALLOC, array->alloc.data, array->reserved_size * sizeof(T), new_size * sizeof(T), array->ptr);
    ASSERT(array->ptr != NULL, "couldn't allocate new memory (array is full!)");
    array->reserved_size = new_size;
}

template<typename T>
void array_reserve(Array<T>* array, s32 to_add) {
    if(array->size + to_add <= array->reserved_size) return; // we already have enough space
    
    s32 new_size = array->reserved_size * 2;
    if(new_size < 8) new_size = 8;
    if(array->size + to_add > new_size) new_size = array->size + to_add;
    array_resize(array, new_size);
    ASSERT(array->size + to_add <= array->reserved_size, "not enough memory allocated! wanted % but was %", array->size + to_add, array->reserved_size);
}

template<typename T>
void array_insert(Array<T>* array, T data, s32 index) {
    array_reserve(array, 1);
    if(!ASSERT_BOUNDS(index, 0, array->size + 1)) return;
    
    //move every data from index to end forward by 1
    for(s32 i = array->size; i > index; i--) {
        array->ptr[i] = array->ptr[i-1];
    }
    
    //add new data
    array->ptr[index] = data;
    array->size++;
}

template<typename T>
void array_append(Array<T>* array, T data) {
    array_reserve(array, 1);
    array->ptr[array->size++] = data;
}

template<typename T>
void array_remove_at(Array<T>* array, s32 index) {
    if(!ASSERT_BOUNDS(index, 0, array->size)) return;
    
    //move every data from index to end back by 1
    for(s32 i = index; i < array->size - 1; i++) {
        ASSERT(i >= 0 && i + 1 < array->size, "out of memory access");
        array->ptr[i] = array->ptr[i + 1];
    }
    
    //then clear last space with zeros (to avoid keeping invalid memory)
    memset(array->ptr + array->size - 1, 0, sizeof(T));
    array->size--;
}

// so you can use this as a stack (push=append)
template<typename T>
T array_pop(Array<T>* array) {
    ASSERT(array->size > 0, "cannot pop from an empty stack/array");
    T element = array->ptr[array->size - 1];
    array_remove_at(array, array->size - 1);
    return element;
}

// so you can use this as a queue (queue=append)
template<typename T>
T array_dequeue(Array<T>* array) {
    ASSERT(array->size > 0, "cannot dequeue from an empty queue/array");
    T element = array->ptr[0];
    array_remove_at(array, 0);
    return element;
}

// NOTE(cogno): most of the times you can simply use the operator overload, so doing array[0] = 10; or auto temp = array[15];, but if you have the pointer to the array (instead of the array) then the operator overload will not work (because you'll be accessing the pointer!) so these functions can be used instead (or you can take a reference to the array instead of a pointer)
template<typename T>
T array_get_data(Array<T>* array, s32 index) {
    ASSERT_BOUNDS(index, 0, array->size);
    return array->ptr[index];
}

template<typename T>
void array_set(Array<T>* array, s32 index, s32 value) {
    ASSERT_BOUNDS(index, 0, array->size);
    array->ptr[index] = value;
}

template<typename T>
T* array_get_ptr(Array<T>* array, s32 index) {
    ASSERT_BOUNDS(index, 0, array->size);
    return &array->ptr[index];
}
