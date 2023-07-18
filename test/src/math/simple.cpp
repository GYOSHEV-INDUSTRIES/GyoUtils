TEST_CASE("Simple Math", "[single-file]"){
    SECTION("npow"){
        REQUIRE(npow(2, 2) == 4);
        REQUIRE(npow(4, 2) == 16);
        REQUIRE(npow(5, 3) == 125);
        REQUIRE(npow(2.2, 2) == 4.84f);
    }
    SECTION("min"){
        REQUIRE(min(2, 4) == 2);
        REQUIRE(min(0, 3) == 0);
        REQUIRE(min(-4, 5) == -4);
        REQUIRE(min(-4, -2) == -4);
        REQUIRE(min(0, 0) == 0);
    }
    SECTION("max"){
        REQUIRE(max(2, 4) == 4);
        REQUIRE(max(0, 3) == 3);
        REQUIRE(max(-4, 5) == 5);
        REQUIRE(max(-4, -2) == -2);
        REQUIRE(max(0, 0) == 0);
    }
    SECTION("abs"){
        REQUIRE(abs(4) == 4);
        REQUIRE(abs(0) == 0);
        REQUIRE(abs(-4) == 4);
    }
    SECTION("sgn"){
        REQUIRE(sgn(4) == 1);
        REQUIRE(sgn(0) == 0);
        REQUIRE(sgn(-4) == -1);
    }
    SECTION("clamp"){
        REQUIRE(clamp(0, -2, 2) == 0);
        REQUIRE(clamp(1, -2, 2) == 1);
        REQUIRE(clamp(3, -2, 2) == 2);
        REQUIRE(clamp(-4, -2, 2) == -2);
    }
    SECTION("floor"){
        REQUIRE(math_floor(0.5) == 0);
        REQUIRE(math_floor(5.2) == 5);
        REQUIRE(math_floor(-1.2) == -2);
    }
    SECTION("ceil"){
        REQUIRE(math_ceil(0.5) == 1);
        REQUIRE(math_ceil(5.2) == 6);
        REQUIRE(math_ceil(-1.2) == -1);
    }
    SECTION("round"){
        REQUIRE(math_round(0.5) == 1);
        REQUIRE(math_round(5.2) == 5);
        REQUIRE(math_round(-1.2) == -1);
    }
    SECTION("roundn"){
        REQUIRE(math_roundn(1.245, 2) == 1.25);
        REQUIRE(math_roundn(1.249, 2) == 1.25);
        REQUIRE(math_roundn(1.152, 1) == 1.2f);
        REQUIRE(math_roundn(1.152125, 3) == 1.152f);
    }
    SECTION("fmod"){
        REQUIRE(fmod(1.0f, 3.0f) == 1.0f);
        REQUIRE(fmod(1.23f, 2.34f) == 1.23f);
        REQUIRE(math_roundn(fmod(5.24f, 1.0f), 2) == 0.24f);
        REQUIRE(fmod(-0.1f, 0.5f) == -0.1f);
        REQUIRE(math_roundn(fmod(-2.53f, 0.5f), 2) == -0.03f);
    }
    // lerp (?)
    SECTION("sqrt"){
        REQUIRE(fast_sqrt(4) == 2);
        REQUIRE(fast_sqrt(9) == 3);
        REQUIRE(fast_sqrt(16) == 4);
        REQUIRE(fast_sqrt(2) == SQRT2);
        REQUIRE(fast_sqrt(3) == SQRT3);
    }
    SECTION("sine"){
        REQUIRE(fast_sin(-1) == 0);
        REQUIRE(fast_sin(-0.75) == 1);
        REQUIRE(fast_sin(-0.5) == 0);
        REQUIRE(fast_sin(-0.25) == -1);
        REQUIRE(fast_sin(0) == 0);
        REQUIRE(fast_sin(0.25) == 1);
        REQUIRE(fast_sin(0.5) == 0);
        REQUIRE(fast_sin(0.75) == -1);
        REQUIRE(fast_sin(1) == 0);
    }
    SECTION("cosine"){
        REQUIRE(fast_cos(-1) == 1);
        REQUIRE(fast_cos(-0.75) == 0);
        REQUIRE(fast_cos(-0.5) == -1);
        REQUIRE(fast_cos(-0.25) == 0);
        REQUIRE(fast_cos(0) == 1);
        REQUIRE(fast_cos(0.25) == 0);
        REQUIRE(fast_cos(0.5) == -1);
        REQUIRE(fast_cos(0.75) == 0);
        REQUIRE(fast_cos(1) == 1);
    }
}
