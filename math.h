#pragma once
#define GYOMATH

/*
In this file:
Many math functions, constructs and concepts! saying all of them would be too much. Here's the more unique/important ones:
- Typical use case functions and macros, like min, max, remap, npow, lerp, random_float, random_bool, etc.
- sin_turns and cos_turns that use turns instead of radians/degrees (1 turn = 360 degrees), making them faster.
- The very common vec2, vec3, vec4 and mat4. These last 2 use sse SIMD to speed them up substantially.
*/

#ifndef DISABLE_INCLUDES
    #include <smmintrin.h>
    #include <cmath>
#endif

#ifndef GYOFIRST
    #include "first.h"
#endif

inline u8 count_digits(u64 x){
    u8 n = 0;
    do {
        n++;
    } while(x /= 10);
    return n;
}

inline float npow(float x, u32 n){
    // API(cogno): can't we use powf? and if we can't, can't we just return x**n?
    float res = x;
    for(u32 i = 1; i < n; i++){
        res *= x;
    }
    return res;
}

inline float roundn(float x, u32 n) { return roundf(x * npow(10, n)) / npow(10, n); }

// API(cogno): remap macro?
inline float remap(float in, float old_from, float old_to, float new_from, float new_to) {
    return (in - old_from) / (old_to - old_from) * (new_to - new_from) + new_from;
}

#ifndef min
    #define min(a, b) (((a) < (b)) ? (a) : (b))
#endif
#ifndef max
    #define max(a, b) (((a) < (b)) ? (b) : (a))
#endif
#define sign(a) (((a) == 0) ? (0) : (((a) > 0) ? 1 : -1))
#define clamp(val, min_, max_) (max(min((val), (max_)), (min_)))
#define lerp(start, dest, t) (((dest) - (start)) * (t) + (start))
#ifndef PI
    #define PI        (3.1415926535f)
#endif
#ifndef TAU
    #define TAU       (6.2831853072f)
#endif
#ifndef E
    #define E         (2.7182818284f)
#endif
#ifndef SQRT2
    #define SQRT2     (1.4142135623f)
#endif
#ifndef SQRT3
    #define SQRT3     (1.7320508075f)
#endif
#ifndef DEG2RAD
    #define DEG2RAD   (PI / 180.0f)
#endif
#ifndef DEG2TURNS
    #define DEG2TURNS (1.0f / 360.0f)
#endif
#ifndef RAD2DEG
    #define RAD2DEG   (180.0f / PI)
#endif
#ifndef RAD2TURNS
    #define RAD2TURNS (1.0f / TAU)
#endif
#ifndef TURNS2DEG
    #define TURNS2DEG (360.0f)
#endif
#ifndef TURNS2RAD
    #define TURNS2RAD (TAU)
#endif

// PERF(Jason): this random may be slow check this: https://thompsonsed.co.uk/random-number-generators-for-c-performance-tested
// Cogno: also this https://www.corsix.org/content/higher-quality-random-floats
inline float random_float() { return (float)rand() / (float)RAND_MAX; }
inline float random_float(float max_f) { return random_float() * max_f; }
inline float random_float(float min_f, float max_f) { return random_float(max_f - min_f) + min_f; }
inline bool  random_bool() { return rand() % 2 == 0; }

inline float _sin_internal(float x) {
    float q = 8 * x - 16 * x * x;
    return 0.225f * (q * q - q) + q;
}

// trigonometric functions in turns (1 turn = 360 deg)
inline float sin_turns(float angle) {
    angle -= int(angle);
    
    if(angle < 0) angle += 1;
    if(angle > 0.5) return -_sin_internal(angle - 0.5f);
    return _sin_internal(angle);
}

inline float cos_turns(float angle)  { return sin_turns(angle + 0.25f); }
inline float tan_turns(float angle)  { return sin_turns(angle) / cos_turns(angle); }
inline float cot_turns(float angle)  { return cos_turns(angle) / sin_turns(angle); }
// API(cogno): implement other trigonometric functions as needed

struct vec2 {
    union {
        struct {float x, y;};
        float ptr[2];
    };
};

struct vec3 {
    union {
        struct {float x,  y,  z;};
        struct {float yz, zx, xy;}; // geometric algebra's bivector (can be interpreted as a plane of rotation. The x axis is the yz plane, the y axis is the zx plane and the z axis is the xy plane). NOTE(cogno): the order of these elements is important, since the x axis is the yz plane, if x is the first float then so is yz. Also since convertion is trivial, bivector_dual is now an useless operation. Turning a vector into a bivector is as simple as calling obj.yz instead of obj.x, making them VERY fast.
        float ptr[3];
    };
};

