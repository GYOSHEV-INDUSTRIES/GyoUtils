void test_unicode() {
    u32 cp1 = 0b0100100;
    u32 cp2 = 0b00010100011;
    u32 cp3 = 0b10000011000;
    u32 cp4 = 0b000100100111001;
    u32 cp5 = 0b0010000010101100;
    u32 cp6 = 0b1101010101011100;
    u32 cp7 = 0b000010000001101001000;
    
    u32 utf1 = 0x00000024;
    u32 utf2 = 0x0000c2a3;
    u32 utf3 = 0x0000d098;
    u32 utf4 = 0x00e0a4b9;
    u32 utf5 = 0x00e282ac;
    u32 utf6 = 0x00ed959c;
    u32 utf7 = 0xf0908d88;
    
    u32 utf1_rotated = 0x24;
    u32 utf2_rotated = 0xa3c2;
    u32 utf3_rotated = 0x98d0;
    u32 utf4_rotated = 0xb9a4e0;
    u32 utf5_rotated = 0xac82e2;
    u32 utf6_rotated = 0x9c95ed;
    u32 utf7_rotated = 0x888d90f0;
    
    u8* utf1_ptr = (u8*)&utf1_rotated;
    u8* utf2_ptr = (u8*)&utf2_rotated;
    u8* utf3_ptr = (u8*)&utf3_rotated;
    u8* utf4_ptr = (u8*)&utf4_rotated;
    u8* utf5_ptr = (u8*)&utf5_rotated;
    u8* utf6_ptr = (u8*)&utf6_rotated;
    u8* utf7_ptr = (u8*)&utf7_rotated;
    
    TITLE("Testing unicode");
    SECTION("utf8 to size",
        REQUIRE(unicode_utf8_to_size(*utf1_ptr), 1);
        REQUIRE(unicode_utf8_to_size(*utf2_ptr), 2);
        REQUIRE(unicode_utf8_to_size(*utf3_ptr), 2);
        REQUIRE(unicode_utf8_to_size(*utf4_ptr), 3);
        REQUIRE(unicode_utf8_to_size(*utf5_ptr), 3);
        REQUIRE(unicode_utf8_to_size(*utf6_ptr), 3);
        REQUIRE(unicode_utf8_to_size(*utf7_ptr), 4);
    )
    SECTION("codepoint to size",
        REQUIRE(unicode_codepoint_to_size(cp1), 1);
        REQUIRE(unicode_codepoint_to_size(cp2), 2);
        REQUIRE(unicode_codepoint_to_size(cp3), 2);
        REQUIRE(unicode_codepoint_to_size(cp4), 3);
        REQUIRE(unicode_codepoint_to_size(cp5), 3);
        REQUIRE(unicode_codepoint_to_size(cp6), 3);
        REQUIRE(unicode_codepoint_to_size(cp7), 4);
    )
    SECTION("codepoint to utf8",
        REQUIRE(unicode_codepoint_to_utf8(cp1), utf1);
        REQUIRE(unicode_codepoint_to_utf8(cp2), utf2);
        REQUIRE(unicode_codepoint_to_utf8(cp3), utf3);
        REQUIRE(unicode_codepoint_to_utf8(cp4), utf4);
        REQUIRE(unicode_codepoint_to_utf8(cp5), utf5);
        REQUIRE(unicode_codepoint_to_utf8(cp6), utf6);
        REQUIRE(unicode_codepoint_to_utf8(cp7), utf7);
    )
    SECTION("utf8 to codepoint",
        REQUIRE(unicode_utf8_to_codepoint(utf1_ptr), cp1);
        REQUIRE(unicode_utf8_to_codepoint(utf2_ptr), cp2);
        REQUIRE(unicode_utf8_to_codepoint(utf3_ptr), cp3);
        REQUIRE(unicode_utf8_to_codepoint(utf4_ptr), cp4);
        REQUIRE(unicode_utf8_to_codepoint(utf5_ptr), cp5);
        REQUIRE(unicode_utf8_to_codepoint(utf6_ptr), cp6);
        REQUIRE(unicode_utf8_to_codepoint(utf7_ptr), cp7);
    )
    SECTION("is header",
        REQUIRE(unicode_is_header(0x24), true);
        REQUIRE(unicode_is_header(0xc2), true);
        REQUIRE(unicode_is_header(0xd0), true);
        REQUIRE(unicode_is_header(0xe0), true);
        REQUIRE(unicode_is_header(0xe2), true);
        REQUIRE(unicode_is_header(0xed), true);
        REQUIRE(unicode_is_header(0xf0), true);
        REQUIRE(unicode_is_header(0xa3), false);
        REQUIRE(unicode_is_header(0x98), false);
        REQUIRE(unicode_is_header(0xa4), false);
        REQUIRE(unicode_is_header(0x82), false);
        REQUIRE(unicode_is_header(0x90), false);
        REQUIRE(unicode_is_header(0x95), false);
    )
}