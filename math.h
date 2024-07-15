#pragma once
#define GYOMATH

/*
In this file:
- Many math functions! saying all of them would be too much. Here's the more unique/important ones:
- sin and cos use turns instead of radians/degrees (1 turn = 360 degrees)
- vec2, vec3, vec4 and mat4, vec4 and mat4 use sse SIMD to speed them up substantially
- Rotor, a replacement to Quaternion from a branch of math called Geometric Algebra.
  Can be used in the same way as Quaternions, but often provide faster code, and they're easier to understand!
*/

#ifndef DISABLE_INCLUDES
    #include <smmintrin.h>
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

// TODO(cogno): count_digits for s64
// TODO(cogno): count_digits for f64

inline float npow(float x, u32 n){
    float res = x;
    for(u32 i = 1; i < n; i++){
        res *= x;
    }
    return res;
}

// API(cogno): I don't know if these guards are a good idea. I think we risk that other libraries use OUR functions instead of theirs, if we have a bug here we are breaking stuff KILOMETERS away...
// API(cogno): can these functions be done branchless? is it faster if it's branchless?
#ifndef floor
    #define floor __floor
    inline float floor(float x) { return (float)(x >= 0 ? int(x) : int(x - 0.9999999999f)); }
#endif
#ifndef ceil
    #define ceil __ceil
    inline float ceil(float x) { return (float)(x < 0 ? int(x) : int(x + 1)); }
#endif
#ifndef round
    #define round __round
    inline float round(float x) { return (float)(x >= 0 ? floor(x + 0.5) : ceil(x - 0.5)); }
#endif
#ifndef trunc
    #define trunc __trunc
    inline float trunc(float x) { return float(int(x)); }
#endif
#ifndef sqrt
    #define sqrt __sqrt
    inline float sqrt(float x)  { return _mm_cvtss_f32(_mm_sqrt_ss(_mm_set_ss(x))); }
#endif

inline float roundn(float x, u32 n) { return round(x * npow(10, n)) / npow(10, n); }

inline float remap(float in, float old_from, float old_to, float new_from, float new_to) {
    return (in - old_from) / (old_to - old_from) * (new_to - new_from) + new_from;
}

// API(cogno): if already defined don't create them (or undef them?)
#define min(a, b) (((a) < (b)) ? (a) : (b))
#define max(a, b) (((a) < (b)) ? (b) : (a))
#define abs(a) (((a) >= 0) ? (a) : (-(a)))
#define sgn(a) (((a) == 0) ? (0) : (((a) > 0) ? 1 : -1)) // API(cogno): maybe have sgn(0)==1 and sgn_with_zero(0)==0? check how this is used!
#define clamp(val, min_, max_) (max(min((val), (max_)), (min_)))
#define fmod(x, y) ((x) - trunc((x) / (y)) * (y))
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

// Todo(Quattro): implement other trigonometric functions
// API(cogno): if already defined don't create them (or undef them?)
inline float cos_turns(float angle)  { return sin_turns(angle + 0.25f); }
inline float tan_turns(float angle)  { return sin_turns(angle) / cos_turns(angle); }
inline float cot_turns(float angle)  { return cos_turns(angle) / sin_turns(angle); }

struct vec2{
    union{
        float ptr[2];
        struct {float x, y;};
    };
    vec2() = default;
    template <typename A, typename B> vec2(A a, B b){
        x = (float)a;
        y = (float)b;
    }
};

struct vec3{
    union{
        float ptr[3];
        struct {float x, y, z;};
    };
    vec3() = default;
    template <typename A, typename B, typename C> vec3(A a, B b, C c){
        x = (float)a;
        y = (float)b;
        z = (float)c;
    }
};

struct vec4{
    union{
        float ptr[4];
        struct {float x, y, z, w;};
        __m128 v;
    };
    vec4() = default;
    template <typename A, typename B, typename C, typename D> vec4(A a, B b, C c, D d){
        this->x = (float)a;
        this->y = (float)b;
        this->z = (float)c;
        this->w = (float)d;
    }
};

