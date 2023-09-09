//UNICODE UTILS
u8 unicode_utf8_to_size(u8 val) {
    if (val < 128) return 1;
    if (val < 224) return 2;
    if (val < 240) return 3;
    else           return 4;
}

u8 unicode_codepoint_to_size(u32 codepoint) {
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

//TODO(cogno): utf8 to codepoint (u32 to u32)
//TODO(cogno): overload utf8 to codepoint (array of u8 to u32)

// implemented manually to avoid the strlen dependency
int c_string_length(const char* s) {
    int len = 0;
    while(s[len++]) { }
    return len - 1;
}

//TODO(cogno): test all of this file

//TODO(cogno): more unicode support (currently str kind of does not support it, I mean utf8 is just an array of bytes but these functions don't take it into account so they might be wrong, alternatively we can make 2 different strings, one with unicode and one without, it might make stuff a lot simpler, I'd say str and unicode_str)
//TODO(cogno): str builder
//TODO(cogno): str concat
//TODO(cogno): str matches
//TODO(cogno): str substring
//TODO(cogno): str split all
//TODO(cogno): str parse to s32
//TODO(cogno): str parse to float/double
//TODO(cogno): str is s32, float, maybe even variants like s8, u8, s16, u16 ?
//TODO(cogno): str is alphanumeric (?)
//TODO(cogno): str is whitespace (?)

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
        for(size = 0; p[size]; size++); //TODO(cogno): isn't this too much by 1??
    }
    
    str() = default; //NOTE(cogno): c++ is shit so we need to define this to do "str{};"

    u8& operator[](s32 i) { ASSERT_BOUNDS(i, 0, size); return ptr[i]; }
};

// makes a new string from a c_str allocating a new buffer for it
str str_new_alloc(const char* c_str) {
    str new_str = {};
    new_str.size = c_string_length(c_str);
    new_str.ptr = (u8*)calloc(new_str.size, sizeof(u8));
    memcpy(new_str.ptr, c_str, sizeof(u8) * new_str.size);
    return new_str;
}

inline void printsl(str v) { for(int i = 0; i < v.size; i++) putchar(v.ptr[i]); }

char* str_to_c_string(str to_convert) {
    ASSERT(to_convert.size != MAX_U32, "str is full, cannot convert to c str");
    u32 c_size = to_convert.size + 1;
    char* ptr = (char*)malloc(c_size);
    memcpy(ptr, to_convert.ptr, to_convert.size);
    ptr[to_convert.size] = 0;
    return ptr;
}

str str_concat(str s1, str s2) {
    str total;
    total.ptr = (u8*)malloc(s1.size + s2.size);
    total.size = s1.size + s2.size;
    
    memcpy(total.ptr, s1.ptr, s1.size);
    memcpy(total.ptr + s1.size, s2.ptr, s2.size);
    
    return total;
}
str str_concat(const char* s1, const char* s2) { return str_concat((str)s1, (str)s2); }
str str_concat(str s1, const char* s2) { return str_concat(s1, (str)s2); }
str str_concat(const char* s1, str s2) { return str_concat((str)s1, s2); }

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

void str_trim_left(str* to_trim) {
    //API(cogno): I don't think space and \t are enough...
    while(true) {
        if(to_trim->size <= 0) return; // nothing left to trim
        if(to_trim->ptr[0] == ' ' || to_trim->ptr[0] == '\t') {
            to_trim->ptr++;
            to_trim->size--;
        } else break;
    }
}

void str_trim_right(str* to_trim) {
    //API(cogno): I don't think space and \t are enough...
    while(true) {
        if(to_trim->size <= 0) return; // nothing left to trim
        if(to_trim->ptr[to_trim->size - 1] == ' ' || to_trim->ptr[to_trim->size - 1] == '\t') {
            to_trim->size--;
        } else break;
    }
}

void str_trim(str* to_trim) {
    str_trim_left(to_trim);
    str_trim_right(to_trim);
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
        ASSERT_BOUNDS(i, 0, to_check.size); //should never happen because of check above
        ASSERT_BOUNDS(i, 0, checker.size);  //should never happen because of check above
        if(to_check[i] != checker[i]) return false;
    }
    
    return true;
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









#define STR_BUILDER_DEFAULT_SIZE 100

struct StringBuilder {
    u8* data;
    s32 size;
    s32 reserved_size;
};

StringBuilder make_string_builder(u8* ptr, s32 size) {
    StringBuilder s = {};
    s.data = ptr;
    s.reserved_size = size;
    return s;
}

StringBuilder make_string_builder() {
    auto* data = (u8*)malloc(STR_BUILDER_DEFAULT_SIZE); // default at 100 bytes
    return make_string_builder(data, STR_BUILDER_DEFAULT_SIZE);
}

