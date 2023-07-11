#if 1
#include <stdio.h>

#include "utils.h"

// Todo(Quattro) use single-header called catch

void test_simple_operations(){
    printf("testing min function\n");
    {
        int cnt = 0;
        if(min(1, 2) != 1){cnt++;}
        if(min(1, 6) != 1){cnt++;}
        if(min(-2, 5) != -2){cnt++;}
        if(min(-5, -3) != -5){cnt++;}
        if(min(-3.12, 0) != -3.12){cnt++;}
        if(cnt == 0){printf("All tests passed succesfully\n");}
    }
    printf("testing max function\n");
    {
        int cnt = 0;
        if(max(1, 2) != 2){cnt++;}
        if(max(1, 6) != 6){cnt++;}
        if(max(-2, 5) != 5){cnt++;}
        if(max(-5, -3) != -3){cnt++;}
        if(max(-3.12, 0) != 0){cnt++;}
        if(cnt == 0){printf("All tests passed succesfully\n");}
    }
    
    // abs
    // sgn
    // clamp
    // trunc
    printf("testing fmod function\n");
    printf("fmod(%.2f, %.2f) = %.2f == %.2f\n", 1.0f, 3.0f, fmod(1, 3), 1.0f);
    printf("fmod(%.2f, %.2f) = %.2f == %.2f\n", 1.23, 2.34, fmod(1.23, 2.34), 1.23);
    printf("fmod(%.2f, %.2f) = %.2f == %.2f\n", 5.24, 1.0f, fmod(5.24, 1), 0.24);
    printf("fmod(%.2f, %.2f) = %.2f == %.2f\n", -0.1, 0.5, fmod(-0.1, 0.5), -0.1);
    printf("fmod(%.2f, %.2f) = %.2f == %.2f\n", -2.53, 0.5, fmod(-2.53, 0.5), -0.03);
}

void test_sin_and_cos(){
    for(int i = -100; i < 101; i++){
        printf("%f %f %f\n", i / 100.0f, fast_sin(i / 100.0f), fast_cos(i / 100.0f));
    }
}

struct ExampleStruct {
    int c = 5;
};

void test_print(){
    print(15);
    print("this is a string");
    print("This is the number %", 12.3f);
    print("this has many numbers like % and % and even %\%", 5, 12.125, 65.3);
    print('c');
    print('d');
    print(15.2);
    print(true);
    print(false);
    int a = 12;
    print(a);
    print(&a);
    print("my final message");
    
    printsl(12);
    printsl(' ');
    printsl("log message ");
    printsl(true);
    printsl(" and then some stuff like % and %\% and the symbol \%", false, 12.5);
    print("and then y try to break it with \\");
    
    ExampleStruct ex = {};
    print("and finally I print something unknown like %", ex);
    print("%", vec2{1, 2});
}

void test_math(){
    // print(vec2{1, 2} != vec2{1, 2});
    // print(vec2{1, 2} != vec2{2, 2});
    // print(vec2{1, 2} != vec2{1, 4});
    // print(vec2{1, 2} != vec2{2, 5});
    // print(vec2{1, 2} == vec2{1, 2});
    // print(vec2{1, 2} == vec2{2, 2});
    // print(vec2{1, 2} == vec2{1, 4});
    // print(vec2{1, 2} == vec2{2, 5});
}

int main(){
    // test_simple_operations();
    // test_sin_and_cos();
    // test_print();
    test_math();
    
    return 0;
}
#else
// benchmark module
#include <stdio.h>
#include <chrono>
#include "first.h"
#include "math.h"

using namespace std;
using namespace std::chrono;
// gcc test.cpp -lstdc++ -march=native
int main(){
    
    mat4 a = {};
    auto start = high_resolution_clock::now();
    for(int i = 0; i < 1000000; i++){
        a = a + mat4_identity();
    }
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    print(a);
    print(mat4_identity());
    printf("transpose sum: %lldns\n", duration.count());
    return 0;
}
#endif
