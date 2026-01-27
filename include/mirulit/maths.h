#ifndef MATHS_H
#define MATHS_H

#include <math.h>
#include <string.h>
#include <stdio.h>

/* ==================== КОНСТАНТЫ ==================== */
#define PI 3.14159265358979323846f
#define DEG_TO_RAD (PI / 180.0f)
#define RAD_TO_DEG (180.0f / PI)
#define FLOAT_EPSILON 0.000001f

/* ==================== ВЕКТОРА ==================== */

/* Вектор 2D */
typedef struct
{
    float x, y;
} vec2;

/* Вектор 3D */
typedef struct
{
    float x, y, z;
} vec3;

/* Вектор 4D */
typedef struct
{
    float x, y, z, w;
} vec4;

/* ==================== МАТРИЦЫ ==================== */

/* Матрица 2x2 в column-major порядке */
typedef struct
{
    float m[4];
} mat2;

/* Матрица 3x3 в column-major порядке */
typedef struct
{
    float m[9];
} mat3;

/* Матрица 4x4 в column-major порядке */
typedef struct
{
    float m[16];
} mat4;

vec2 vec2_create(float x, float y);
vec2 vec2_add(vec2 a, vec2 b);
vec2 vec2_sub(vec2 a, vec2 b);
vec2 vec2_mul(vec2 a, vec2 b);
vec2 vec2_div(vec2 a, vec2 b);
vec2 vec2_scale(vec2 v, float s);
float vec2_dot(vec2 a, vec2 b);
float vec2_length(vec2 v);
vec2 vec2_normalize(vec2 v);
float vec2_distance(vec2 a, vec2 b);
vec2 vec2_lerp(vec2 a, vec2 b, float t);
void vec2_print(const char* name, vec2 v);

/* Вектора 3D */
vec3 vec3_create(float x, float y, float z);
vec3 vec3_add(vec3 a, vec3 b);
vec3 vec3_sub(vec3 a, vec3 b);
vec3 vec3_mul(vec3 a, vec3 b);
vec3 vec3_div(vec3 a, vec3 b);
vec3 vec3_scale(vec3 v, float s);
float vec3_dot(vec3 a, vec3 b);
vec3 vec3_cross(vec3 a, vec3 b);
float vec3_length(vec3 v);
vec3 vec3_normalize(vec3 v);
float vec3_distance(vec3 a, vec3 b);
vec3 vec3_lerp(vec3 a, vec3 b, float t);
vec3 vec3_reflect(vec3 v, vec3 n);
vec3 vec3_from_vec4(vec4 v);
void vec3_print(const char* name, vec3 v);

/* Вектора 4D */
vec4 vec4_create(float x, float y, float z, float w);
vec4 vec4_add(vec4 a, vec4 b);
vec4 vec4_sub(vec4 a, vec4 b);
vec4 vec4_mul(vec4 a, vec4 b);
vec4 vec4_div(vec4 a, vec4 b);
vec4 vec4_scale(vec4 v, float s);
float vec4_dot(vec4 a, vec4 b);
float vec4_length(vec4 v);
vec4 vec4_normalize(vec4 v);
vec4 vec4_lerp(vec4 a, vec4 b, float t);
void vec4_print(const char* name, vec4 v);

/* Матрицы 2x2 */
mat2 mat2_identity(void);

/* Матрицы 3x3 */
mat3 mat3_identity(void);
mat3 mat3_from_mat4(mat4 m);
mat3 mat3_transpose(mat3 m);

/* Матрицы 4x4 */
mat4 mat4_identity(void);
mat4 mat4_zero(void);
mat4 mat4_transpose(mat4 m);
mat4 mat4_mul(mat4 a, mat4 b);
vec4 mat4_mul_vec4(mat4 m, vec4 v);
mat4 mat4_translation(vec3 translation);
mat4 mat4_scaling(vec3 scale);
mat4 mat4_rotation_x(float angle);
mat4 mat4_rotation_y(float angle);
mat4 mat4_rotation_z(float angle);
mat4 mat4_rotation(vec3 axis, float angle);
mat4 mat4_look_at(vec3 eye, vec3 center, vec3 up);
mat4 mat4_perspective(float fov, float aspect, float near, float far);
mat4 mat4_orthographic(float left, float right, float bottom, float top, float near, float far);
float mat4_determinant(mat4 m);
mat4 mat4_inverse(mat4 m);
void mat4_print(const char* name, mat4 m);

