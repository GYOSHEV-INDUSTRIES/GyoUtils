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
#include "src/unicode.cpp"
#include "src/str.cpp"

#include "src/prints.cpp"

#include "../win64_files.h"
#include <Windows.h>

bool test(int a) {
    print("a %", a);
    defer { print("b %", a); print("another one"); };
    
    if(a < 3) return false;
    
    print("c %", a);
    defer { print("d %", a); };
    
    return true;
}


// 58.51us
void c_printf() { 
    printf("Lorem ipsum dolor sit amet, consectetur adipiscing elit. Nam semper tempor justo, eget efficitur lorem ultricies non. Duis tempor feugiat tortor et malesuada. Aenean malesuada, massa ut pretium vestibulum, quam dui suscipit sapien, a malesuada nisl sem at felis. Aliquam erat volutpat. Mauris ac tellus scelerisque, euismod turpis id, porttitor metus. Fusce aliquet, arcu ac cursus finibus, eros justo lobortis massa, at semper felis arcu vitae lorem. Nunc eget mauris velit. Fusce dignissim faucibus ante, nec tristique nisi euismod nec. Aenean dignissim, mauris sed venenatis porta, libero augue ornare urna, quis venenatis dui mi sed ante.%d\n", 15);
}

void gyoutils_old() {
    print("Lorem ipsum dolor sit amet, consectetur adipiscing elit. Nam semper tempor justo, eget efficitur lorem ultricies non. Duis tempor feugiat tortor et malesuada. Aenean malesuada, massa ut pretium vestibulum, quam dui suscipit sapien, a malesuada nisl sem at felis. Aliquam erat volutpat. Mauris ac tellus scelerisque, euismod turpis id, porttitor metus. Fusce aliquet, arcu ac cursus finibus, eros justo lobortis massa, at semper felis arcu vitae lorem. Nunc eget mauris velit. Fusce dignissim faucibus ante, nec tristique nisi euismod nec. Aenean dignissim, mauris sed venenatis porta, libero augue ornare urna, quis venenatis dui mi sed ante.%", 15);
}

// 56.95us
void print_new_strat() {
    print("Lorem ipsum dolor sit amet, consectetur adipiscing elit. Nam semper tempor justo, eget efficitur lorem ultricies non. Duis tempor feugiat tortor et malesuada. Aenean malesuada, massa ut pretium vestibulum, quam dui suscipit sapien, a malesuada nisl sem at felis. Aliquam erat volutpat. Mauris ac tellus scelerisque, euismod turpis id, porttitor metus. Fusce aliquet, arcu ac cursus finibus, eros justo lobortis massa, at semper felis arcu vitae lorem. Nunc eget mauris velit. Fusce dignissim faucibus ante, nec tristique nisi euismod nec. Aenean dignissim, mauris sed venenatis porta, libero augue ornare urna, quis venenatis dui mi sed ante.%", 15);
}

// 58.37us
void print_with_builder() {
    // print_builder("Lorem ipsum dolor sit amet, consectetur adipiscing elit. Nam semper tempor justo, eget efficitur lorem ultricies non. Duis tempor feugiat tortor et malesuada. Aenean malesuada, massa ut pretium vestibulum, quam dui suscipit sapien, a malesuada nisl sem at felis. Aliquam erat volutpat. Mauris ac tellus scelerisque, euismod turpis id, porttitor metus. Fusce aliquet, arcu ac cursus finibus, eros justo lobortis massa, at semper felis arcu vitae lorem. Nunc eget mauris velit. Fusce dignissim faucibus ante, nec tristique nisi euismod nec. Aenean dignissim, mauris sed venenatis porta, libero augue ornare urna, quis venenatis dui mi sed ante.%", 15);
}

// 22.88 s
void printf_unbuffered() {
    for(int i=0;i<=1000000;i++) {
        printf("%d",i);
    }
}

// 19.57 s
void gyoprint() {
    for(int i=0;i<=1000000;i++) {
        printsl(i);
    }
}

char buffer[256];
void printf_buffered() {
    setvbuf(stdout, buffer, _IOFBF, sizeof(buffer));
    for(int i=0;i<=1000000;i++) {
        printf("%d",i);
    }
}

void normal_printf() {
    printf("example\n");
}

void full_buffered_printf() {
    setvbuf(stdout, buffer, _IOFBF, sizeof(buffer));
    printf("example\n");
    fflush(stdout);
}