struct vec4 {
    union {
        struct {float x, y, z, w;};
        struct {float r, g, b, a;}; // normalized colors in range [0, 1]
        float ptr[4];
        __m128 v;
    };
};

// NOTE(cogno): In the past I had constructors to "simplify" creations of vec2/vec3/vec4,
// like auto casting to float or auto making vec3 -> vec2 without vec3.z. 
// I now find them basically useless.
// Most of the times you can just use vec2{...}, and the code is clearer.
// For example automatic conversion to float was used only in 3 occasions out of 290,
// 3/189 for vec2, 0/100 for vec3, 0/1 for vec4.
// Maybe we'll add back vec3->vec2 if usage increases, but most of the times
// people just forgets you can use vec2(...) so most stuff never gets used anyway.
// UPDATE:
// I've had a project where a ton of times the compile would complain about
// int to float casts when constructing vec2. I've also tried to
// re-add it to avoid the problem, but it turns out C++ is stupid,
// so some cool and useful things you can do become impossible when you
// use custom constructors (like vec3{.xy = 20}).
// Also you can just disable the compiler warning about int->float casts to
// avoid the problem.
// In other words: the auto float casts are most likely not gonna come back.
//                - cogno 2025/01/28

struct mat4{
    union{
        float ptr[16];
        float mat_f[4][4];
        struct {
            float m11, m12, m13, m14;
            float m21, m22, m23, m24;
            float m31, m32, m33, m34;
            float m41, m42, m43, m44;
        };
        struct { __m128 r1, r2, r3, r4; };
        __m128 mat_r[4];
        struct { vec4 v1, v2, v3, v4; };
        vec4 mat_v[4];
    };
    inline float* operator [](int idx){
        return this->mat_f[idx];
    }
};
// NOTE(cogno): mat4 is a *row-major* matrix.


inline void printsl_custom(vec2 v) {_buffer_append("(%.5f, %.5f)", v.x, v.y);}
inline void printsl_custom(vec3 v) {_buffer_append("(%.5f, %.5f, %.5f)", v.x, v.y, v.z);}
inline void printsl_custom(vec4 v) {_buffer_append("(%.5f, %.5f, %.5f, %.5f)", v.x, v.y, v.z, v.w);}
inline void printsl_custom(mat4 m) {_buffer_append("|%.5f %.5f %.5f %.5f|\n|%.5f %.5f %.5f %.5f|\n|%.5f %.5f %.5f %.5f|\n|%.5f %.5f %.5f %.5f|\n", m.m11, m.m12, m.m13, m.m14, m.m21, m.m22, m.m23, m.m24, m.m31, m.m32, m.m33, m.m34, m.m41, m.m42, m.m43, m.m44);}


