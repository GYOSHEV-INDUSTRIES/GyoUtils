void test_fixed_array(){
    TITLE("Testing fixed array");
    SECTION("new",
        auto a = fixed_array_new<int>(100);
        REQUIRE(a.size, 0);
        REQUIRE(a.reserved_size, 100);
        REQUIRE_NOT_NULL(a.ptr);
        free(a.ptr);
    )
    SECTION("append",
        auto a = fixed_array_new<int>(100);
        REQUIRE(a.size, 0);
        fixed_array_append(&a, 5);
        fixed_array_append(&a, 10);
        fixed_array_append(&a, 15);
        REQUIRE(a.size, 3);
        REQUIRE(a[0], 5);
        REQUIRE(a[1], 10);
        REQUIRE(a[2], 15);
        free(a.ptr);
    )
    SECTION("free",
        auto a = fixed_array_new<int>(100);
        REQUIRE(a.size, 0);
        REQUIRE(a.reserved_size, 100);
        REQUIRE_NOT_NULL(a.ptr);
        fixed_array_free(&a);
        REQUIRE(a.size, 0);
        REQUIRE(a.reserved_size, 0);
        REQUIRE_NULL(a.ptr);
    )
    SECTION("insert",
        auto a = fixed_array_new<int>(100);
        REQUIRE(a.size, 0);
        fixed_array_append(&a, 5);
        fixed_array_append(&a, 10);
        fixed_array_append(&a, 15);
        REQUIRE(a.size, 3);
        REQUIRE(a[0], 5);
        REQUIRE(a[1], 10);
        REQUIRE(a[2], 15);
        fixed_array_insert(&a, 100, 0);
        REQUIRE(a.size, 4);
        REQUIRE(a[0], 100);
        REQUIRE(a[1], 5);
        REQUIRE(a[2], 10);
        REQUIRE(a[3], 15);
        fixed_array_free(&a);
    )
    SECTION("remove at",
        auto a = fixed_array_new<int>(100);
        fixed_array_append(&a, 5);
        fixed_array_append(&a, 10);
        fixed_array_append(&a, 15);
        REQUIRE(a.size, 3);
        REQUIRE(a[0], 5);
        REQUIRE(a[1], 10);
        REQUIRE(a[2], 15);
        fixed_array_remove_at(&a, 1);
        REQUIRE(a.size, 2);
        REQUIRE(a[0], 5);
        REQUIRE(a[1], 15);
        fixed_array_free(&a);
    )
    SECTION("pop",
        auto a = fixed_array_new<int>(100);
        REQUIRE(a.size, 0);
        fixed_array_append(&a, 5);
        fixed_array_append(&a, 10);
        fixed_array_append(&a, 15);
        REQUIRE(a.size, 3);
        REQUIRE(a[0], 5);
        REQUIRE(a[1], 10);
        REQUIRE(a[2], 15);
        REQUIRE(fixed_array_pop(&a), 15);
        REQUIRE(fixed_array_pop(&a), 10);
        REQUIRE(fixed_array_pop(&a), 5);
        REQUIRE(a.size, 0);
        fixed_array_free(&a);
    )
    SECTION("dequeue",
        auto a = fixed_array_new<int>(100);
        REQUIRE(a.size, 0);
        fixed_array_append(&a, 5);
        fixed_array_append(&a, 10);
        fixed_array_append(&a, 15);
        REQUIRE(a.size, 3);
        REQUIRE(a[0], 5);
        REQUIRE(a[1], 10);
        REQUIRE(a[2], 15);
        REQUIRE(fixed_array_dequeue(&a), 5);
        REQUIRE(fixed_array_dequeue(&a), 10);
        REQUIRE(fixed_array_dequeue(&a), 15);
        REQUIRE(a.size, 0);
        fixed_array_free(&a);
    )
}
