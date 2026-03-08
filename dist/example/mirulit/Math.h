#ifndef MATH_H
#define MATH_H

#include <math.h>

typedef struct Vector2 {
    float x;
    float y;
} Vector2;

typedef struct Vector3 {
    float x;
    float y;
    float z;
} Vector3;

typedef struct Quaternion {
    float x;
    float y;
    float z;
    float w;
} Quaternion;

Vector2 AddVec2(Vector2 vec1, Vector2 vec2) {
    return (Vector2){vec1.x + vec2.x, vec1.y + vec2.y};
}

Vector3 AddVec3(Vector3 vec1, Vector3 vec2) {
    return (Vector3){vec1.x + vec2.x, vec1.y + vec2.y, vec1.z + vec2.z};
}

Vector3 MinVec3(Vector3 vec1, Vector3 vec2) {
    return (Vector3){vec1.x - vec2.x, vec1.y - vec2.y, vec1.z - vec2.z};
}

Quaternion QuatIdentity() {
    return (Quaternion){0, 0, 0, 1};
}

Quaternion QuatMultiply(Quaternion q1, Quaternion q2) {
    Quaternion result;
    
    result.w = q1.w * q2.w - q1.x * q2.x - q1.y * q2.y - q1.z * q2.z;
    result.x = q1.w * q2.x + q1.x * q2.w + q1.y * q2.z - q1.z * q2.y;
    result.y = q1.w * q2.y - q1.x * q2.z + q1.y * q2.w + q1.z * q2.x;
    result.z = q1.w * q2.z + q1.x * q2.y - q1.y * q2.x + q1.z * q2.w;
    
    return result;
}

Quaternion QuatConjugate(Quaternion q) {
    return (Quaternion){-q.x, -q.y, -q.z, q.w};
}

float QuatNorm(Quaternion q) {
    return sqrt(q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w);
}

Quaternion QuatNormalize(Quaternion q) {
    float norm = QuatNorm(q);
    if (norm == 0) return q;
    
    return (Quaternion){q.x / norm, q.y / norm, q.z / norm, q.w / norm};
}

Quaternion QuatInverse(Quaternion q) {
    float norm = QuatNorm(q);
    float normSq = norm * norm;
    
    if (normSq == 0) return q;
    
    Quaternion conj = QuatConjugate(q);
    return (Quaternion){conj.x / normSq, conj.y / normSq, conj.z / normSq, conj.w / normSq};
}

Quaternion QuatFromAxisAngle(Vector3 axis, float angle) {
    float halfAngle = angle * 0.5f;
    float sinHalf = sin(halfAngle);
    
    float len = sqrt(axis.x * axis.x + axis.y * axis.y + axis.z * axis.z);
    if (len == 0) return QuatIdentity();
    
    return (Quaternion){
        axis.x / len * sinHalf,
        axis.y / len * sinHalf,
        axis.z / len * sinHalf,
        cos(halfAngle)
    };
}

Vector3 QuatRotateVector(Quaternion q, Vector3 v) {
    q = QuatNormalize(q);
    
    Quaternion p = {v.x, v.y, v.z, 0};
    
    Quaternion qInv = QuatInverse(q);
    Quaternion temp = QuatMultiply(q, p);
    Quaternion result = QuatMultiply(temp, qInv);
    
    return (Vector3){result.x, result.y, result.z};
}

Quaternion QuatSlerp(Quaternion q1, Quaternion q2, float t) {
    q1 = QuatNormalize(q1);
    q2 = QuatNormalize(q2);
    
    float dot = q1.x * q2.x + q1.y * q2.y + q1.z * q2.z + q1.w * q2.w;
    
    if (dot < 0) {
        q2.x = -q2.x;
        q2.y = -q2.y;
        q2.z = -q2.z;
        q2.w = -q2.w;
        dot = -dot;
    }
    
    const float DOT_THRESHOLD = 0.9995f;
    if (dot > DOT_THRESHOLD) {
        Quaternion result = {
            q1.x + t * (q2.x - q1.x),
            q1.y + t * (q2.y - q1.y),
            q1.z + t * (q2.z - q1.z),
            q1.w + t * (q2.w - q1.w)
        };
        return QuatNormalize(result);
    }
    
    float theta_0 = acos(dot);
    float theta = theta_0 * t;
    float sin_theta = sin(theta);
    float sin_theta_0 = sin(theta_0);
    
    float s1 = cos(theta) - dot * sin_theta / sin_theta_0;
    float s2 = sin_theta / sin_theta_0;
    
    return (Quaternion){
        q1.x * s1 + q2.x * s2,
        q1.y * s1 + q2.y * s2,
        q1.z * s1 + q2.z * s2,
        q1.w * s1 + q2.w * s2
    };
}

#endif