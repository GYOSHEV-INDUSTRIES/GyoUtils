void test_vec4_math(){
    TITLE("Testing vec4 math");
    SECTION("vec4 sum",
        REQUIRE(vec4{1, 3, 3, -2} + vec4{5, 4, -3, 0}, vec4{6, 7, 0, -2});
        REQUIRE(vec4{-4, 2, 0, 1} + vec4{0, 3, 2, 2}, vec4{-4, 5, 2, 3});
    )
    SECTION("vec4 sub",
        REQUIRE(vec4{1, 3, 1, 2} - vec4{5, 4, 0, -4}, vec4{-4, -1, 1, 6});
        REQUIRE(vec4{-4, 2, 4, 1} - vec4{0, 3, 2, 0}, vec4{-4, -1, 2, 1});
    )
    SECTION("vec4 mul",
        REQUIRE(vec4{1, 3, -3, 4} * 3, vec4{3, 9, -9, 12});
        REQUIRE(vec4{5, 4, 0, 1} * -2, vec4{-10, -8, 0, -2});
    )
    SECTION("vec4 div",
        REQUIRE(vec4{4, 6, 5, -3} / 2, vec4{2, 3, 2.5f, -1.5f});
        REQUIRE(vec4{10, -15, 5, 0} / -5, vec4{-2, 3, -1, 0});
    )
    SECTION("vec4 negative",
        REQUIRE(-vec4{2, 5, 1, -3}, vec4{-2, -5, -1, 3});
        REQUIRE(-vec4{-4, 0, 2, 1}, vec4{4, 0, -2, -1});
    )
    SECTION("vec4 fast sum",
        vec4 a = vec4{-1, 5, 3, -2};
        a += vec4{2, 3, 2, 1};
        REQUIRE(a, vec4{1, 8, 5, -1});
        vec4 b = {5, -2, 0, 1};
        b += vec4{0, 1, 1, -4};
        REQUIRE(b, vec4{5, -1, 1, -3});
    )
    SECTION("vec4 fast sub",
        vec4 a = {-1, 5, 4, -3};
        a -= vec4{2, 3, 2, -2};
        REQUIRE(a, vec4{-3, 2, 2, -1});
        vec4 b = {5, -2, -3, 2};
        b -= vec4{0, 1, -6, 5};
        REQUIRE(b, vec4{5, -3, 3, -3});
    )
    SECTION("vec4 equal",
        REQUIRE(vec4{0, 0, 0, 0} == vec4{0, 0, 0, 0}, 1);
        REQUIRE(vec4{0, 0, 0, 1} == vec4{0, 0, 0, 0}, 0);
        REQUIRE(vec4{0, 0, 1, 0} == vec4{0, 0, 0, 0}, 0);
        REQUIRE(vec4{0, 0, 1, 1} == vec4{0, 0, 0, 0}, 0);
        REQUIRE(vec4{0, 1, 0, 0} == vec4{0, 0, 0, 0}, 0);
        REQUIRE(vec4{0, 1, 0, 1} == vec4{0, 0, 0, 0}, 0);
        REQUIRE(vec4{0, 1, 1, 0} == vec4{0, 0, 0, 0}, 0);
        REQUIRE(vec4{0, 1, 1, 1} == vec4{0, 0, 0, 0}, 0);
        REQUIRE(vec4{1, 0, 0, 0} == vec4{0, 0, 0, 0}, 0);
        REQUIRE(vec4{1, 0, 0, 1} == vec4{0, 0, 0, 0}, 0);
        REQUIRE(vec4{1, 0, 1, 0} == vec4{0, 0, 0, 0}, 0);
        REQUIRE(vec4{1, 0, 1, 1} == vec4{0, 0, 0, 0}, 0);
        REQUIRE(vec4{1, 1, 0, 0} == vec4{0, 0, 0, 0}, 0);
        REQUIRE(vec4{1, 1, 0, 1} == vec4{0, 0, 0, 0}, 0);
        REQUIRE(vec4{1, 1, 1, 0} == vec4{0, 0, 0, 0}, 0);
        REQUIRE(vec4{1, 1, 1, 1} == vec4{0, 0, 0, 0}, 0);
    )
    SECTION("vec4 not equal",
        REQUIRE(vec4{0, 0, 0, 0} != vec4{0, 0, 0, 0}, 0);
        REQUIRE(vec4{0, 0, 0, 1} != vec4{0, 0, 0, 0}, 1);
        REQUIRE(vec4{0, 0, 1, 0} != vec4{0, 0, 0, 0}, 1);
        REQUIRE(vec4{0, 0, 1, 1} != vec4{0, 0, 0, 0}, 1);
        REQUIRE(vec4{0, 1, 0, 0} != vec4{0, 0, 0, 0}, 1);
        REQUIRE(vec4{0, 1, 0, 1} != vec4{0, 0, 0, 0}, 1);
        REQUIRE(vec4{0, 1, 1, 0} != vec4{0, 0, 0, 0}, 1);
        REQUIRE(vec4{0, 1, 1, 1} != vec4{0, 0, 0, 0}, 1);
        REQUIRE(vec4{1, 0, 0, 0} != vec4{0, 0, 0, 0}, 1);
        REQUIRE(vec4{1, 0, 0, 1} != vec4{0, 0, 0, 0}, 1);
        REQUIRE(vec4{1, 0, 1, 0} != vec4{0, 0, 0, 0}, 1);
        REQUIRE(vec4{1, 0, 1, 1} != vec4{0, 0, 0, 0}, 1);
        REQUIRE(vec4{1, 1, 0, 0} != vec4{0, 0, 0, 0}, 1);
        REQUIRE(vec4{1, 1, 0, 1} != vec4{0, 0, 0, 0}, 1);
        REQUIRE(vec4{1, 1, 1, 0} != vec4{0, 0, 0, 0}, 1);
        REQUIRE(vec4{1, 1, 1, 1} != vec4{0, 0, 0, 0}, 1);
    )
    SECTION("vec4 length",
        REQUIRE(vec4_length(vec4{2, 0, 0, 0}), 2.0f);
        REQUIRE(vec4_length(vec4{4, 0, 0, 0}), 4.0f);
        REQUIRE(vec4_length(vec4{sqrt(8), 6, -2, 1}), 7.0f);
    )
    SECTION("vec4 normalize",
        REQUIRE(vec4_normalize(vec4{3, 0, 0, 0}), {1, 0, 0, 0});
        REQUIRE(vec4_normalize(vec4{2, 2, 2, 0}), {SQRT3 / 3, SQRT3 / 3, SQRT3 / 3, 0});
    )
    // round (?)
    // floor (?)
    // ceil (?)
    // trunc (?)
}
