void test_str() {
    TITLE("Testing str, str_builder and str_parser");
    SECTION("build parse",
        StrBuilder b = make_str_builder(10);
        str_builder_append(&b, "ciao");
        str_builder_append(&b, 'x');
        str_builder_append(&b, true);
        str_builder_append(&b, 'x');
        str_builder_append(&b, 'd');
        str_builder_append(&b, 'x');
        str_builder_append(&b, (u8)120);
        str_builder_append(&b, 'x');
        str_builder_append(&b, (u16)12000);
        str_builder_append(&b, 'x');
        str_builder_append(&b, (u32)70000);
        str_builder_append(&b, 'x');
        str_builder_append(&b, (u64)12345678901);
        str_builder_append(&b, 'x');
        str_builder_append(&b, (s8)-120);
        str_builder_append(&b, 'x');
        str_builder_append(&b, (s16)-12000);
        str_builder_append(&b, 'x');
        str_builder_append(&b, (s32)-70000);
        str_builder_append(&b, 'x');
        str_builder_append(&b, (s64)-12345678901);
        str_builder_append(&b, 'x');
        str_builder_append(&b, (f32)-12.4);
        str_builder_append(&b, 'x');
        str_builder_append(&b, (f64)-12.42232);
        str_builder_append(&b, 'x');
        str_builder_append(&b, vec2{12, 14});
        str_builder_append(&b, 'x');
        str_builder_append(&b, vec3{12, 14, 16});
        str_builder_append(&b, 'x');
        str_builder_append(&b, vec4{12, 14, 16, 18});
        str_builder_append(&b, 'x');
        
        str out = str_builder_get_str(&b);
        REQUIRE(str_matches(out, "ciaoxtruexdx120x12000x70000x12345678901x-120x-12000x-70000x-12345678901x-12.40000x-12.42232x(12.00000,14.00000)x(12.00000,14.00000,16.00000)x(12.00000,14.00000,16.00000,18.00000)x"), true);
        
        StrParser p = make_str_parser(out);
        REQUIRE(str_parser_check_magic(&p, "ciao"), true);
        REQUIRE(str_parser_get_char(&p), 'x');
        bool ok, bool_out;
        ok = str_parser_parse_bool(&p, &bool_out);
        REQUIRE(ok, true);
        REQUIRE(bool_out, true);
        
        REQUIRE(str_parser_get_char(&p), 'x');
        REQUIRE(str_parser_get_char(&p), 'd');
        REQUIRE(str_parser_get_char(&p), 'x');
        
        u8 parsed_u8 = 0;
        ok = str_parser_parse_u8(&p, &parsed_u8);
        REQUIRE(ok, true);
        REQUIRE(parsed_u8, 120);
        REQUIRE(str_parser_get_char(&p), 'x');
        
        u16 parsed_u16 = 0;
        ok = str_parser_parse_u16(&p, &parsed_u16);
        REQUIRE(ok, true);
        REQUIRE(parsed_u16, 12000);
        REQUIRE(str_parser_get_char(&p), 'x');
        
        u32 parsed_u32 = 0;
        ok = str_parser_parse_u32(&p, &parsed_u32);
        REQUIRE(ok, true);
        REQUIRE(parsed_u32, 70000);
        REQUIRE(str_parser_get_char(&p), 'x');
        
        u64 parsed_u64 = 0;
        ok = str_parser_parse_u64(&p, &parsed_u64);
        REQUIRE(ok, true);
        REQUIRE(parsed_u64, 12345678901);
        REQUIRE(str_parser_get_char(&p), 'x');
    )
}