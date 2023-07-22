void test_mat4_math(){
    TITLE("testing mat4 math");
    SECTION("mat4 new", {
        mat4 a = mat4_new(1);
        for(int i = 0; i < 4; i++){
            for(int k = 0; k < 4; k++){
                if(i == k){
                    REQUIRE(a[i][k], 1);
                }else{
                    REQUIRE(a[i][k], 0);
                }
            }
        }
        mat4 b = mat4_new(4);
        for(int i = 0; i < 4; i++){
            for(int k = 0; k < 4; k++){
                if(i == k){
                    REQUIRE(b[i][k], 4);
                }else{
                    REQUIRE(b[i][k], 0);
                }
            }
        }
    })
    // ...
}
