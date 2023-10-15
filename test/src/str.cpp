void test_str() {
    TITLE("Testing str, str_builder and str_parser");
    SECTION("build parse",
        StrBuilder b = make_str_builder(10);
        str_builder_append(&b, "ciao");
        str_builder_append(&b, true);
        str_builder_append(&b, 'd');
        str_builder_append(&b, (u8)120);
        str_builder_append(&b, (u16)12000);
        str_builder_append(&b, (u32)70000);
        str_builder_append(&b, (u64)12345678901);
        str_builder_append(&b, (s8)-120);
        str_builder_append(&b, (s16)-12000);
        str_builder_append(&b, (s32)-70000);
        str_builder_append(&b, (s64)-12345678901);
        str_builder_append(&b, (f32)-12.4);
        str_builder_append(&b, (f64)-12.42232);
        str_builder_append(&b, vec2{12, 14});
        str_builder_append(&b, vec3{12, 14, 16});
        str_builder_append(&b, vec4{12, 14, 16, 18});
        
        str out = str_builder_get_str(&b);
        REQUIRE(str_matches(out, "ciaotrued120120007000012345678901-120-12000-70000-12345678901-12.40000-12.42232(12.00000,14.00000)(12.00000,14.00000,16.00000)(12.00000,14.00000,16.00000,18.00000)"), true);
        
        StrParser p = make_str_parser(out);
        REQUIRE(str_parser_check_magic(&p, "ciao"), true);
        bool ok, bool_out;
        ok = str_parser_parse_bool(&p, &bool_out);
        REQUIRE(ok, true);
        REQUIRE(bool_out, true);
    )
}