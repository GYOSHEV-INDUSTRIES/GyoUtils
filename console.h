// this header has been implemented by `https://github.com/crim4`
// TODO(crim):
// * implement input functions (in a pythonic way would be pretty)
// * the buffer system may be rewritten in this way:
//   it may be a fixed-length local declaration
//   the length would be calculated with `k + u`
//   where `k` represents the total length of all the string
//   literal chunks
//   and `u` represents the total length of all the string
//   values passed as formatting arguments

#pragma once

#if __cplusplus < 202002L
    #error This header requires at least C++20
#endif


#ifndef DISABLE_INCLUDES
    #include <stdio.h>
    #include <string.h>
    #include <io.h> // TODO(crim): use `unistd.h` on unix systems

    #include "first.h"
#endif

// this type captures at compile time
// a literal string with length as well
template<size_t length>
struct string_literal_hack_t {
    consteval string_literal_hack_t(const char (&str)[length]) { 
        for (size_t i = 0; i < length; i++)
            value[i] = str[i];
    }

    char value[length];
};

// template<string_literal_hack_t error_message>
// consteval void compilation_error_impl() {
//     static_assert(error_message);
// }

// this function is used in consteval and constexpr contexts
// to arbitrarily crash the compilation process.
// the function is on purpose declared without a body
// so that the compiler will raise a compilation error
// showing somewhere `error_message` as well.
//
// do not use this function in runtime context or you will
// run in linkage errors
void raise_consteval_error(char const* error_message);

constexpr size_t internal_pwrite_buffer_max_capacity = 150;
static char internal_pwrite_buffer[internal_pwrite_buffer_max_capacity];
static size_t internal_pwrite_buffer_length = 0;

void pflush() {
    _write(1, internal_pwrite_buffer, internal_pwrite_buffer_length);
    internal_pwrite_buffer_length = 0;
}

void pwrite(char const* content, size_t content_length) {
    if (content_length > internal_pwrite_buffer_max_capacity) {
        pflush();
        _write(1, content, content_length);
        return;
    }

    if (internal_pwrite_buffer_length + content_length > internal_pwrite_buffer_max_capacity)
        pflush();

    // yes, but why not a memcpy?
    // `content_length` is usually very small, byte-by-byte is still faster
    // when the buffer is not big, a chunk-by-chunk (where chunk may be 4 or 8 bytes)
    // may be faster but we have no warranty `content_length` is perfectly splittable in
    // N chunks, checking what's left from the division of the buffer makes the process just slower
    for (size_t i = 0; i < content_length; i++)
        internal_pwrite_buffer[internal_pwrite_buffer_length + i] = content[i];

    internal_pwrite_buffer_length += content_length;
}

// this structure is only used in consteval code
struct internal_chunk_t {
    bool is_valid;
    char const* value;
    size_t length;

    consteval internal_chunk_t(char const* value, size_t length) {
        is_valid = true;
        this->value = value;
        this->length = length;
    }

    consteval internal_chunk_t() {
        is_valid = false;
    }
};

template<string_literal_hack_t fmt>
inline consteval size_t __get_fmt_chunk_length(size_t i) {
    auto j = i;

    for (; j < sizeof(fmt); j++)
        if (fmt.value[j] == '%')
            break;

    return j - i;
}

template<string_literal_hack_t fmt, size_t step>
inline consteval internal_chunk_t __get_fmt_chunk_by_step() {
    auto i = size_t(0);
    auto s = size_t(0);

    for (; i < sizeof(fmt); i++) {
        if (s == step)
            return internal_chunk_t(
                fmt.value + i,
                __get_fmt_chunk_length<fmt>(i)
            );

        if (fmt.value[i] != '%')
            continue;

        s++;
    }

    return internal_chunk_t();
}

constexpr size_t object_repr_max_length = 40;
constexpr size_t floating_repr_default_precision = 5;

// NOTE(crim): this declaration needs to be before all other overloads
//             so that the compiler will use those ones for specific types
//             and this one if it fails to match those ones with `object_t`
template<typename object_t>
inline void print_object_impl(object_t o) {
    o.print();
}

// NOTE(crim): this declaration needs to be before the `print_object_impl(const char* s)` overload
//             so that the compiler will use that one for c-strings this one for other pointers
template<typename object_t>
inline void print_object_impl(object_t* o) {
    size_t value = u64(o);
    char digits[] = "0123456789abcdef";
    
    char buffer[object_repr_max_length];
    int i = object_repr_max_length;

    do {
        buffer[--i] = digits[value % 16];
        value /= 16;
    } while (value > 0);

    buffer[--i] = 'x';
    buffer[--i] = '0';

    auto length = object_repr_max_length - i;
    pwrite(buffer + i, length);
}

