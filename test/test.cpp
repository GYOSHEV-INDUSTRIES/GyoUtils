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
    volatile int c = a + b;
}

int test_another(int a, int b) {
    return cos(a) + 2 * sin(b) * cos(a * a * sin (b * b));
}

void test_func(mat4 &a) {
    _MM_TRANSPOSE4_PS(a.r1, a.r2, a.r3, a.r4);
}

vec4 test_func_empty(mat4 &a) {
    vec4 res;
    return res;
}

int main() {
    
    // BENCHMARK_FUNC(test_func, 10, 20);
    // BENCHMARK_FUNC(test_another, a, b);
    auto a = mat4_new(4);
    a.m23 = 25;
    print("t1: \n%", a);
    // BENCHMARK_VOID_FUNC(test_func, a);
    BENCHMARK_FUNC(test_func_empty, a);
    print("t2: \n%", a);
    // BENCHMARK_FUNC(test_func_empty, a);
    
    // test_simple_math();
    // test_vec2_math();
    // test_vec3_math();
    // test_vec4_math();
    // test_mat4_math();
    
    // print("\nAll % tests passed succesfully", _cnt);
    return 0;
}
