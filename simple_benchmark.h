
#ifndef SIMPLE_BENCHMARK
#define SIMPLE_BENCHMARK 1
#endif

#if SIMPLE_BENCHMARK

#define BENCHMARK_WITH_COUNT(count, func_name, ...) \
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
    u8 digits_count = log10(count) + 1; \
    printf("function '%s' x%-10d ", STRING_JOIN( STRING_JOIN( STRING_JOIN(#func_name, "("), #__VA_ARGS__  ) , ")" ), count); \
    print("|> cycles (min, avg, max): %, %, %", min_cycles, total_cycles / count, max_cycles ); \
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

#else

#define BENCHMARK_FUNC(func_name, ...)

#endif
