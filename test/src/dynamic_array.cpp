void test_dynamic_array(){
    TITLE("Testing dynamic array");
    SECTION("new",
        Array<int> a = array_new<int>(100);
        REQUIRE(a.size, 0);
        REQUIRE(a.reserved_size, 100);
        REQUIRE_NOT_NULL(a.ptr);
        free(a.ptr);
    )
    SECTION("free",
        Array<int> a = array_new<int>(100);
        REQUIRE(a.size, 0);
        REQUIRE(a.reserved_size, 100);
        REQUIRE_NOT_NULL(a.ptr);
        array_free(&a);
        REQUIRE(a.size, 0);
        REQUIRE(a.reserved_size, 0);
        REQUIRE_NULL(a.ptr);
    )
    SECTION("resize",
        Array<int> a = array_new<int>(100);
        REQUIRE(a.reserved_size, 100);
        array_resize(&a, 200);
        REQUIRE(a.reserved_size, 200);
        array_resize(&a, 50);
        REQUIRE(a.reserved_size, 50);
        array_free(&a);
    )
    SECTION("reserve",
        Array<int> a;
        REQUIRE(a.reserved_size, 0);
        array_reserve(&a, 5);
        REQUIRE(a.reserved_size, 8);
        array_reserve(&a, 4);
        REQUIRE(a.reserved_size, 8);
        array_reserve(&a, 20);
        REQUIRE(a.reserved_size, 20);
        array_reserve(&a, 50);
        REQUIRE(a.reserved_size, 50);
        array_free(&a);
    )
    SECTION("insert",
        Array<int> a;
        REQUIRE(a.size, 0);
        array_append(&a, 5);
        array_append(&a, 10);
        array_append(&a, 15);
        REQUIRE(a.size, 3);
        REQUIRE(a[0], 5);
        REQUIRE(a[1], 10);
        REQUIRE(a[2], 15);
        array_insert(&a, 100, 0);
        REQUIRE(a.size, 4);
        REQUIRE(a[0], 100);
        REQUIRE(a[1], 5);
        REQUIRE(a[2], 10);
        REQUIRE(a[3], 15);
        array_free(&a);
    )
    SECTION("append",
        Array<int> a;
        REQUIRE(a.size, 0);
        array_append(&a, 5);
        REQUIRE(a.size, 1);
        REQUIRE(a[0], 5);
        array_append(&a, 10);
        array_append(&a, 15);
        REQUIRE(a.size, 3);
        REQUIRE(a[0], 5);
        REQUIRE(a[1], 10);
        REQUIRE(a[2], 15);
        array_free(&a);
    )
    SECTION("remove at",
        Array<int> a;
        array_append(&a, 5);
        array_append(&a, 10);
        array_append(&a, 15);
        REQUIRE(a.size, 3);
        REQUIRE(a[0], 5);
        REQUIRE(a[1], 10);
        REQUIRE(a[2], 15);
        array_remove_at(&a, 1);
        REQUIRE(a.size, 2);
        REQUIRE(a[0], 5);
        REQUIRE(a[1], 15);
        array_free(&a);
    )
    SECTION("pop",
        Array<int> a;
        REQUIRE(a.size, 0);
        array_append(&a, 5);
        array_append(&a, 10);
        array_append(&a, 15);
        REQUIRE(a.size, 3);
        REQUIRE(a[0], 5);
        REQUIRE(a[1], 10);
        REQUIRE(a[2], 15);
        REQUIRE(array_pop(&a), 15);
        REQUIRE(array_pop(&a), 10);
        REQUIRE(array_pop(&a), 5);
        REQUIRE(a.size, 0);
        array_free(&a);
    )
    SECTION("dequeue",
        Array<int> a;
        REQUIRE(a.size, 0);
        array_append(&a, 5);
        array_append(&a, 10);
        array_append(&a, 15);
        REQUIRE(a.size, 3);
        REQUIRE(a[0], 5);
        REQUIRE(a[1], 10);
        REQUIRE(a[2], 15);
        REQUIRE(array_dequeue(&a), 5);
        REQUIRE(array_dequeue(&a), 10);
        REQUIRE(array_dequeue(&a), 15);
        REQUIRE(a.size, 0);
        array_free(&a);
    )
    SECTION("clear",
        Array<int> a;
        REQUIRE(a.size, 0);
        array_append(&a, 5);
        array_append(&a, 10);
        array_append(&a, 15);
        REQUIRE(a.size, 3);
        REQUIRE(a[0], 5);
        REQUIRE(a[1], 10);
        REQUIRE(a[2], 15);
        array_clear(&a);
        REQUIRE(a.size, 0);
        array_free(&a);
    )
}