/* Утилиты */
float degrees_to_radians(float degrees);
float radians_to_degrees(float radians);
float clamp(float value, float min, float max);
float lerp(float a, float b, float t);
int float_equal(float a, float b);

/* ==================== РЕАЛИЗАЦИИ ==================== */
/* Вектора 2D */
vec2 vec2_create(float x, float y)
{
    vec2 result = {x, y};
    return result;
}

vec2 vec2_add(vec2 a, vec2 b)
{
    vec2 result = {a.x + b.x, a.y + b.y};
    return result;
}

vec2 vec2_sub(vec2 a, vec2 b)
{
    vec2 result = {a.x - b.x, a.y - b.y};
    return result;
}

vec2 vec2_mul(vec2 a, vec2 b)
{
    vec2 result = {a.x * b.x, a.y * b.y};
    return result;
}

vec2 vec2_div(vec2 a, vec2 b)
{
    vec2 result = {a.x / b.x, a.y / b.y};
    return result;
}

vec2 vec2_scale(vec2 v, float s)
{
    vec2 result = {v.x * s, v.y * s};
    return result;
}

float vec2_dot(vec2 a, vec2 b)
{
    return a.x * b.x + a.y * b.y;
}

float vec2_length(vec2 v)
{
    return sqrtf(v.x * v.x + v.y * v.y);
}

vec2 vec2_normalize(vec2 v)
{
    float length = vec2_length(v);
    if (length > FLOAT_EPSILON)
    {
        float inv_length = 1.0f / length;
        vec2 result = {v.x * inv_length, v.y * inv_length};
        return result;
    }
    return v;
}

float vec2_distance(vec2 a, vec2 b)
{
    float dx = b.x - a.x;
    float dy = b.y - a.y;
    return sqrtf(dx * dx + dy * dy);
}

vec2 vec2_lerp(vec2 a, vec2 b, float t)
{
    t = clamp(t, 0.0f, 1.0f);
    vec2 result = {
        a.x + (b.x - a.x) * t,
        a.y + (b.y - a.y) * t
    };
    return result;
}

void vec2_print(const char* name, vec2 v)
{
    printf("%s: [%.3f, %.3f]\n", name, v.x, v.y);
}

/* Вектора 3D */
vec3 vec3_create(float x, float y, float z)
{
    vec3 result = {x, y, z};
    return result;
}

vec3 vec3_add(vec3 a, vec3 b)
{
    vec3 result = {a.x + b.x, a.y + b.y, a.z + b.z};
    return result;
}

vec3 vec3_sub(vec3 a, vec3 b)
{
    vec3 result = {a.x - b.x, a.y - b.y, a.z - b.z};
    return result;
}

vec3 vec3_mul(vec3 a, vec3 b)
{
    vec3 result = {a.x * b.x, a.y * b.y, a.z * b.z};
    return result;
}

vec3 vec3_div(vec3 a, vec3 b)
{
    vec3 result = {a.x / b.x, a.y / b.y, a.z / b.z};
    return result;
}

vec3 vec3_scale(vec3 v, float s)
{
    vec3 result = {v.x * s, v.y * s, v.z * s};
    return result;
}

