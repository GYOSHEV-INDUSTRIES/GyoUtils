TEST_CASE("VEC2", "[single-file]"){
    SECTION("various"){
        REQUIRE(sizeof(vec2) == 8);
    }
    SECTION("vec2 sum"){
        vec2 a = {1, 3}, b = {5, 4};
        vec2 c = {-4, 2}, d = {0, 3};
        REQUIRE((a + b).x == 6);
        REQUIRE((a + b).y == 7);
        REQUIRE((c + d).x == -4);
        REQUIRE((c + d).y == 5);
    }
    SECTION("vec2 sub"){
        vec2 a = {1, 3}, b = {5, 4};
        vec2 c = {-4, 2}, d = {0, 3};
        REQUIRE((a - b).x == -4);
        REQUIRE((a - b).y == -1);
        REQUIRE((c - d).x == -4);
        REQUIRE((c - d).y == -1);
    }
    // ...
}