// 56.77
void print_new_v2() {
    print2("Lorem ipsum dolor sit amet, consectetur adipiscing elit. Nam semper tempor justo, eget efficitur lorem ultricies non. Duis tempor feugiat tortor et malesuada. Aenean malesuada, massa ut pretium vestibulum, quam dui suscipit sapien, a malesuada nisl sem at felis. Aliquam erat volutpat. Mauris ac tellus scelerisque, euismod turpis id, porttitor metus. Fusce aliquet, arcu ac cursus finibus, eros justo lobortis massa, at semper felis arcu vitae lorem. Nunc eget mauris velit. Fusce dignissim faucibus ante, nec tristique nisi euismod nec. Aenean dignissim, mauris sed venenatis porta, libero augue ornare urna, quis venenatis dui mi sed ante.", 15);
}

char dest[20];

__declspec(noinline) void setup_test() {
    PROFILE_BLOCK("setup");
    dest[0] = 't';
    dest[1] = 'e';
    dest[2] = 's';
    dest[3] = 't';
    dest[4] = 'i';
    dest[5] = 'n';
    dest[6] = 'g';
}

__declspec(noinline) void memcpy_test() {
    auto start = read_cpu_timer();
    memcpy(dest, "0.00000", 7);
    auto end = read_cpu_timer();
    print("memcpy took % clocks to make '%'", end - start, dest);
}

__declspec(noinline) void dumb_shit() {
    auto start = read_cpu_timer();
    dest[0] = '0';
    dest[1] = '.';
    dest[2] = '0';
    dest[3] = '0';
    dest[4] = '0';
    dest[5] = '0';
    dest[6] = '0';
    auto end = read_cpu_timer();
    print("peak of performance took % clocks ot make '%'", end - start, dest);
}

int func_to_test(int thing) {
    int sum = 0;
    for(int i = 0; i < thing; i++) {
        sum += i;
    }
    return sum;
}


