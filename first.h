#pragma once
#include <stdio.h>
#include <stdint.h>

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

// variant of the print below without the ending newline (\n)
char _print_buff[0xFF] = "";
#define fast_print(fmt, ...) sprintf_s(_print_buff, fmt, __VA_ARGS__);  fputs(_print_buff, stdout)

template<typename T> void printsl(T v)  { fast_print("(unknown type)"); }
template<typename T> void printsl(T* v) { fast_print("0x%p", v); } // NOTE(cogno): leave this before const char* s so strings are printed as such (and not as pointers)

// print standard specializations
inline void printsl(const char* s) { fast_print("%s", s); }
inline void printsl(char* s)       { fast_print("%s", s); }
inline void printsl(char c)        { putchar(c); }
inline void printsl(s8  d)         { fast_print("%d", d); }
inline void printsl(s16 d)         { fast_print("%d", d); }
inline void printsl(s32 d)         { fast_print("%ld", d); }
inline void printsl(s64 d)         { fast_print("%lld", d); }
inline void printsl(u8  d)         { fast_print("%u", d); }
inline void printsl(u16 d)         { fast_print("%u", d); }
inline void printsl(u32 d)         { fast_print("%lu", d); }
inline void printsl(u64 d)         { fast_print("%llu", d); }
inline void printsl(float f)       { fast_print("%.3f", f); }
inline void printsl(double f)      { fast_print("%.3f", f); }
inline void printsl(bool b)        { fast_print("%s", b ? "true" : "false"); }
inline void printsl() { }

// equal to printsl but with automatic \n after the string
template<typename T> inline void print(T v) { printsl(v); putchar('\n'); }
inline void print() { }

template <typename T, typename... Types>
void printsl(const char* s, T t1, Types... others) {
    int current_index = 0;
    while(true) {
        char c = s[current_index++];
        if(c == 0) return;
        else if(c == '\\') {
            char next = s[current_index];
            // escape characters
            if(next == '%') { putchar('%'); current_index++; }
        } else if(c == '%') {
            break; // put formatted input
        } else {
            // just a character to print
            putchar(c);
            continue;
        }
    }
    printsl(t1);
    printsl(s + current_index, others...);
}

template <typename T, typename... Types>
void print(const char* s, T t1, Types... others) {
    int current_index = 0;
    while(true) {
        char c = s[current_index++];
        if(c == 0) {
            putchar('\n');
            return;
        } else if(c == '\\') {
            char next = s[current_index];
            // escape characters
            if(next == '%') { putchar('%'); current_index++; }
        } else if(c == '%') {
            break; // put formatted input
        } else {
            // just a character to print
            putchar(c);
            continue;
        }
    }
    printsl(t1);
    print(s + current_index, others...);
}

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
//TODO(cogno): test if these really work
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

//API(cogno): I think scope_exit with counter can be avoided, you're never gonna have 2 defer on the same line, replace
#define STRING_JOIN_(arg1, arg2) arg1 ## arg2
#define STRING_JOIN(arg1, arg2) STRING_JOIN_(arg1, arg2)
#define defer(code) auto STRING_JOIN(scope_exit_, __COUNTER__) = MakeScopeExit([=](){code;})