inline void print_object_impl(char const* o) {
    pwrite(o, strlen(o));
}
inline void print_object_impl(char* o) { print_object_impl((const char*)o); }

inline void print_object_impl(s64 o) {
    char buffer[object_repr_max_length];
    int i = object_repr_max_length;
    bool is_negative = o < 0;

    if (is_negative)
        o = -o;

    do {
        buffer[--i] = '0' + (o % 10);
        o /= 10;
    } while (o > 0);

    if (is_negative)
        buffer[--i] = '-';

    auto length = object_repr_max_length - i;
    pwrite(buffer + i, length);
}
inline void print_object_impl(s32 o) { print_object_impl(s64(o)); }
inline void print_object_impl(s16 o) { print_object_impl(s64(o)); }
inline void print_object_impl(s8  o) { print_object_impl(s64(o)); }

inline void print_object_impl(u64 o) {
    char buffer[object_repr_max_length];
    int i = object_repr_max_length;

    do {
        buffer[--i] = '0' + (o % 10);
        o /= 10;
    } while (o > 0);

    auto length = object_repr_max_length - i;
    pwrite(buffer + i, length);
}
inline void print_object_impl(u32 o) { print_object_impl(u64(o)); }
inline void print_object_impl(u16 o) { print_object_impl(u64(o)); }
inline void print_object_impl(u8  o) { print_object_impl(u64(o)); }

inline void print_object_impl(double o) {
    char buffer[object_repr_max_length];
    int i = object_repr_max_length - floating_repr_default_precision;
    bool is_negative = o < 0;

    if (is_negative)
        o = -o;
    
    auto head = u64(o);
    auto tail = o - double(head);
    for (int j = floating_repr_default_precision; j >= 0; j--) {
        tail *= 10;
        auto digit = u64(tail);
        buffer[object_repr_max_length - j] = '0' + digit;
        tail -= digit;
    }
    
    buffer[--i] = '.';

    do {
        buffer[--i] = '0' + (head % 10);
        head /= 10;
    } while (head > 0);

    if (is_negative)
        buffer[--i] = '-';

    auto length = object_repr_max_length - i;
    pwrite(buffer + i, length);
}
inline void print_object_impl(float o) { print_object_impl(double(o)); }

inline void print_object_impl(char o) {
    pwrite(&o, 1);
}

inline void print_object_impl(bool o) {
    if (o)
        pwrite("true", 4);
    else
        pwrite("false", 5);
}

template<string_literal_hack_t fmt, size_t step>
inline consteval void __check_fmt_chunk_was_last() {
    if (__get_fmt_chunk_by_step<fmt, step + 1>().is_valid)
        raise_consteval_error("pprint() rules violation: you wrote too many `%` in the fmt, did you forget to pass some argument in the call?");
}

template<string_literal_hack_t fmt, size_t step>
inline consteval void __check_fmt_chunk_was_not_last() {
    if (!__get_fmt_chunk_by_step<fmt, step + 1>().is_valid)
        raise_consteval_error("pprint() rules violation: you passed too many arguments, did you forget to put some `%` in the fmt?");
}

template<string_literal_hack_t fmt, size_t step>
inline void print_impl() {
    constexpr auto chunk = __get_fmt_chunk_by_step<fmt, step>();
    if (!chunk.is_valid)
        return;
    
    pwrite(chunk.value, chunk.length);

    // ensuring there are no trailing `%` in `fmt`
    __check_fmt_chunk_was_last<fmt, step>();
}

template<string_literal_hack_t fmt, size_t step, typename first_object_t, typename... other_objects_t>
inline void print_impl(
    first_object_t first,
    other_objects_t... others
) {
    // ensuring `first` is not a trailing formatting argument
    // it must be associated to the respective `%` in `fmt`
    __check_fmt_chunk_was_not_last<fmt, step>();

    constexpr auto chunk = __get_fmt_chunk_by_step<fmt, step>();
    if (!chunk.is_valid)
        return;
    
    pwrite(chunk.value, chunk.length);
    print_object_impl(first);
    print_impl<fmt, step + 1>(others...);
}

// this is ugly but necessary to get `__LINE__`
// value as a string
#define STRINGIZE(x) STRINGIZE2(x)
#define STRINGIZE2(x) #x
#define LINE_STRING STRINGIZE(__LINE__)

