#pragma once

/*
In this file:
- Array, a simple replacement to std::vector
- For macro to provide a more convenient way to iterate over Array
- Range macro to also simplify basic for cycle syntax
*/

#ifndef GYOFIRST
    #include "first.h"
#endif

template <typename T>
struct Array {
    s32 size = 0;
    s32 reserved_size = 0;
    T* ptr = NULL;
    T& operator[](s32 i) { ASSERT_BOUNDS(i, 0, size); return ptr[i]; }
};

template<typename T>
Array<T> array_new(s32 size) {
    ASSERT(size >= 0, "cannot create array with negative size %", size);
    Array<T> array;
    array.reserved_size = size;
    array.size = 0;
    array.ptr = (T*)calloc(size, sizeof(T));
    return array;
}

template<typename T>
void array_free(Array<T>* array) {
    free(array->ptr);
    array->size = 0;
    array->reserved_size = 0;
    array->ptr = NULL;
}

template<typename T>
void array_clear(Array<T>* array) {
    array->size = 0;
    // API(cogno): maybe we should add a slow path that sets all to 0? maybe always do it? maybe provide 2 options? I don't know...
}

template<typename T>
void array_resize(Array<T>* array, s32 new_size) {
    array->ptr = (T*)realloc(array->ptr, sizeof(*array->ptr) * new_size);
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
    if(!ASSERT_BOUNDS(index, 0, array->size)) return;
    
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

// 
// macros for improved for cycle. 
// Works with any structs with 'size' and 'ptr' values. 
// The 4 alternatives can iterate over elements by values, by pointer, by values in reverse order and
// by pointer in reverse order
// Example:
// For(array) {
//     print("index % = %", it_index, it);    
// }
// Which is much less stuff to write than:
// for(int i = 0; i < array.size; i++) {
//     auto value = array.ptr[i];
//     print("index % = %", i, value);
// }
//
#define For(arr) \
for(int it_index = 0, _=1;_;_=0) \
    for(auto it = (arr).ptr[it_index]; it_index < (arr).size; it = (arr).ptr[++it_index])

#define For_ptr(arr) \
for(int it_index = 0, _=1;_;_=0) \
    for(auto* it = &((arr).ptr[it_index]); it_index < (arr).size; it = &((arr).ptr[++it_index]))

#define For_rev(arr) \
for(int it_index = (arr).size - 1, _=1;_;_=0) \
    for(auto it = (arr).ptr[it_index]; it_index >= 0; it = (arr).ptr[--it_index])

#define For_ptr_rev(arr) \
for(int it_index = (arr).size - 1, _=1;_;_=0) \
    for(auto* it = &((arr).ptr[it_index]); it_index >= 0; it = &((arr).ptr[--it_index]))

#define For_rev_ptr(arr) For_ptr_rev((arr))

// A simple macro to write for(Range(10, 30)) instead of for(int it = 10; it < 30; it++), just for brevity
// TODO(cogno): what if you put an array inside another? "it" name conflict?
#define FOR_RANGE(min, max) s32 it = min; it < max; it++
#define Range(min, max) FOR_RANGE(min, max)
