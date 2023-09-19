#pragma once
#define GYOFIRST

#ifndef DISABLE_INCLUDES
    #include <stdio.h>
    #include <stdint.h>
    #include <stdlib.h>
#endif

typedef int8_t   s8;
typedef uint8_t  u8;
typedef int16_t  s16;
typedef uint16_t u16;
typedef int32_t  s32;
typedef uint32_t u32;
typedef int64_t  s64;
typedef uint64_t u64;
typedef float    f32;
typedef double   f64;

#define MAX_U8  0xFF
#define MAX_U16 0xFFFF
#define MAX_U32 0xFFFFFFFF
#define MAX_U64 0xFFFFFFFFFFFFFFFF
#define MAX_S8  0x7F
#define MAX_S16 0x7FFF
#define MAX_S32 0x7FFFFFFF
#define MAX_S64 0x7FFFFFFFFFFFFFFF
#define MIN_S8  0x80
#define MIN_S16 0x8000
#define MIN_S32 0x80000000
#define MIN_S64 0x8000000000000000

//
// alternative to printf, print and printsl:
// any print is just printsl with a \n at the end (sl = single line)
//
// usage: either print(variable), or print("message with replacements", var1, var2, ...)
// printf uses % followed by a character (or more) to know how to print your variable, for
// example %d prints integers, %f floats, %p pointers etc, we don't do that, we decide how
// to print the variable depending on it's type (floats will be printed as floats,
// integers as integers, etc.).
// to do so simply put '%' where you want your variable to be printed.
// if you want to print % as a symbol, add \\ before the %
//
// if you want more advanted formatting (how many characters a float has, padding, identation, ...)
// then simply use printf
//
// example usage:
// int a = 15;
// float b = 12.5;
// print(a);
// print("this is value %", a);
// print("this is a percentage: %\\%", a);
// print("values a=%, b=%", a, b);
//

// TODO(cogno): also convert, f32, f64
int u64_to_char_ptr(u64 value, char* dest) {
    char temp[22];
    char* temp_ptr = temp;
    u64 v = value;
    
    // put each digit into temp (smallest to highest units)
    int i;
    while(v || temp_ptr == temp) {
        i = v % 10;
        v /= 10;
        *temp_ptr++ = i + '0';
    }
    
    int digits_count = temp_ptr - temp;
    
    // copy digits into final buffer in the correct order
    while(temp_ptr > temp) *dest++ = *--temp_ptr;
    return digits_count;
}


int s64_to_char_ptr(s64 value, char* dest) {
    if(value >= 0) return u64_to_char_ptr((u64)value, dest);
    
    *dest++ = '-';
    return 1 + u64_to_char_ptr(-value, dest);
}

int s32_to_char_ptr(s32 value, char* dest) { return s64_to_char_ptr((s64)value, dest); }
int u32_to_char_ptr(u32 value, char* dest) { return u64_to_char_ptr((u64)value, dest); }
int u16_to_char_ptr(u16 value, char* dest) { return u64_to_char_ptr((u64)value, dest); }
int u8_to_char_ptr( u8  value, char* dest) { return u64_to_char_ptr((u64)value, dest); }
int s16_to_char_ptr(s16 value, char* dest) { return s64_to_char_ptr((s64)value, dest); }
int s8_to_char_ptr( s8  value, char* dest) { return s64_to_char_ptr((s64)value, dest); }

//
// new print (like old but buffered for extra speed)
//



int __buffer_index = 0;
const int __BUFF_SIZE = 0xFF;
char __print_buff[__BUFF_SIZE] = "";
inline void flush_to_stdout() {
    fwrite(__print_buff, 1, __buffer_index, stdout);
    __buffer_index = 0;
}

// TODO(cogno): temp, will be replaced with our custom implementations
#define buffer_append(fmt, ...) __buffer_index += sprintf_s(__print_buff + __buffer_index, __BUFF_SIZE - __buffer_index, fmt, __VA_ARGS__)