int main() {
    
    StrBuilder b = make_str_builder();
    {
        print("str builder pre");
        
        defer { 
            str_builder_free(&b);
        };
        
        str_builder_append(&b, "edit");
        str_builder_append(&b, "text");
        
        print("str builder post: %", str_builder_get_str(&b));
    }
    
    print("ptr: %", b.ptr);
    b.ptr[0] = 's'; // should explode
    
    // StrParser p = make_str_parser(b.ptr, b.size);
    // str_parser_check_magic(&p, "edit");
    // u8 version = str_parser_get_u8(&p);
    // ASSERT(version == 1, "wrong value");
    
    // setup_test();
    // memcpy_test();
    // dumb_shit();
    // memcpy_test();
    // dumb_shit();
    
    // BENCHMARK_MANY_INPUTS(10, func_to_test, 1, 10, 100, 1000, 10000, 100000, 1000000, 10000000, 100000000, 1000000000);
    
    // print("start");
    // BENCHMARK_FUNC(func_to_test, 100000000);
    // print("end");
    
    // BENCHMARK_VOID_WITH_COUNT(1000, fast_print_v2);
    // BENCHMARK_COMPARE_VOID(1000, c_printf, print_new_strat);
    
    // ASSERT(false, "testing");
    
    // printf("%.5f\n", (f32)111.0);
    // print((f32)11.12);
    // print_v2("the number ", 12, " and then the string ", "hello world");
    
    // init_print();
    
    // struct temp{
    //     int a = 15;
    // };
    // temp t = {};
    // print("%, (expected unknown type)", t);
    // print("%, (expected ptr)", &t);
    // print("% (expected vec2)", vec2{10, 20});
    
    /*
    print("u32:");
    print((u32)0);
    print((u32)20231);
    print((u32)MAX_U32);
    
    print("s32:");
    print((s32)MAX_S32);
    print((s32)2021);
    print((s32)0);
    print((s32)-1);
    print((s32)-2103);
    print((s32)MIN_S32);
    
    print("u16:");
    print((u16)20231);
    print((u16)0);
    print((u16)MAX_U16);
    
    print("s16:");
    print((s16)MAX_S16);
    print((s16)2021);
    print((s16)0);
    print((s16)-1);
    print((s16)-2103);
    print((s16)MIN_S16);
    
    print("u8:");
    print((u8)123);
    print((u8)0);
    print((u8)MAX_U8);
    
    print("s8:");
    print((s8)MAX_S8);
    print((s8)21);
    print((s8)0);
    print((s8)-1);
    print((s8)-103);
    print((s8)MIN_S8);
    
    print("u64:");
    print((u64)0);
    print((u64)20231);
    print((u64)MAX_U64);
    
    print("s64:");
    print((s64)MAX_S64);
    print((s64)21);
    print((s64)0);
    print((s64)-1);
    print((s64)-103);
    print((s64)MIN_S64);
    
    print(true);
    print(false);
    */
    
    // Array<s32> arr = array_new<s32>(5);
    // array_append(&arr, 10);
    // array_append(&arr, 15);
    // array_append(&arr, 20);
    
    // print("reserved %", arr.reserved_size);
    // array_reserve(&arr, 10);
    // array_append(&arr, 25);
    // array_append(&arr, 30);
    // array_append(&arr, 35);
    // array_append(&arr, 40);
    // array_append(&arr, 45);
    // array_append(&arr, 50);
    
    // For(arr) {
    //     print("arr[%]=%", it_index, it);
    // }
    
    // print("reserved %", arr.reserved_size);
    
    // {
    //     PROFILE_BLOCK("print");
    //     print("%, (expected ptr)", &t);
    // }
    
    
    // print("%, (expected int)", 15);
    // print("%, (expected char)", 'c');
    // print("%, (expected string)", "a sentence");
    
    // BENCHMARK_VOID_WITH_COUNT(10000, print_new_v2);
    
    // BENCHMARK_COMPARE_VOID(1000, print_new_strat, print_new_v2);
    
    // print_new_v2();
    // print_new_v2();
    // print_new_v2();
    // c_printf();
    
    // {
    //     PROFILE_BLOCK("printf");
    //     printf("this is a message\n");
    // }
    // {
    //     PROFILE_BLOCK("gyoprint");
    //     print_v2("this is a message");
    // }
    
    // {
    //     PROFILE_BLOCK("redirect 2");
    //     setvbuf(stdout, NULL, _IOLBF, BUFSIZ);
    // }
    // {
    //     PROFILE_BLOCK("redirect 3");
    //     setvbuf(stdout, __print_buff, _IOLBF, __BUFF_SIZE);
    // }
    // {
    //     PROFILE_BLOCK("redirect 4");
    //     setvbuf(stdout, NULL, _IOLBF, BUFSIZ);
    // }

    
    // BENCHMARK_VOID_WITH_COUNT(100000, print_new_v2);
    
    // StrBuilder b = make_str_builder(30);
    // str_builder_append(&b, "hello '");
    // str_builder_append(&b, (f32)123123123123123123123123123123.999);
    // str_builder_append(&b, '|');
    // str_builder_append(&b, (f64)123123123123123123123123123123.999);
    // str_builder_append(&b, "' world");
    // print(b);
    
    // print_buffered_v1("test %, test %, test", 1, 2);
    
    // {
    //     PROFILE_BLOCK("normal");
    //     normal_printf();
    // }
    
    // BENCHMARK_COMPARE_VOID(1, printf_unbuffered, printf_buffered);
    
    #if 0    
    Array<str> arr = array_new<str>(20);
    get_only_files_in_dir(".\\src", &arr);
    
    Array<str> arr2 = array_new<str>(20);
    get_only_files_in_dir(".\\src\\math", &arr2);
    
    print("CLASSIC FOR:");
    for(int i = 0; i < arr.size; i++) {
        print("file %=%", i, arr[i]);
    }
    
    print("\nCLASSIC FOR AGAIN:");
    for(int i = 0; i < arr.size; i++) {
        print("file %=%", i, arr[i]);
    }
    
    print("\nNEW FOR:");
    For(arr) {
        print("file %='%'", it_index, it);
    }
    
    print("\nNEW FOR AGAIN:");
    For(arr) {
        print("file %=%", it_index, it);
    }
    
    print("\nNEW FOR WITH PTR:");
    For_ptr(arr) {
        print("file %=%", it_index, *it);
    }
    
    print("\nNEW FOR REV:");
    For_rev(arr) {
        print("file %=%", it_index, it);
    }
    
    print("\nNEW FOR REV WITH PTR:");
    For_ptr_rev(arr) {
        print("file %=%", it_index, *it);
    }
    
    print("\nNEW FOR REV WITH PTR:");
    For_rev_ptr(arr) {
        print("file %=%", it_index, *it);
    }
    
    print("\nCLASSIC FOR 2:");
    for(int i = 0; i < arr.size; i++) {
        print("file %=%", i, arr[i]);
    }

    print("\nNEW FOR OF STR:");
    For(arr[0]) {
        print("file %=%", it_index, (char)it);
    }

    print("\nOLD FOR:");
    OLDFor(arr) {
        print("file %=%", it.index, it.value);
    }
    
    print("\nOLD FOR AGAIN:");
    OLDFor(arr) {
        print("file %=%", it.index, it.value);
    }
    #endif
    
    test_str();
    test_unicode();
    test_simple_math();
    test_vec2_math();
    test_vec3_math();
    test_vec4_math();
    test_mat4_math();
    
    // print("\nAll % tests passed succesfully", _cnt);
    return 0;
}