inline vec2 operator +(vec2 a, vec2 b)  {return {a.x + b.x, a.y + b.y};}
inline vec3 operator +(vec3 a, vec3 b)  {return {a.x + b.x, a.y + b.y, a.z + b.z};}
inline vec4 operator +(vec4 a, vec4 b)  {vec4 res; res.v = _mm_add_ps(a.v, b.v); return res;}
inline vec2 operator -(vec2 a, vec2 b)  {return {a.x - b.x, a.y - b.y};}
inline vec3 operator -(vec3 a, vec3 b)  {return {a.x - b.x, a.y - b.y, a.z - b.z};}
inline vec4 operator -(vec4 a, vec4 b)  {vec4 res; res.v = _mm_sub_ps(a.v, b.v); return res;}
inline vec2 operator *(vec2 a, float s) {return {a.x * s, a.y * s};}
inline vec3 operator *(vec3 a, float s) {return {a.x * s, a.y * s, a.z * s};}
inline vec4 operator *(vec4 a, float s) {vec4 res; res.v = _mm_mul_ps(a.v, _mm_set1_ps(s)); return res;}
inline vec2 operator *(float s, vec2 a) {return {a.x * s, a.y * s};}
inline vec3 operator *(float s, vec3 a) {return {a.x * s, a.y * s, a.z * s};}
inline vec4 operator *(float s, vec4 a) {vec4 res; res.v = _mm_mul_ps(a.v, _mm_set1_ps(s)); return res;}
inline vec2 operator *(vec2 a, vec2 b)  {return {a.x * b.x, a.y * b.y};}
inline vec3 operator *(vec3 a, vec3 b)  {return {a.x * b.x, a.y * b.y, a.z * b.z};}
inline vec4 operator *(vec4 a, vec4 b)  {vec4 res; res.v = _mm_mul_ps(a.v, b.v); return res;}
inline vec2 operator /(vec2 a, float s) {return {a.x / s, a.y / s};}
inline vec3 operator /(vec3 a, float s) {return {a.x / s, a.y / s, a.z / s};}
inline vec4 operator /(vec4 a, float s) {vec4 res; res.v = _mm_div_ps(a.v, _mm_set1_ps(s)); return res;}
inline vec2 operator /(float s, vec2 a) {return {s / a.x, s / a.y};}
inline vec3 operator /(float s, vec3 a) {return {s / a.x, s / a.y, s / a.z};}
inline vec4 operator /(float s, vec4 a) {vec4 res; res.v = _mm_div_ps(_mm_set1_ps(s), a.v); return res;}
inline vec2 operator /(vec2 a, vec2 b)  {return {a.x / b.x, a.y / b.y};}
inline vec3 operator /(vec3 a, vec3 b)  {return {a.x / b.x, a.y / b.y, a.z / b.z};}
inline vec4 operator /(vec4 a, vec4 b)  {vec4 res; res.v = _mm_div_ps(a.v, b.v); return res;}
inline vec2 operator -(vec2 a)  {return {-a.x, -a.y};}
inline vec3 operator -(vec3 a)  {return {-a.x, -a.y, -a.z};}
inline vec4 operator -(vec4 a)  {vec4 res;  res.v = _mm_sub_ps(__m128{}, a.v);  return res;}
inline vec2 operator +=(vec2& a, const vec2 &b) {a = a + b;  return a;}
inline vec3 operator +=(vec3& a, const vec3 &b) {a = a + b;  return a;}
inline vec4 operator +=(vec4& a, const vec4 &b) {a = a + b;  return a;}
inline vec2 operator -=(vec2& a, const vec2 &b) {a = a - b;  return a;}
inline vec3 operator -=(vec3& a, const vec3 &b) {a = a - b;  return a;}
inline vec4 operator -=(vec4& a, const vec4 &b) {a = a - b;  return a;}
inline vec2 operator *=(vec2& a, const float &b) {a = a * b;  return a;}
inline vec3 operator *=(vec3& a, const float &b) {a = a * b;  return a;}
inline vec4 operator *=(vec4& a, const float &b) {a = a * b;  return a;}
inline vec2 operator /=(vec2& a, const float &b) {a = a / b;  return a;}
inline vec3 operator /=(vec3& a, const float &b) {a = a / b;  return a;}
inline vec4 operator /=(vec4& a, const float &b) {a = a / b;  return a;}
// API(cogno): since comparing float is problematic due to precision loss we could check if abs(a.x) <= small epsilon. Is this a good idea?
inline bool operator ==(vec2 a, vec2 b) {return (a.x == b.x) && (a.y == b.y);}
inline bool operator ==(vec3 a, vec3 b) {return (a.x == b.x) && (a.y == b.y) && (a.z == b.z);}
inline bool operator ==(vec4 a, vec4 b) {return _mm_movemask_ps(_mm_cmpeq_ps(a.v, b.v)) == 0b1111;}
inline bool operator !=(vec2 a, vec2 b) {return !((a.x == b.x) && (a.y == b.y));}
inline bool operator !=(vec3 a, vec3 b) {return !((a.x == b.x) && (a.y == b.y) && (a.z == b.z));}
inline bool operator !=(vec4 a, vec4 b) {return _mm_movemask_ps(_mm_cmpeq_ps(a.v, b.v)) != 0b1111;}



inline vec2 vec2_round(vec2 v) {return {round(v.x), round(v.y)};}
inline vec3 vec3_round(vec3 v) {return {round(v.x), round(v.y), round(v.z)};}
inline vec4 vec4_round(vec4 v) {return {round(v.x), round(v.y), round(v.z), round(v.w)};}

inline vec2 vec2_floor(vec2 v) {return {floor(v.x), floor(v.y)};}
inline vec3 vec3_floor(vec3 v) {return {floor(v.x), floor(v.y), floor(v.z)};}
inline vec4 vec4_floor(vec4 v) {return {floor(v.x), floor(v.y), floor(v.z), floor(v.w)};}