StringBuilder make_string_builder(s32 size) {
    auto* data = (u8*)malloc(size);
    return make_string_builder(data, size);
}

str string_builder_get_str(StringBuilder* b) {
    str s = {};
    s.ptr = b->data;
    s.size = b->size;
    return s;
}

void string_builder_resize(StringBuilder* b) {
    u8 old_start = b->data[0];
    s32 new_size = b->reserved_size * 2;
    new_size = new_size >= STR_BUILDER_DEFAULT_SIZE ? new_size : STR_BUILDER_DEFAULT_SIZE; // API(cogno): 'max' identifier not found error
    b->data = (u8*)realloc(b->data, new_size);
    ASSERT(b->data[0] == old_start, "ERROR ON REALLOC, initial byte unexpectedly change, Undefined Behaviour prevented");
}

void string_builder_resize(StringBuilder* b, s32 min_size) {
    u8 old_start = b->data[0];
    s32 new_size = b->reserved_size * 2;
    new_size = new_size >= STR_BUILDER_DEFAULT_SIZE ? new_size : STR_BUILDER_DEFAULT_SIZE; // API(cogno): 'max' identifier not found error
    new_size = new_size >= min_size ? new_size : min_size; // API(cogno): 'max' identifier not found error
    b->data = (u8*)realloc(b->data, new_size);
    ASSERT(b->data[0] == old_start, "ERROR ON REALLOC, initial byte unexpectedly change, Undefined Behaviour prevented");
}

void string_builder_reserve(StringBuilder* b, s32 to_reserve) {
    if(b->reserved_size - b->size >= to_reserve) return; // there's already enough space
    string_builder_resize(b, b->size + to_reserve);
}

void string_builder_append(StringBuilder* b, str to_append) {
    s32 new_size = b->size + to_append.size;
    if(new_size > b->reserved_size) string_builder_resize(b, new_size);
    ASSERT(b->reserved_size >= new_size, "not enough memory allocated");
    memcpy(b->data + b->size, to_append.ptr, to_append.size);
    b->size = new_size;
}

// NOTE(cogno): since we're making a **string** builder, everything gets converted to a string representation (for example a bool becomes the string "true" or "false"). If you need to append without conversion you should use string_builder_append_raw()
// API(cogno): I don't know yet if this is a good idea, maybe append should just add and not convert to str?

void string_builder_append(StringBuilder* b, const char* to_append) {
    str converted = to_append;
    string_builder_append(b, converted);
}

void string_builder_append(StringBuilder* b, bool boolean) { string_builder_append(b, boolean? "true" : "false" ); }

// TODO(cogno): string builder append u8, u16, s16, u32, s32, u64, s64, f32, f64, char


// NOTE(cogno): all append_raw are little-endian

void string_builder_append_raw(StringBuilder* b, u8* pointer_to_data, s32 data_size) {
    string_builder_reserve(b, data_size);
    ASSERT(b->size + data_size <= b->reserved_size, "out of memory after a reserve??");
    for(int i = 0; i < data_size; i++) {
        b->data[b->size + i] = pointer_to_data[i];
    }
    b->size += data_size;
}

void string_builder_append_raw(StringBuilder* b, u8 to_add) {
    string_builder_reserve(b, 1);
    ASSERT(b->size + 1 <= b->reserved_size, "out of memory after a reserve??");
    b->data[b->size++] = to_add;
}

void string_builder_append_raw(StringBuilder* b, bool to_add) {
    string_builder_append_raw(b, (u8)to_add);
}

void string_builder_append_raw(StringBuilder* b, u64 to_add) { string_builder_append_raw(b, (u8*)(&to_add), sizeof(to_add)); }
void string_builder_append_raw(StringBuilder* b, u32 to_add) { string_builder_append_raw(b, (u8*)(&to_add), sizeof(to_add)); }
void string_builder_append_raw(StringBuilder* b, u16 to_add) { string_builder_append_raw(b, (u8*)(&to_add), sizeof(to_add)); }
void string_builder_append_raw(StringBuilder* b, s64 to_add) { string_builder_append_raw(b, (u8*)(&to_add), sizeof(to_add)); }
void string_builder_append_raw(StringBuilder* b, s32 to_add) { string_builder_append_raw(b, (u8*)(&to_add), sizeof(to_add)); }
void string_builder_append_raw(StringBuilder* b, s16 to_add) { string_builder_append_raw(b, (u8*)(&to_add), sizeof(to_add)); }
void string_builder_append_raw(StringBuilder* b, f32 to_add) { string_builder_append_raw(b, (u8*)(&to_add), sizeof(to_add)); }
void string_builder_append_raw(StringBuilder* b, f64 to_add) { string_builder_append_raw(b, (u8*)(&to_add), sizeof(to_add)); }

// TODO(cogno): string builder insert at index
// TODO(cogno): string builder replace
