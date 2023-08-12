template <typename T>
struct Array {
    s32 size;
    s32 reserved_size;
    T* ptr;
    T& operator[](s32 i) { ASSERT_BOUNDS(i, 0, size); return ptr[i]; }
};

template<typename T>
Array<T> array_new(s32 size) {
    Array<T> array;
    array.reserved_size = size;
    array.size = 0;
    array.ptr = (T*)calloc(size, sizeof(T));
    return array;
}

template<typename T>
void array_resize(Array<T>* array, s32 new_size) {
    array->ptr = (T*)realloc(array->ptr, sizeof(*array->ptr) * new_size);
    array->reserved_size = new_size;
}

template<typename T>
void array_add(Array<T>* array, T data, s32 index) {
    ASSERT(index >= 0 && index < array->reserved_size * 2, "index out of range, index is %, 2*reserved is %", index, array->reserved_size * 2);
    
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
    s32 index = array->size;
    ASSERT(index >= 0 && index < array->reserved_size * 2, "index out of range, index is %, 2*reserved is %", index, array->reserved_size * 2);
    
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
    ASSERT(array->size > 0, "cannot remove an element of an empty array");
    ASSERT(index >= 0 && index < array->size, "index out of range, index is %, buffer size is %", index, array->size);
    
    //move every data from index to end back by 1
    for(s32 i = index; i < array->size - 1; i++) {
        ASSERT(i >= 0 && i + 1 < array->size, "out of memory access");
        array->ptr[i] = array->ptr[i + 1];
    }
    
    //then clear last space with zeros (to avoid keeping invalid memory)
    memset(array->ptr + array->size - 1, 0, sizeof(T));
    
    array->size--;
}

template<typename T>
T array_get_data(Array<T>* array, s32 index) {
    ASSERT(index >= 0 && index < array->size, "index out of range, index is %, buffer size is %", index, array->size);
    return array->ptr[index];
}

template<typename T>
T array_set(Array<T>* array, s32 index, s32 value) {
    ASSERT(index >= 0 && index < array->size, "index out of range, index is %, buffer size is %", index, array->size);
    array->ptr[index] = value;
}

template<typename T>
T* array_get_ptr(Array<T>* array, s32 index) {
    ASSERT(index >= 0 && index < array->size, "index out of range, index is %, buffer size is %", index, array->size);
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


//macros for improved for cycle. only works with Array's and compatible structs (structs with 'size' and 'ptr'). Can return values (by-copy) and pointers (by-refs)
// usage: For(array) { code }
// TODO(cogno): what if the array is empty? out of bounds/undefined behaviour?
// TODO(cogno): what if you put an array inside another? "it" name conflict?
// BUG(cogno): For(str) gives a segfault after the first cycle, (so at array read?)
#define For(arr) for(struct {s32 index; decltype(arr.ptr[0]) value; } it = {0, arr.ptr[0]}; it.index < arr.size; it.index++, it.value = arr.ptr[it.index])
#define For_ptr(arr) for(struct {s32 index; decltype(arr.ptr) ptr; } it = {0, &arr.ptr[0]}; it.index < arr.size; it.index++, it.ptr = &arr.ptr[it.index])

// usage: for(Range(10, 30)) OR for(Range(50)) or stuff like this
// TODO(cogno): what if you put an array inside another? "it" name conflict?
#define FOR_RANGE(min, max) s32 it = min; it < max; it++
#define Range(min, max) FOR_RANGE(min, max)