inline vec2 vec2_ceil(vec2 v)  {return {ceil(v.x), ceil(v.y)};}
inline vec3 vec3_ceil(vec3 v)  {return {ceil(v.x), ceil(v.y), ceil(v.z)};}
inline vec4 vec4_ceil(vec4 v)  {return {ceil(v.x), ceil(v.y), ceil(v.z), ceil(v.w)};}

inline vec2 vec2_trunc(vec2 v) {return {trunc(v.x), trunc(v.y)};}
inline vec3 vec3_trunc(vec3 v) {return {trunc(v.x), trunc(v.y), trunc(v.z)};}
inline vec4 vec4_trunc(vec4 v) {return {trunc(v.x), trunc(v.y), trunc(v.z), trunc(v.w)};}

inline vec2 remap(vec2 in, vec2 old_from, vec2 old_to, vec2 new_from, vec2 new_to) {
    return vec2{
        remap(in.x, old_from.x, old_to.x, new_from.x, new_to.x),
        remap(in.y, old_from.y, old_to.y, new_from.y, new_to.y)
    };
}

inline vec3 remap(vec3 in, vec3 old_from, vec3 old_to, vec3 new_from, vec3 new_to) {
    return vec3{
        remap(in.x, old_from.x, old_to.x, new_from.x, new_to.x),
        remap(in.y, old_from.y, old_to.y, new_from.y, new_to.y),
        remap(in.z, old_from.z, old_to.z, new_from.z, new_to.z)
    };
}

inline vec4 remap(vec4 in, vec4 old_from, vec4 old_to, vec4 new_from, vec4 new_to) {
    vec4 num;
    num.v = _mm_sub_ps(in.v, old_from.v);
    vec4 den;
    den.v = _mm_sub_ps(old_to.v, old_from.v);
    vec4 t;
    t.v = _mm_sub_ps(new_to.v, new_from.v);
    vec4 res;
    res.v = _mm_div_ps(num.v, den.v);
    res.v = _mm_mul_ps(res.v, t.v);
    res.v = _mm_add_ps(res.v, new_from.v);
    return res;
}

inline float vec2_length_squared(vec2 v) { return v.x * v.x + v.y * v.y; }
inline float vec3_length_squared(vec3 v) { return v.x * v.x + v.y * v.y + v.z * v.z; }
inline float vec4_length_squared(vec4 v) { return _mm_cvtss_f32(_mm_dp_ps(v.v, v.v, 0b11110001)); }

inline float vec2_length(vec2 v) { return sqrtf(vec2_length_squared(v)); }
inline float vec3_length(vec3 v) { return sqrtf(vec3_length_squared(v)); }
inline float vec4_length(vec4 v) {
    vec4 res;
    res.v = _mm_dp_ps(v.v, v.v, 0b11110001);
    res.v = _mm_sqrt_ss(res.v);
    return _mm_cvtss_f32(res.v);
}

inline vec2 vec2_normalize(vec2 v, vec2 default_out, float epsilon = 0.001f) {
    float len = vec2_length(v);
    if (len <= epsilon) return default_out;
    return {v.x / len, v.y / len};
}
inline vec3 vec3_normalize(vec3 v, vec3 default_out, float epsilon = 0.001f) {
    float len = vec3_length(v);
    if (len <= epsilon) return default_out;
    return {v.x / len, v.y / len, v.z / len};
}
inline vec4 vec4_normalize(vec4 v, vec4 default_out, float epsilon = 0.001f) {
    float len = vec4_length(v);
    if (len <= epsilon) return default_out;
    vec4 res;
    res.v = _mm_div_ps(v.v, _mm_set1_ps(len));
    return res;
}

//returns random vector from range (0,0) to (1,1) inclusive (NOT normalized, if needed use vecX_random_dir() instead)
inline vec2 vec2_random() { return {random_float(), random_float()}; }
inline vec3 vec3_random() { return {random_float(), random_float(), random_float()}; }
inline vec4 vec4_random() { return {random_float(), random_float(), random_float(), random_float()}; }

inline vec2 vec2_random_dir() {
    auto rand = vec2_random() * 2 - vec2{1, 1};
    return vec2_normalize(rand, {0, 1});
}
inline vec3 vec3_random_dir() {
    auto rand = vec3_random() * 2 - vec3{1, 1, 1};
    return vec3_normalize(rand, {0, 1, 0});
}
// TODO(cogno): vec4_random_dir()

