
// implemented manually to avoid the strlen dependency
int c_string_length(const char* s) {
    int len = 0;
    while(s[len++]) { }
    return len - 1;
}

//TODO(cogno): test all of this file

//TODO: unicode (?) (currently str kind of does not support it, I mean utf8 is just an array of bytes but these functions don't take it into account so they might be wrong)
//TODO: str builder
//TODO: str concat
//TODO: str matches
//TODO: str substring
//TODO: str split all
//TODO: str parse to s32
//TODO: str parse to float/double
//TODO: str is number (?)
//TODO: str is alphanumeric (?)
//TODO: str is whitespace (?)

struct str{
    u8* ptr;
    int size;
    
    // conversion constructor from const char* to str (so you can do 'str s = "a string";')
    str(const char* c) {
        ptr = (u8*)c;
        size = c_string_length(c);
    }
    
    str(u8* p, int len) { //NOTE(cogno): c++ is shit so we need to define this to do "str{ptr, len};"
        ptr = p;
        size = len;
    }
    
    str() = default; //NOTE(cogno): c++ is shit so we need to define this to do "str{};"
};
//TODO(cogno): operator overload [] to do string[i] instead of string.ptr[i] with bounds checking to avoid undefined behaviour

//PERF(cogno): can we make this fast like other prints or is it already fast like those because of the putchar() ? (look at first.h)
inline void printsl(str v) {
    for(int i = 0; i < v.size; i++) putchar(v.ptr[i]);
}



// splits a single str in 2 parts on the first occurrence of a char, no allocations necessary.
// the character split is NOT included in the final strings, 
// put the original string in to_split and empty ones in left_side and right_side.
// if the character is found the function returns true and fills left_side and right_side,
// if the character is NOT found false is returned, left_side will contain the full string and
// right_side will be empty
bool str_split_left(str to_split, u8 char_to_split, str* left_side, str* right_side) {
    for(int i = 0; i < to_split.size; i++) {
        if(to_split.ptr[i] == char_to_split) {
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
            
            ASSERT(original_index + to_split_index < to_split.size, "reading outside memory\n");
            ASSERT(to_split_index < to_split.size, "reading outside memory\n");
            
            if(to_split.ptr[original_index + to_split_index] != to_split.ptr[to_split_index]) {
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
        if(to_split.ptr[i] == '\n') {
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
    //TODO(cogno): bounds check?
    while(true) {
        if(to_trim->ptr[0] == ' ' || to_trim->ptr[0] == '\t') {
            to_trim->ptr++;
            to_trim->size--;
        } else break;
    }
}

void str_trim_right(str* to_trim) {
    //API(cogno): I don't think space and \t are enough...
    //TODO(cogno): bounds check?
    while(true) {
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
        char ch = to_convert.ptr[i];
        if(ch > '9' || ch < '0') return false;
        *out_value = (*out_value) * 10 + (ch - '0');
    }
    return true;
}

bool str_starts_with(str to_check, char ch) {
    return to_check.size > 0 && to_check.ptr[0] == ch;
}

bool str_starts_with(str to_check, str checker) {
    if (to_check.size < checker.size) return false; //not enough character to check
    
    for(int i = 0; i < checker.size; i++) {
        ASSERT_BOUNDS(i, 0, to_check.size); //should never happen because of check above
        ASSERT_BOUNDS(i, 0, checker.size);  //should never happen because of check above
        if(to_check.ptr[i] != checker.ptr[i]) return false;
    }
    
    return true;
}