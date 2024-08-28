void test_simple_math(){
    TITLE("Testing simple math");
    SECTION("npow",
        REQUIRE(npow(2, 2), 4);
        REQUIRE(npow(4, 2), 16);
        REQUIRE(npow(5, 3), 125);
        REQUIRE(npow(2.2, 2), 4.84f);
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
    SECTION("trunc",
        REQUIRE(trunc(4.3), 4.0f);
        REQUIRE(trunc(0.6), 0.0f);
        REQUIRE(trunc(-0.6), 0.0f);
        REQUIRE(trunc(-2.8), -2.0f);
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
    SECTION("fmod",
        REQUIRE(fmod(1.0f, 3.0f), 1.0f);
        REQUIRE(fmod(1.23f, 2.34f), 1.23f);
        REQUIRE(roundn(fmod(5.24f, 1.0f), 2), 0.24f);
        REQUIRE(fmod(-0.1f, 0.5f), -0.1f);
        REQUIRE(roundn(fmod(-2.53f, 0.5f), 2), -0.03f);
    )
    SECTION("constants",
        REQUIRE(30  * DEG2RAD, PI / 6);
        REQUIRE(45  * DEG2RAD, PI / 4);
        REQUIRE(60  * DEG2RAD, PI / 3);
        REQUIRE(90  * DEG2RAD, PI / 2);
        REQUIRE(180 * DEG2RAD, PI);
        REQUIRE(360 * DEG2RAD, 2 * PI);
        
        REQUIRE(30  * DEG2TURNS, 1.0f / 12);
        REQUIRE(45  * DEG2TURNS, 1.0f / 8);
        REQUIRE(60  * DEG2TURNS, 1.0f / 6);
        REQUIRE(90  * DEG2TURNS, 1.0f / 4);
        REQUIRE(180 * DEG2TURNS, 1.0f / 2);
        REQUIRE(360 * DEG2TURNS, 1.0f);
        
        REQUIRE(PI / 6 * RAD2DEG, 30.0f);
        REQUIRE(PI / 4 * RAD2DEG, 45.0f);
        REQUIRE(PI / 3 * RAD2DEG, 60.0f);
        REQUIRE(PI / 2 * RAD2DEG, 90.0f);
        REQUIRE(PI     * RAD2DEG, 180.0f);
        REQUIRE(2 * PI * RAD2DEG, 360.0f);
        
        REQUIRE(PI / 6 * RAD2TURNS, 1.0f / 12);
        REQUIRE(PI / 4 * RAD2TURNS, 1.0f / 8);
        REQUIRE(PI / 3 * RAD2TURNS, 1.0f / 6);
        REQUIRE(PI / 2 * RAD2TURNS, 1.0f / 4);
        REQUIRE(PI     * RAD2TURNS, 1.0f / 2);
        REQUIRE(2 * PI * RAD2TURNS, 1.0f);
        
        REQUIRE(1.0f / 12 * TURNS2DEG, 30.0f);
        REQUIRE(1.0f / 8  * TURNS2DEG, 45.0f);
        REQUIRE(1.0f / 6  * TURNS2DEG, 60.0f);
        REQUIRE(1.0f / 4  * TURNS2DEG, 90.0f);
        REQUIRE(1.0f / 2  * TURNS2DEG, 180.0f);
        REQUIRE(1.0f      * TURNS2DEG, 360.0f);
        
        REQUIRE(1.0f / 12 * TURNS2RAD, PI / 6);
        REQUIRE(1.0f / 8  * TURNS2RAD, PI / 4);
        REQUIRE(1.0f / 6  * TURNS2RAD, PI / 3);
        REQUIRE(1.0f / 4  * TURNS2RAD, PI / 2);
        REQUIRE(1.0f / 2  * TURNS2RAD, PI);
        REQUIRE(1.0f      * TURNS2RAD, 2 * PI);
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
    SECTION("tan",
        REQUIRE(tan(-1), 0);
        REQUIRE(tan(-0.75), INFINITY);
        REQUIRE(tan(-0.5), 0);
        REQUIRE(tan(-0.25), -INFINITY);
        REQUIRE(tan(0), 0);
        REQUIRE(tan(0.25), INFINITY);
        REQUIRE(tan(0.5), 0);
        REQUIRE(tan(0.75), -INFINITY);
        REQUIRE(tan(1), 0);
        REQUIRE(tan(45 * DEG2TURNS), 1);
        REQUIRE(tan(-45 * DEG2TURNS), -1);
        REQUIRE(roundn(tan(60 * DEG2TURNS), 2), roundn(SQRT3, 2));
        REQUIRE(roundn(tan(30 * DEG2TURNS), 2), roundn(SQRT3 / 3, 2));
    )
    SECTION("sqrt",
        REQUIRE(sqrt(4), 2);
        REQUIRE(sqrt(9), 3);
        REQUIRE(sqrt(16), 4);
        REQUIRE(sqrt(2), SQRT2);
        REQUIRE(sqrt(3), SQRT3);
    )
}
