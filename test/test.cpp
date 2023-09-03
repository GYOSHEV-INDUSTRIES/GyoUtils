#define sin __sin
#define cos __cos
#define tan __tan
#define sqrt __sqrt
#include <chrono>
#undef sqrt
#undef sin
#undef cos
#undef tan

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../gyoutils.h"

#define PRINT_TESTS
#include "src/lib.cpp"

#include "src/math/simple.cpp"
#include "src/math/vec2.cpp"
#include "src/math/vec3.cpp"
#include "src/math/vec4.cpp"
#include "src/math/mat4.cpp"

#include "src/prints.cpp"


void test_func(int a, int b) {
    PROFILE_FUNC;
    print("hello world");
}

int main() {
    test_func(10, 20);
    // test_simple_math();
    // test_vec2_math();
    // test_vec3_math();
    // test_vec4_math();
    // test_mat4_math();
    
    // print("\nAll % tests passed succesfully", _cnt);
    return 0;
}
