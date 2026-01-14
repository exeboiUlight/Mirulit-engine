#ifndef MIRULIT_MATH_H
#define MIRULIT_MATH_H

#include <math.h>
#include <stdbool.h>

// ============================================================================
// ВЕКТОРА
// ============================================================================

typedef struct {
    float x;
    float y;
} Vec2;

typedef struct {
    float x;
    float y;
    float z;
} Vec3;

typedef struct {
    float x;
    float y;
    float z;
    float w;
} Vec4;

// ============================================================================
// МАТРИЦЫ
// ============================================================================

typedef struct {
    float m[4][4]; // [row][column]
} Mat4;

typedef struct {
    float x[4];
    float y[4];
    float z[4];
    float w[4];
} Matrix; // Альтернативное представление (совместимость)

// ============================================================================
// КОНСТАНТЫ
// ============================================================================

#define PI 3.14159265358979323846f
#define DEG_TO_RAD (PI / 180.0f)
#define RAD_TO_DEG (180.0f / PI)
#define EPSILON 0.000001f

// ============================================================================
// ВСПОМОГАТЕЛЬНЫЕ ФУНКЦИИ
// ============================================================================

static inline float clampf(float value, float min, float max) {
    if (value < min) return min;
    if (value > max) return max;
    return value;
}

static inline float lerpf(float a, float b, float t) {
    return a + (b - a) * t;
}

static inline float deg_to_rad(float degrees) {
    return degrees * DEG_TO_RAD;
}

static inline float rad_to_deg(float radians) {
    return radians * RAD_TO_DEG;
}

// ============================================================================
// ФУНКЦИИ ДЛЯ Vec2
// ============================================================================

static inline Vec2 vec2(float x, float y) {
    Vec2 v = {x, y};
    return v;
}

static inline Vec2 vec2_zero(void) {
    return vec2(0.0f, 0.0f);
}

static inline Vec2 vec2_one(void) {
    return vec2(1.0f, 1.0f);
}

static inline Vec2 vec2_add(Vec2 a, Vec2 b) {
    return vec2(a.x + b.x, a.y + b.y);
}

static inline Vec2 vec2_sub(Vec2 a, Vec2 b) {
    return vec2(a.x - b.x, a.y - b.y);
}

static inline Vec2 vec2_mul(Vec2 v, float scalar) {
    return vec2(v.x * scalar, v.y * scalar);
}

static inline Vec2 vec2_div(Vec2 v, float scalar) {
    if (fabsf(scalar) < EPSILON) return v;
    return vec2(v.x / scalar, v.y / scalar);
}

static inline float vec2_dot(Vec2 a, Vec2 b) {
    return a.x * b.x + a.y * b.y;
}

static inline float vec2_length_sq(Vec2 v) {
    return vec2_dot(v, v);
}

static inline float vec2_length(Vec2 v) {
    return sqrtf(vec2_length_sq(v));
}

static inline Vec2 vec2_normalize(Vec2 v) {
    float len = vec2_length(v);
    if (len < EPSILON) return v;
    return vec2_div(v, len);
}

static inline float vec2_distance(Vec2 a, Vec2 b) {
    return vec2_length(vec2_sub(a, b));
}

// ============================================================================
// ФУНКЦИИ ДЛЯ Vec3
// ============================================================================

static inline Vec3 vec3(float x, float y, float z) {
    Vec3 v = {x, y, z};
    return v;
}

static inline Vec3 vec3_zero(void) {
    return vec3(0.0f, 0.0f, 0.0f);
}

static inline Vec3 vec3_one(void) {
    return vec3(1.0f, 1.0f, 1.0f);
}

static inline Vec3 vec3_add(Vec3 a, Vec3 b) {
    return vec3(a.x + b.x, a.y + b.y, a.z + b.z);
}

static inline Vec3 vec3_sub(Vec3 a, Vec3 b) {
    return vec3(a.x - b.x, a.y - b.y, a.z - b.z);
}

