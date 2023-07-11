#pragma once
#include <stdint.h>

//TODO(cogno): have a define to deactivate asserts (slow path vs fast path)
//TODO(cogno): put __debugbreak(); so using a debugger stops the program on the macro
#define ASSERT(expr, message, ...)                                                             \
if (!(expr)) {                                                                                 \
    printf("Assertion failed file %s, line %d: " message, __FILE__, __LINE__, ##__VA_ARGS__);  \
    abort();                                                                                   \
}

typedef int8_t   s8;
typedef uint8_t  u8;
typedef int16_t  s16;
typedef uint16_t u16;
typedef int32_t  s32;
typedef uint32_t u32;
typedef int64_t  s64;
typedef uint64_t u64;

//variant of the print below without the ending newline (\n)
template<typename T> void printsl(T v)  { printf("(unknown type)"); }
template<typename T> void printsl(T* v) { printf("0x%p", v); } // NOTE(cogno): leave this before const char* s so strings are printed as such (and not as pointers)

//print standard specializations
inline void printsl(const char* s) { printf("%s", s); }
inline void printsl(char c)        { putchar(c); }
inline void printsl(s8  d)         { printf("%d", d); }
inline void printsl(s16 d)         { printf("%d", d); }
inline void printsl(s32 d)         { printf("%d", d); }
inline void printsl(s64 d)         { printf("%lld", d); }
inline void printsl(u8  d)         { printf("%d", d); }
inline void printsl(u16 d)         { printf("%d", d); }
inline void printsl(u32 d)         { printf("%d", d); }
inline void printsl(u64 d)         { printf("%lld", d); }
inline void printsl(float f)       { printf("%f", f); }
inline void printsl(double f)      { printf("%f", f); }
inline void printsl(bool b)        { printf("%s", b ? "true" : "false" ); }

//equal to printsl but with automatic \n after the string
template<typename T> inline void print(T v) { printsl(v); putchar('\n'); }

template <typename T, typename... Types>
void printsl(const char* s, T t1, Types... others) {
    int current_index = 0;
    while(true) {
        char c = s[current_index++];
        if(c == 0) return;
        else if(c == '\\') {
            char next = s[current_index];
            //escape characters
            if(next == '%') { putchar('%'); current_index++; }
        } else if(c == '%') {
            break; // put formatted input
        } else {
            //just a character to print
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
            //escape characters
            if(next == '%') { putchar('%'); current_index++; }
        } else if(c == '%') {
            break; // put formatted input
        } else {
            //just a character to print
            putchar(c);
            continue;
        }
    }
    printsl(t1);
    print(s + current_index, others...);
}