float vec3_dot(vec3 a, vec3 b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

vec3 vec3_cross(vec3 a, vec3 b)
{
    vec3 result = {
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    };
    return result;
}

float vec3_length(vec3 v)
{
    return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
}

vec3 vec3_normalize(vec3 v)
{
    float length = vec3_length(v);
    if (length > FLOAT_EPSILON)
    {
        float inv_length = 1.0f / length;
        vec3 result = {v.x * inv_length, v.y * inv_length, v.z * inv_length};
        return result;
    }
    return v;
}

float vec3_distance(vec3 a, vec3 b)
{
    float dx = b.x - a.x;
    float dy = b.y - a.y;
    float dz = b.z - a.z;
    return sqrtf(dx * dx + dy * dy + dz * dz);
}

vec3 vec3_lerp(vec3 a, vec3 b, float t)
{
    t = clamp(t, 0.0f, 1.0f);
    vec3 result = {
        a.x + (b.x - a.x) * t,
        a.y + (b.y - a.y) * t,
        a.z + (b.z - a.z) * t
    };
    return result;
}

vec3 vec3_reflect(vec3 v, vec3 n)
{
    float dot = vec3_dot(v, n);
    vec3 result = {
        v.x - 2.0f * dot * n.x,
        v.y - 2.0f * dot * n.y,
        v.z - 2.0f * dot * n.z
    };
    return result;
}

vec3 vec3_from_vec4(vec4 v)
{
    vec3 result = {v.x, v.y, v.z};
    return result;
}

void vec3_print(const char* name, vec3 v)
{
    printf("%s: [%.3f, %.3f, %.3f]\n", name, v.x, v.y, v.z);
}

/* Вектора 4D */
vec4 vec4_create(float x, float y, float z, float w)
{
    vec4 result = {x, y, z, w};
    return result;
}

vec4 vec4_add(vec4 a, vec4 b)
{
    vec4 result = {a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w};
    return result;
}

vec4 vec4_sub(vec4 a, vec4 b)
{
    vec4 result = {a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w};
    return result;
}

vec4 vec4_mul(vec4 a, vec4 b)
{
    vec4 result = {a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w};
    return result;
}

vec4 vec4_div(vec4 a, vec4 b)
{
    vec4 result = {a.x / b.x, a.y / b.y, a.z / b.z, a.w / b.w};
    return result;
}

vec4 vec4_scale(vec4 v, float s)
{
    vec4 result = {v.x * s, v.y * s, v.z * s, v.w * s};
    return result;
}

float vec4_dot(vec4 a, vec4 b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}

float vec4_length(vec4 v)
{
    return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w);
}

vec4 vec4_normalize(vec4 v)
{
    float length = vec4_length(v);
    if (length > FLOAT_EPSILON)
    {
        float inv_length = 1.0f / length;
        vec4 result = {v.x * inv_length, v.y * inv_length, v.z * inv_length, v.w * inv_length};
        return result;
    }
    return v;
}

vec4 vec4_lerp(vec4 a, vec4 b, float t)
{
    t = clamp(t, 0.0f, 1.0f);
    vec4 result = {
        a.x + (b.x - a.x) * t,
        a.y + (b.y - a.y) * t,
        a.z + (b.z - a.z) * t,
        a.w + (b.w - a.w) * t
    };
    return result;
}

void vec4_print(const char* name, vec4 v)
{
    printf("%s: [%.3f, %.3f, %.3f, %.3f]\n", name, v.x, v.y, v.z, v.w);
}

/* Матрицы 4x4 */
mat4 mat4_identity(void)
{
    mat4 result = {0};
    result.m[0] = 1.0f; result.m[5] = 1.0f; result.m[10] = 1.0f; result.m[15] = 1.0f;
    return result;
}

mat4 mat4_zero(void)
{
    mat4 result = {0};
    return result;
}

mat4 mat4_transpose(mat4 m)
{
    mat4 result;
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            result.m[i * 4 + j] = m.m[j * 4 + i];
        }
    }
    return result;
}

mat4 mat4_mul(mat4 a, mat4 b)
{
    mat4 result = {0};
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            float sum = 0.0f;
            for (int k = 0; k < 4; k++)
                sum += a.m[i + k * 4] * b.m[k + j * 4];
            result.m[i + j * 4] = sum;
        }
    }
    return result;
}

vec4 mat4_mul_vec4(mat4 m, vec4 v)
{
    vec4 result = {0};
    result.x = m.m[0] * v.x + m.m[4] * v.y + m.m[8] * v.z + m.m[12] * v.w;
    result.y = m.m[1] * v.x + m.m[5] * v.y + m.m[9] * v.z + m.m[13] * v.w;
    result.z = m.m[2] * v.x + m.m[6] * v.y + m.m[10] * v.z + m.m[14] * v.w;
    result.w = m.m[3] * v.x + m.m[7] * v.y + m.m[11] * v.z + m.m[15] * v.w;
    return result;
}

mat4 mat4_translation(vec3 translation)
{
    mat4 result = mat4_identity();
    result.m[12] = translation.x;
    result.m[13] = translation.y;
    result.m[14] = translation.z;
    return result;
}

mat4 mat4_scaling(vec3 scale)
{
    mat4 result = mat4_identity();
    result.m[0] = scale.x;
    result.m[5] = scale.y;
    result.m[10] = scale.z;
    return result;
}

