void test_mat4_math(){
    TITLE("testing mat4 math");
    SECTION("mat4 new",
        REQUIRE(mat4_new(1), {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1});
        REQUIRE(mat4_new(4), {4, 0, 0, 0, 0, 4, 0, 0, 0, 0, 4, 0, 0, 0, 0, 4});
    )
    SECTION("transpose",
        REQUIRE(mat4_transpose(mat4_new(1)), mat4_new(1));
        REQUIRE(mat4_transpose({0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15}), {0, 4, 8, 12, 1, 5, 9, 13, 2, 6, 10, 14, 3, 7, 11, 15});
    )
    SECTION("add",
        REQUIRE(mat4_new(1) + mat4_new(2), mat4_new(3));
    )
    SECTION("sub",
        REQUIRE(mat4_new(5) - mat4_new(2), mat4_new(3));
    )
    SECTION("mul",
        REQUIRE(mat4_new(1) * 6, mat4_new(6));
    )
    SECTION("div",
        REQUIRE(mat4_new(8) / 2, mat4_new(4));
    )
    SECTION("matrix mul",
        REQUIRE(mat4_new(1) * mat4_new(1), mat4_new(1));
        mat4 a = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
        mat4 b = {0, 4, -6, 3, 5, 1, 0, -8, -3, 0, 2, 4, 0, 6, 2, 0};
        REQUIRE(a * b, {1, 30, 8, -1, 9, 74, 0, -5, 17, 118, -8, -9, 25, 162, -16, -13});
    )
    // ...
}
