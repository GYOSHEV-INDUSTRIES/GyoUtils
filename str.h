#pragma once

/*
In this file:
- unicode utility functions
- str, a simple replacement to std::string, simply told, a ptr to char array + size, making them more useful in many situations.
- StrBuilder, a simple way to dynamically construct str (since str is an array of bytes you can use str_builder to also build binary files and many other things!)
- StrParser, a simple way to dynamically DEconstruct a str (since str is an array of bytes you can use str_parser to also parse binary files and many other things!)
*/

#ifndef DISABLE_INCLUDES
    #include <string.h> // for memcpy
#endif

#ifndef GYOFIRST
    #include "first.h"
#endif

//UNICODE UTILS
s8 unicode_utf8_to_size(u8 val) {
    if (val < 128) return 1;
    if (val < 224) return 2;
    if (val < 240) return 3;
    else           return 4;
}

s8 unicode_codepoint_to_size(u32 codepoint) {
    if (codepoint < 0x80)    return 1;
    if (codepoint < 0x800)   return 2;
    if (codepoint < 0x10000) return 3;
    else                     return 4;
}

u32 unicode_codepoint_to_utf8(u32 codepoint) {
    if (codepoint < 0x80)    return codepoint; //no need to convert
    if (codepoint < 0x800)   return (0x0000c080) + (codepoint & 0x3f) + ((codepoint << 2) & 0x1f00);
    if (codepoint < 0x10000) return (0x00e08080) + (codepoint & 0x3f) + ((codepoint << 2) & 0x3f00) + ((codepoint << 4) & 0x0f0000);
    else                     return (0xf0808080) + (codepoint & 0x3f) + ((codepoint << 2) & 0x3f00) + ((codepoint << 4) & 0x3f0000) + ((codepoint << 6) & 0x07000000);
}

bool unicode_is_header(u8 byte) {
    return (byte & 0xc0) != 0x80;
}

u32 unicode_utf8_to_codepoint(u8* utf8) {
    u8 utf8_head = utf8[0];
    s8 unicode_size = unicode_utf8_to_size(utf8_head);
    u32 codepoint_head = utf8_head & (0xff >> unicode_size);
    u32 codepoint_body = 0;
    for(int i = 1; i < unicode_size; i++) {
        u8 portion = utf8[i];
        u8 filtered = portion & 0x3f;
        codepoint_body = (codepoint_body << 6) + filtered;
    }
    u32 codepoint = (codepoint_head << (6 * (unicode_size - 1))) + codepoint_body;
    return codepoint;
}

// implemented manually to avoid the strlen dependency
int c_string_length(const char* s) {
    int len = 0;
    while(s[len++]) { }
    return len - 1;
}

// nice things to have but which we haven't used yet, we'll do these when we need. If you want these you can implement them and send the code to us!
//API(cogno): more unicode support (currently str kind of does not support it, I mean utf8 is just an array of bytes but these functions don't take it into account so they might be wrong, alternatively we can make 2 different strings, one with unicode and one without, it might make stuff a lot simpler, I'd say str and unicode_str)
//API(cogno): str substring
//API(cogno): str split all
//API(cogno): str parse to s32
//API(cogno): str parse to float/double
//API(cogno): str is s32, float, maybe even variants like s8, u8, s16, u16 ?
//API(cogno): str is alphanumeric (?)
//API(cogno): str is whitespace (?)

struct str{
    u8* ptr;
    s32 size;
    
    // conversion constructor from const char* to str (so you can do 'str s = "a string";')
    str(const char* c) {
        ptr = (u8*)c;
        size = c_string_length(c);
    }
    
    str(u8* p, int len) { //NOTE(cogno): c++ is shit so we need to define this to do "str{ptr, len};"
        ptr = p;
        size = len;
    }
    
    str(u8* p) { // when you have the ptr but not the size we calculate it
        ptr = p;
        for(size = 0; p[size]; size++);
    }
    
