#pragma once
#define GYOMATH

#ifndef DISABLE_INCLUDES
    #include <smmintrin.h>
#endif

#ifndef GYOFIRST
    #include "first.h"
#endif

inline int count_digits(u64 x){
    int n = 0;
    int p = x;
    do{
        n++;
    }while(p /= 10);
    return n;
}
inline float npow(float x, u32 n){
    float res = x;
    for(int i = 1; i < n; i++){
        res *= x;
    }
    return res;
}
inline float floor(float x)          { return x >= 0 ? int(x) : int(x - 0.9999999999f); }
inline float ceil(float x)           { return x < 0 ? int(x) : int(x + 1); }
inline float round(float x)          { return x >= 0 ? floor(x + 0.5) : ceil(x - 0.5); }
inline float roundn(float x, u32 n)  { return round(x * npow(10, n)) / npow(10, n); }
inline float trunc(float x)          { return float(int(x)); }

#define min(a, b) (((a) < (b)) ? (a) : (b))
#define max(a, b) (((a) < (b)) ? (b) : (a))
#define abs(a) (((a) >= 0) ? (a) : (-(a)))
#define sgn(a) (((a) == 0) ? (0) : (((a) > 0) ? 1 : -1))
#define clamp(val, min_, max_) (max(min((val), (max_)), (min_)))
#define fmod(x, y) ((x) - trunc((x) / (y)) * (y))
#define lerp(start, dest, t) ((dest - start) * t + start)

#define PI        (3.1415926535f)
#define TAU       (6.2831853072f)
#define E         (2.7182818284f)
#define SQRT2     (1.4142135623f)
#define SQRT3     (1.7320508075f)
#define DEG2RAD   (PI / 180.0f)
#define DEG2TURNS (1.0f / 360.0f)
#define RAD2DEG   (180.0f / PI)
#define RAD2TURNS (1.0f / TAU)
#define TURNS2DEG (360.0f)
#define TURNS2RAD (TAU)

inline float _sin_internal(float x) {
    float q = 8 * x - 16 * x * x;
    return 0.225 * (q * q - q) + q;
}

// trigonometric functions in turns (1 turn = 360 deg)
inline float sin(float angle) {
    angle -= int(angle);
    
    if(angle < 0) angle += 1;
    if(angle > 0.5) return -_sin_internal(angle - 0.5);
    return _sin_internal(angle);
}
// Todo(Quattro): implement other trigonometric functions
inline float cos(float angle)  { return sin(angle + 0.25); }
inline float tan(float angle)  { return sin(angle) / cos(angle); }
inline float cot(float angle)  { return cos(angle) / sin(angle); }
inline float sqrt(float x)     { return _mm_cvtss_f32(_mm_sqrt_ss(_mm_set_ss(x))); }

struct vec2{
    union{
        float ptr[2];
        struct {float x, y;};
    };
};

struct vec3{
    union{
        float ptr[3];
        struct {float x, y, z;};
    };
};

struct vec4{
    union{
        float ptr[4];
        struct {float x, y, z, w;};
        __m128 v;
    };
};

struct col{
    union{
        float ptr[4];
        struct {float r, g, b, a;};
        __m128 v;
    };
};

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


inline void printsl(vec2 v) {fast_print("(%.5f, %.5f)", v.x, v.y);}
inline void printsl(vec3 v) {fast_print("(%.5f, %.5f, %.5f)", v.x, v.y, v.z);}
inline void printsl(vec4 v) {fast_print("(%.5f, %.5f, %.5f, %.5f)", v.x, v.y, v.z, v.w);}
inline void printsl(mat4 m) {fast_print("|%.5f %.5f %.5f %.5f|\n|%.5f %.5f %.5f %.5f|\n|%.5f %.5f %.5f %.5f|\n|%.5f %.5f %.5f %.5f|\n", m.m11, m.m12, m.m13, m.m14, m.m21, m.m22, m.m23, m.m24, m.m31, m.m32, m.m33, m.m34, m.m41, m.m42, m.m43, m.m44);}
// TODO(cogno): printsl col


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
inline vec2 operator *(vec2 a, vec2 b) {return {a.x * b.x, a.y * b.y};}
inline vec3 operator *(vec3 a, vec3 b) {return {a.x * b.x, a.y * b.y, a.z * b.z};}
inline vec4 operator *(vec4 a, vec4 b) {vec4 res; res.v = _mm_mul_ps(a.v, b.v); return res;}
inline vec2 operator /(vec2 a, float s) {return {a.x / s, a.y / s};}
inline vec3 operator /(vec3 a, float s) {return {a.x / s, a.y / s, a.z / s};}
inline vec4 operator /(vec4 a, float s) {vec4 res; res.v = _mm_div_ps(a.v, _mm_set1_ps(s)); return res;}
inline vec2 operator /(float s, vec2 a) {return {s / a.x, s / a.y};}
inline vec3 operator /(float s, vec3 a) {return {s / a.x, s / a.y, s / a.z};}
inline vec4 operator /(float s, vec4 a) {vec4 res; res.v = _mm_div_ps(_mm_set1_ps(s), a.v); return res;}
inline vec2 operator -(vec2 a)  {return {-a.x, -a.y};}
inline vec3 operator -(vec3 a)  {return {-a.x, -a.y, -a.z};}
inline vec4 operator -(vec4 a)  {vec4 res;  res.v = _mm_sub_ps({}, a.v);  return res;}
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

