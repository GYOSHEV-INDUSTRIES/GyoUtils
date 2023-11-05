#pragma once
#define GYOHASH

#ifndef GYOFIRST
    #include "gyofirst.h"
#endif

#define HASH_INIT 5381
#define FNV_64_PRIME       0x100000001b3
#define FNV_64_OFFSET_BIAS 0xcbf29ce484222325

u32 sdbm_hash(void* data, int size, u32 h=HASH_INIT) {
    u32 t = h;
    for(int i = 0; i < size; i++){
        t = (t << 16) + (t << 6) - t + ((u8*)data)[i];
    }
    return t;
}
union _x {
    float f;
    u32 u;
};
u32 sdbm_float_hash(float* f, int count, u32 h=HASH_INIT) {
    u32 t = h;
    for(int i = 0; i < count; i++){
        _x x;
        x.f = f[i];
        if(x.u == 0x80000000) x.u = 0;
        t = sdbm_hash(&x, 4, t);
    }
    return t;
}
u32 djb2_hash(char* string, int size) {
    u32 hash = HASH_INIT;
    for(int i = 0; i < size; i++){
        hash = ((hash << 5) + hash) + string[i];
    }
    return hash;
}
u64 fnv1a_hash(u64 val, u64 h=FNV_64_OFFSET_BIAS) {
    u64 t = h;
    t ^= val;
    return t * FNV_64_PRIME;
}
u64 fnv1a_hash(void* data, int size, u64 h=FNV_64_OFFSET_BIAS) {
    u64 t = h;
    for(int i = 0; i < size; i++){
        t = fnv1a_hash(((u8*)data)[i], t);
    }
    return t;
}
u64 knuth_hash(u64 x) {
    return 11400714819323198485 * x;
}

u32 get_hash(int x, u32 h=HASH_INIT) {
    return (u32)(knuth_hash(((u64) x) ^ h) >> 32);
}
u32 get_hash(float x, u32 h=HASH_INIT) {
    return sdbm_float_hash(&x, 1, h);
}
u32 get_hash(char* string, int size, u32 h=HASH_INIT) {
    return (u32)fnv1a_hash(string, size, h);
}