    str() = default; //NOTE(cogno): c++ is shit so we need to define this to do "str{};"

    u8& operator[](s32 i) { ASSERT_BOUNDS(i, 0, size); return ptr[i]; }
};

// NOTE(cogno): you can directly cast a const char* to a str (so you can do str name = "YourName"; and it will work)
inline void printsl_custom(str v) { for(int i = 0; i < v.size; i++) printsl_custom((char)v.ptr[i]); }

const char* str_to_c_string(str to_convert, Allocator alloc) {
    ASSERT(to_convert.size != MAX_U32, "str is full, cannot convert to c str");
    u32 c_size = to_convert.size + 1;
    char* ptr = (char*)mem_alloc(alloc, c_size);
    memcpy(ptr, to_convert.ptr, to_convert.size);
    ptr[to_convert.size] = 0;
    return (const char*)ptr;
}
const char* str_to_c_string(str to_convert) { return str_to_c_string(to_convert, default_allocator); }

str str_concat(str s1, str s2, Allocator alloc) {
    str total;
    total.ptr = (u8*)mem_alloc(alloc, s1.size + s2.size);
    total.size = s1.size + s2.size;
    
    memcpy(total.ptr, s1.ptr, s1.size);
    memcpy(total.ptr + s1.size, s2.ptr, s2.size);
    
    return total;
}
str str_concat(str s1, str s2) { return str_concat(s1, s2, default_allocator); }

// copies a string allocating into a given allocator
str str_copy(str to_copy, Allocator alloc) {
    str copy;
    copy.ptr = (u8*)mem_alloc(alloc, to_copy.size);
    copy.size = to_copy.size;
    memcpy(copy.ptr, to_copy.ptr, to_copy.size);
    return copy;
}
str str_copy(str to_copy) { return str_copy(to_copy, default_allocator); }

// splits a single str in 2 parts on the first occurrence of a char, no allocations necessary.
// the character split is NOT included in the final strings, 
// put the original string in to_split and empty ones in left_side and right_side.
// if the character is found the function returns true and fills left_side and right_side,
// if the character is NOT found false is returned, left_side will contain the full string and
// right_side will be empty
bool str_split_left(str to_split, u8 char_to_split, str* left_side, str* right_side) {
    for(int i = 0; i < to_split.size; i++) {
        if(to_split[i] == char_to_split) {
            left_side->ptr = to_split.ptr;
            left_side->size = i;
            right_side->ptr  = to_split.ptr  + (i + 1); //remember, we skip the character
            right_side->size = to_split.size - (i + 1); //remember, we skip the character
            return true;
        }
    }
    left_side->ptr = to_split.ptr;
    left_side->size = to_split.size;
    return false;
}

//version of split left that splits an entire str instead of a single char
//the string to split is NOT included in the final strings
bool str_split_left(str to_split, str splitter, str* left_side, str* right_side) {
    for(int original_index = 0; original_index < to_split.size; original_index++) {
        bool matches = true;
        for(int to_split_index = 0; to_split_index < splitter.size; to_split_index++) {
            if(original_index + to_split_index >= to_split.size) return false; //string finished, couldn't find anything
            
            ASSERT(original_index + to_split_index < to_split.size, "reading outside memory");
            ASSERT(to_split_index < to_split.size, "reading outside memory");
            
            if(to_split[original_index + to_split_index] != to_split[to_split_index]) {
                matches = false;
                break;
            }
        }
        
        if (matches) {
            left_side->ptr = to_split.ptr;
            left_side->size = original_index;
            right_side->ptr  = to_split.ptr  + (original_index + to_split.size); //remember, we skip the str
            right_side->size = to_split.size - (original_index + to_split.size); //remember, we skip the str
            return true;
        }
    }
    
    left_side->ptr = to_split.ptr;
    left_side->size = to_split.size;
    return false;
}

