int _cnt;
int _local_cnt = 0;
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
#ifdef PRINT_TESTS
#define SECTION(sec, ...) printf("Testing section %-20s", sec); {__VA_ARGS__}; print("Passed % tests", _local_cnt); _local_cnt = 0;
#define TITLE(t)        print("\n%s", t);
#else
#define SECTION(sec, ...)  {__VA_ARGS__}
#define COMPLETED()   _local_cnt = 0
#define TITLE(t)      ;
#endif
