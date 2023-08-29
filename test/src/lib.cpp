int _cnt;
int _local_cnt = 1;
inline void REQUIRE(float test, float expected){
    if(test == expected){
        ++_cnt;
        ++_local_cnt;
    }else{
        print("Assertion failed, got % instead of % in REQUIRE %", test, expected, _local_cnt);
        exit(1);
    }
}
inline void REQUIRE(vec2 test, vec2 expected){
    if(test.x == expected.x && test.y == expected.y){
        ++_cnt;
        ++_local_cnt;
    }else{
        print("Assertion failed, got % instead of % in REQUIRE %", test, expected, _local_cnt);
        exit(1);
    }
}
inline void REQUIRE(vec3 test, vec3 expected){
    if(test.x == expected.x && test.y == expected.y && test.z == expected.z){
        ++_cnt;
        ++_local_cnt;
    }else{
        print("Assertion failed, got % instead of % in REQUIRE %", test, expected, _local_cnt);
        exit(1);
    }
}
inline void REQUIRE(vec4 test, vec4 expected){
    if(test.x == expected.x && test.y == expected.y && test.z == expected.z, test.w == expected.w){
        ++_cnt;
        ++_local_cnt;
    }else{
        print("Assertion failed, got % instead of % in REQUIRE %", test, expected, _local_cnt);
        exit(1);
    }
}
#define BENCHMARK(tests, amt, func, res_type, ...)                         \
    {Array<double> durations = array_new<double>(1);             \
    for(int k = 0; k < tests; k++){                              \
        auto start = std::chrono::high_resolution_clock::now();  \
        res_type res;                                            \
        for(int i = 0; i < amt; i++){                            \
            res += func(__VA_ARGS__);                            \
        }                                                        \
        auto stop = std::chrono::high_resolution_clock::now();   \
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start).count();  \
        printf("", res);                                              \
        array_append(&durations, duration / 1000.0);                  \
    }                                                                 \
    double min_value = durations[0];                                  \
    double max_value = durations[0];                                  \
    double average = 0;                                               \
    for(int i = 0; i < durations.size; i++){                          \
        if(durations[i] < min_value){ min_value = durations[i]; }     \
        if(durations[i] > max_value){ max_value = durations[i]; }     \
        average += durations[i];                                      \
    }                                                                 \
    print("Executed benchmark of function % % times", #func, tests);  \
    print("Fastest: %ms", min_value);                                 \
    print("Slowest: %ms", max_value);                                 \
    print("Average: %ms", average / tests);                           \
    array_free_all(&durations);}

#ifdef PRINT_TESTS
#define SECTION(sec, ...) printf("Testing section %-20s", sec); {__VA_ARGS__}; printf("Passed %d tests\n", _local_cnt - 1); _local_cnt = 1;
#define TITLE(t)        printf("\n%s\n", t);
#else
#define SECTION(sec, ...)  {__VA_ARGS__}
#define COMPLETED()   _local_cnt = 1
#define TITLE(t)
#endif