//splits on newline (\n) and removes \r if found (fuck \r\n, fuck windows)
bool str_split_newline_left(str to_split, str* left_side, str* right_side) {
    for(int i = 0; i < to_split.size; i++) {
        if(to_split[i] == '\n') {
            left_side->ptr = to_split.ptr;
            left_side->size = i;
            right_side->ptr  = to_split.ptr  + (i + 1);
            right_side->size = to_split.size - (i + 1);
            
            if(left_side->size > 0 && left_side->ptr[left_side->size - 1] == '\r') left_side->size--;
            return true;
        }
    }
    left_side->ptr = to_split.ptr;
    left_side->size = to_split.size;
    return false;
}

bool str_split_right(str to_split, u8 char_to_split, str* left_side, str* right_side) {
    for(int i = to_split.size - 1; i >= 0; i--) {
        if(to_split[i] == char_to_split) {
            left_side->ptr = to_split.ptr;
            left_side->size = i;
            right_side->ptr  = to_split.ptr  + (i + 1); //remember, we skip the character
            right_side->size = to_split.size - (i + 1); //remember, we skip the character
            return true;
        }
    }
    left_side->ptr = to_split.ptr;
    left_side->size = to_split.size;
    return false;
}

void str_trim_left_inplace(str* to_trim) {
    //API(cogno): I don't think space and \t are enough...
    while(true) {
        if(to_trim->size <= 0) return; // nothing left to trim
        if(to_trim->ptr[0] == ' ' || to_trim->ptr[0] == '\t') {
            to_trim->ptr++;
            to_trim->size--;
        } else break;
    }
}

void str_trim_right_inplace(str* to_trim) {
    //API(cogno): I don't think space and \t are enough...
    while(true) {
        if(to_trim->size <= 0) return; // nothing left to trim
        if(to_trim->ptr[to_trim->size - 1] == ' ' || to_trim->ptr[to_trim->size - 1] == '\t') {
            to_trim->size--;
        } else break;
    }
}

void str_trim_inplace(str* to_trim) {
    str_trim_left_inplace(to_trim);
    str_trim_right_inplace(to_trim);
}

str str_trim_left(str to_trim) {
    //API(cogno): I don't think space and \t are enough...
    str out = to_trim;
    while(true) {
        if(out.size <= 0) return out; // nothing left to trim
        if(out.ptr[0] == ' ' || out.ptr[0] == '\t') {
            out.ptr++;
            out.size--;
        } else break;
    }
    return out;
}

str str_trim_right(str to_trim) {
    //API(cogno): I don't think space and \t are enough...
    str out = to_trim;
    while(true) {
        if(out.size <= 0) return out; // nothing left to trim
        if(out.ptr[out.size - 1] == ' ' || out.ptr[out.size - 1] == '\t') {
            out.size--;
        } else break;
    }
    return out;
}

str str_trim(str to_trim) {
    str trim1 = str_trim_left(to_trim);
    str trim2 = str_trim_right(trim1);
    return trim2;
}

bool str_to_u32(str to_convert, u32* out_value) {
    for(int i = 0; i < to_convert.size; i++) {
        char ch = to_convert[i];
        if(ch > '9' || ch < '0') return false;
        *out_value = (*out_value) * 10 + (ch - '0');
    }
    return true;
}

//variant of the above without error checking
u32 str_to_u32(str to_convert) {
    u32 out_value = 0;
    for(int i = 0; i < to_convert.size; i++) {
        char ch = to_convert[i];
        out_value = out_value * 10 + (ch - '0');
    }
    return out_value;
}

bool str_starts_with(str to_check, char ch) {
    return to_check.size > 0 && to_check[0] == ch;
}

bool str_starts_with(str to_check, str checker) {
    if (to_check.size < checker.size) return false; //not enough character to check
    
    for(int i = 0; i < checker.size; i++) {
        if(to_check[i] != checker[i]) return false;
    }
    
    return true;
}

bool str_ends_with(str to_check, char ch) {
    return to_check.size > 0 && to_check[to_check.size - 1] == ch;
}