// print standard specializations
// API(cogno): maybe a name like custom_format is better? I don't know
inline void printsl_custom(const char* s) { int index = 0; while(s[index]) __print_buff[__buffer_index++] = s[index++]; }
inline void printsl_custom(char* s)       { int index = 0; while(s[index]) __print_buff[__buffer_index++] = s[index++]; }
inline void printsl_custom(char c)        { __print_buff[__buffer_index++] = c; }
inline void printsl_custom(s8  d)         { __buffer_index += s8_to_char_ptr( d, __print_buff + __buffer_index); }
inline void printsl_custom(s16 d)         { __buffer_index += s16_to_char_ptr(d, __print_buff + __buffer_index); }
inline void printsl_custom(s32 d)         { __buffer_index += s32_to_char_ptr(d, __print_buff + __buffer_index); }
inline void printsl_custom(s64 d)         { __buffer_index += s64_to_char_ptr(d, __print_buff + __buffer_index); }
inline void printsl_custom(u8  d)         { __buffer_index += u8_to_char_ptr( d, __print_buff + __buffer_index); }
inline void printsl_custom(u16 d)         { __buffer_index += u16_to_char_ptr(d, __print_buff + __buffer_index); }
inline void printsl_custom(u32 d)         { __buffer_index += u32_to_char_ptr(d, __print_buff + __buffer_index); }
inline void printsl_custom(u64 d)         { __buffer_index += u64_to_char_ptr(d, __print_buff + __buffer_index); }
inline void printsl_custom(float f)       { buffer_append("%.5f", f); }
inline void printsl_custom(double f)      { buffer_append("%.5f", f); }
inline void printsl_custom(bool b)        { if (b) printsl_custom("true"); else printsl_custom("false"); }
inline void printsl_custom() { }

// default behaviour, unknown types prints "(unknown type)", while pointers are printed as such
template<typename T> void printsl_custom(T v)  { printsl_custom("(unknown type)"); }
template<typename T> void printsl_custom(T* v) { buffer_append("0x%p", v); } // NOTE(cogno): leave this before const char* s so strings are printed as such (and not as pointers)


// first we recursively accumulate into a buffer, then we flush it
inline void accumulate_into_buffer(const char* s) { buffer_append("%s", s); }
template <typename T, typename... Types>
void accumulate_into_buffer(const char* s, T t1, Types... others) {
    int current_index = 0;
    while(true) {
        char c = s[current_index++];
        if(c == 0) return;
        else if(c == '\\') {
            char next = s[current_index];
            // escape characters
            if(next == '%') { printsl_custom('%'); current_index++; }
        } else if(c == '%') {
            break; // put formatted input
        } else {
            // just a character to print
            printsl_custom(c);
        }
    }
    printsl_custom(t1);
    accumulate_into_buffer(s + current_index, others...);
}

//
// print single inputs
//
template <typename T>
void printsl(T t) {
    printsl_custom(t);
    flush_to_stdout();
}

template <typename T>
void print(T t) {
    printsl_custom(t);
    printsl_custom('\n');
    flush_to_stdout();
}

//
// print multiple inputs
//
template <typename T, typename... Types>
void printsl(const char* s, T t1, Types... others) {
    accumulate_into_buffer(s, t1, others...);
    flush_to_stdout();
}

// print formatting
template <typename T, typename... Types>
void print(const char* s, T t1, Types... others) {
    accumulate_into_buffer(s, t1, others...);
    printsl_custom('\n');
    flush_to_stdout();
}




//
// a new type of print, even faster but with a different syntax
// print("this is a var: %, this is another var: %", var1, var2);
// now becomes
// print2("this is a var: ", var, ", this is another var: ", var2);
//
void accumulate_v2() { }
template <typename T, typename... Types>
void accumulate_v2(T t1, Types... others) {
    printsl_custom(t1);
    accumulate_v2(others...);
}

// print formatting
template <typename... Types>
void print2(Types... others) {
    accumulate_v2(others...);
    printsl_custom('\n');
    flush_to_stdout();
}














