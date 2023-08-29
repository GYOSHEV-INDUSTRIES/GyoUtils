void test_simple_math(){
    TITLE("Testing simple math");
    SECTION("npow",
        REQUIRE(npow(2, 2), 4);
        REQUIRE(npow(4, 2), 16);
        REQUIRE(npow(5, 3), 125);
        REQUIRE(npow(2.2, 2), 4.84f);
    )
    SECTION("min",
        REQUIRE(min(2, 4), 2);
        REQUIRE(min(0, 3), 0);
        REQUIRE(min(-4, 5), -4);
        REQUIRE(min(-4, -2), -4);
        REQUIRE(min(0, 0), 0);
    )
    SECTION("max",
        REQUIRE(max(2, 4), 4);
        REQUIRE(max(0, 3), 3);
        REQUIRE(max(-4, 5), 5);
        REQUIRE(max(-4, -2), -2);
        REQUIRE(max(0, 0), 0);
    )
    SECTION("abs",
        REQUIRE(abs(4), 4);
        REQUIRE(abs(0), 0);
        REQUIRE(abs(-4), 4);
    )
    SECTION("sgn",
        REQUIRE(sgn(4), 1);
        REQUIRE(sgn(0), 0);
        REQUIRE(sgn(-4), -1);
    )
    SECTION("clamp",
        REQUIRE(clamp(0, -2, 2), 0);
        REQUIRE(clamp(1, -2, 2), 1);
        REQUIRE(clamp(3, -2, 2), 2);
        REQUIRE(clamp(-4, -2, 2), -2);
    )
    SECTION("floor",
        REQUIRE(floor(0.5), 0);
        REQUIRE(floor(5.2), 5);
        REQUIRE(floor(-1.2), -2);
        REQUIRE(floor(-0.4), -1);
    )
    SECTION("ceil",
        REQUIRE(ceil(0.5), 1);
        REQUIRE(ceil(5.2), 6);
        REQUIRE(ceil(-1.2), -1);
        REQUIRE(ceil(-0.5), 0);
    )
    SECTION("round",
        REQUIRE(round(0.5), 1);
        REQUIRE(round(5.2), 5);
        REQUIRE(round(-1.2), -1);
    )
    SECTION("roundn",
        REQUIRE(roundn(1.245, 2), 1.25);
        REQUIRE(roundn(1.249, 2), 1.25);
        REQUIRE(roundn(1.152, 1), 1.2f);
        REQUIRE(roundn(1.152125, 3), 1.152f);
    )
    SECTION("fmod",
        REQUIRE(fmod(1.0f, 3.0f), 1.0f);
        REQUIRE(fmod(1.23f, 2.34f), 1.23f);
        REQUIRE(roundn(fmod(5.24f, 1.0f), 2), 0.24f);
        REQUIRE(fmod(-0.1f, 0.5f), -0.1f);
        REQUIRE(roundn(fmod(-2.53f, 0.5f), 2), -0.03f);
    )
    // lerp (?)
    SECTION("sqrt",
        REQUIRE(sqrt(4), 2);
        REQUIRE(sqrt(9), 3);
        REQUIRE(sqrt(16), 4);
        REQUIRE(sqrt(2), SQRT2);
        REQUIRE(sqrt(3), SQRT3);
    )
    SECTION("sin",
        REQUIRE(sin(-1.25), -1);
        REQUIRE(sin(-1), 0);
        REQUIRE(sin(-0.75), 1);
        REQUIRE(sin(-0.5), 0);
        REQUIRE(sin(-0.25), -1);
        REQUIRE(sin(0), 0);
        REQUIRE(sin(0.25), 1);
        REQUIRE(sin(0.5), 0);
        REQUIRE(sin(0.75), -1);
        REQUIRE(sin(1), 0);
        REQUIRE(sin(1.25), 1);
    )
    SECTION("cos",
        REQUIRE(cos(-1), 1);
        REQUIRE(cos(-0.75), 0);
        REQUIRE(cos(-0.5), -1);
        REQUIRE(cos(-0.25), 0);
        REQUIRE(cos(0), 1);
        REQUIRE(cos(0.25), 0);
        REQUIRE(cos(0.5), -1);
        REQUIRE(cos(0.75), 0);
        REQUIRE(cos(1), 1);
    )
}