bool str_ends_with(str to_check, str checker) {
    if(to_check.size < checker.size) return false; // not enough characters
    
    for(int i = 0; i < checker.size; i++) {
        s32 index = to_check.size - checker.size + i;
        if(to_check[index] != checker[i]) return false;
    }
    
    return true;
}

// counts occurrencies of a character in the given string
int str_count(str to_check, char to_count) {
    int the_count = 0;
    For(to_check) {
        if(it == to_count) the_count++;
    }
    return the_count;
}

// supports unicode utf8
u32 str_length_in_char(str string) {
    u32 char_count = 0;
    int read_index = 0;
    while(true) {
        if (read_index >= string.size) return char_count;
        ASSERT(read_index < string.size, "reading out of memory");
        u8 unicode_header = string[read_index];
        u8 unicode_size = unicode_utf8_to_size(unicode_header);
        read_index += unicode_size;
        char_count++;
    }
}

bool str_is_u32(str to_check) {
    if (to_check.size <= 0) return false;
    for(int i = 0; i < to_check.size; i++) {
        u8 ch = to_check[i];
        if(ch > '9' || ch < '0') return false;
    }
    return true;
}

bool str_matches(str a, str b) {
    if(a.size != b.size) return false;
    for(int i = 0; i < a.size; i++) {
        u8 a1 = a[i];
        u8 b1 = b[i];
        if(a1 != b1) return false;
    }
    return true;
}

/*
StrBuilder, used to dinamically construct str.
Since str is an array of bytes you can also use this to construct binary data (like files)
*/

#define STR_BUILDER_DEFAULT_SIZE 100

// API(cogno): make this work automatically if make_str_builder is not called.
struct StrBuilder {
    u8* ptr;
    s32 size;
    s32 reserved_size;
    Allocator alloc;
    u8& operator[](s32 i) { ASSERT_BOUNDS(i, 0, size); return ptr[i]; }
};

// TODO(cogno): make StrBuilder usable when zero initialized
// TODO(cogno): make StrParser usable when zero initialized
inline void printsl_custom(StrBuilder b) { for(int i = 0; i < b.size; i++) printsl_custom((char)b.ptr[i]); }

StrBuilder make_str_builder(s32 size, Allocator alloc) {
    StrBuilder s = {};
    ASSERT(alloc.handle != NULL, "StrBuilder invalid allocator!");
    s.alloc = alloc;
    s.size = 0;
    s.reserved_size = size;
    s.ptr = (u8*)s.alloc.handle(AllocOp::ALLOC, s.alloc.data, 0, size * sizeof(u8), NULL);
    return s;
}

StrBuilder make_str_builder() { return make_str_builder(STR_BUILDER_DEFAULT_SIZE, default_allocator); }
StrBuilder make_str_builder(s32 size) { return make_str_builder(size, default_allocator); }

void str_builder_free(StrBuilder* b) {
    b->alloc.handle(AllocOp::FREE, b->alloc.data, 0, 0, b->ptr);
    b->size = b->reserved_size = 0;
}

void str_builder_clear(StrBuilder* b) {
    b->size = 0;
}

StrBuilder str_builder_copy(StrBuilder* b) {
    StrBuilder copy;
    copy.ptr = (u8*)b->alloc.handle(AllocOp::ALLOC, b->alloc.data, 0, b->reserved_size * sizeof(u8), NULL);
    copy.size = b->size;
    copy.reserved_size = b->reserved_size;
    memcpy(copy.ptr, b->ptr, b->size);
    return copy;
}

str str_builder_get_str(StrBuilder* b) {
    str s = {};
    s.ptr = b->ptr;
    s.size = b->size;
    return s;
}