inline float vec2_dot(vec2 a, vec2 b){ return a.x * b.x + a.y * b.y; }
inline float vec3_dot(vec3 a, vec3 b){ return a.x * b.x + a.y * b.y + a.z * b.z; }
inline float vec4_dot(vec4 a, vec4 b){ return _mm_cvtss_f32(_mm_dp_ps(a.v, b.v, 0b11110001)); }


inline vec3 vec3_cross(vec3 a, vec3 b){
    vec3 res;
    res.x = a.y * b.z - b.y * a.z;
    res.y = a.z * b.x - b.z * a.x;
    res.z = a.x * b.y - b.x * a.y;
    return res;
}

inline vec2 vec2_rotate(vec2 v, float angle){
    float cos_value = cos_turns(angle);
    float sin_value = sin_turns(angle);
    
    vec2 res;
    res.x = v.x * cos_value - v.y * sin_value;
    res.y = v.x * sin_value + v.y * cos_value;
    return res;
}
inline vec2 vec2_project_point_on_line(vec2 point, vec2 line_start, vec2 line_dir) {
    // API(cogno): can we make it faster with 2D PGA?
    
    auto dot_value = line_dir.x * (point.x - line_start.x) + line_dir.y * (point.y - line_start.y);
    vec2 proj = line_start + line_dir * dot_value;
    return proj;
}