mat4 mat4_rotation_x(float angle)
{
    float cos_a = cosf(angle);
    float sin_a = sinf(angle);
    
    mat4 result = mat4_identity();
    result.m[5] = cos_a;
    result.m[6] = sin_a;
    result.m[9] = -sin_a;
    result.m[10] = cos_a;
    return result;
}

mat4 mat4_rotation_y(float angle)
{
    float cos_a = cosf(angle);
    float sin_a = sinf(angle);
    
    mat4 result = mat4_identity();
    result.m[0] = cos_a;
    result.m[2] = -sin_a;
    result.m[8] = sin_a;
    result.m[10] = cos_a;
    return result;
}

mat4 mat4_rotation_z(float angle)
{
    float cos_a = cosf(angle);
    float sin_a = sinf(angle);
    
    mat4 result = mat4_identity();
    result.m[0] = cos_a;
    result.m[1] = sin_a;
    result.m[4] = -sin_a;
    result.m[5] = cos_a;
    return result;
}

mat4 mat4_rotation(vec3 axis, float angle)
{
    float cos_a = cosf(angle);
    float sin_a = sinf(angle);
    float one_minus_cos = 1.0f - cos_a;
    
    vec3 n = vec3_normalize(axis);
    float x = n.x;
    float y = n.y;
    float z = n.z;
    
    mat4 result;
    result.m[0] = cos_a + x * x * one_minus_cos;
    result.m[1] = y * x * one_minus_cos + z * sin_a;
    result.m[2] = z * x * one_minus_cos - y * sin_a;
    result.m[3] = 0.0f;
    
    result.m[4] = x * y * one_minus_cos - z * sin_a;
    result.m[5] = cos_a + y * y * one_minus_cos;
    result.m[6] = z * y * one_minus_cos + x * sin_a;
    result.m[7] = 0.0f;
    
    result.m[8] = x * z * one_minus_cos + y * sin_a;
    result.m[9] = y * z * one_minus_cos - x * sin_a;
    result.m[10] = cos_a + z * z * one_minus_cos;
    result.m[11] = 0.0f;
    
    result.m[12] = 0.0f;
    result.m[13] = 0.0f;
    result.m[14] = 0.0f;
    result.m[15] = 1.0f;
    
    return result;
}

mat4 mat4_look_at(vec3 eye, vec3 center, vec3 up)
{
    vec3 f = vec3_normalize(vec3_sub(center, eye));
    vec3 s = vec3_normalize(vec3_cross(f, up));
    vec3 u = vec3_cross(s, f);
    
    mat4 result = mat4_identity();
    result.m[0] = s.x;
    result.m[4] = s.y;
    result.m[8] = s.z;
    
    result.m[1] = u.x;
    result.m[5] = u.y;
    result.m[9] = u.z;
    
    result.m[2] = -f.x;
    result.m[6] = -f.y;
    result.m[10] = -f.z;
    
    result.m[12] = -vec3_dot(s, eye);
    result.m[13] = -vec3_dot(u, eye);
    result.m[14] = vec3_dot(f, eye);
    
    return result;
}

mat4 mat4_perspective(float fov, float aspect, float near, float far)
{
    float tan_half_fov = tanf(fov * 0.5f);
    float range = near - far;
    
    mat4 result = {0};
    result.m[0] = 1.0f / (aspect * tan_half_fov);
    result.m[5] = 1.0f / tan_half_fov;
    result.m[10] = (-near - far) / range;
    result.m[11] = 1.0f;
    result.m[14] = 2.0f * far * near / range;
    
    return result;
}

mat4 mat4_orthographic(float left, float right, float bottom, float top, float near, float far)
{
    mat4 result = mat4_identity();
    result.m[0] = 2.0f / (right - left);
    result.m[5] = 2.0f / (top - bottom);
    result.m[10] = -2.0f / (far - near);
    result.m[12] = -(right + left) / (right - left);
    result.m[13] = -(top + bottom) / (top - bottom);
    result.m[14] = -(far + near) / (far - near);
    return result;
}

