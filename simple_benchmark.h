#pragma once

/*
In this file: 
Basic benchmarking functionality
- BENCHMARK_VOID_WITH_COUNT to run a function returning void a given number of times and get execution informations
- BENCHMARK_WITH_COUNT to run a non-void function a given number of times and get execution informations
- BENCHMARK_VOID_FUNC to run a function returning void many times and get execution informations
- BENCHMARK_FUNC to run a non-void function many times and get execution informations
- BENCHMARK_COMPARE_VOID to run 2 functions returning void at the same time and give info on the faster one
- BENCHMARK_COMPARE to run 2 non-void functions at the same time and give info on the faster one
*/


#ifndef SIMPLE_BENCHMARK
#define SIMPLE_BENCHMARK 1
#endif

#if SIMPLE_BENCHMARK

#ifndef GYOFIRST
    #include "first.h"
#endif
#ifndef GYOPERFORMANCE_COUNTER
    #include "performance_counter.h"
#endif

void print_benchmark_times(u64 min_cycles, u64 avg_cycles, u64 max_cycles) {
    u64 cpu_frequency = estimate_cpu_frequency();
    
    f64 min_time = min_cycles * 1000000000.0f / cpu_frequency;
    f64 avg_time = avg_cycles * 1000000000.0f / cpu_frequency;
    f64 max_time = max_cycles * 1000000000.0f / cpu_frequency;
    if(min_time < 1000) { print("%, %, % (ns)", min_time, avg_time, max_time); return; }
    
    min_time /= 1000;
    avg_time /= 1000;
    max_time /= 1000;
    if(min_time < 1000) { print("%, %, % (us)", min_time, avg_time, max_time); return; }
    
    min_time /= 1000;
    avg_time /= 1000;
    max_time /= 1000;
    if(min_time < 1000) { print("%, %, % (ms)", min_time, avg_time, max_time); return; }
    
    min_time /= 1000;
    avg_time /= 1000;
    max_time /= 1000;
    print("%, %, % (s)", min_time, avg_time, max_time);
}