static inline Vec3 vec3_mul(Vec3 v, float scalar) {
    return vec3(v.x * scalar, v.y * scalar, v.z * scalar);
}

static inline Vec3 vec3_div(Vec3 v, float scalar) {
    if (fabsf(scalar) < EPSILON) return v;
    return vec3(v.x / scalar, v.y / scalar, v.z / scalar);
}

static inline float vec3_dot(Vec3 a, Vec3 b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

static inline Vec3 vec3_cross(Vec3 a, Vec3 b) {
    return vec3(
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    );
}

static inline float vec3_length_sq(Vec3 v) {
    return vec3_dot(v, v);
}

static inline float vec3_length(Vec3 v) {
    return sqrtf(vec3_length_sq(v));
}

static inline Vec3 vec3_normalize(Vec3 v) {
    float len = vec3_length(v);
    if (len < EPSILON) return v;
    return vec3_div(v, len);
}

static inline float vec3_distance(Vec3 a, Vec3 b) {
    return vec3_length(vec3_sub(a, b));
}

static inline Vec3 vec3_lerp(Vec3 a, Vec3 b, float t) {
    return vec3(
        lerpf(a.x, b.x, t),
        lerpf(a.y, b.y, t),
        lerpf(a.z, b.z, t)
    );
}

// ============================================================================
// ФУНКЦИИ ДЛЯ Vec4
// ============================================================================

static inline Vec4 vec4(float x, float y, float z, float w) {
    Vec4 v = {x, y, z, w};
    return v;
}

static inline Vec4 vec4_zero(void) {
    return vec4(0.0f, 0.0f, 0.0f, 0.0f);
}

static inline Vec4 vec4_one(void) {
    return vec4(1.0f, 1.0f, 1.0f, 1.0f);
}

static inline Vec4 vec4_from_vec3(Vec3 v, float w) {
    return vec4(v.x, v.y, v.z, w);
}

static inline Vec3 vec4_to_vec3(Vec4 v) {
    return vec3(v.x, v.y, v.z);
}

// ============================================================================
// ФУНКЦИИ ДЛЯ Mat4
// ============================================================================

static inline Mat4 mat4_identity(void) {
    Mat4 m = {{
        {1.0f, 0.0f, 0.0f, 0.0f},
        {0.0f, 1.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 1.0f, 0.0f},
        {0.0f, 0.0f, 0.0f, 1.0f}
    }};
    return m;
}

static inline Mat4 mat4_zero(void) {
    Mat4 m = {{{0}}};
    return m;
}

static inline Mat4 mat4_mul(Mat4 a, Mat4 b) {
    Mat4 result = mat4_zero();
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            for (int k = 0; k < 4; k++) {
                result.m[i][j] += a.m[i][k] * b.m[k][j];
            }
        }
    }
    return result;
}

static inline Vec4 mat4_mul_vec4(Mat4 m, Vec4 v) {
    Vec4 result = vec4_zero();
    result.x = m.m[0][0] * v.x + m.m[0][1] * v.y + m.m[0][2] * v.z + m.m[0][3] * v.w;
    result.y = m.m[1][0] * v.x + m.m[1][1] * v.y + m.m[1][2] * v.z + m.m[1][3] * v.w;
    result.z = m.m[2][0] * v.x + m.m[2][1] * v.y + m.m[2][2] * v.z + m.m[2][3] * v.w;
    result.w = m.m[3][0] * v.x + m.m[3][1] * v.y + m.m[3][2] * v.z + m.m[3][3] * v.w;
    return result;
}

static inline Vec3 mat4_mul_vec3(Mat4 m, Vec3 v) {
    Vec4 v4 = vec4(v.x, v.y, v.z, 1.0f);
    v4 = mat4_mul_vec4(m, v4);
    return vec4_to_vec3(v4);
}

static inline Mat4 mat4_transpose(Mat4 m) {
    Mat4 result;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            result.m[i][j] = m.m[j][i];
        }
    }
    return result;
}