float mat4_determinant(mat4 m)
{
    float det = 0.0f;
    
    det = m.m[0] * (m.m[5] * (m.m[10] * m.m[15] - m.m[11] * m.m[14]) -
                    m.m[6] * (m.m[9] * m.m[15] - m.m[11] * m.m[13]) +
                    m.m[7] * (m.m[9] * m.m[14] - m.m[10] * m.m[13]));
    
    det -= m.m[1] * (m.m[4] * (m.m[10] * m.m[15] - m.m[11] * m.m[14]) -
                     m.m[6] * (m.m[8] * m.m[15] - m.m[11] * m.m[12]) +
                     m.m[7] * (m.m[8] * m.m[14] - m.m[10] * m.m[12]));
    
    det += m.m[2] * (m.m[4] * (m.m[9] * m.m[15] - m.m[11] * m.m[13]) -
                     m.m[5] * (m.m[8] * m.m[15] - m.m[11] * m.m[12]) +
                     m.m[7] * (m.m[8] * m.m[13] - m.m[9] * m.m[12]));
    
    det -= m.m[3] * (m.m[4] * (m.m[9] * m.m[14] - m.m[10] * m.m[13]) -
                     m.m[5] * (m.m[8] * m.m[14] - m.m[10] * m.m[12]) +
                     m.m[6] * (m.m[8] * m.m[13] - m.m[9] * m.m[12]));
    
    return det;
}

mat4 mat4_inverse(mat4 m)
{
    mat4 result = {0};
    float det = mat4_determinant(m);
    
    if (fabsf(det) < FLOAT_EPSILON)
        return mat4_identity();

    float inv_det = 1.0f / det;
    result.m[0] = (m.m[5] * (m.m[10] * m.m[15] - m.m[11] * m.m[14]) -
                   m.m[6] * (m.m[9] * m.m[15] - m.m[11] * m.m[13]) +
                   m.m[7] * (m.m[9] * m.m[14] - m.m[10] * m.m[13])) * inv_det;
    
    result.m[1] = (-m.m[1] * (m.m[10] * m.m[15] - m.m[11] * m.m[14]) +
                   m.m[2] * (m.m[9] * m.m[15] - m.m[11] * m.m[13]) -
                   m.m[3] * (m.m[9] * m.m[14] - m.m[10] * m.m[13])) * inv_det;
    
    result.m[2] = (m.m[1] * (m.m[6] * m.m[15] - m.m[7] * m.m[14]) -
                   m.m[2] * (m.m[5] * m.m[15] - m.m[7] * m.m[13]) +
                   m.m[3] * (m.m[5] * m.m[14] - m.m[6] * m.m[13])) * inv_det;
    
    result.m[3] = (-m.m[1] * (m.m[6] * m.m[11] - m.m[7] * m.m[10]) +
                   m.m[2] * (m.m[5] * m.m[11] - m.m[7] * m.m[9]) -
                   m.m[3] * (m.m[5] * m.m[10] - m.m[6] * m.m[9])) * inv_det;
    
    result.m[4] = (-m.m[4] * (m.m[10] * m.m[15] - m.m[11] * m.m[14]) +
                   m.m[6] * (m.m[8] * m.m[15] - m.m[11] * m.m[12]) -
                   m.m[7] * (m.m[8] * m.m[14] - m.m[10] * m.m[12])) * inv_det;
    
    result.m[5] = (m.m[0] * (m.m[10] * m.m[15] - m.m[11] * m.m[14]) -
                   m.m[2] * (m.m[8] * m.m[15] - m.m[11] * m.m[12]) +
                   m.m[3] * (m.m[8] * m.m[14] - m.m[10] * m.m[12])) * inv_det;
    
    result.m[6] = (-m.m[0] * (m.m[6] * m.m[15] - m.m[7] * m.m[14]) +
                   m.m[2] * (m.m[4] * m.m[15] - m.m[7] * m.m[12]) -
                   m.m[3] * (m.m[4] * m.m[14] - m.m[6] * m.m[12])) * inv_det;
    
    result.m[7] = (m.m[0] * (m.m[6] * m.m[11] - m.m[7] * m.m[10]) -
                   m.m[2] * (m.m[4] * m.m[11] - m.m[7] * m.m[9]) +
                   m.m[3] * (m.m[4] * m.m[10] - m.m[6] * m.m[9])) * inv_det;
    
    result.m[8] = (m.m[4] * (m.m[9] * m.m[15] - m.m[11] * m.m[13]) -
                   m.m[5] * (m.m[8] * m.m[15] - m.m[11] * m.m[12]) +
                   m.m[7] * (m.m[8] * m.m[13] - m.m[9] * m.m[12])) * inv_det;
    
    result.m[9] = (-m.m[0] * (m.m[9] * m.m[15] - m.m[11] * m.m[13]) +
                   m.m[1] * (m.m[8] * m.m[15] - m.m[11] * m.m[12]) -
                   m.m[3] * (m.m[8] * m.m[13] - m.m[9] * m.m[12])) * inv_det;
    
    result.m[10] = (m.m[0] * (m.m[5] * m.m[15] - m.m[7] * m.m[13]) -
                    m.m[1] * (m.m[4] * m.m[15] - m.m[7] * m.m[12]) +
                    m.m[3] * (m.m[4] * m.m[13] - m.m[5] * m.m[12])) * inv_det;
    
    result.m[11] = (-m.m[0] * (m.m[5] * m.m[11] - m.m[7] * m.m[9]) +
                    m.m[1] * (m.m[4] * m.m[11] - m.m[7] * m.m[9]) -
                    m.m[3] * (m.m[4] * m.m[9] - m.m[5] * m.m[9])) * inv_det;
    
    result.m[12] = (-m.m[4] * (m.m[9] * m.m[14] - m.m[10] * m.m[13]) +
                    m.m[5] * (m.m[8] * m.m[14] - m.m[10] * m.m[12]) -
                    m.m[6] * (m.m[8] * m.m[13] - m.m[9] * m.m[12])) * inv_det;
    
    result.m[13] = (m.m[0] * (m.m[9] * m.m[14] - m.m[10] * m.m[13]) -
                    m.m[1] * (m.m[8] * m.m[14] - m.m[10] * m.m[12]) +
                    m.m[2] * (m.m[8] * m.m[13] - m.m[9] * m.m[12])) * inv_det;
    
    result.m[14] = (-m.m[0] * (m.m[5] * m.m[14] - m.m[6] * m.m[13]) +
                    m.m[1] * (m.m[4] * m.m[14] - m.m[6] * m.m[12]) -
                    m.m[2] * (m.m[4] * m.m[13] - m.m[5] * m.m[12])) * inv_det;
    
    result.m[15] = (m.m[0] * (m.m[5] * m.m[10] - m.m[6] * m.m[9]) -
                    m.m[1] * (m.m[4] * m.m[10] - m.m[6] * m.m[9]) +
                    m.m[2] * (m.m[4] * m.m[9] - m.m[5] * m.m[9])) * inv_det;
    
    return result;
}

