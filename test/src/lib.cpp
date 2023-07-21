int _cnt;
int _local_cnt = 0;
inline void REQUIRE(float test, float expected){
    if(test == expected){
        ++_cnt;
        ++_local_cnt;
    }else{
        printf("Assertion failed, got %f instead of %f in REQUIRE %d\n", test, expected, _local_cnt + 1);
        exit(1);
    }
}
inline void REQUIRE(vec2 test, vec2 expected){
    if(test.x == expected.x && test.y == expected.y){
        ++_cnt;
        ++_local_cnt;
    }else{
        printf("Assertion failed, got (%f, %f) instead of (%f, %f) in REQUIRE %d\n", test.x, test.y, expected.x, expected.y, _local_cnt + 1);
        exit(1);
    }
}
inline void REQUIRE(vec3 test, vec3 expected){
    if(test.x == expected.x && test.y == expected.y && test.z == expected.z){
        ++_cnt;
        ++_local_cnt;
    }else{
        printf("Assertion failed, got (%f, %f, %f) instead of (%f, %f, %f) in REQUIRE %d\n", test.x, test.y, test.z, expected.x, expected.y, expected.z, _local_cnt + 1);
        exit(1);
    }
}
#define SECTION(sec)  printf("Testing section %-20s", sec);
#define COMPLETED()   printf("Passed %d tests\n", _local_cnt);  _local_cnt = 0