static inline Mat4 mat4_translation(Vec3 translation) {
    Mat4 m = mat4_identity();
    m.m[0][3] = translation.x;
    m.m[1][3] = translation.y;
    m.m[2][3] = translation.z;
    return m;
}

static inline Mat4 mat4_scale(Vec3 scale) {
    Mat4 m = mat4_identity();
    m.m[0][0] = scale.x;
    m.m[1][1] = scale.y;
    m.m[2][2] = scale.z;
    return m;
}

static inline Mat4 mat4_rotation_x(float angle) {
    float cosA = cosf(angle);
    float sinA = sinf(angle);
    
    Mat4 m = mat4_identity();
    m.m[1][1] = cosA;
    m.m[1][2] = -sinA;
    m.m[2][1] = sinA;
    m.m[2][2] = cosA;
    return m;
}

static inline Mat4 mat4_rotation_y(float angle) {
    float cosA = cosf(angle);
    float sinA = sinf(angle);
    
    Mat4 m = mat4_identity();
    m.m[0][0] = cosA;
    m.m[0][2] = sinA;
    m.m[2][0] = -sinA;
    m.m[2][2] = cosA;
    return m;
}

static inline Mat4 mat4_rotation_z(float angle) {
    float cosA = cosf(angle);
    float sinA = sinf(angle);
    
    Mat4 m = mat4_identity();
    m.m[0][0] = cosA;
    m.m[0][1] = -sinA;
    m.m[1][0] = sinA;
    m.m[1][1] = cosA;
    return m;
}

static inline Mat4 mat4_perspective(float fov, float aspect, float near, float far) {
    float tanHalfFov = tanf(fov * 0.5f);
    
    Mat4 m = mat4_zero();
    m.m[0][0] = 1.0f / (aspect * tanHalfFov);
    m.m[1][1] = 1.0f / tanHalfFov;
    m.m[2][2] = -(far + near) / (far - near);
    m.m[2][3] = -(2.0f * far * near) / (far - near);
    m.m[3][2] = -1.0f;
    
    return m;
}

static inline Mat4 mat4_orthographic(float left, float right, float bottom, float top, float near, float far) {
    Mat4 m = mat4_identity();
    m.m[0][0] = 2.0f / (right - left);
    m.m[1][1] = 2.0f / (top - bottom);
    m.m[2][2] = -2.0f / (far - near);
    
    m.m[0][3] = -(right + left) / (right - left);
    m.m[1][3] = -(top + bottom) / (top - bottom);
    m.m[2][3] = -(far + near) / (far - near);
    
    return m;
}

static inline Mat4 mat4_look_at(Vec3 eye, Vec3 target, Vec3 up) {
    Vec3 z = vec3_normalize(vec3_sub(eye, target));
    Vec3 x = vec3_normalize(vec3_cross(up, z));
    Vec3 y = vec3_cross(z, x);
    
    Mat4 m = mat4_identity();
    m.m[0][0] = x.x; m.m[0][1] = x.y; m.m[0][2] = x.z;
    m.m[1][0] = y.x; m.m[1][1] = y.y; m.m[1][2] = y.z;
    m.m[2][0] = z.x; m.m[2][1] = z.y; m.m[2][2] = z.z;
    
    m.m[0][3] = -vec3_dot(x, eye);
    m.m[1][3] = -vec3_dot(y, eye);
    m.m[2][3] = -vec3_dot(z, eye);
    
    return m;
}

// ============================================================================
// ФУНКЦИИ ДЛЯ Matrix (совместимость)
// ============================================================================

static inline Matrix matrix_from_mat4(Mat4 m) {
    Matrix result;
    for (int i = 0; i < 4; i++) {
        result.x[i] = m.m[0][i];
        result.y[i] = m.m[1][i];
        result.z[i] = m.m[2][i];
        result.w[i] = m.m[3][i];
    }
    return result;
}

#endif // MIRULIT_MATH_H