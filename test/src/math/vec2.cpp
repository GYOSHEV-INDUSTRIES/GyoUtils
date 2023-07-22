void test_vec2_math(){
    TITLE("Testing vec2 math");
    SECTION("vec2 sum",
        REQUIRE(vec2{1, 3} + vec2{5, 4}, vec2{6, 7});
        REQUIRE(vec2{-4, 2} + vec2{0, 3}, vec2{-4, 5});
    )
    SECTION("vec2 sub",
        REQUIRE(vec2{1, 3} - vec2{5, 4}, vec2{-4, -1});
        REQUIRE(vec2{-4, 2} - vec2{0, 3}, vec2{-4, -1});
    )
    SECTION("vec2 mul",
        REQUIRE(vec2{1, 3} * 3, vec2{3, 9});
        REQUIRE(vec2{5, 4} * -2, vec2{-10, -8});
    )
    SECTION("vec2 div",
        REQUIRE(vec2{4, 6} / 2, vec2{2, 3});
        REQUIRE(vec2{10, -15} / -5, vec2{-2, 3});
    )
    SECTION("vec2 negative",
        REQUIRE(-vec2{2, 5}, vec2{-2, -5});
        REQUIRE(-vec2{-4, 0}, vec2{4, 0});
    )
    SECTION("vec2 equal",
        REQUIRE(vec2{-5, 0} == vec2{-5, 0}, 1);
        REQUIRE(vec2{-5, 4} == vec2{-5, 0}, 0);
        REQUIRE(vec2{5, 4} == vec2{3, 4}, 0);
        REQUIRE(vec2{5, 4} == vec2{3, 3}, 0);
    )
    SECTION("vec2 not equal",
        REQUIRE(vec2{-5, 0} != vec2{-5, 0}, 0);
        REQUIRE(vec2{-5, 4} != vec2{-5, 0}, 1);
        REQUIRE(vec2{5, 4} != vec2{3, 4}, 1);
        REQUIRE(vec2{5, 4} != vec2{3, 3}, 1);
    )
    SECTION("vec2 rotate",
        REQUIRE(rotate(vec2{1, 0}, 0.25), vec2{0, 1});
        REQUIRE(rotate(vec2{1, 0}, 1), vec2{1, 0});
        REQUIRE(rotate(vec2{1, 0}, 0.125), vec2{cos(0.125), sin(0.125)});
    )
    SECTION("vec2 length",
        REQUIRE(length(vec2{2, 0}), 2.0f);
        REQUIRE(length(vec2{4, 0}), 4.0f);
        REQUIRE(length(vec2{3, 4}), 5.0f);
    )
    SECTION("vec2 normalize",
        REQUIRE(normalize(vec2{3, 0}), {1, 0});
        REQUIRE(normalize(vec2{2, 2}), {SQRT2 / 2, SQRT2 / 2});
    )
    SECTION("vec2 fast sum",
        vec2 a = vec2{-1, 5};
        a += vec2{2, 3};
        REQUIRE(a, vec2{1, 8});
        vec2 b = {5, -2};
        b += vec2{0, 1};
        REQUIRE(b, vec2{5, -1});
    )
    SECTION("vec2 fast sub",
        vec2 a = {-1, 5};
        a -= vec2{2, 3};
        REQUIRE(a, vec2{-3, 2});
        vec2 b = {5, -2,};
        b -= vec2{0, 1,};
        REQUIRE(b, vec2{5, -3});
    )
    // round (?)
    // floor (?)
    // ceil (?)
    // trunc (?)
}
