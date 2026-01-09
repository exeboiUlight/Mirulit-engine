#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <math.h>
#include <stdlib.h>

typedef struct {
    float x;
    float y;
    float z;
} Vec3;

typedef struct {
    float x;
    float y;
} Vec2;

typedef struct {
    float w;
    float x;
    float y;
    float z;
} Quaternion;

static inline Vec3 vec3_create(float x, float y, float z) {
    Vec3 v = {x, y, z};
    return v;
}

static inline float vec3_length(const Vec3* v) {
    return sqrtf(v->x * v->x + v->y * v->y + v->z * v->z);
}

static inline Vec3 vec3_normalized(const Vec3* v) {
    float len = vec3_length(v);
    if (len > 0) {
        return vec3_create(v->x / len, v->y / len, v->z / len);
    }
    return vec3_create(0, 0, 0);
}

static inline float vec3_dot(const Vec3* a, const Vec3* b) {
    return a->x * b->x + a->y * b->y + a->z * b->z;
}

static inline Vec3 vec3_cross(const Vec3* a, const Vec3* b) {
    Vec3 result = {
        a->y * b->z - a->z * b->y,
        a->z * b->x - a->x * b->z,
        a->x * b->y - a->y * b->x
    };
    return result;
}

static inline Vec3 vec3_add(const Vec3* a, const Vec3* b) {
    return vec3_create(a->x + b->x, a->y + b->y, a->z + b->z);
}

static inline Vec3 vec3_sub(const Vec3* a, const Vec3* b) {
    return vec3_create(a->x - b->x, a->y - b->y, a->z - b->z);
}

static inline Vec3 vec3_mul_scalar(const Vec3* v, float scalar) {
    return vec3_create(v->x * scalar, v->y * scalar, v->z * scalar);
}

static inline Vec3 vec3_div_scalar(const Vec3* v, float scalar) {
    return vec3_create(v->x / scalar, v->y / scalar, v->z / scalar);
}

static inline Vec2 vec2_create(float x, float y) {
    Vec2 v = {x, y};
    return v;
}

static inline float vec2_length(const Vec2* v) {
    return sqrtf(v->x * v->x + v->y * v->y);
}

static inline Vec2 vec2_normalized(const Vec2* v) {
    float len = vec2_length(v);
    if (len > 0) {
        return vec2_create(v->x / len, v->y / len);
    }
    return vec2_create(0, 0);
}

static inline float vec2_dot(const Vec2* a, const Vec2* b) {
    return a->x * b->x + a->y * b->y;
}

static inline Vec2 vec2_add(const Vec2* a, const Vec2* b) {
    return vec2_create(a->x + b->x, a->y + b->y);
}

static inline Vec2 vec2_sub(const Vec2* a, const Vec2* b) {
    return vec2_create(a->x - b->x, a->y - b->y);
}

static inline Vec2 vec2_mul_scalar(const Vec2* v, float scalar) {
    return vec2_create(v->x * scalar, v->y * scalar);
}

static inline Vec2 vec2_div_scalar(const Vec2* v, float scalar) {
    return vec2_create(v->x / scalar, v->y / scalar);
}

static inline Quaternion quaternion_create(float w, float x, float y, float z) {
    Quaternion q = {w, x, y, z};
    return q;
}

static inline Quaternion quaternion_from_axis_angle(const Vec3* axis, float angle) {
    float halfAngle = angle * 0.5f;
    float sinHalf = sinf(halfAngle);
    Vec3 normAxis = vec3_normalized(axis);
    
    return quaternion_create(
        cosf(halfAngle),
        normAxis.x * sinHalf,
        normAxis.y * sinHalf,
        normAxis.z * sinHalf
    );
}

static inline Quaternion quaternion_from_euler_angles(float pitch, float yaw, float roll) {
    float cy = cosf(yaw * 0.5f);
    float sy = sinf(yaw * 0.5f);
    float cp = cosf(pitch * 0.5f);
    float sp = sinf(pitch * 0.5f);
    float cr = cosf(roll * 0.5f);
    float sr = sinf(roll * 0.5f);

    return quaternion_create(
        cy * cp * cr + sy * sp * sr,
        cy * cp * sr - sy * sp * cr,
        sy * cp * sr + cy * sp * cr,
        sy * cp * cr - cy * sp * sr
    );
}

static inline float quaternion_length(const Quaternion* q) {
    return sqrtf(q->w * q->w + q->x * q->x + q->y * q->y + q->z * q->z);
}

static inline Quaternion quaternion_normalized(const Quaternion* q) {
    float len = quaternion_length(q);
    if (len > 0) {
        return quaternion_create(q->w / len, q->x / len, q->y / len, q->z / len);
    }
    return quaternion_create(1.0f, 0.0f, 0.0f, 0.0f);
}

static inline Quaternion quaternion_conjugate(const Quaternion* q) {
    return quaternion_create(q->w, -q->x, -q->y, -q->z);
}

static inline Quaternion quaternion_inverse(const Quaternion* q) {
    float lenSq = q->w * q->w + q->x * q->x + q->y * q->y + q->z * q->z;
    if (lenSq > 0) {
        float invLenSq = 1.0f / lenSq;
        return quaternion_create(q->w * invLenSq, -q->x * invLenSq, -q->y * invLenSq, -q->z * invLenSq);
    }
    return quaternion_create(1.0f, 0.0f, 0.0f, 0.0f);
}

static inline Quaternion quaternion_multiply(const Quaternion* a, const Quaternion* b) {
    return quaternion_create(
        a->w * b->w - a->x * b->x - a->y * b->y - a->z * b->z,
        a->w * b->x + a->x * b->w + a->y * b->z - a->z * b->y,
        a->w * b->y - a->x * b->z + a->y * b->w + a->z * b->x,
        a->w * b->z + a->x * b->y - a->y * b->x + a->z * b->w
    );
}

static inline Vec3 quaternion_rotate_vector(const Quaternion* q, const Vec3* v) {
    Quaternion vq = quaternion_create(0.0f, v->x, v->y, v->z);
    Quaternion conj = quaternion_conjugate(q);
    Quaternion result = quaternion_multiply(q, &vq);
    result = quaternion_multiply(&result, &conj);
    
    return vec3_create(result.x, result.y, result.z);
}

static inline Quaternion quaternion_lerp(const Quaternion* a, const Quaternion* b, float t) {
    float t1 = 1.0f - t;
    Quaternion result = quaternion_create(
        a->w * t1 + b->w * t,
        a->x * t1 + b->x * t,
        a->y * t1 + b->y * t,
        a->z * t1 + b->z * t
    );
    return quaternion_normalized(&result);
}

static inline Quaternion quaternion_slerp(const Quaternion* a, const Quaternion* b, float t) {
    float cosTheta = a->w * b->w + a->x * b->x + a->y * b->y + a->z * b->z;
    
    Quaternion b2 = *b;
    if (cosTheta < 0.0f) {
        b2 = quaternion_create(-b->w, -b->x, -b->y, -b->z);
        cosTheta = -cosTheta;
    }

    if (cosTheta > 0.9995f) {
        return quaternion_lerp(a, &b2, t);
    }

    float theta = acosf(cosTheta);
    float sinTheta = sinf(theta);
    
    float wa = sinf((1.0f - t) * theta) / sinTheta;
    float wb = sinf(t * theta) / sinTheta;

    return quaternion_create(
        a->w * wa + b2.w * wb,
        a->x * wa + b2.x * wb,
        a->y * wa + b2.y * wb,
        a->z * wa + b2.z * wb
    );
}

#endif