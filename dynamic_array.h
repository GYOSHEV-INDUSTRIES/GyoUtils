template <typename T>
struct Array {
    u32 size;
    u32 reserved_size;
    T* ptr;
};

template<typename T>
Array<T> array_new(u32 size) {
    Array<T> array;
    array.reserved_size = size;
    array.size = 0;
    array.ptr = (T*)calloc(size, sizeof(T));
    return array;
}

template<typename T>
void array_resize(Array<T>* array, u32 new_size) {
    array->ptr = (T*)realloc(array->ptr, sizeof(*array->ptr) * new_size);
    array->reserved_size = new_size;
}

template<typename T>
void array_add(Array<T>* array, T data, s32 index) {
    ASSERT(index >= 0 && index < array->reserved_size * 2, "index out of range, index is %d, 2*reserved is %d\n", index, array->reserved_size * 2);
    
    //allocate if necessary
    if (array->size + 1 > array->reserved_size) {
        array_resize(array, array->reserved_size * 2);
    }
    
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
    u32 index = array->size;
    ASSERT(index >= 0 && index < array->reserved_size * 2, "index out of range, index is %d, 2*reserved is %d\n", index, array->reserved_size * 2);
    
    //allocate if necessary
    if (array->size + 1 > array->reserved_size) {
        array_resize(array, array->reserved_size * 2);
    }
    
    //add new data
    array->ptr[index] = data;
    array->size++;
}

template<typename T>
void array_remove_at(Array<T>* array, s32 index) {
    ASSERT(array->size > 0, "cannot remove an element of an empty array\n");
    ASSERT(index >= 0 && index < array->size, "index out of range, index is %d, buffer size is %d\n", index, array->size);
    
    //move every data from index to end back by 1
    for(s32 i = index; i < array->size - 1; i++) {
        ASSERT(i >= 0 && i + 1 < array->size, "out of memory access\n");
        array->ptr[i] = array->ptr[i + 1];
    }
    
    //then clear last space with zeros (to avoid keeping invalid memory)
    memset(array->ptr + array->size - 1, 0, sizeof(T));
    
    array->size--;
}

template<typename T>
T array_get_data(Array<T>* array, s32 index) {
    ASSERT(index >= 0 && index < array->size, "index out of range, index is %d, buffer size is %d\n", index, array->size);
    return array->ptr[index];
}

template<typename T>
T array_set(Array<T>* array, s32 index, s32 value) {
    ASSERT(index >= 0 && index < array->size, "index out of range, index is %d, buffer size is %d\n", index, array->size);
    array->ptr[index] = value;
}

template<typename T>
T* array_get_ptr(Array<T>* array, s32 index) {
    ASSERT(index >= 0 && index < array->size, "index out of range, index is %d, buffer size is %d\n", index, array->size);
    return &array->ptr[index];
}

template<typename T>
void array_clear(Array<T>* array) {
    array->size = 0;
}

template<typename T>
void array_free_all(Array<T>* array) {
    free(array->ptr);
    array->size = 0;
    array->reserved_size = 0;
}