inline mat4 mat4_new(float n = 1){
    mat4 res;
    res.r1 = _mm_setr_ps(n, 0, 0, 0);
    res.r2 = _mm_setr_ps(0, n, 0, 0);
    res.r3 = _mm_setr_ps(0, 0, n, 0);
    res.r4 = _mm_setr_ps(0, 0, 0, n);
    return res;
}
inline mat4 mat4_transpose(mat4 m) {
    mat4 res;
    res.r1 = _mm_setr_ps(m.m11, m.m21, m.m31, m.m41);
    res.r2 = _mm_setr_ps(m.m12, m.m22, m.m32, m.m42);
    res.r3 = _mm_setr_ps(m.m13, m.m23, m.m33, m.m43);
    res.r4 = _mm_setr_ps(m.m14, m.m24, m.m34, m.m44);
    return res;
}
inline mat4 operator +(mat4 m1, mat4 m2){
    mat4 res;
    res.r1 = _mm_add_ps(m1.r1, m2.r1);
    res.r2 = _mm_add_ps(m1.r2, m2.r2);
    res.r3 = _mm_add_ps(m1.r3, m2.r3);
    res.r4 = _mm_add_ps(m1.r4, m2.r4);
    return res;
}
inline mat4 operator -(mat4 m1, mat4 m2){
    mat4 res;
    res.r1 = _mm_sub_ps(m1.r1, m2.r1);
    res.r2 = _mm_sub_ps(m1.r2, m2.r2);
    res.r3 = _mm_sub_ps(m1.r3, m2.r3);
    res.r4 = _mm_sub_ps(m1.r4, m2.r4);
    return res;
}
inline mat4 operator *(mat4 m, float s){
    mat4 res;
    __m128 val = _mm_set1_ps(s);
    res.r1 = _mm_mul_ps(m.r1, val);
    res.r2 = _mm_mul_ps(m.r2, val);
    res.r3 = _mm_mul_ps(m.r3, val);
    res.r4 = _mm_mul_ps(m.r4, val);
    return res;
}
inline mat4 operator *(float s, mat4 m){
    mat4 res;
    __m128 val = _mm_set1_ps(s);
    res.r1 = _mm_mul_ps(m.r1, val);
    res.r2 = _mm_mul_ps(m.r2, val);
    res.r3 = _mm_mul_ps(m.r3, val);
    res.r4 = _mm_mul_ps(m.r4, val);
    return res;
}
inline mat4 operator /(mat4 m1, float s){
    mat4 res;
    __m128 val = _mm_set1_ps(s);
    res.r1 = _mm_div_ps(m1.r1, val);
    res.r2 = _mm_div_ps(m1.r2, val);
    res.r3 = _mm_div_ps(m1.r3, val);
    res.r4 = _mm_div_ps(m1.r4, val);
    return res;
}
inline mat4 operator /(float s, mat4 m1){
    mat4 res;
    __m128 val = _mm_set1_ps(s);
    res.r1 = _mm_div_ps(val, m1.r1);
    res.r2 = _mm_div_ps(val, m1.r2);
    res.r3 = _mm_div_ps(val, m1.r3);
    res.r4 = _mm_div_ps(val, m1.r4);
    return res;
}
inline mat4 operator *(mat4 m1, mat4 m2){
    mat4 temp = mat4_transpose(m2);
    
    mat4 res;
    res.r1 = _mm_or_ps(_mm_or_ps(_mm_dp_ps(m1.r1, temp.r1, 0b11110001), _mm_dp_ps(m1.r1, temp.r2, 0b11110010)), _mm_or_ps(_mm_dp_ps(m1.r1, temp.r3, 0b11110100), _mm_dp_ps(m1.r1, temp.r4, 0b11111000)));
    res.r2 = _mm_or_ps(_mm_or_ps(_mm_dp_ps(m1.r2, temp.r1, 0b11110001), _mm_dp_ps(m1.r2, temp.r2, 0b11110010)), _mm_or_ps(_mm_dp_ps(m1.r2, temp.r3, 0b11110100), _mm_dp_ps(m1.r2, temp.r4, 0b11111000)));
    res.r3 = _mm_or_ps(_mm_or_ps(_mm_dp_ps(m1.r3, temp.r1, 0b11110001), _mm_dp_ps(m1.r3, temp.r2, 0b11110010)), _mm_or_ps(_mm_dp_ps(m1.r3, temp.r3, 0b11110100), _mm_dp_ps(m1.r3, temp.r4, 0b11111000)));
    res.r4 = _mm_or_ps(_mm_or_ps(_mm_dp_ps(m1.r4, temp.r1, 0b11110001), _mm_dp_ps(m1.r4, temp.r2, 0b11110010)), _mm_or_ps(_mm_dp_ps(m1.r4, temp.r3, 0b11110100), _mm_dp_ps(m1.r4, temp.r4, 0b11111000)));
    return res;
}
inline vec4 operator* (mat4 m, vec4 v) {
    vec4 res;
    res.v = _mm_or_ps(_mm_or_ps(_mm_dp_ps(m.r1, v.v, 0b11110001), _mm_dp_ps(m.r2, v.v, 0b11110010)), _mm_or_ps(_mm_dp_ps(m.r3, v.v, 0b11110100), _mm_dp_ps(m.r4, v.v, 0b11111000)));
    return res;
}
inline vec4 operator* (vec4 v, mat4 m) {
    vec4 res;
    mat4 mt = mat4_transpose(m);
    res.v = _mm_or_ps(_mm_or_ps(_mm_dp_ps(mt.r1, v.v, 0b11110001), _mm_dp_ps(mt.r2, v.v, 0b11110010)), _mm_or_ps(_mm_dp_ps(mt.r3, v.v, 0b11110100), _mm_dp_ps(mt.r4, v.v, 0b11111000)));
    return res;
}
inline mat4 operator +=(mat4& m1, const mat4& m2) { m1 = m1 + m2; return m1; }
inline mat4 operator -=(mat4& m1, const mat4& m2) { m1 = m1 - m2; return m1; }
inline mat4 operator *=(mat4& m1, const float& b) { m1 = m1 * b;  return m1; }
inline mat4 operator *=(mat4& m1, const mat4& m2) { m1 = m1 * m2; return m1; }
inline mat4 operator /=(mat4& m1, const float& b) { m1 = m1 / b;  return m1; }

inline mat4 mat4_perspective_camera(float fov, float aspect_ratio, float z_near, float z_far) {
    mat4 res = {};
    float tan_value = tan_turns(fov / 2.0f);
    float cotangent = 1.0f / tan_value;
    res.m11 = 1.0f / (aspect_ratio * tan_value);
    res.m22 = cotangent;
    res.m33 = -(z_far + z_near) / (z_far - z_near);
    res.m43 = -1.0f;
    res.m34 = -(2.0f * z_near * z_far) / (z_far - z_near);
    return res;
}

inline mat4 mat4_orthographic_camera(float left, float right, float bottom, float top, float z_near, float z_far) {
    mat4 res = {};
    res.m11 = 2.0f / (right - left);
    res.m22 = 2.0f / (top - bottom);
    res.m33 = -2.0f / (z_far - z_near);
    res.m14 = -(right + left) / (right - left);
    res.m24 = -(top + bottom) / (top - bottom);
    res.m34 = -(z_far + z_near) / (z_far - z_near);
    res.m44 = 1.0f;
    return res;
}
inline mat4 mat4_orthographic_camera(float left, float right, float bottom, float top) {
    return mat4_orthographic_camera(left, right, bottom, top, -2.0f, 0.0f);
}

