#ifndef MIRULIT_MATH_H
#define MIRULIT_MATH_H

typedef struct {
    float x;
    float y;
} Vector2;

typedef struct {
    float x;
    float y;
    float z;
} Vector3;

typedef struct {
    float x;
    float y;
    float z;
    float w;
} Vector4;

typedef struct {
    float m[3][3];
} Matrix3x3;

typedef struct {
    float m[4][4];
} Matrix4x4;

Vector2 Vec2(float x, float y) {
    return (Vector2){x, y};
}

Vector3 Vec3(float x, float y, float z) {
    return (Vector3){x, y, z};
}

Vector4 Vec4(float x, float y, float z, float w) {
    return (Vector4){x, y, z, w};
}

Vector2 Vec2ADD(Vector2 left, Vector2 right) {
    return (Vector2){
        left.x + right.x,
        left.y + right.y,
    };
};

Vector2 Vec2DED(Vector2 left, Vector2 right) {
    return (Vector2){
        left.x - right.x,
        left.y - right.y,
    };
};

Vector3 Vec3ADD(Vector3 left, Vector3 right) {
    return (Vector3){
        left.x + right.x,
        left.y + right.y,
        left.z + right.z,
    };
};

Vector3 Vec3DED(Vector3 left, Vector3 right) {
    return (Vector3){
        left.x - right.x,
        left.y - right.y,
        left.z - right.z,
    };
};

Vector4 Vec4ADD(Vector4 left, Vector4 right) {
    return (Vector4){
        left.x + right.x,
        left.y + right.y,
        left.z + right.z,
        left.w + right.w,
    };
};

Vector4 Vec4DED(Vector4 left, Vector4 right) {
    return (Vector4){
        left.x - right.x,
        left.y - right.y,
        left.z - right.z,
        left.w - right.w,
    };
};

Vector3 ColorRGB(int r, int g, int b) {
    return (Vector3)
    { 
        r / 255.0f, 
        g / 255.0f,
        b / 255.0f
    };
}

#endif