struct col{
    union{
        float ptr[4];
        struct {float r, g, b, a;}; // by default each is from 0 to 255, but when they're used they get normalized from 0 to 1 using col_normalize
        struct {float h, s, v, a;}; // h is [0, 360], s,v and a are [0, 1]. If you take a color rgb and use hsv values will be wrong, you have to first convert it using col_rgb_to_hsv and vice versa
        __m128 vec;
    };
    col() = default;
    template <typename A, typename B, typename C, typename D> col(A r, B g, C b, D a){
        this->r = (float)r;
        this->g = (float)g;
        this->b = (float)b;
        this->a = (float)a;
    }
};

// colors from raylib
#define LIGHTGRAY  col{ 200, 200, 200, 255 }   // Light Gray
#define GRAY       col{ 130, 130, 130, 255 }   // Gray
#define DARKGRAY   col{ 80, 80, 80, 255 }      // Dark Gray
#define YELLOW     col{ 253, 249, 0, 255 }     // Yellow
#define GOLD       col{ 255, 203, 0, 255 }     // Gold
#define ORANGE     col{ 255, 161, 0, 255 }     // Orange
#define PINK       col{ 255, 109, 194, 255 }   // Pink
#define RED        col{ 230, 41, 55, 255 }     // Red
#define MAROON     col{ 190, 33, 55, 255 }     // Maroon
#define GREEN      col{ 0, 228, 48, 255 }      // Green
#define LIME       col{ 0, 158, 47, 255 }      // Lime
#define DARKGREEN  col{ 0, 117, 44, 255 }      // Dark Green
#define SKYBLUE    col{ 102, 191, 255, 255 }   // Sky Blue
#define BLUE       col{ 0, 121, 241, 255 }     // Blue
#define DARKBLUE   col{ 0, 82, 172, 255 }      // Dark Blue
#define PURPLE     col{ 200, 122, 255, 255 }   // Purple
#define VIOLET     col{ 135, 60, 190, 255 }    // Violet
#define DARKPURPLE col{ 112, 31, 126, 255 }    // Dark Purple
#define BEIGE      col{ 211, 176, 131, 255 }   // Beige
#define BROWN      col{ 127, 106, 79, 255 }    // Brown
#define DARKBROWN  col{ 76, 63, 47, 255 }      // Dark Brown

#define WHITE      col{ 255, 255, 255, 255 }   // White
#define BLACK      col{ 0, 0, 0, 255 }         // Black
#define BLANK      col{ 0, 0, 0, 0 }           // Blank (Transparent)
#define MAGENTA    col{ 255, 0, 255, 255 }     // Magenta
#define RAYWHITE   col{ 245, 245, 245, 255 }   // My own White (raylib logo)

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


inline void printsl_custom(vec2 v) {buffer_append("(%.5f, %.5f)", v.x, v.y);}
inline void printsl_custom(vec3 v) {buffer_append("(%.5f, %.5f, %.5f)", v.x, v.y, v.z);}
inline void printsl_custom(vec4 v) {buffer_append("(%.5f, %.5f, %.5f, %.5f)", v.x, v.y, v.z, v.w);}
inline void printsl_custom(col v)  {buffer_append("(%.2f, %.2f, %.2f, %.2f)", v.r, v.g, v.b, v.a);}
inline void printsl_custom(mat4 m) {buffer_append("|%.5f %.5f %.5f %.5f|\n|%.5f %.5f %.5f %.5f|\n|%.5f %.5f %.5f %.5f|\n|%.5f %.5f %.5f %.5f|\n", m.m11, m.m12, m.m13, m.m14, m.m21, m.m22, m.m23, m.m24, m.m31, m.m32, m.m33, m.m34, m.m41, m.m42, m.m43, m.m44);}


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
inline float vec4_length_squared(vec4 v) {
    return _mm_cvtss_f32(_mm_dp_ps(v.v, v.v, 0b11110001));
}

inline float vec2_length(vec2 v) { return sqrt(vec2_length_squared(v)); }
inline float vec3_length(vec3 v) { return sqrt(vec3_length_squared(v)); }
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

inline col col_normalize(col c) {col res; res.vec = _mm_div_ps(c.vec, _mm_set1_ps(255)); return res;} // normalizes color from [0, 255] to [0, 1]
inline col col_denormalize(col c) {col res; res.vec = _mm_mul_ps(c.vec, _mm_set1_ps(255)); return res;} // de-normalizes color from [0, 1] to [0, 255]

