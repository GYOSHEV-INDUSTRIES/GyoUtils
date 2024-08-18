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
- BENCHMARK_VOID_MANY_INPUTS to run a function returning void a given number of times with different inputs
- BENCHMARK_MANY_INPUTS to run a non-void function a given number of times with different inputs
*/

// to quickly deactivate benchmarking without changing code you can #define SIMPLE_BENCHMARK 0 before importing this file
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

void print_benchmark_time(u64 cycles) {
    u64 cpu_frequency = estimate_cpu_frequency();
    
    f64 time = cycles * 1000000000.0f / cpu_frequency;
    if(time < 1000) { print("% ns", time); return; }
    
    time /= 1000;
    if(time < 1000) { print("% us", time); return; }
    
    time /= 1000;
    if(time < 1000) { print("% ms", time); return; }
    
    time /= 1000;
    print("% s", time);
}


/*
Runs a given function a given number of times with the given inputs.
If you function returns void you should use BENCHMARK_VOID_WITH_COUNT.
Usage: BENCHMARK_WITH_COUNT(10000, func_to_run, input1, input2);
*/
#define BENCHMARK_WITH_COUNT(count, func_name, ...) \
do { \
    u64 min_cycles = MAX_U64; \
    for(int i = 0; i < count; i++) { \
        u64 start = read_cpu_timer(); \
        volatile auto temp = func_name(__VA_ARGS__); \
        u64 current_cycles = read_cpu_timer() - start; \
        if(current_cycles < min_cycles) min_cycles = current_cycles; \
    } \
    printf("function '%s' x%-10d ", STRING_JOIN( STRING_JOIN( STRING_JOIN(#func_name, "("), #__VA_ARGS__  ) , ")" ), count); \
    printsl("|> min cycles: %", min_cycles); \
    printsl(" | min time: "); \
    print_benchmark_time(min_cycles); \
} while(0)

// alternative to BENCHMARK_WITH_COUNT for functions returning void,
// if your function returns something you should use BENCHMARK_WITH_COUNT
#define BENCHMARK_VOID_WITH_COUNT(count, func_name, ...) \
do { \
    u64 min_cycles = MAX_U64; \
    for(int i = 0; i < count; i++) { \
        u64 start = read_cpu_timer(); \
        func_name(__VA_ARGS__); \
        u64 current_cycles = read_cpu_timer() - start; \
        if(current_cycles < min_cycles) min_cycles = current_cycles; \
    } \
    printf("function '%s' x%-10d ", STRING_JOIN( STRING_JOIN( STRING_JOIN(#func_name, "("), #__VA_ARGS__  ) , ")" ), count); \
    printsl("|> min cycles: %", min_cycles); \
    printsl(" | min time: "); \
    print_benchmark_time(min_cycles); \
} while(0)

/*
Macro to repeatedly test a function n times with different inputs. Will record min, avg and max 
execution time for each run and display it at the end of the whole test.
Simply call BENCHMARK_VOID_MANY_INPUTS with the number of repetitions per test,
the function to test and the inputs to each test (1 input per test)
Only works with functions that return void and have 1 input.
If you function returns something you can use BENCHMARK_MANY_INPUTS instead.
Functions with multiple inputs are NOT supported.

Example:
BENCHMARK_VOID_MANY_INPUTS(10000, func_to_test, "a", "a string", "a longer string", "a very very long string");
*/

#define GET_FIRST(PAR, ...) PAR
#define HAS_PARAMETER(...) GET_FIRST(placeholder, ##__VA_ARGS__, 0)

#define BENCHMARK_VOID_MANY_INPUTS(count, func_name, ...) \
do { \
    decltype(MSVC_BUG(GET_FIRST, (__VA_ARGS__))) tests_inputs[] = {__VA_ARGS__}; \
    const char* tests_inputs_names[] = {STRINGIFY(__VA_ARGS__)}; \
    const int TESTS_INPUTS_COUNT = NUM_ARGS(__VA_ARGS__); \
    u64 tests_mins[TESTS_INPUTS_COUNT] = {}; \
    for(int test_index = 0; test_index < TESTS_INPUTS_COUNT; test_index++) { \
        for(int rep = 0; rep < count; rep++) { \
            u64 start = read_cpu_timer(); \
            func_name(tests_inputs[test_index]); \
            u64 current_cycles = read_cpu_timer() - start; \
            if(rep == 0 || current_cycles < tests_mins[test_index]) tests_mins[test_index] = current_cycles; \
        } \
        print("completed testing with input %", tests_inputs_names[test_index]); \
    } \
    print("function '%' tested x% times", STRING_JOIN( STRING_JOIN( STRING_JOIN(#func_name, "("), #__VA_ARGS__  ) , ")" ), count); \
    for(int i = 0; i < TESTS_INPUTS_COUNT; i++) { \
        print("input '%': min=% cycles", tests_inputs_names[i], tests_mins[i]); \
    } \
} while (0)

// alternative to BENCHMARK_VOID_MANY_INPUTS for functions that return something,
// we use volatile so the code doesn't get optimized away
#define BENCHMARK_MANY_INPUTS(count, func_name, ...) \
do { \
    decltype(MSVC_BUG(GET_FIRST, (__VA_ARGS__))) tests_inputs[] = {__VA_ARGS__}; \
    const char* tests_inputs_names[] = {STRINGIFY(__VA_ARGS__)}; \
    const int TESTS_INPUTS_COUNT = NUM_ARGS(__VA_ARGS__); \
    u64 tests_mins[TESTS_INPUTS_COUNT] = {}; \
    for(int test_index = 0; test_index < TESTS_INPUTS_COUNT; test_index++) { \
        for(int rep = 0; rep < count; rep++) { \
            u64 start = read_cpu_timer(); \
            volatile auto temp = func_name(tests_inputs[test_index]); \
            u64 current_cycles = read_cpu_timer() - start; \
            if(rep == 0 || current_cycles < tests_mins[test_index]) tests_mins[test_index] = current_cycles; \
        } \
        print("completed testing with input %", tests_inputs_names[test_index]); \
    } \
    print("function '%' tested x% times", STRING_JOIN( STRING_JOIN( STRING_JOIN(#func_name, "("), #__VA_ARGS__  ) , ")" ), count); \
    for(int i = 0; i < TESTS_INPUTS_COUNT; i++) { \
        print("input '%': min=% cycles", tests_inputs_names[i], tests_mins[i]); \
    } \
} while (0)

/*
Runs a given function with given inputs many times and tells statistics.
Will run the function until there's a run of 10 seconds with no changes to min execution time.
If your function returns void you should use BENCHMARK_VOID_FUNC.

Usage: BENCHMARK_FUNC(func_to_test, input_to_func);
*/
#define BENCHMARK_FUNC(func_name, ...) \
{ \
    u64 freq = estimate_cpu_frequency(); \
    u64 timer_start = read_cpu_timer(); \
    u64 min_time = MAX_U64; \
    u64 count = 0; \
    while(true) { \
        u64 current_time = read_cpu_timer(); \
        f64 seconds_elapsed = (current_time - timer_start) / freq; \
        if(seconds_elapsed > 10.0f) break; \
        u64 start = read_cpu_timer(); \
        volatile auto temp = func_name(__VA_ARGS__); \
        u64 end = read_cpu_timer(); \
        u64 elapsed_time = end - start; \
        if(elapsed_time < min_time) { \
            min_time = elapsed_time; \
            timer_start = read_cpu_timer(); \
            print("new min time: %", min_time); \
        } \
        count++; \
    } \
    print("function '%' tested x% times", STRING_JOIN( STRING_JOIN( STRING_JOIN(#func_name, "("), #__VA_ARGS__  ) , ")" ), count); \
    print("    min: % cycles", min_time); \
}

// Alternative to BENCHMARK_FUNC for functions returning void,
// if your function returns something you should use BENCHMARK_FUNC
#define BENCHMARK_VOID_FUNC(func_name, ...) \
{ \
    u64 freq = estimate_cpu_frequency(); \
    u64 timer_start = read_cpu_timer(); \
    u64 min_time = MAX_U64; \
    u64 count = 0; \
    while(true) { \
        u64 current_time = read_cpu_timer(); \
        f64 seconds_elapsed = (current_time - timer_start) / freq; \
        if(seconds_elapsed > 10.0f) break; \
        u64 start = read_cpu_timer(); \
        func_name(__VA_ARGS__); \
        u64 end = read_cpu_timer(); \
        u64 elapsed_time = end - start; \
        if(elapsed_time < min_time) { \
            min_time = elapsed_time; \
            timer_start = read_cpu_timer(); \
            print("new min time: %", min_time); \
        } \
        count++; \
    } \
    print("function '%' tested x% times", STRING_JOIN( STRING_JOIN( STRING_JOIN(#func_name, "("), #__VA_ARGS__  ) , ")" ), count); \
    print("    min: % cycles", min_time); \
}

// TODO(cogno): change BENCHMARK_COMPARE_VOID and BENCHMARK_COMPARE to continue running until they have 10 seconds of the same best time
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
    for(int i = 0; i < count; i++) { \
        bool switchup = random_bool(); \
        u64 current_cycles_f1, current_cycles_f2; \
        if (switchup) { \
            u64 start_f1 = read_cpu_timer(); \
            func1(__VA_ARGS__); \
            current_cycles_f1 = read_cpu_timer() - start_f1; \
            u64 start_f2 = read_cpu_timer(); \
            func2(__VA_ARGS__); \
            current_cycles_f2 = read_cpu_timer() - start_f2; \
        } else { \
            u64 start_f2 = read_cpu_timer(); \
            func2(__VA_ARGS__); \
            current_cycles_f2 = read_cpu_timer() - start_f2; \
            u64 start_f1 = read_cpu_timer(); \
            func1(__VA_ARGS__); \
            current_cycles_f1 = read_cpu_timer() - start_f1; \
        } \
        if(current_cycles_f1 < min_cycles_f1) min_cycles_f1 = current_cycles_f1; \
        if(current_cycles_f2 < min_cycles_f2) min_cycles_f2 = current_cycles_f2; \
    } \
    auto f1_name = STRING_JOIN( STRING_JOIN( STRING_JOIN(#func1, "("), #__VA_ARGS__  ) , ")" ); \
    auto f2_name = STRING_JOIN( STRING_JOIN( STRING_JOIN(#func2, "("), #__VA_ARGS__  ) , ")" ); \
    printsl("function '%' x% ", f1_name, count); \
    printsl("|> cycles: %", min_cycles_f1); \
    printsl(" | time: "); \
    print_benchmark_time(min_cycles_f1); \
    printsl("function '%' x% ", f2_name, count); \
    printsl("|> cycles: %", min_cycles_f2); \
    printsl(" | time: "); \
    print_benchmark_time(min_cycles_f2); \
    print("final result when comparing '%' againts '%':", f2_name, f1_name); \
    if(min_cycles_f1 < min_cycles_f2) print("    % more cycles (x% slowdown)", min_cycles_f2 - min_cycles_f1, (f64)min_cycles_f2 / min_cycles_f1); \
    else                              print("    % less cycles (x% speedup)", min_cycles_f1 - min_cycles_f2, (f64)min_cycles_f1 / min_cycles_f2); \
} while(0)

// Alternative to BENCHMARK_COMPARE_VOID where the input functions return some values.
// This macro prevents the compiler/optimizer from removing that output, which could
// interfere with your analysis.
#define BENCHMARK_COMPARE(count, func1, func2, ...) \
do { \
    u64 min_cycles_f1 = MAX_U64; \
    u64 min_cycles_f2 = MAX_U64; \
    for(int i = 0; i < count; i++) { \
        bool switchup = random_bool(); \
        u64 current_cycles_f1, current_cycles_f2; \
        if (switchup) { \
            u64 start_f1 = read_cpu_timer(); \
            volatile auto temp1 = func1(__VA_ARGS__); \
            current_cycles_f1 = read_cpu_timer() - start_f1; \
            u64 start_f2 = read_cpu_timer(); \
            volatile auto temp2 = func2(__VA_ARGS__); \
            current_cycles_f2 = read_cpu_timer() - start_f2; \
        } else { \
            u64 start_f2 = read_cpu_timer(); \
            volatile auto temp2 = func2(__VA_ARGS__); \
            current_cycles_f2 = read_cpu_timer() - start_f2; \
            u64 start_f1 = read_cpu_timer(); \
            volatile auto temp1 = func1(__VA_ARGS__); \
            current_cycles_f1 = read_cpu_timer() - start_f1; \
        } \
        if(current_cycles_f1 < min_cycles_f1) min_cycles_f1 = current_cycles_f1; \
        if(current_cycles_f2 < min_cycles_f2) min_cycles_f2 = current_cycles_f2; \
    } \
    auto f1_name = STRING_JOIN( STRING_JOIN( STRING_JOIN(#func1, "("), #__VA_ARGS__  ) , ")" ); \
    auto f2_name = STRING_JOIN( STRING_JOIN( STRING_JOIN(#func2, "("), #__VA_ARGS__  ) , ")" ); \
    printsl("function '%' x% ", f1_name, count); \
    printsl("|> cycles: %", min_cycles_f1); \
    printsl(" | time: "); \
    print_benchmark_time(min_cycles_f1); \
    printsl("function '%' x% ", f2_name, count); \
    printsl("|> cycles: %", min_cycles_f2); \
    printsl(" | time: "); \
    print_benchmark_time(min_cycles_f2); \
    print("final result when comparing '%' againts '%':", f2_name, f1_name); \
    if(min_cycles_f1 < min_cycles_f2) print("    % more cycles (x% slowdown)", min_cycles_f2 - min_cycles_f1, (f64)min_cycles_f2 / min_cycles_f1); \
    else                              print("    % less cycles (x% speedup)", min_cycles_f1 - min_cycles_f2, (f64)min_cycles_f1 / min_cycles_f2); \
} while(0)

#else

#define BENCHMARK_FUNC(func_name, ...)
#define BENCHMARK_WITH_COUNT(count, func_name, ...)
#define BENCHMARK_VOID_FUNC(func_name, ...)
#define BENCHMARK_VOID_WITH_COUNT(count, func_name, ...)
#define BENCHMARK_COMPARE_VOID(count, func1, func2, ...)
#define BENCHMARK_COMPARE(count, func1, func2, ...)
#define BENCHMARK_VOID_MANY_INPUTS(count, func_name, ...)
#define BENCHMARK_MANY_INPUTS(count, func_name, ...)

#endif
