struct ExampleStruct {
    int c = 5;
};

void test_print(){
    print(15);
    print("this is a string");
    print("This is the number %", 12.3f);
    print("this has many numbers like % and % and even %\%", 5, 12.125, 65.3);
    print('c');
    print('d');
    print(15.2);
    print(true);
    print(false);
    int a = 12;
    print(a);
    print(&a);
    print("my final message");
    
    printsl(12);
    printsl(' ');
    printsl("log message ");
    printsl(true);
    printsl(" and then some stuff like % and %\% and the symbol \%", false, 12.5);
    print("and then y try to break it with \\");
    
    ExampleStruct ex = {};
    print("and finally I print something unknown like %", ex);
    print("%", vec2{1, 2});
}
