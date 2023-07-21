#include <stdio.h>
#include <stdlib.h>

#include "../utils.h"

#include "src/lib.cpp"

#include "src/math/simple.cpp"
#include "src/math/vec2.cpp"
#include "src/math/vec3.cpp"

int main(){
    test_simple_math();
    test_vec2_math();
    test_vec3_math();
    
    printf("\nAll %d tests passed succesfully\n", _cnt);
    return 0;
}
