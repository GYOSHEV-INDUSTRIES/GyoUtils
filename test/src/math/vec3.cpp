void test_vec3_math(){
    TITLE("Testing vec3 math");
    SECTION("vec3 sum",
        REQUIRE(vec3{1, 3, 3} + vec3{5, 4, -3}, vec3{6, 7, 0});
        REQUIRE(vec3{-4, 2, 0} + vec3{0, 3, 2}, vec3{-4, 5, 2});
    )
    SECTION("vec3 sub",
        REQUIRE(vec3{1, 3, 1} - vec3{5, 4, 0}, vec3{-4, -1, 1});
        REQUIRE(vec3{-4, 2, 4} - vec3{0, 3, 2}, vec3{-4, -1, 2});
    )
    SECTION("vec3 mul",
        REQUIRE(vec3{1, 3, -3} * 3, vec3{3, 9, -9});
        REQUIRE(vec3{5, 4, 0} * -2, vec3{-10, -8, 0});
    )
    SECTION("vec3 div",
        REQUIRE(vec3{4, 6, 5} / 2, vec3{2, 3, 2.5f});
        REQUIRE(vec3{10, -15, 5} / -5, vec3{-2, 3, -1});
    )
    SECTION("vec3 negative",
        REQUIRE(-vec3{2, 5, 1}, vec3{-2, -5, -1});
        REQUIRE(-vec3{-4, 0, 2}, vec3{4, 0, -2});
    )
    SECTION("vec3 fast sum",
        vec3 a = vec3{-1, 5, 3};
        a += vec3{2, 3, 2};
        REQUIRE(a, vec3{1, 8, 5});
        vec3 b = {5, -2, 0};
        b += vec3{0, 1, 1};
        REQUIRE(b, vec3{5, -1, 1});
    )
    SECTION("vec3 fast sub",
        vec3 a = {-1, 5, 4};
        a -= vec3{2, 3, 2};
        REQUIRE(a, vec3{-3, 2, 2});
        vec3 b = {5, -2, -3};
        b -= vec3{0, 1, -6};
        REQUIRE(b, vec3{5, -3, 3});
    )
    SECTION("vec3 equal",
        REQUIRE(vec3{0, 0, 0} == vec3{0, 0, 0}, 1);
        REQUIRE(vec3{0, 0, 1} == vec3{0, 0, 0}, 0);
        REQUIRE(vec3{0, 1, 0} == vec3{0, 0, 0}, 0);
        REQUIRE(vec3{0, 1, 1} == vec3{0, 0, 0}, 0);
        REQUIRE(vec3{1, 0, 0} == vec3{0, 0, 0}, 0);
        REQUIRE(vec3{1, 0, 1} == vec3{0, 0, 0}, 0);
        REQUIRE(vec3{1, 1, 0} == vec3{0, 0, 0}, 0);
        REQUIRE(vec3{1, 1, 1} == vec3{0, 0, 0}, 0);
    )
    SECTION("vec3 not equal",
        REQUIRE(vec3{0, 0, 0} != vec3{0, 0, 0}, 0);
        REQUIRE(vec3{0, 0, 1} != vec3{0, 0, 0}, 1);
        REQUIRE(vec3{0, 1, 0} != vec3{0, 0, 0}, 1);
        REQUIRE(vec3{0, 1, 1} != vec3{0, 0, 0}, 1);
        REQUIRE(vec3{1, 0, 0} != vec3{0, 0, 0}, 1);
        REQUIRE(vec3{1, 0, 1} != vec3{0, 0, 0}, 1);
        REQUIRE(vec3{1, 1, 0} != vec3{0, 0, 0}, 1);
        REQUIRE(vec3{1, 1, 1} != vec3{0, 0, 0}, 1);
    )
    SECTION("vec3 length",
        REQUIRE(length(vec3{2, 0, 0}), 2.0f);
        REQUIRE(length(vec3{4, 0, 0}), 4.0f);
        REQUIRE(length(vec3{sqrt(2), 3, 5}), 6.0f);
    )
    SECTION("vec3 normalize",
        REQUIRE(normalize(vec3{3, 0, 0}), {1, 0, 0});
        REQUIRE(normalize(vec3{2, 2, 2}), {SQRT3 / 3, SQRT3 / 3, SQRT3 / 3});
    )
    // round (?)
    // floor (?)
    // ceil (?)
    // trunc (?)
}