/*

custom test implementation using str_builders to make the string to print
it worked perfectly, and we might even go for it!
we'd just need to find a better way for the user to implement it's own functions


struct StrBuilder;
struct str;
StrBuilder make_str_builder();
void str_builder_append(StrBuilder* b, str to_append);
void str_builder_append(StrBuilder* b, const char* to_append);
void str_builder_append(StrBuilder* b, bool boolean);
void str_builder_append(StrBuilder* b, bool boolean);
void str_builder_append(StrBuilder* b, u8 to_append);
void str_builder_append(StrBuilder* b, u16 to_append);
void str_builder_append(StrBuilder* b, u32 to_append);
void str_builder_append(StrBuilder* b, u64 to_append);
void str_builder_append(StrBuilder* b, s8 to_append);
void str_builder_append(StrBuilder* b, s16 to_append);
void str_builder_append(StrBuilder* b, s32 to_append);
void str_builder_append(StrBuilder* b, s64 to_append);
void str_builder_append(StrBuilder* b, f32 to_append);
void str_builder_append(StrBuilder* b, f64 to_append);

void prepare_string_for_printing(StrBuilder* b, const char* s) { str_builder_append(b, s); }

template <typename T, typename... Types>
void prepare_string_for_printing(StrBuilder* b, const char* s, T t1, Types... others) {
    int current_index = 0;
    while(true) {
        char c = s[current_index++];
        if(c == 0) return;
        else if(c == '\\') {
            char next = s[current_index];
            // escape characters
            if(next == '%') {
                str_builder_append(b, '%');
                current_index++;
            }
        } else if(c == '%') {
            break; // put formatted input
        } else {
            // just a character to print
            str_builder_append(b, c);
        }
    }
    
    str_builder_append(b, t1);
    prepare_string_for_printing(b, s + current_index, others...);
}

// printsl formatting
template <typename T, typename... Types>
void printsl_builder(const char* s, T t1, Types... others) {
    auto builder = make_str_builder(100);
    prepare_string_for_printing(&builder, s, t1, others...);
    fwrite(builder.ptr, 1, builder.size, stdout);
    free(builder.ptr);
}



// print formatting
template <typename T, typename... Types>
void print_builder(const char* s, T t1, Types... others) {
    auto builder = make_str_builder(100);
    prepare_string_for_printing(&builder, s, t1, others...);
    str_builder_append(&builder, '\n');
    fwrite(builder.ptr, 1, builder.size, stdout);
    free(builder.ptr);
}

*/



#ifdef NO_ASSERT
#define ASSERT(expr, message, ...)
#define ASSERT_BOUNDS(var, min_val, max_val)
#else

void print_tab_if_there_is_a_message(const char* msg) { printsl("    "); }
void print_tab_if_there_is_a_message() { }

#define ASSERT(expr, message, ...) \
if (!(expr)) {                     \
    print("### Assertion failed: '%'", #expr); \
    print_tab_if_there_is_a_message(message); \
    print(message, __VA_ARGS__);  \
    print("    File: %", __FILE__); \
    print("    Line: %", __LINE__); \
    print("    Function: %", __FUNCTION__); \
    print("Stack Trace:");  \
    __debugbreak();                \
    abort();                       \
}

//NOTE(cogno): careful, the max_val is NOT included (so you can directly pass array.size instead of array.size - 1)
#define ASSERT_BOUNDS(var, min_val, max_val) ASSERT(((var) >= (min_val)) && ((var) < (max_val)), "OUT OF BOUNDS! expected between % and % but was %", (min_val), (max_val - 1), (var))

#endif

//defer macros. calls code inside a defer(...) macro when the current scope closes (function exit, if ending, for cycle ending, ...)
//API(cogno): I've seen a macro where you can do defer { code } instead of defer({ code }), can you figure out how?
template <typename F>
struct ScopeExit {
    ScopeExit(F f) : f(f) {}
    ~ScopeExit() { f(); }
    F f;
};

template <typename F>
ScopeExit<F> MakeScopeExit(F f) {
    return ScopeExit<F>(f);
};

#define STRING_JOIN_(arg1, arg2) arg1 ## arg2
#define STRING_JOIN(arg1, arg2) STRING_JOIN_(arg1, arg2)
#define defer(code) auto STRING_JOIN(scope_exit_, __LINE__) = MakeScopeExit([=](){code;})