inline float vec2_length(vec2 v) { return sqrt(v.x * v.x + v.y * v.y); }
inline float vec3_length(vec3 v) { return sqrt(v.x * v.x + v.y * v.y + v.z * v.z); }
inline float vec4_length(vec4 v) {
    vec4 res;
    res.v = _mm_dp_ps(v.v, v.v, 0b11110001);
    res.v = _mm_sqrt_ss(res.v);
    return _mm_cvtss_f32(res.v);
}

inline vec2 vec2_normalize(vec2 v){
    float len = vec2_length(v);
    return {v.x / len, v.y / len};
}
inline vec3 vec3_normalize(vec3 v){
    float len = vec3_length(v);
    return {v.x / len, v.y / len, v.z / len};
}
inline vec4 vec4_normalize(vec4 v){
    vec4 res;
    res.v = _mm_div_ps(v.v, _mm_sqrt_ps(_mm_dp_ps(v.v, v.v, 0b11111111)));
    return res;
}

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

inline col col_normalize(col c) {col res; res.v = _mm_div_ps(c.v, _mm_set1_ps(255)); return res;}
inline vec2 vec2_rotate(vec2 v, float angle){
    float cos_value = cos(angle);
    float sin_value = sin(angle);
    
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
inline mat4 operator +=(mat4& m1, const mat4& m2) { m1 = m1 + m2; return m1;}
inline mat4 operator -=(mat4& m1, const mat4& m2) { m1 = m1 - m2; return m1;}
inline mat4 operator *=(mat4& m1, const float& b) { m1 = m1 * b; return m1; }
inline mat4 operator *=(mat4& m1, const mat4& m2) { m1 = m1 * m2; return m2; }
inline mat4 operator /=(mat4& m1, const float& b) { m1 = m1 / b; return m1; }
inline mat4 perspective(float fov, float aspect_ratio, float z_near, float z_far){
    mat4 res = {};
    float tan_value = tan(fov / 2.0f);
    float cotangent = 1.0f / tan_value;
    res.m11 = 1 / (aspect_ratio * tan_value);
    res.m22 = cotangent;
    res.m33 = - (z_far + z_near) / (z_far - z_near);
    res.m34 = -1.0;
    res.m43 = -(2.0 * z_near * z_far) / (z_far - z_near);
    return res;
}
inline mat4 ortho(float left, float right, float bottom, float top, float z_near, float z_far){
    mat4 res = {};
    res.m11 = 2.0f / (right - left);
    res.m22 = 2.0f / (top - bottom);
    res.m33 = -2.0f / (z_far - z_near);
    res.m41 = -(right + left) / (right - left);
    res.m42 = -(top + bottom) / (top - bottom);
    res.m43 = -(z_far + z_near) / (z_far - z_near);
    res.m44 = 1;
    return res;
}
inline float determinant(mat4 m){
    vec3 col1 = {m.m11, m.m21, m.m31};
    vec3 col2 = {m.m12, m.m22, m.m32};
    vec3 col3 = {m.m13, m.m23, m.m33};
    vec3 col4 = {m.m14, m.m24, m.m34};
    
    vec3 C01 = vec3_cross(col1, col2);
    vec3 C23 = vec3_cross(col3, col4);
    vec3 B10 = col1 * m.m42 - col2 * m.m41;
    vec3 B32 = col2 * m.m44 - col4 * m.m43;
    
    return vec3_dot(C01, B32) + vec3_dot(C23, B10);
}
inline mat4 mat4_inverse(mat4 m){
    vec3 col1 = {m.m11, m.m21, m.m31};
    vec3 col2 = {m.m12, m.m22, m.m32};
    vec3 col3 = {m.m13, m.m23, m.m33};
    vec3 col4 = {m.m14, m.m24, m.m34};
    
    vec3 C01 = vec3_cross(col1, col2);
    vec3 C23 = vec3_cross(col3, col4);
    vec3 B10 = col1 * m.m42 - col2 * m.m41;
    vec3 B32 = col2 * m.m44 - col4 * m.m43;
    
    float inv_det = 1.0f / (vec3_dot(C01, B32) + vec3_dot(C23, B10));
    C01 = C01 * inv_det;
    C23 = C23 * inv_det;
    B10 = B10 * inv_det;
    B32 = B32 * inv_det;

    mat4 res;
    vec3 i1 = (vec3_cross(col2, B32) + (C23 * m.m42));
    vec3 i2 = (vec3_cross(B32, col1) - (C23 * m.m41));
    vec3 i3 = (vec3_cross(col4, B10) + (C01 * m.m44));
    vec3 i4 = (vec3_cross(B10, col3) - (C01 * m.m43));
    
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
    float c = cos(angle);
    float s = sin(angle);
    
    res.m11 = 1;
    res.m22 = c;  res.m23 = -s;
    res.m32 = s;  res.m33 = c;
    res.m44 = 1;
    return res;
}
inline mat4 mat4_rotation_y_mat(float angle){
    mat4 res = {};
    float c = cos(angle);
    float s = sin(angle);
    
    res.m11 = c;  res.m13 = s;
    res.m22 = 1;
    res.m31 = -s; res.m33 = c;
    res.m44 = 1;
    return res;
}
inline mat4 mat4_rotation_z_mat(float angle){
    mat4 res = {};
    float c = cos(angle);
    float s = sin(angle);
    
    res.m11 = c;  res.m12 = -s;
    res.m21 = s;  res.m22 = c;
    res.m33 = 1;
    res.m44 = 1;
    return res;
}
inline mat4 mat4_rotation_mat(vec3 axis, float angle){
    mat4 res = {}; 
    float c = cos(angle);
    float s = sin(angle);

    axis = vec3_normalize(axis);
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

struct Rotor {
    float s;
    float e12, e31, e23;
};

#define DEFAULT_EPSILON 0.01f

inline Rotor rotor_new(vec3 v1, vec3 v2) {
    vec3 a = vec3_normalize(v1);
    vec3 b = vec3_normalize(v2);
    
    Rotor r;
    r.s = vec3_dot(a, b);
    r.e12 = a.x * b.y - a.y * b.x;
    r.e31 = a.z * b.y - a.y * b.z;
    r.e23 = a.x * b.z - a.z * b.x;
    return r;
}

inline Rotor rotor_new(vec3 rot_axis, float turns) {
    vec3 v = vec3_normalize(rot_axis);
    float c = cos(turns / 2);
    float s = sin(turns / 2);
    
    Rotor r;
    r.s = c;
    r.e12 = v.z * s;
    r.e31 = v.y * s;
    r.e23 = v.x * s;
    return r;
}

inline vec3 vec3_rotate(vec3 to_rotate, Rotor r) {
    vec3 v = to_rotate;
    //r is ab
    
    //first product ((ba) * v)
    float x =   v.x * r.s   - v.y * r.e12 + v.z * r.e31; // e1
    float y =   v.y * r.s   + v.x * r.e12 - v.z * r.e23; // e2
    float z =   v.z * r.s   - v.x * r.e31 + v.y * r.e23; // e3
    float w = - v.y * r.e31 - v.z * r.e12 - v.x * r.e23; // e123
    
    //second product (first * r)
    vec3 out;
    out.x =   x * r.s   - y * r.e12 + z * r.e31 - w * r.e23;
    out.y =   x * r.e12 + y * r.s   - w * r.e31 - z * r.e23;
    out.z = - x * r.e31 + y * r.e23 + z * r.s   - w * r.e12;
    
    float sanity_check = w * r.s + z * r.e12 + y * r.e31 + x * r.e23;
    ASSERT(abs(sanity_check) <= DEFAULT_EPSILON, "WRONG ROTOR CALCULATIONS, rotor was supposed to be fully 3d but it had a non zero bivector component");
    return out;
}