inline float mat4_determinant(mat4 m) {
    vec3 col1 = {m.m11, m.m21, m.m31};
    vec3 col2 = {m.m12, m.m22, m.m32};
    vec3 col3 = {m.m13, m.m23, m.m33};
    vec3 col4 = {m.m14, m.m24, m.m34};
    
    vec3 C01 = vec3_cross(col1, col2);
    vec3 C23 = vec3_cross(col3, col4);
    vec3 B10 = col1 * m.m42 - col2 * m.m41;
    vec3 B32 = col3 * m.m44 - col4 * m.m43;
    
    return vec3_dot(C01, B32) + vec3_dot(C23, B10);
}

inline mat4 mat4_inverse(mat4 m) {
    vec3 col1 = {m.m11, m.m21, m.m31};
    vec3 col2 = {m.m12, m.m22, m.m32};
    vec3 col3 = {m.m13, m.m23, m.m33};
    vec3 col4 = {m.m14, m.m24, m.m34};
    
    vec3 C01 = vec3_cross(col1, col2);
    vec3 C23 = vec3_cross(col3, col4);
    vec3 B10 = col1 * m.m42 - col2 * m.m41;
    vec3 B32 = col3 * m.m44 - col4 * m.m43;
    
    float inv_det = 1.0f / (vec3_dot(C01, B32) + vec3_dot(C23, B10));
    C01 = C01 * inv_det;
    C23 = C23 * inv_det;
    B10 = B10 * inv_det;
    B32 = B32 * inv_det;

    vec3 i1 = (vec3_cross(col2, B32) + (C23 * m.m42));
    vec3 i2 = (vec3_cross(B32, col1) - (C23 * m.m41));
    vec3 i3 = (vec3_cross(col4, B10) + (C01 * m.m44));
    vec3 i4 = (vec3_cross(B10, col3) - (C01 * m.m43));
    
    mat4 res;
    res.r1 = _mm_setr_ps(i1.x, i1.y, i1.z, -vec3_dot(col2, C23));
    res.r2 = _mm_setr_ps(i2.x, i2.y, i2.z, +vec3_dot(col1, C23));
    res.r3 = _mm_setr_ps(i3.x, i3.y, i3.z, -vec3_dot(col4, C01));
    res.r4 = _mm_setr_ps(i4.x, i4.y, i4.z, +vec3_dot(col3, C01));
    return res;
}

inline mat4 mat4_translation_mat(vec3 v){
    mat4 res = mat4_new(1);
    res.m14 = v.x;
    res.m24 = v.y;
    res.m34 = v.z;
    return res;
}
inline mat4 mat4_scale_mat(vec3 v){
    mat4 res = {};
    res.m11 = v.x;
    res.m22 = v.y;
    res.m33 = v.z;
    res.m44 = 1;
    return res;
}
inline mat4 mat4_rotation_x_mat(float angle){
    mat4 res = {};
    float c = cos_turns(angle);
    float s = sin_turns(angle);
    
    res.m11 = 1;
    res.m22 = c;  res.m23 = -s;
    res.m32 = s;  res.m33 = c;
    res.m44 = 1;
    return res;
}
inline mat4 mat4_rotation_y_mat(float angle){
    mat4 res = {};
    float c = cos_turns(angle);
    float s = sin_turns(angle);
    
    res.m11 = c;  res.m13 = s;
    res.m22 = 1;
    res.m31 = -s; res.m33 = c;
    res.m44 = 1;
    return res;
}