void str_builder_resize(StrBuilder* b, s32 min_size) {
    u8 old_start = b->ptr[0];
    s32 new_size = b->reserved_size * 2;
    new_size = new_size >= STR_BUILDER_DEFAULT_SIZE ? new_size : STR_BUILDER_DEFAULT_SIZE; // API(cogno): 'max' identifier not found error
    new_size = new_size >= min_size ? new_size : min_size; // API(cogno): 'max' identifier not found error
    b->ptr = (u8*)b->alloc.handle(AllocOp::REALLOC, b->alloc.data, b->reserved_size * sizeof(u8), new_size * sizeof(u8), b->ptr);
    b->reserved_size = new_size;
    ASSERT(b->ptr[0] == old_start, "ERROR ON REALLOC, initial byte unexpectedly changed, this is not supposed to happen...");
}

void str_builder_reserve(StrBuilder* b, s32 to_reserve) {
    if(b->reserved_size - b->size >= to_reserve) return; // there's already enough space
    str_builder_resize(b, b->size + to_reserve);
}

void str_builder_append(StrBuilder* b, str to_append) {
    s32 new_size = b->size + to_append.size;
    if(new_size > b->reserved_size) str_builder_resize(b, new_size);
    ASSERT(b->reserved_size >= new_size, "not enough memory allocated, wanted % but allocated %", new_size, b->reserved_size);
    memcpy(b->ptr + b->size, to_append.ptr, to_append.size);
    b->size = new_size;
}

// NOTE(cogno): since we're making a **string** builder, everything gets converted to a string representation (for example a bool becomes the string "true" or "false", the number 12 becomes the string "12" etc.). If you need to append without conversion you should use str_builder_append_raw()

void str_builder_append(StrBuilder* b, const char* to_append) {
    str converted = to_append;
    str_builder_append(b, converted);
}

void str_builder_append(StrBuilder* b, bool boolean) {
    // minor optimization where we immediately create the string with the proper size since we know what they are
    str to_append;
    if (boolean) {
        to_append.ptr = (u8*)"true";
        to_append.size = 4;
    } else {
        to_append.ptr = (u8*)"false";
        to_append.size = 5;
    }
    str_builder_append(b, to_append);
}

void str_builder_append(StrBuilder* b, char c) {
    str_builder_reserve(b, 1);
    b->ptr[b->size++] = c;
}

void str_builder_append(StrBuilder* b, u8 to_append) {
    char buff[4];
    snprintf(buff, 4, "%u", to_append);
    str converted = buff;
    str_builder_append(b, converted);
}

void str_builder_append(StrBuilder* b, u16 to_append) {
    char buff[6];
    snprintf(buff, 6, "%u", to_append);
    str converted = buff;
    str_builder_append(b, converted);
}

void str_builder_append(StrBuilder* b, u32 to_append) {
    char buff[11];
    snprintf(buff, 11, "%lu", to_append);
    str converted = buff;
    str_builder_append(b, converted);
}

void str_builder_append(StrBuilder* b, u64 to_append) {
    char buff[21];
    snprintf(buff, 21, "%llu", to_append);
    str converted = buff;
    str_builder_append(b, converted);
}

void str_builder_append(StrBuilder* b, s8 to_append) {
    char buff[5];
    snprintf(buff, 5, "%d", to_append);
    str converted = buff;
    str_builder_append(b, converted);
}

void str_builder_append(StrBuilder* b, s16 to_append) {
    char buff[8];
    snprintf(buff, 8, "%d", to_append);
    str converted = buff;
    str_builder_append(b, converted);
}

void str_builder_append(StrBuilder* b, s32 to_append) {
    char buff[15];
    snprintf(buff, 15, "%ld", to_append);
    str converted = buff;
    str_builder_append(b, converted);
}

void str_builder_append(StrBuilder* b, s64 to_append) {
    char buff[25];
    snprintf(buff, 25, "%lld", to_append);
    str converted = buff;
    str_builder_append(b, converted);
}

void str_builder_append(StrBuilder* b, f32 to_append) {
    char buff[50];
    snprintf(buff, 50, "%.5f", to_append);
    str converted = buff;
    str_builder_append(b, converted);
}

