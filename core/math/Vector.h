#ifndef VECTOR_H
#define VECTOR_H

typedef struct Vector2 {
    float x;
    float y;
} Vector2;

Vector2 AddVectors(Vector2 a, Vector2 b) {
    return (Vector2){
        a.x + b.x,
        a.y + b.y
    };
}

Vector2 MinVectors(Vector2 a, Vector2 b) {
    return (Vector2){
        a.x - b.x,
        a.y - b.y
    };
}

typedef struct Vector3 {
    float x;
    float y;
    float z;
} Vector3;

Vector3 AddVectors(Vector3 a, Vector3 b) {
    return (Vector3){
        a.x + b.x,
        a.y + b.y,
        a.z + b.z
    };
}

Vector3 MinVectors(Vector3 a, Vector3 b) {
    return (Vector3){
        a.x - b.x,
        a.y - b.y,
        a.z - b.z
    };
}

typedef struct Vector4 {
    float x;
    float y;
    float z;
    float w;
} Vector4;

Vector4 AddVectors(Vector4 a, Vector4 b) {
    return (Vector4){
        a.x + b.x,
        a.y + b.y,
        a.z + b.z,
        a.w + b.w
    };
}

Vector4 MinVectors(Vector4 a, Vector4 b) {
    return (Vector4){
        a.x - b.x,
        a.y - b.y,
        a.z - b.z,
        a.w - b.w
    };
}

#endif