/*
In this file:
- FixedArray, an array of fixed size with bounds checking
*/

#pragma once


#ifndef GYOFIRST
    #include "first.h"
#endif

// NOTE(cogno): THIS IS NOT A BUFFER, if you give it a space of 100 but you only put 10 elements, reading at index 20 WILL explode
template <typename T>
struct FixedArray {
    s32 size = 0;
    s32 reserved_size = 0;
    T* ptr = NULL;
    T& operator[](s32 i) { ASSERT_BOUNDS(i, 0, size); return ptr[i]; }
};

template<typename T>
FixedArray<T> fixed_array_new(s32 size) {
    ASSERT(size >= 0, "cannot create array with negative size %", size);
    FixedArray<T> array;
    array.reserved_size = size;
    array.size = 0;
    array.ptr = (T*)calloc(size, sizeof(T));
    return array;
}

template<typename T>
void fixed_array_free(FixedArray<T>* array) {
    free(array->ptr);
    array->size = 0;
    array->reserved_size = 0;
    array->ptr = NULL;
}

template<typename T>
void fixed_array_clear(FixedArray<T>* array) {
    array->size = 0;
    // API(cogno): maybe we should add a slow path that sets all to 0? maybe always do it? maybe provide 2 options? I don't know...
}

template<typename T>
void fixed_array_insert(FixedArray<T>* array, T data, s32 index) {
    MUST_ASSERT_BOUNDS(index, 0, array->size + 1);
    
    //move every data from index to end forward by 1
    for(s32 i = array->size; i > index; i--) {
        array->ptr[i] = array->ptr[i-1];
    }
    
    //add new data
    array->ptr[index] = data;
    array->size++;
}

template<typename T>
void fixed_array_append(FixedArray<T>* array, T data) {
    array->ptr[array->size++] = data;
}

template<typename T>
void fixed_array_remove_at(FixedArray<T>* array, s32 index) {
    MUST_ASSERT_BOUNDS(index, 0, array->size);
    
    //move every data from index to end back by 1
    for(s32 i = index; i < array->size - 1; i++) {
        MUST_ASSERT(i >= 0 && i + 1 < array->size, "out of memory access");
        array->ptr[i] = array->ptr[i + 1];
    }
    
    //then clear last space with zeros (to avoid keeping invalid memory)
    memset(array->ptr + array->size - 1, 0, sizeof(T));
    array->size--;
}

// so you can use this as a stack (push=append)
template<typename T>
T fixed_array_pop(FixedArray<T>* array) {
    MUST_ASSERT(array->size > 0, "cannot pop from an empty stack/array");
    T element = array->ptr[array->size - 1];
    fixed_array_remove_at(array, array->size - 1);
    return element;
}

// so you can use this as a queue (queue=append)
template<typename T>
T fixed_array_dequeue(FixedArray<T>* array) {
    MUST_ASSERT(array->size > 0, "cannot dequeue from an empty queue/array");
    T element = array->ptr[0];
    fixed_array_remove_at(array, 0);
    return element;
}

// NOTE(cogno): most of the times you can simply use the operator overload, so doing array[0] = 10; or auto temp = array[15];, but if you have the pointer to the array (instead of the array) then the operator overload will not work (because you'll be accessing the pointer!) so these functions can be used instead (or you can take a reference to the array instead of a pointer)
template<typename T>
T fixed_array_get_data(FixedArray<T>* array, s32 index) {
    MUST_ASSERT_BOUNDS(index, 0, array->size);
    return array->ptr[index];
}

template<typename T>
T fixed_array_set(FixedArray<T>* array, s32 index, s32 value) {
    MUST_ASSERT_BOUNDS(index, 0, array->size);
    array->ptr[index] = value;
}

template<typename T>
T* fixed_array_get_ptr(FixedArray<T>* array, s32 index) {
    MUST_ASSERT_BOUNDS(index, 0, array->size);
    return &array->ptr[index];
}