void str_builder_append(StrBuilder* b, f64 to_append) {
    char buff[50];
    snprintf(buff, 50, "%.5f", to_append);
    str converted = buff;
    str_builder_append(b, converted);
}

void str_builder_append_hex(StrBuilder* b, u64 to_append) {
    char buff[20];
    snprintf(buff, 20, "%04X_%04X_%04X_%04X", (u32)(to_append >> 48) & 0xffff, (u32)(to_append >> 32) & 0xffff, (u32)(to_append >> 16) & 0xffff, (u32)to_append & 0xffff);
    str converted = buff;
    str_builder_append(b, converted);
}

void str_builder_append_hex(StrBuilder* b, u32 to_append) {
    char buff[12];
    snprintf(buff, 12, "%04X_%04X", (to_append >> 16) & 0xffff, to_append & 0xffff);
    str converted = buff;
    str_builder_append(b, converted);
}

// converts the ptr to hex text, it will NOT read the contents of the ptr itself
template<typename T> 
void str_builder_append_ptr(StrBuilder* b, T* to_append) {
    union Temp {
        T* ptr;
        u64 u64;
    } t; // NOTE(cogno): if we cast T* to u64/u32 we might get a warning (which some projects turn into an error), so we use an union
    t.ptr = to_append;
    char buff[20];
    snprintf(buff, 20, "%04X_%04X_%04X", (u32)(t.u64 >> 32) & 0xffff, (u32)(t.u64 >> 16) & 0xffff, (u32)t.u64 & 0xffff);
    str converted = buff;
    str_builder_append(b, converted);
}

// custom str_builder variants, you can add yours too!
#ifdef GYOMATH
void str_builder_append(StrBuilder* b, vec2 to_append) {
    str_builder_append(b, '(');
    str_builder_append(b, to_append.x);
    str_builder_append(b, ',');
    str_builder_append(b, to_append.y);
    str_builder_append(b, ')');
}

void str_builder_append(StrBuilder* b, vec3 to_append) {
    str_builder_append(b, '(');
    str_builder_append(b, to_append.x);
    str_builder_append(b, ',');
    str_builder_append(b, to_append.y);
    str_builder_append(b, ',');
    str_builder_append(b, to_append.z);
    str_builder_append(b, ')');
}

void str_builder_append(StrBuilder* b, vec4 to_append) {
    str_builder_append(b, '(');
    str_builder_append(b, to_append.x);
    str_builder_append(b, ',');
    str_builder_append(b, to_append.y);
    str_builder_append(b, ',');
    str_builder_append(b, to_append.z);
    str_builder_append(b, ',');
    str_builder_append(b, to_append.w);
    str_builder_append(b, ')');
}

//API(cogno): str_builder append mat4, col, etc.
#endif

// NOTE(cogno): all append_raw are little-endian

void str_builder_append_raw(StrBuilder* b, u8* pointer_to_data, s32 data_size) {
    str_builder_reserve(b, data_size);
    ASSERT(b->size + data_size <= b->reserved_size, "out of memory after a reserve??");
    for(int i = 0; i < data_size; i++) {
        b->ptr[b->size + i] = pointer_to_data[i];
    }
    b->size += data_size;
}

void str_builder_append_raw(StrBuilder* b, u8 to_add) {
    str_builder_reserve(b, 1);
    ASSERT(b->size + 1 <= b->reserved_size, "out of memory after a reserve??");
    b->ptr[b->size++] = to_add;
}

void str_builder_append_raw(StrBuilder* b, bool to_add) {
    str_builder_append_raw(b, (u8)to_add);
}