void mat4_print(const char* name, mat4 m)
{
    printf("%s:\n", name);
    for (int i = 0; i < 4; i++)
    {
        printf("[ ");
        for (int j = 0; j < 4; j++)
            printf("%7.3f ", m.m[i + j * 4]);
        printf("]\n");
    }
}

/* Матрицы 3x3 */
mat3 mat3_identity(void)
{
    mat3 result = {0};
    result.m[0] = 1.0f; result.m[4] = 1.0f; result.m[8] = 1.0f;
    return result;
}

mat3 mat3_from_mat4(mat4 m)
{
    mat3 result;
    result.m[0] = m.m[0]; result.m[1] = m.m[1]; result.m[2] = m.m[2];
    result.m[3] = m.m[4]; result.m[4] = m.m[5]; result.m[5] = m.m[6];
    result.m[6] = m.m[8]; result.m[7] = m.m[9]; result.m[8] = m.m[10];
    return result;
}

mat3 mat3_transpose(mat3 m)
{
    mat3 result;
    result.m[0] = m.m[0]; result.m[1] = m.m[3]; result.m[2] = m.m[6];
    result.m[3] = m.m[1]; result.m[4] = m.m[4]; result.m[5] = m.m[7];
    result.m[6] = m.m[2]; result.m[7] = m.m[5]; result.m[8] = m.m[8];
    return result;
}

/* Матрицы 2x2 */
mat2 mat2_identity(void)
{
    mat2 result = {0};
    result.m[0] = 1.0f; result.m[3] = 1.0f;
    return result;
}

/* Утилиты */
float degrees_to_radians(float degrees)
{
    return degrees * DEG_TO_RAD;
}

float radians_to_degrees(float radians)
{
    return radians * RAD_TO_DEG;
}

float clamp(float value, float min, float max)
{
    if (value < min) return min;
    if (value > max) return max;
    return value;
}

float lerp(float a, float b, float t)
{
    return a + (b - a) * t;
}

int float_equal(float a, float b)
{
    return fabsf(a - b) < FLOAT_EPSILON;
}

#endif