// converts a non normalized color to hsv notation
inline col col_rgb_to_hsv(col in) {
    // algorithm from https://math.stackexchange.com/questions/556341/rgb-to-hsv-color-conversion-algorithm
    auto conv = col_normalize(in);
    auto cmax = max(max(conv.r, conv.g), conv.b);
    auto cmin = min(min(conv.r, conv.g), conv.b);
    auto delta = cmax - cmin;
    float sat = 0.0f;
    float value = cmax;
    if (delta == 0) return {0, 0, cmax, conv.a};
    if (cmax != 0) sat = delta / cmax;
    float hue = 0;
    if (cmax == conv.r) hue = 60 * (fmodf(    (conv.g - conv.b) / delta, 6));
    if (cmax == conv.g) hue = 60 * (fmodf(2 + (conv.b - conv.r) / delta, 6));
    if (cmax == conv.b) hue = 60 * (fmodf(4 + (conv.r - conv.g) / delta, 6));
    return {hue, sat, value, conv.a};
}

// converts an hsv valid color into non normalized rgb (so from 0 to 255 each)
inline col color_hsv_to_rgb(col in) {
    // algorithm from https://scratch.mit.edu/discuss/topic/694772/
    auto c = in.v * in.s;
    auto m = in.v - c;
    auto x = c * (1 - abs(fmodf(in.h / 60, 2) - 1));
    float r = 0;
    float g = 0;
    float b = 0;
    if      (in.h <  60) { r=c; g=x; b=0; }
    else if (in.h < 120) { r=x; g=c; b=0; }
    else if (in.h < 180) { r=0; g=c; b=x; }
    else if (in.h < 240) { r=0; g=x; b=c; }
    else if (in.h < 300) { r=x; g=0; b=c; }
    else if (in.h < 360) { r=c; g=0; b=x; }
    
    col out = {r+m, g+m, b+m, in.a};
    return col_denormalize(out);
}