void str_builder_append_raw(StrBuilder* b, u64 to_add) { str_builder_append_raw(b, (u8*)(&to_add), sizeof(to_add)); }
void str_builder_append_raw(StrBuilder* b, u32 to_add) { str_builder_append_raw(b, (u8*)(&to_add), sizeof(to_add)); }
void str_builder_append_raw(StrBuilder* b, u16 to_add) { str_builder_append_raw(b, (u8*)(&to_add), sizeof(to_add)); }
void str_builder_append_raw(StrBuilder* b, s64 to_add) { str_builder_append_raw(b, (u8*)(&to_add), sizeof(to_add)); }
void str_builder_append_raw(StrBuilder* b, s32 to_add) { str_builder_append_raw(b, (u8*)(&to_add), sizeof(to_add)); }
void str_builder_append_raw(StrBuilder* b, s16 to_add) { str_builder_append_raw(b, (u8*)(&to_add), sizeof(to_add)); }
void str_builder_append_raw(StrBuilder* b, f32 to_add) { str_builder_append_raw(b, (u8*)(&to_add), sizeof(to_add)); }
void str_builder_append_raw(StrBuilder* b, f64 to_add) { str_builder_append_raw(b, (u8*)(&to_add), sizeof(to_add)); }

// custom str_builder variants, you can add yours too!
#ifdef GYOMATH
void str_builder_append_raw(StrBuilder* b, vec2 to_add) {
    str_builder_append_raw(b, to_add.x);
    str_builder_append_raw(b, to_add.y);
}
void str_builder_append_raw(StrBuilder* b, vec3 to_add) {
    str_builder_append_raw(b, to_add.x);
    str_builder_append_raw(b, to_add.y);
    str_builder_append_raw(b, to_add.z);
}
void str_builder_append_raw(StrBuilder* b, vec4 to_add) {
    str_builder_append_raw(b, to_add.x);
    str_builder_append_raw(b, to_add.y);
    str_builder_append_raw(b, to_add.z);
    str_builder_append_raw(b, to_add.w);
}
// API(cogno): str builder append raw mat4, col etc.
#endif

// API(cogno): string builder insert at index
// API(cogno): string builder replace

void str_builder_remove_last_bytes(StrBuilder* b, s32 bytes_to_remove) { b->size -= bytes_to_remove; }

// counts how many bytes are right of the last <to_find> character in the string,
// returns -1 if <to_find> is not found
int str_builder_count_right(StrBuilder* b, u8 to_find) {
    for(int i = b->size - 1; i >= 0; i--) {
        if(b->ptr[i] == to_find) return b->size - i - 1;
    }
    return -1;
}

void str_builder_remove_right(StrBuilder* b, u8 to_find) {
    auto size_found = str_builder_count_right(b, to_find);
    if (size_found > 0) str_builder_remove_last_bytes(b, size_found);
}

/*
StrParser, used to dinamically deconstruct str.
Since str is an array of bytes you can also use this to parse binary data (like files)
*/

struct StrParser {
    u8* ptr;
    s32 size;
    u8& operator[](s32 i) { ASSERT_BOUNDS(i, 0, size); return ptr[i]; }
};

inline void printsl_custom(StrParser p) { printsl_custom(str(p.ptr, p.size)); }

StrParser make_str_parser(str s) {
    StrParser p = {};
    p.size = s.size;
    p.ptr = s.ptr;
    return p;
}

StrParser make_str_parser(u8* ptr, s32 size) {
    StrParser p = {};
    p.size = size;
    p.ptr = ptr;
    return p;
}

StrParser copy_str_parser(StrParser to_copy) {
    StrParser p = {};
    p.size = to_copy.size;
    p.ptr = to_copy.ptr;
    return p;
}

str str_parser_to_str(StrParser p) {
    str out = {};
    out.ptr = p.ptr;
    out.size = p.size;
    return out;
}

bool str_parser_is_empty(StrParser* p) { return p->size == 0; }

void str_parser_advance(StrParser* p, s32 size) {
    if(!ASSERT(size <= p->size, "advancing by too much! the string is % long, but you're advancing by %", p->size, size)) return;
    p->ptr  += size;
    p->size -= size;
}

