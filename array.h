#pragma once

/*
In this file:
- Array, a simple replacement to std::vector
- Array, a variant of Array which cannot resize
- Generic functions implemented on raw pointers, if you want to use them directly
*/

#ifndef GYOFIRST
    #include "first.h"
#endif

#ifndef GYO_ALLOCATORS
    #include "allocators.h"
#endif

#define GYO_ARRAY
#define GYO_ARRAY_DEFAULT_SIZE 8


template <typename T>
void print_as_array(T* ptr, int array_size) {
    if(ptr == NULL) return printsl("(null array)");
    printsl('[');
    for(int i = 0; i < array_size; i++) {
        if (i != 0) printsl(',');
        printsl(ptr[i]);
    }
    printsl(']');
}

template <typename T>
void array_insert(T* ptr, int array_size, T to_insert, int index) {
    ASSERT(ptr != NULL, "invalid buffer given (was NULL)");
    ASSERT_ALWAYS(index >= 0 && index <= array_size, "OUT OF RANGE remove attempt. Index is %, range is from 0 to % (both inclusive)", index, array_size);
    
    //move every data from index to end forward by 1
    for(s32 i = array_size; i > index; i--) {
        ptr[i] = ptr[i-1];
    }
    
    ptr[index] = to_insert; //add new data
}

template <typename T>
void array_remove_at(T* ptr, int array_size, int index) {
    ASSERT(ptr != NULL, "invalid buffer given (was NULL)");
    ASSERT_ALWAYS(index >= 0 && index < array_size, "OUT OF RANGE remove attempt. Index is %, range is from 0 (inclusive) to %", index, array_size);

    //move every data from index to end back by 1
    for(s32 i = index; i < array_size - 1; i++) {
        ptr[i] = ptr[i + 1];
    }
    
    //then clear last space with zeros (to avoid keeping invalid memory)
    memset(ptr + array_size - 1, 0, sizeof(T));
}


// so you can use this as a stack (push=append)
template<typename T>
T array_pop(T* ptr, int array_size) {
    ASSERT(array_size > 0, "cannot pop from an empty stack/array");
    T element = ptr[array_size - 1];
    memset(ptr + array_size - 1, 0, sizeof(T));
    return element;
}

// so you can use this as a queue (queue=append)
template<typename T>
T array_dequeue(T* ptr, int array_size) {
    ASSERT(array_size > 0, "cannot dequeue from an empty queue/array");
    T element = ptr[0];
    array_remove_at(ptr, array_size, 0);
    return element;
}

template<typename T>
T array_get_data(T* ptr, int array_size, int index) {
    ASSERT_BOUNDS(index, 0, array_size);
    return ptr[index];
}

template<typename T>
void array_set(T* ptr, int array_size, int index, T value) {
    ASSERT_BOUNDS(index, 0, array_size);
    ptr[index] = value;
}

template<typename T>
T* array_get_ptr(T* ptr, int array_size, int index) {
    ASSERT_BOUNDS(index, 0, array_size);
    return &ptr[index];
}


template <typename T>
struct Array {
    s32 size = 0;
    s32 reserved_size = 0; // TODO(cogno): pass to s64/u64, an array of 4gb of data is kind of bad...
    T* ptr = NULL;
    Allocator alloc;
    T& operator[](s32 i) { ASSERT_BOUNDS(i, 0, size); return ptr[i]; }
};


template <typename T> void printsl_custom(Array<T> arr) { print_as_array(arr.ptr, arr.size); }

// uses the custom given allocator
template<typename T>
Array<T> make_array(s32 size, Allocator alloc) {
    ASSERT(size >= 0, "cannot create array with negative size %", size);
    Array<T> array;
    array.reserved_size = size;
    array.size = 0;
    array.alloc = alloc;
    array.ptr = (T*)mem_alloc(alloc, size * sizeof(T));
    return array;
}

// overloads, pretty obvious

template<typename T> Array<T> make_array(Allocator alloc) { return make_array<T>(GYO_ARRAY_DEFAULT_SIZE, alloc); }
template<typename T> Array<T> make_array(s32 size) { return make_array<T>(size, default_allocator); }


// will free from the allocator only the space used by the array
template<typename T>
void array_free(Array<T>* array) {
    if(array->alloc.handle != NULL) array->ptr = (T*)mem_free(array->alloc, array->ptr);
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
    array->ptr = (T*)mem_realloc(array->alloc, array->reserved_size * sizeof(T), new_size * sizeof(T), array->ptr);
    ASSERT(array->ptr != NULL, "couldn't allocate new memory (array is full! it's size is %)", array->reserved_size);
    array->reserved_size = new_size;
}

template<typename T>
void array_reserve(Array<T>* array, s32 to_add) {
    if(array->size + to_add <= array->reserved_size) return; // we already have enough space
    
    s32 new_size = array->reserved_size * 2;
    if(new_size < GYO_ARRAY_DEFAULT_SIZE) new_size = GYO_ARRAY_DEFAULT_SIZE;
    if(array->size + to_add > new_size) new_size = array->size + to_add;
    array_resize(array, new_size);
    ASSERT(array->size + to_add <= array->reserved_size, "not enough memory allocated! wanted % but was %", array->size + to_add, array->reserved_size);
}

template<typename T>
void array_insert(Array<T>* array, T data, s32 index) {
    array_reserve(array, 1);
    array_insert(array->ptr, array->size++, data, index);
}

template<typename T>
int array_append(Array<T>* array, T data) {
    array_reserve(array, 1);
    array->ptr[array->size++] = data;
    return array->size - 1; // return the index we just inserted in
}

template<typename T> void array_remove_at(Array<T>* array, s32 index) { array_remove_at(array->ptr, array->size--, index); }

// so you can use this as a stack (push=append)
template<typename T> T array_pop(Array<T>* array) { return array_pop(array->ptr, array->size--); }

// so you can use this as a queue (queue=append)
template<typename T> T array_dequeue(Array<T>* array) { return array_dequeue(array->ptr, array->size--); }

// NOTE(cogno): most of the times you can simply use the operator overload, so doing array[0] = 10; or auto temp = array[15];, but if you have the pointer to the array (instead of the array) then the operator overload will not work (because you'll be accessing the pointer!) so these functions can be used instead (or you can take a reference to the array instead of a pointer)
template<typename T> T array_get_data(Array<T>* array, s32 index) { return array_get_data(array->ptr, array->size, index); }
template<typename T> void array_set(Array<T>* array, s32 index, T value) { return array_set(array->ptr, array->size, index, value); }
template<typename T> T* array_get_ptr(Array<T>* array, s32 index) { return array_get_ptr(array->ptr, array->size, index); }