// interpolates between 2 colors using hsv interpolation, returns a non-normalized rgb color
inline col color_lerp_hsv(col c1, col c2, float t) {
    auto v1 = col_rgb_to_hsv(c1);
    auto v2 = col_rgb_to_hsv(c2);
    auto h = lerp(v1.h, v2.h, t);
    auto s = lerp(v1.s, v2.s, t);
    auto v = lerp(v1.v, v2.v, t);
    auto a = lerp(v1.a, v2.a, t);
    h = fmodf(h + 360, 360); // circularly lerp
    col out = color_hsv_to_rgb({h, s, v, a});
    return out;
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


inline mat4 mat4_new(float n){
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
    float angle = atan2f(det, dot) * RAD2DEG; //API(cogno): maybe atan2 is better?
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
    A•B / B (where A•B is the dot product)
    
                    - Cogno 2023/07/21
    */
    
    vec3 a = to_project;
    vec3 b = dir;
    float dot = vec3_dot(a, b);
    float magn = vec3_length_squared(b);
    float term = dot / magn;
    vec3 perp = term * b;
    return perp;
    /*
    fallback just in case
    TODO(cogno): compare performance:
        Vec3 a = to_project;
        Vec3 b = plane_norm;
        float bmag = vec3_magn(b);
        Vec3 c1 = vec3_cross(a, b);
        Vec3 c1_norm = c1 / bmag;
        Vec3 c2 = vec3_cross(b, c1_norm);
        Vec3 c2_norm = c2 / bmag;
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

struct bivec { float xy, yz, xz; }; // API(cogno): wouldn't it be better if we use vec3 so we don't have to convert back and forth?
struct trivec { float xyz; }; // API(cogno): wouldn't it be better to just use float so we don't have to convert back and forth?

void printsl_custom(bivec b) {
    printsl("(%, %, %)", b.xy, b.yz, b.xz);
}

void printsl_custom(trivec t) {
    printsl("%", t.xyz);
}

// TODO(cogno): bivec + bivec, bivec * scalar and the rest
inline bivec operator+(bivec a, bivec b) { return {a.xy + b.xy, a.yz + b.yz, a.xz + b.xz}; }
inline bivec operator-(bivec a, bivec b) { return {a.xy - b.xy, a.yz - b.yz, a.xz - b.xz}; }
inline bivec operator-(bivec a)          { return {-a.xy, -a.yz, -a.xz}; }
inline bivec operator*(bivec a, float b) { return {a.xy * b, a.yz * b, a.xz * b}; }
inline bivec operator*(float a, bivec b) { return {b.xy * a, b.yz * a, b.xz * a}; }
inline bivec operator/(bivec a, float b) { return {a.xy / b, a.yz / b, a.xz / b}; }
inline bivec operator/(float a, bivec b) { return {a / b.xy, a / b.yz, a / b.xz}; }

inline bivec& operator*=(bivec& a, float b) { a.xy *= b; a.yz *= b; a.xz *= b; return a; }

// basic operations:
// we already have vec3 dot vec3
bivec vec3_wedge(vec3 a, vec3 b) {
    bivec out = {};
    out.xy = a.x * b.y - a.y * b.x;
    out.yz = a.y * b.z - a.z * b.y;
    out.xz = a.x * b.z - a.z * b.x;
    return out;
}

vec3 bivec_dual(bivec in) {
    vec3 out = {};
    out.x = in.yz;
    out.y = in.xz;
    out.z = in.xy;
    return out;
}

bivec bivec_dual(vec3 in) {
    bivec out = {};
    out.xy = in.z;
    out.yz = in.x;
    out.xz = in.y;
    return out;
}

// now we use GA for real work, to substitute Quaternions!
struct rotor {
    float s = 1;
    bivec bivec;
};

void printsl_custom(rotor r) {
    printsl("(%, %, %, %)", r.s, r.bivec.xy, r.bivec.yz, r.bivec.xz);
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
    ASSERT(half.x != 0 && half.y != 0 and half.z != 0, "attempted to create a rotation between vectors opposite of each other. There are INFINITELY many rotations, so we don't know which one to choose. You should handle this case separately. Rotation was from % to %", from_dir, to_dir);
    
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
    
    auto rotation_plane = bivec_dual(rot_axis);
    rotor r;
    r.s        = c;
    r.bivec.xy =  rotation_plane.xy * s;
    r.bivec.yz =  rotation_plane.yz * s;
    r.bivec.xz = -rotation_plane.xz * s;
    return r;
}

// return a rotation of in.xy turns on the xy plane, in.yz turns in the yz plane and in.xz turns in the xz plane 
rotor rotor_from_turns(bivec in) {
    // converting from turns to rotors is really easy,
    // you just split the full rotation into 3 separate ones one after another!
    // first rotate on the xy plane, then the yz plane then the xz plane.
    // we could just use rotor_from_axis_angle and rotor_combine to do so, but 
    // that would be more expensive than what we need
    // since we only rotate on 1 plane at a time we can avoid most calculations
    
    bivec angles = in / 2; // rotors move by half the angle
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
    out.bivec.yz = cos_xy * sin_yz * cos_xz + sin_xy * cos_yz * sin_xz;
    out.bivec.xz = cos_xy * cos_yz * sin_xz - sin_xy * sin_yz * cos_xz;
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
    // it's the sandwich product R v R^-1, where R^-1 is reverse(R) which just
    // flips the sign of the bivec, so I did it here in one go to make it faster
    
    float r0 = r.s;
    float r1 = r.bivec.xy;
    float r2 = r.bivec.yz;
    float r3 = r.bivec.xz;
    
    // s = R * v
    float s1 = v.x*r0 + v.y*r1 + v.z*r3;
    float s2 = v.y*r0 - v.x*r1 + v.z*r2;
    float s3 = v.z*r0 - v.x*r3 - v.y*r2;
    float s4 = v.z*r1 + v.x*r2 - v.y*r3;
    
    // as a check, when finishing the calculations, the trivector component should disappear (aka be zero)
    #if !NO_ASSERT
    float trivec = r0*s4 - r1*s3 - r2*s1 + r3*s2;
    ASSERT(abs(trivec) <= 0.01f, "WRONG ROTOR CALCULATIONS. We expected the trivector component to disappear but it remained (was %).", trivec);
    #endif
    
    // out = s * R^-1
    vec3 out = {};
    out.x = r0*s1 + r1*s2 + r2*s4 + r3*s3;
    out.y = r0*s2 - r1*s1 + r2*s3 - r3*s4;
    out.z = r0*s3 + r1*s4 - r2*s2 - r3*s1;
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