bool str_parser_starts_with(StrParser* p, str start) {
    if(start.size > p->size) return false;
    
    for(int i = 0; i < start.size; i++) {
        char ch_p = p->ptr[i];
        char ch_s = start[i];
        if(ch_p != ch_s) return false;
    }
    
    return true;
}

bool str_parser_check_magic(StrParser* p, str magic) {
    ASSERT(magic.size == 4, "magic should only be 4 characters long!");
    bool magic_correct = str_parser_starts_with(p, magic);
    str_parser_advance(p, 4);
    return magic_correct;
}

// get functions return raw bytes as types
template<typename T>
T str_parser_get(StrParser* p) {
    T out = *(T*)p->ptr;
    str_parser_advance(p, sizeof(T));
    return out;
}

bool str_parser_starts_with_digit(StrParser* p) {
    return p->ptr[0] >= '0' && p->ptr[0] <= '9';
}

// NOTE(cogno): each _parse function returns a boolean if it was parsed correctly and the given pointer with the parsed value

bool str_parser_parse_bool(StrParser* p, bool* out) {
    str to_check = str_parser_to_str(*p);
    if(str_starts_with(to_check, "true")) {
        str_parser_advance(p, 4);
        *out = true;
        return true;
    }
    if(str_starts_with(to_check, "false")) {
        str_parser_advance(p, 5);
        *out = false;
        return true;
    }
    return false;
}

bool str_parser_parse_u8(StrParser* p, u8* out) {
    if(!str_parser_starts_with_digit(p)) return false;
    
    char start = str_parser_get<char>(p);
    *out = start - '0';
    for(int i = 1; i < 3; i++) { // u8 have at most 3 digits (value 255)
        if(!str_parser_starts_with_digit(p)) return true; //we no longer have portions of the number, but we previously found some, so we're done successfully
        
        char ch = str_parser_get<char>(p);
        *out = (*out * 10) + ch - '0'; // add the new digit to the mix
    }
    return true;
}

bool str_parser_parse_u16(StrParser* p, u16* out) {
    if(!str_parser_starts_with_digit(p)) return false;
    
    char start = str_parser_get<char>(p);
    *out = start - '0';
    for(int i = 1; i < 5; i++) { // u16 have at most 5 digits (value 65535)
        if(!str_parser_starts_with_digit(p)) return true; //we no longer have portions of the number, but we previously found some, so we're done successfully

        char ch = str_parser_get<char>(p);
        *out = (*out * 10) + ch - '0'; // add the new digit to the mix
    }
    return true;
}

bool str_parser_parse_u32(StrParser* p, u32* out) {
    if(!str_parser_starts_with_digit(p)) return false;
    
    char start = str_parser_get<char>(p);
    *out = start - '0';
    for(int i = 1; i < 10; i++) { // u32 have at most 10 digits (value 4294967295)
        if(!str_parser_starts_with_digit(p)) return true; //we no longer have portions of the number, but we previously found some, so we're done successfully

        char ch = str_parser_get<char>(p);
        *out = (*out * 10) + ch - '0'; // add the new digit to the mix
    }
    return true;
}

bool str_parser_parse_u64(StrParser* p, u64* out) {
    if(!str_parser_starts_with_digit(p)) return false;
    
    char start = str_parser_get<char>(p);
    *out = start - '0';
    for(int i = 1; i < 20; i++) { // u64 have at most 20 digits (value 18446744073709551615)
        if(!str_parser_starts_with_digit(p)) return true; //we no longer have portions of the number, but we previously found some, so we're done successfully

        char ch = str_parser_get<char>(p);
        *out = (*out * 10) + ch - '0'; // add the new digit to the mix
    }
    return true;
}

inline bool operator ==(str a, str b) {return str_matches(a,b);}


// parse functions convert str to types and return them
// API(cogno): parse s8
// API(cogno): parse s16
// API(cogno): parse s32
// API(cogno): parse s64
// API(cogno): parse f32
// API(cogno): parse f64