// prints to stdout without flushing.
// allows to format passing any object without specifying type.
// you can use symbol `%` to mark a formatting point.
//
// usage:
//  ```
//  pprint("hello world!\n")
//  pprintln("hello world!");
//
//  int a = 10;
//  float b = 1.2;
//  bool c = true;
//  char const* d = "hello";
//  
//  pprintln("a: %, b: %, c: %, d: %d", a, b, c, d);
//  pprintln("%%%%", a, b, c, d);
//
//  // this is a compilation error
//  //  pprintln("foo: %");
//
//  // this is a compilation error
//  //  pprintln("foo!", 1);
//  ```
//
// to use the actual percentage character you need to use this trick:
//  ```
//  pprintln("percentage character: '%'", "%");
//  ```
//
// the function uses `_write` under the hood.
// `p` stands for preprocessed, since the formatting process is done before runtime.
#define pprint(fmt, ...) print_impl<string_literal_hack_t(fmt), 0>(__VA_ARGS__)

// the same of `print` but adds a newline character at the end.
// since the function uses `_write` under the hood, the `\n` should trigger buffer flushing.
// `p` stands for preprocessed, since the formatting process is done before runtime.
#define pprintln(fmt, ...) pprint(fmt "\n", __VA_ARGS__)

// the same of `print` but flushes out the buffer.
// `i` stands for immediate.
#define iprint(fmt, ...) { pprint(fmt, __VA_ARGS__); pflush(); }

// the same of `println` but flushes out the buffer.
// `i` stands for immediate.
#define iprintln(fmt, ...) { pprintln(fmt, __VA_ARGS__); pflush(); }

// prints helpful information for an expression
// the output looks like `file:line expr = actual-value`
//
// usage:
//  ```
//  float a = 3.14;
//  char const* b = "hello world";
//  int c = 12;
//
//  inspect(a);
//  inspect(b);
//  inspect(c);
//
//  inspect(10 * a * c + 20);
//  ```
//
#define inspect(expr) iprintln(__FILE__ ":" LINE_STRING " " #expr " = %", (expr))

// more usage examples
#if 0

    void __example() {
        int a = 20;
        float b = 1;
        bool c = true;

        pprintln("hello world!");
        pprintln("percentage character: '%'", "%");
        pprintln("negative: %", -12);
        pprintln("address: %", &a);
        pprintln("floating: %", 3.14);
        pprintln();

        // this is a compilation error
        //  pprintln("foo: %");
        
        // this is a compilation error
        //  pprintln("foo!", 1);

        pprintln("a: %, b: %, c: %", a, b, c);
        pprintln("%%%", a, b, c);
        pprintln();

        inspect(a);
        inspect(b);
        inspect(c);

        struct person_t {
            char const* name;
            u8 age;

            person_t(char const* name, u8 age) : name(name), age(age) {

            }

            void print() const {
                pprint("person_t { name: %, age: % }", name, age);
            }
        };
        inspect(person_t("chri", 18));
    }

    #include "performance_counter.h"
    #include <float.h>

    int main() {
        auto best_cycles = FLT_MAX;

        for (u32 i = 0; i < UINT16_MAX; i++) {
            auto a = read_cpu_timer();
            // printf("abc: %d%d%f, def: '%c', bool: %s aaaaaaaaaaaaaaaaaaaaaa bbbbbbbbbbbbbbbbbbbbbbb ccccccccccccccccccccc\n", 1000000, -100000, 23.22332, 'a', true ? "true" : "false");
            // pprintln("abc: %%%, def: '%', bool: % aaaaaaaaaaaaaaaaaaaaaa bbbbbbbbbbbbbbbbbbbbbbb ccccccccccccccccccccc", 1000000, -100000, 23.22332, 'a', true);
            iprintln("abc: %%%, def: '%', bool: % aaaaaaaaaaaaaaaaaaaaaa bbbbbbbbbbbbbbbbbbbbbbb ccccccccccccccccccccc", 1000000, -100000, 23.22332, 'a', true);
            // print("abc: %%%, def: '%', bool: % aaaaaaaaaaaaaaaaaaaaaa bbbbbbbbbbbbbbbbbbbbbbb ccccccccccccccccccccc", 1000000, -100000, 23.22332, 'a', true);
            auto cycles = read_cpu_timer() - a;

            if (cycles < best_cycles)
                best_cycles = cycles;
        }

        auto micros = best_cycles * 1000000000.0f / estimate_cpu_frequency() / 1000;
        iprintln("time: % us (microseconds)", micros);
    }

#endif