#define BENCHMARK_WITH_COUNT(count, func_name, ...) \
do { \
    u64 min_cycles = MAX_U64; \
    u64 max_cycles = 0; \
    u64 total_cycles = 0; \
    for(int i = 0; i < count; i++) { \
        u64 start = read_cpu_timer(); \
        volatile auto temp = func_name(__VA_ARGS__); \
        u64 current_cycles = read_cpu_timer() - start; \
        if(current_cycles < min_cycles) min_cycles = current_cycles; \
        if(current_cycles > max_cycles) max_cycles = current_cycles; \
        total_cycles += current_cycles; \
    } \
    printf("function '%s' x%-10d ", STRING_JOIN( STRING_JOIN( STRING_JOIN(#func_name, "("), #__VA_ARGS__  ) , ")" ), count); \
    printsl("|> cycles (min, avg, max): %, %, %", min_cycles, total_cycles / count, max_cycles ); \
    printsl(" | time (min, avg, max): "); \
    print_benchmark_times(min_cycles, total_cycles / count, max_cycles); \
} while(0)

#define BENCHMARK_VOID_WITH_COUNT(count, func_name, ...) \
do { \
    u64 min_cycles = MAX_U64; \
    u64 max_cycles = 0; \
    u64 total_cycles = 0; \
    for(int i = 0; i < count; i++) { \
        u64 start = read_cpu_timer(); \
        func_name(__VA_ARGS__); \
        u64 current_cycles = read_cpu_timer() - start; \
        if(current_cycles < min_cycles) min_cycles = current_cycles; \
        if(current_cycles > max_cycles) max_cycles = current_cycles; \
        total_cycles += current_cycles; \
    } \
    printf("function '%s' x%-10d ", STRING_JOIN( STRING_JOIN( STRING_JOIN(#func_name, "("), #__VA_ARGS__  ) , ")" ), count); \
    printsl("|> cycles (min, avg, max): %, %, %", min_cycles, total_cycles / count, max_cycles ); \
    printsl(" | time (min, avg, max): "); \
    print_benchmark_times(min_cycles, total_cycles / count, max_cycles); \
} while(0)


#define BENCHMARK_FUNC(func_name, ...) \
do { \
    BENCHMARK_WITH_COUNT(10, func_name, __VA_ARGS__); \
    BENCHMARK_WITH_COUNT(100, func_name, __VA_ARGS__); \
    BENCHMARK_WITH_COUNT(1000, func_name, __VA_ARGS__); \
    BENCHMARK_WITH_COUNT(10000, func_name, __VA_ARGS__); \
    BENCHMARK_WITH_COUNT(100000, func_name, __VA_ARGS__); \
    BENCHMARK_WITH_COUNT(1000000, func_name, __VA_ARGS__); \
} while(0)

#define BENCHMARK_VOID_FUNC(func_name, ...) \
do { \
    BENCHMARK_VOID_WITH_COUNT(10, func_name, __VA_ARGS__); \
    BENCHMARK_VOID_WITH_COUNT(100, func_name, __VA_ARGS__); \
    BENCHMARK_VOID_WITH_COUNT(1000, func_name, __VA_ARGS__); \
    BENCHMARK_VOID_WITH_COUNT(10000, func_name, __VA_ARGS__); \
    BENCHMARK_VOID_WITH_COUNT(100000, func_name, __VA_ARGS__); \
    BENCHMARK_VOID_WITH_COUNT(1000000, func_name, __VA_ARGS__); \
} while(0)

/*
Compares 2 different functions against the same inputs n times to know if the second one is faster or slower.
You can use this when you're developing a new alternative to an old function and you'd like to know if
it's faster or slower than the old one.
If the functions you want to test return some input you should instead use BENCHMARK_COMPARE.

Example usage:
BENCHMARK_COMPARE_VOID(1000, old_func, new_func, optional_input_to_both_functions);
*/
#define BENCHMARK_COMPARE_VOID(count, func1, func2, ...) \
do { \
    u64 min_cycles_f1 = MAX_U64; \
    u64 min_cycles_f2 = MAX_U64; \
    u64 max_cycles_f1 = 0; \
    u64 max_cycles_f2 = 0; \
    u64 total_cycles_f1 = 0; \
    u64 total_cycles_f2 = 0; \
    for(int i = 0; i < count; i++) { \
        u64 start_f1 = read_cpu_timer(); \
        func1(__VA_ARGS__); \
        u64 current_cycles_f1 = read_cpu_timer() - start_f1; \
        u64 start_f2 = read_cpu_timer(); \
        func2(__VA_ARGS__); \
        u64 current_cycles_f2 = read_cpu_timer() - start_f2; \
        if(current_cycles_f1 < min_cycles_f1) min_cycles_f1 = current_cycles_f1; \
        if(current_cycles_f2 < min_cycles_f2) min_cycles_f2 = current_cycles_f2; \
        if(current_cycles_f1 > max_cycles_f1) max_cycles_f1 = current_cycles_f1; \
        if(current_cycles_f2 > max_cycles_f2) max_cycles_f2 = current_cycles_f2; \
        total_cycles_f1 += current_cycles_f1; \
        total_cycles_f2 += current_cycles_f2; \
    } \
    u64 avg_cycles_f1 = total_cycles_f1 / count; \
    u64 avg_cycles_f2 = total_cycles_f2 / count; \
    auto f1_name = STRING_JOIN( STRING_JOIN( STRING_JOIN(#func1, "("), #__VA_ARGS__  ) , ")" ); \
    auto f2_name = STRING_JOIN( STRING_JOIN( STRING_JOIN(#func2, "("), #__VA_ARGS__  ) , ")" ); \
    printsl("\nfunction '%' x% ", f1_name, count); \
    printsl("|> cycles (min, avg, max): %, %, %", min_cycles_f1, avg_cycles_f1, max_cycles_f1 ); \
    printsl(" | time (min, avg, max): "); \
    print_benchmark_times(min_cycles_f1, avg_cycles_f1, max_cycles_f1); \
    printsl("function '%' x% ", f2_name, count); \
    printsl("|> cycles (min, avg, max): %, %, %", min_cycles_f2, avg_cycles_f2, max_cycles_f2 ); \
    printsl(" | time (min, avg, max): "); \
    print_benchmark_times(min_cycles_f2, avg_cycles_f2, max_cycles_f2); \
    print("final result when comparing '%' againts '%':", f2_name, f1_name); \
    if(min_cycles_f1 < min_cycles_f2) print("    min: % more cycles (x% slowdown)", min_cycles_f2 - min_cycles_f1, (f64)min_cycles_f2 / min_cycles_f1); \
    else                              print("    min: % less cycles (x% speedup)", min_cycles_f1 - min_cycles_f2, (f64)min_cycles_f1 / min_cycles_f2); \
    if(avg_cycles_f1 < avg_cycles_f2) print("    avg: % more cycles (x% slowdown)", avg_cycles_f2 - avg_cycles_f1, (f64)avg_cycles_f2 / avg_cycles_f1); \
    else                              print("    avg: % less cycles (x% speedup)", avg_cycles_f1 - avg_cycles_f2, (f64)avg_cycles_f1 / avg_cycles_f2); \
    if(max_cycles_f1 < max_cycles_f2) print("    max: % more cycles (x% slowdown)", max_cycles_f2 - max_cycles_f1, (f64)max_cycles_f2 / max_cycles_f1); \
    else                              print("    max: % less cycles (x% speedup)", max_cycles_f1 - max_cycles_f2, (f64)max_cycles_f1 / max_cycles_f2); \
} while(0)

// Alternative to BENCHMARK_COMPARE_VOID where the input functions return some values.
// This macro prevents the compiler/optimizer from removing that output, which could
// interfere with your analysis.
#define BENCHMARK_COMPARE(count, func1, func2, ...) \
do { \
    u64 min_cycles_f1 = MAX_U64; \
    u64 min_cycles_f2 = MAX_U64; \
    u64 max_cycles_f1 = 0; \
    u64 max_cycles_f2 = 0; \
    u64 total_cycles_f1 = 0; \
    u64 total_cycles_f2 = 0; \
    for(int i = 0; i < count; i++) { \
        u64 start_f1 = read_cpu_timer(); \
        volatile auto temp1 = func1(__VA_ARGS__); \
        u64 current_cycles_f1 = read_cpu_timer() - start_f1; \
        u64 start_f2 = read_cpu_timer(); \
        volatile auto temp2 = func2(__VA_ARGS__); \
        u64 current_cycles_f2 = read_cpu_timer() - start_f2; \
        if(current_cycles_f1 < min_cycles_f1) min_cycles_f1 = current_cycles_f1; \
        if(current_cycles_f2 < min_cycles_f2) min_cycles_f2 = current_cycles_f2; \
        if(current_cycles_f1 > max_cycles_f1) max_cycles_f1 = current_cycles_f1; \
        if(current_cycles_f2 > max_cycles_f2) max_cycles_f2 = current_cycles_f2; \
        total_cycles_f1 += current_cycles_f1; \
        total_cycles_f2 += current_cycles_f2; \
    } \
    u64 avg_cycles_f1 = total_cycles_f1 / count; \
    u64 avg_cycles_f2 = total_cycles_f2 / count; \
    auto f1_name = STRING_JOIN( STRING_JOIN( STRING_JOIN(#func1, "("), #__VA_ARGS__  ) , ")" ); \
    auto f2_name = STRING_JOIN( STRING_JOIN( STRING_JOIN(#func2, "("), #__VA_ARGS__  ) , ")" ); \
    printsl("function '%' x% ", f1_name, count); \
    printsl("|> cycles (min, avg, max): %, %, %", min_cycles_f1, avg_cycles_f1, max_cycles_f1 ); \
    printsl(" | time (min, avg, max): "); \
    print_benchmark_times(min_cycles_f1, avg_cycles_f1, max_cycles_f1); \
    printsl("function '%' x% ", f2_name, count); \
    printsl("|> cycles (min, avg, max): %, %, %", min_cycles_f2, avg_cycles_f2, max_cycles_f2 ); \
    printsl(" | time (min, avg, max): "); \
    print_benchmark_times(min_cycles_f2, avg_cycles_f2, max_cycles_f2); \
    print("final result when comparing '%' againts '%':", f2_name, f1_name); \
    if(min_cycles_f1 < min_cycles_f2) print("    min: % more cycles (x% slowdown)", min_cycles_f2 - min_cycles_f1, (f64)min_cycles_f2 / min_cycles_f1); \
    else                              print("    min: % less cycles (x% speedup)", min_cycles_f1 - min_cycles_f2, (f64)min_cycles_f1 / min_cycles_f2); \
    if(avg_cycles_f1 < avg_cycles_f2) print("    avg: % more cycles (x% slowdown)", avg_cycles_f2 - avg_cycles_f1, (f64)avg_cycles_f2 / avg_cycles_f1); \
    else                              print("    avg: % less cycles (x% speedup)", avg_cycles_f1 - avg_cycles_f2, (f64)avg_cycles_f1 / avg_cycles_f2); \
    if(max_cycles_f1 < max_cycles_f2) print("    max: % more cycles (x% slowdown)", max_cycles_f2 - max_cycles_f1, (f64)max_cycles_f2 / max_cycles_f1); \
    else                              print("    max: % less cycles (x% speedup)", max_cycles_f1 - max_cycles_f2, (f64)max_cycles_f1 / max_cycles_f2); \
} while(0)

#else

#define BENCHMARK_FUNC(func_name, ...)
#define BENCHMARK_WITH_COUNT(count, func_name, ...)
#define BENCHMARK_VOID_FUNC(func_name, ...)
#define BENCHMARK_VOID_WITH_COUNT(count, func_name, ...)
#define BENCHMARK_COMPARE_VOID(count, func1, func2, ...)
#define BENCHMARK_COMPARE(count, func1, func2, ...)

#endif
