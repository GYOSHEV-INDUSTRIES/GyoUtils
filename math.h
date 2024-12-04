#pragma once
#define GYOMATH

/*
In this file:
Many math functions, constructs and concepts! saying all of them would be too much. Here's the more unique/important ones:
- Typical use case functions and macros, like min, max, remap, npow, lerp, random_float, random_bool, etc.
- sin_turns and cos_turns that use turns instead of radians/degrees (1 turn = 360 degrees), making them faster.
- The very common vec2, vec3, vec4 and mat4. These last 2 use sse SIMD to speed them up substantially.
- rotor, a replacement to Quaternion from a branch of math called Geometric Algebra.
  Can be used in the same way as Quaternions, but often provide faster code, and they're easier to understand!
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

struct vec2; // forward decl to avoid circular reference
struct vec3; // forward decl to avoid circular reference
struct vec4; // forward decl to avoid circular reference

struct vec2 {
    union {
        struct {float x, y;};
        float ptr[2];
    };
};

struct vec3 {
    union {
        struct {float x,  y,  z;};
        struct {float yz, xz, xy;}; // geometric algebra's bivector (can be interpreted as a plane of rotation. The x axis is the yz plane, the y axis is the xz plane and the z axis is the xy plane). NOTE(cogno): the order of these elements is important, since the x axis is the yz plane, if x is the first float then so is yz. Also since convertion is trivial, bivector_dual is now an useless operation. Turning a vector into a bivector is as simple as calling obj.yz instead of obj.x, making them VERY fast.
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

// NOTE(cogno): In the past I had constructors to "simplify" creations of vec2/vec3/vec4 (like auto casting to float or auto making vec3 -> vec2 without vec3.z). I now find them basically useless. Most of the times you can just use vec2{...}, and the code is clearer. For example automatic conversion to float was used only in 3 occasions out of 290 (3/189 for vec2, 0/100 for vec3, 0/1 for vec4). Maybe we'll add back vec3->vec2 if usage increases, but most of the times people just forgets you can use vec2(...) so most stuff never gets used anyway.

struct rgb {
    union {
        struct { u8 r, g, b, a; };
        u8 ptr[4];
    };
};

struct hsv {
    union{
        struct { float h, s, v, a; }; // h is [0, 1] in turns, s,v and a are [0, 1].
        float ptr[4];
    };
};

// NOTE(cogno): most of the times you want colors to be normalized. Without creating useless structs and stuff, a normalized color is just 4 floats, aka a vec4!

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
inline void printsl_custom(rgb  c) {_buffer_append("rgb(%d, %d, %d, %d)", c.r, c.g, c.b, c.a);}
inline void printsl_custom(hsv  c) {_buffer_append("hsv(%.5f, %.5f, %.5f, %.5f)", c.h, c.s, c.v, c.a);}
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



inline vec4 rgb_normalize(rgb c) { return vec4{(float)c.r, (float)c.g, (float)c.b, (float)c.a} / 255; } // normalizes color from [0, 255] to [0, 1]
inline rgb rgb_denormalize(vec4 c) { c *= 255; return rgb{(u8)c.r, (u8)c.g, (u8)c.b, (u8)c.a}; } // de-normalizes color from [0, 1] to [0, 255]

inline rgb rgb_lerp(rgb a, rgb b, float t) {
    return {
        (u8)lerp(a.r, b.r, t),
        (u8)lerp(a.g, b.g, t),
        (u8)lerp(a.b, b.b, t),
        (u8)lerp(a.a, b.a, t)
    };
}

// converts a non normalized color to hsv notation
inline hsv hsv_from_rgb(rgb in) {
    // algorithm from https://math.stackexchange.com/questions/556341/rgb-to-hsv-color-conversion-algorithm
    auto conv = rgb_normalize(in);
    auto cmax = max(max(conv.r, conv.g), conv.b);
    auto cmin = min(min(conv.r, conv.g), conv.b);
    auto delta = cmax - cmin;
    float sat = 0.0f;
    float value = cmax;
    if (delta == 0) return {0, 0, cmax, conv.a};
    if (cmax != 0) sat = delta / cmax;
    float hue = 0;
    if (cmax == conv.r) hue = fmodf(    (conv.g - conv.b) / delta, 6) / 6;
    if (cmax == conv.g) hue = fmodf(2 + (conv.b - conv.r) / delta, 6) / 6;
    if (cmax == conv.b) hue = fmodf(4 + (conv.r - conv.g) / delta, 6) / 6;
    if (hue < 0) hue += 1;
    return {hue, sat, value, conv.a};
}

// converts an hsv valid color into non normalized rgb (so from 0 to 255 each)
inline rgb rgb_from_hsv(hsv in) {
    // algorithm from https://scratch.mit.edu/discuss/topic/694772/
    auto c = in.v * in.s;
    auto m = in.v - c;
    auto x = c * (1 - abs(fmodf(in.h * 6, 2) - 1));
    float r = 0;
    float g = 0;
    float b = 0;
    if      (in.h < 1.0f / 6) { r=c; g=x; b=0; }
    else if (in.h < 2.0f / 6) { r=x; g=c; b=0; }
    else if (in.h < 3.0f / 6) { r=0; g=c; b=x; }
    else if (in.h < 4.0f / 6) { r=0; g=x; b=c; }
    else if (in.h < 5.0f / 6) { r=x; g=0; b=c; }
    else if (in.h < 6.0f / 6) { r=c; g=0; b=x; }
    auto out = rgb_denormalize({r+m, g+m, b+m, in.a});
    
    return out;
}

// interpolates between 2 colors using hsv interpolation, returns a non-normalized rgb color
inline hsv hsv_lerp(hsv c1, hsv c2, float t) {
    auto s = lerp(c1.s, c2.s, t);
    auto v = lerp(c1.v, c2.v, t);
    auto a = lerp(c1.a, c2.a, t);

    auto maxh = max(c1.h, c2.h);
    auto minh = min(c1.h, c2.h);
    float h = 0;
    if (maxh - minh < 0.5) h = lerp(c1.h, c2.h, t); // normal angle lerp
    else {
        // circular lerp (instead of going 0.9 to 0.1 we passing by 0.5 we pass by 0.0)
        if (c1.h > c2.h) h = fmodf(lerp(c1.h, c2.h + 1, t), 1);
        else             h = fmodf(lerp(c1.h + 1, c2.h, t), 1);
    }
    hsv out = {h, s, v, a};
    return out;
}

inline vec4 hsv_normalize(hsv in) { return rgb_normalize(rgb_from_hsv(in)); }
inline hsv hsv_denormalize(vec4 in) { return hsv_from_rgb(rgb_denormalize(in)); }

// colors from raylib
#define LIGHTGRAY  rgb_normalize({ 200, 200, 200, 255 })   // Light Gray
#define GRAY       rgb_normalize({ 130, 130, 130, 255 })   // Gray
#define DARKGRAY   rgb_normalize({ 80, 80, 80, 255 })      // Dark Gray
#define YELLOW     rgb_normalize({ 253, 249, 0, 255 })     // Yellow
#define GOLD       rgb_normalize({ 255, 203, 0, 255 })     // Gold
#define ORANGE     rgb_normalize({ 255, 161, 0, 255 })     // Orange
#define PINK       rgb_normalize({ 255, 109, 194, 255 })   // Pink
#define RED        rgb_normalize({ 230, 41, 55, 255 })     // Red
#define MAROON     rgb_normalize({ 190, 33, 55, 255 })     // Maroon
#define GREEN      rgb_normalize({ 0, 228, 48, 255 })      // Green
#define LIME       rgb_normalize({ 0, 158, 47, 255 })      // Lime
#define DARKGREEN  rgb_normalize({ 0, 117, 44, 255 })      // Dark Green
#define SKYBLUE    rgb_normalize({ 102, 191, 255, 255 })   // Sky Blue
#define BLUE       rgb_normalize({ 0, 121, 241, 255 })     // Blue
#define DARKBLUE   rgb_normalize({ 0, 82, 172, 255 })      // Dark Blue
#define PURPLE     rgb_normalize({ 200, 122, 255, 255 })   // Purple
#define VIOLET     rgb_normalize({ 135, 60, 190, 255 })    // Violet
#define DARKPURPLE rgb_normalize({ 112, 31, 126, 255 })    // Dark Purple
#define BEIGE      rgb_normalize({ 211, 176, 131, 255 })   // Beige
#define BROWN      rgb_normalize({ 127, 106, 79, 255 })    // Brown
#define DARKBROWN  rgb_normalize({ 76, 63, 47, 255 })      // Dark Brown
#define WHITE      rgb_normalize({ 255, 255, 255, 255 })   // White
#define BLACK      rgb_normalize({ 0, 0, 0, 255 })         // Black
#define BLANK      rgb_normalize({ 0, 0, 0, 0 })           // Blank (Transparent)
#define MAGENTA    rgb_normalize({ 255, 0, 255, 255 })     // Magenta
#define RAYWHITE   rgb_normalize({ 245, 245, 245, 255 })   // My own White (raylib logo)

#define RGB_LIGHTGRAY  rgb{ 200, 200, 200, 255 }   // Light Gray
#define RGB_GRAY       rgb{ 130, 130, 130, 255 }   // Gray
#define RGB_DARKGRAY   rgb{ 80, 80, 80, 255 }      // Dark Gray
#define RGB_YELLOW     rgb{ 253, 249, 0, 255 }     // Yellow
#define RGB_GOLD       rgb{ 255, 203, 0, 255 }     // Gold
#define RGB_ORANGE     rgb{ 255, 161, 0, 255 }     // Orange
#define RGB_PINK       rgb{ 255, 109, 194, 255 }   // Pink
#define RGB_RED        rgb{ 230, 41, 55, 255 }     // Red
#define RGB_MAROON     rgb{ 190, 33, 55, 255 }     // Maroon
#define RGB_GREEN      rgb{ 0, 228, 48, 255 }      // Green
#define RGB_LIME       rgb{ 0, 158, 47, 255 }      // Lime
#define RGB_DARKGREEN  rgb{ 0, 117, 44, 255 }      // Dark Green
#define RGB_SKYBLUE    rgb{ 102, 191, 255, 255 }   // Sky Blue
#define RGB_BLUE       rgb{ 0, 121, 241, 255 }     // Blue
#define RGB_DARKBLUE   rgb{ 0, 82, 172, 255 }      // Dark Blue
#define RGB_PURPLE     rgb{ 200, 122, 255, 255 }   // Purple
#define RGB_VIOLET     rgb{ 135, 60, 190, 255 }    // Violet
#define RGB_DARKPURPLE rgb{ 112, 31, 126, 255 }    // Dark Purple
#define RGB_BEIGE      rgb{ 211, 176, 131, 255 }   // Beige
#define RGB_BROWN      rgb{ 127, 106, 79, 255 }    // Brown
#define RGB_DARKBROWN  rgb{ 76, 63, 47, 255 }      // Dark Brown
#define RGB_WHITE      rgb{ 255, 255, 255, 255 }   // White
#define RGB_BLACK      rgb{ 0, 0, 0, 255 }         // Black
#define RGB_BLANK      rgb{ 0, 0, 0, 0 }           // Blank (Transparent)
#define RGB_MAGENTA    rgb{ 255, 0, 255, 255 }     // Magenta
#define RGB_RAYWHITE   rgb{ 245, 245, 245, 255 }   // My own White (raylib logo)

#define HSV_LIGHTGRAY  hsv_from_rgb({ 200, 200, 200, 255 })   // Light Gray
#define HSV_GRAY       hsv_from_rgb({ 130, 130, 130, 255 })   // Gray
#define HSV_DARKGRAY   hsv_from_rgb({ 80, 80, 80, 255 })      // Dark Gray
#define HSV_YELLOW     hsv_from_rgb({ 253, 249, 0, 255 })     // Yellow
#define HSV_GOLD       hsv_from_rgb({ 255, 203, 0, 255 })     // Gold
#define HSV_ORANGE     hsv_from_rgb({ 255, 161, 0, 255 })     // Orange
#define HSV_PINK       hsv_from_rgb({ 255, 109, 194, 255 })   // Pink
#define HSV_RED        hsv_from_rgb({ 230, 41, 55, 255 })     // Red
#define HSV_MAROON     hsv_from_rgb({ 190, 33, 55, 255 })     // Maroon
#define HSV_GREEN      hsv_from_rgb({ 0, 228, 48, 255 })      // Green
#define HSV_LIME       hsv_from_rgb({ 0, 158, 47, 255 })      // Lime
#define HSV_DARKGREEN  hsv_from_rgb({ 0, 117, 44, 255 })      // Dark Green
#define HSV_SKYBLUE    hsv_from_rgb({ 102, 191, 255, 255 })   // Sky Blue
#define HSV_BLUE       hsv_from_rgb({ 0, 121, 241, 255 })     // Blue
#define HSV_DARKBLUE   hsv_from_rgb({ 0, 82, 172, 255 })      // Dark Blue
#define HSV_PURPLE     hsv_from_rgb({ 200, 122, 255, 255 })   // Purple
#define HSV_VIOLET     hsv_from_rgb({ 135, 60, 190, 255 })    // Violet
#define HSV_DARKPURPLE hsv_from_rgb({ 112, 31, 126, 255 })    // Dark Purple
#define HSV_BEIGE      hsv_from_rgb({ 211, 176, 131, 255 })   // Beige
#define HSV_BROWN      hsv_from_rgb({ 127, 106, 79, 255 })    // Brown
#define HSV_DARKBROWN  hsv_from_rgb({ 76, 63, 47, 255 })      // Dark Brown
#define HSV_WHITE      hsv_from_rgb({ 255, 255, 255, 255 })   // White
#define HSV_BLACK      hsv_from_rgb({ 0, 0, 0, 255 })         // Black
#define HSV_BLANK      hsv_from_rgb({ 0, 0, 0, 0 })           // Blank (Transparent)
#define HSV_MAGENTA    hsv_from_rgb({ 255, 0, 255, 255 })     // Magenta
#define HSV_RAYWHITE   hsv_from_rgb({ 245, 245, 245, 255 })   // My own White (raylib logo)



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
    res.y = b.x * a.z - a.x * b.z;
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
inline mat4 operator +=(mat4& m1, const mat4& m2) { m1 = m1 + m2; return m1;}
inline mat4 operator -=(mat4& m1, const mat4& m2) { m1 = m1 - m2; return m1;}
inline mat4 operator *=(mat4& m1, const float& b) { m1 = m1 * b; return m1; }
inline mat4 operator *=(mat4& m1, const mat4& m2) { m1 = m1 * m2; return m2; }
inline mat4 operator /=(mat4& m1, const float& b) { m1 = m1 / b; return m1; }

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


//
// 3D Geometric Algebra
//

// basic operations:
// we already have vec3 dot vec3
vec3 vec3_wedge(vec3 a, vec3 b) {
    vec3 out = {};
    out.xy = a.x * b.y - a.y * b.x;
    out.yz = a.y * b.z - a.z * b.y;
    out.xz = a.x * b.z - a.z * b.x;
    return out;
}

// now we use GA for real work, to substitute Quaternions!
struct rotor {
    float s = 1;
    vec3 bivec;
};

void printsl_custom(rotor r) {
    printsl("(%, xy=%, yz=%, xz=%)", r.s, r.bivec.xy, r.bivec.yz, r.bivec.xz);
}

// produces a rotation which rotates from one vector to another
rotor rotor_from_to(vec3 from, vec3 to) {
    vec3 from_dir = vec3_normalize(from, vec3{0, 0, 0});
    vec3 to_dir   = vec3_normalize(to, vec3{0, 0, 0});
    
    // if we multiply 2 vectors we get a rotor for a rotation in the plane of the 
    // 2 vectors but with TWICE the angle. So we calculate the middle vector
    // between the 2 input ones and use that.
    // if the 2 vectors point opposite to each other the half will be 0, because we can
    // rotate in ANY direction perpendicular to the input 2
    vec3 half = vec3_normalize(from_dir + to_dir, vec3{0, 0, 0});
    ASSERT(half.x != 0 || half.y != 0 || half.z != 0, "attempted to create a rotation between vectors opposite of each other. There are INFINITELY many rotations, so we don't know which one to choose. You should handle this case separately. Rotation was from % to %", from_dir, to_dir);
    
    // the product of 2 vectors ab is the dot + the wedge
    // ab = a . b + a ^ b;
    rotor out = {};
    out.s = vec3_dot(from_dir, half);
    out.bivec = vec3_wedge(from_dir, half);
    return out;
}

rotor rotor_from_axis_angle(vec3 rot_axis, float angle) {
    rot_axis = vec3_normalize(rot_axis, vec3{0, 0, 0});
    float c = cos_turns(angle / 2);
    float s = sin_turns(angle / 2);
    
    // turn the axis into a bivector by flipping components,
    // a rotation around the x axis is a rotation on the yz plane.
    // then we embed the angle into each component following this post: 
    // https://jacquesheunis.com/post/rotors/#axis-angle-representation-for-rotors

    rotor r = {};
    r.s        = c;
    r.bivec.xy =  rot_axis.xy * s;
    r.bivec.yz =  rot_axis.yz * s;
    r.bivec.xz = -rot_axis.xz * s;
    return r;
}

// return a rotation of in.xy turns on the xy plane, in.yz turns in the yz plane and in.xz turns in the xz plane
rotor rotor_from_turns(vec3 in) {
    // converting from turns to rotors is really easy,
    // you just split the full rotation into 3 separate ones one after another!
    // first rotate on the xy plane, then the yz plane then the xz plane.
    // we could just use rotor_from_axis_angle and rotor_combine to do so, but 
    // that would be more expensive than what we need
    // since we only rotate on 1 plane at a time we can avoid most calculations
    
    vec3 angles = in / 2; // rotors move by half the angle
    float cos_xy = cos_turns(angles.xy);
    float cos_yz = cos_turns(angles.yz);
    float cos_xz = cos_turns(angles.xz);
    float sin_xy = sin_turns(angles.xy);
    float sin_yz = sin_turns(angles.yz);
    float sin_xz = sin_turns(angles.xz);
    
    // again, paper calculations
    rotor out = {};
    out.s        = cos_xy * cos_yz * cos_xz - sin_xy * sin_yz * sin_xz;
    out.bivec.xy = sin_xy * cos_yz * cos_xz + cos_xy * sin_yz * sin_xz;
    out.bivec.yz = cos_xy * sin_yz * cos_xz - sin_xy * cos_yz * sin_xz;
    out.bivec.xz = cos_xy * cos_yz * sin_xz + sin_xy * sin_yz * cos_xz;
    return out;
}

// combines 2 rotation into 1 total one
rotor rotor_combine(rotor a, rotor b) {
    // API(cogno): maybe we can make a product overload so you can rotor * rotor ?
    // API(cogno): we might also do this for rotor * scalar, rotor * vec3 and rotor * trivec
    float a0 = a.s;
    float a1 = a.bivec.xy;
    float a2 = a.bivec.yz;
    float a3 = a.bivec.xz;
    float b0 = b.s;
    float b1 = b.bivec.xy;
    float b2 = b.bivec.yz;
    float b3 = b.bivec.xz;
    
    // just tedious paper calculations, nothing exciting
    rotor out = {};
    out.s        = a0*b0 - a1*b1 - a2*b2 - a3*b3;
    out.bivec.xy = a0*b1 + a1*b0 + a2*b3 - a3*b2;
    out.bivec.yz = a0*b2 + a2*b0 - a1*b3 + a3*b1;
    out.bivec.xz = a0*b3 + a3*b0 + a1*b2 - a2*b1;
    return out;
}

// creates a rotation in the opposite direction
rotor rotor_reverse(rotor in) {
    rotor out = in;
    out.bivec *= -1; // we just invert the bivector component, scalar doesn't change
    return out;
}

// rotates a vector based on a rotor
vec3 vec3_rotate(vec3 v, rotor r) {
    // again, tedious paper calculations, not much to do
    // it's the sandwich product R^-1 v R, where R^-1 is reverse(R) which just
    // flips the sign of the bivec, so I did it here in one go to make it faster
    // NOTE(cogno): Almost every resource says R v R^-1, but that's wrong,
    // if you do it like that you will have *clockwise* rotations, not counter-clockwise!
    // the proper rotation is seen in https://marctenbosch.com/quaternions/
    
    float r0 = r.s;
    float r1 = r.bivec.xy;
    float r2 = r.bivec.yz;
    float r3 = r.bivec.xz;
    
    // S = R^-1 * v
    float s1 =   v.x*r0 - v.y*r1 - v.z*r3;
    float s2 =   v.y*r0 + v.x*r1 - v.z*r2;
    float s3 =   v.z*r0 + v.x*r3 + v.y*r2;
    float s4 = - v.z*r1 - v.x*r2 + v.y*r3;
    
    // as a check, when finishing the calculations, the trivector component should disappear (aka be zero)
    #if !NO_ASSERT
    float trivec = r0*s4 + r1*s3 + r2*s1 - r3*s2;
    ASSERT(abs(trivec) <= 0.01f, "WRONG ROTOR CALCULATIONS. We expected the trivector component to disappear but it remained (was %).", trivec);
    #endif
    
    // out = S * R
    vec3 out = {};
    out.x = r0*s1 - r1*s2 - r2*s4 - r3*s3;
    out.y = r0*s2 + r1*s1 - r2*s3 + r3*s4;
    out.z = r0*s3 - r1*s4 + r2*s2 + r3*s1;
    return out;
}

mat4 make_matrix_from_rotor(rotor r){
    vec3 new_x = vec3_rotate(vec3{1, 0, 0}, r);
    vec3 new_y = vec3_rotate(vec3{0, 1, 0}, r);
    vec3 new_z = vec3_rotate(vec3{0, 0, 1}, r);

    mat4 out = {};
    out.m11  = new_x.x;
    out.m21  = new_x.y;
    out.m31  = new_x.z;
    out.m12  = new_y.x;
    out.m22  = new_y.y;
    out.m32  = new_y.z;
    out.m13  = new_z.x;
    out.m23  = new_z.y;
    out.m33  = new_z.z;
    out.m44 = 1;
    return out;
}

rotor rotor_normalize(rotor to_norm) {
    auto reverse = rotor_reverse(to_norm);
    auto result = rotor_combine(to_norm, reverse);
    ASSERT(result.bivec.xz == 0 && result.bivec.xy == 0 && result.bivec.yz == 0, "wrong math!, expected float only but got %", result);
    rotor out = {};
    out.s = to_norm.s / result.s;
    out.bivec = to_norm.bivec / result.s;
    return out;
}