inline mat4 mat4_rotation_z_mat(float angle){
    mat4 res = {};
    float c = cos_turns(angle);
    float s = sin_turns(angle);
    
    res.m11 = c;  res.m12 = -s;
    res.m21 = s;  res.m22 = c;
    res.m33 = 1.0f;
    res.m44 = 1.0f;
    return res;
}
inline mat4 mat4_rotation_mat(vec3 axis, float angle){
    mat4 res = {}; 
    float c = cos_turns(angle);
    float s = sin_turns(angle);

    axis = vec3_normalize(axis, vec3{0, 0, -1});
    vec3 temp = axis * (1 - c);
    
    res.m11 = c + temp.x * axis.x;
    res.m12 = temp.x * axis.y + s * axis.z;
    res.m13 = temp.x * axis.z - s * axis.y;

    res.m21 = temp.y * axis.x - s * axis.z;
    res.m22 = c + temp.y * axis.y;
    res.m23 = temp.y * axis.z + s * axis.x;

    res.m31 = temp.z * axis.x + s * axis.y;
    res.m32 = temp.z * axis.y - s * axis.x;
    res.m33 = c + temp.z * axis.z;
    
    res.m44 = 1;
    return res;
}



//
// 2D Geometric Algebra
//
float vec2_wedge(vec2 a, vec2 b) { return a.x * b.y - a.y * b.x; }



// TODO(cogno): same for vec3 and vec4 ? do we need to? can we do it?
inline float vec2_signed_angle_between(vec2 a, vec2 b) {
    float dot = vec2_dot(a, b);
    float det = vec2_wedge(a, b);
    float angle = atan2f(det, dot) * RAD2DEG;
    return angle;
}

// TODO(cogno): same for vec3 and vec4 ? do we need to? can we do it?
inline float vec2_angle_between(vec2 a, vec2 b) {
    return abs(vec2_signed_angle_between(a, b));
}


inline float vec3_angle_between(vec3 a, vec3 b) {
    float dot = vec3_dot(a, b);
    float det = vec3_length(vec3_cross(a, b));
    float angle = atan2f(det, dot) * RAD2DEG;
    return angle;
    /*
    TODO(cogno): code from this link:
    https://it.mathworks.com/matlabcentral/answers/2092961-how-to-calculate-the-angle-between-two-3d-vectors

    // most robust, most accurate, recovers tiny angles very well, slowest
    atan2(norm(cross(u,v)), dot(u,v)) 

    // robust, does not recover tiny angles, faster
    max(min(dot(u,v)/(norm(u)*norm(v)),1),-1)

    // not robust (may get domain error), does not recover tiny angles, fasterer
    acos(dot(u,v)/(norm(u)*norm(v)))

    // not robust (may get domain error), does not recover tiny angles, fasterer
    acos(dot(u/norm(u),v/norm(v)))

    // not robust (may get domain error), does not recover tiny angles, fastest
    acos(dot(u,v)/sqrt(dot(u,u)*dot(v,v)))

    we should try them out and see, maybe we can use the second one and the precision loss is not a problem...
    */
}

// TODO(cogno): same for vec3 and vec4
inline vec2 vec2_limit(vec2 in, float max_length) {
    float magn = vec2_length(in);
    if (magn < max_length) return in;
    return vec2_normalize(in, {}) * max_length;
}

inline vec3 vec3_project_on_dir(vec3 to_project, vec3 dir) {
    /*
    A common way to project a vector on a plane is this:
    (A = to_project, B = plane_normal)
    B × (A×B / |B|) / |B|
    
    we instead use another way from geometric algebra
    because it seems to be more efficient!
    C = A•B / B (where A•B is the dot product).
    C is just the vector projected on the plane's normal
    If you then want the vector projected on the plane it's just A - C
    Dividing by a vector is simply multiplying by it and dividing by it's length squared.
    And since B is a unit direction, the division is skipped!
    The final calculation is thus (A•B)*B. 
    Since the dot makes a scalar this is just the input direction times a constant.

                    - Cogno 2023/07/21
    */
    
    vec3 b = vec3_normalize(dir, {});
    float dot = vec3_dot(to_project, b);
    vec3 perp = dot * b;
    return perp;
    /*
    fallback of an old version where this would project on a plane, not the plane normal,
    kept here just in case
    TODO(cogno): compare performance: 
    (I don't think it's necessary anymore, GA is obviously faster)
        vec3 a = to_project;
        vec3 b = plane_norm;
        float bmag = vec3_magn(b);
        vec3 c1 = vec3_cross(a, b);
        vec3 c1_norm = c1 / bmag;
        vec3 c2 = vec3_cross(b, c1_norm);
        vec3 c2_norm = c2 / bmag;
        return c2_norm;
    */
}
inline vec3 vec3_project_on_plane(vec3 to_project, vec3 plane_norm) {
    vec3 projection = vec3_project_on_dir(to_project, plane_norm);
    vec3 on_plane = to_project - projection;
    return on_plane;
}

