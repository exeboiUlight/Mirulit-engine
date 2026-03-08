#ifndef ENTITY_H
#define ENTITy_H

typedef struct Transform2D {
    Vector2 position;
    Vector2 scale;
    float rotate;
} Transform2D;

typedef struct Entity2D {
    bool visibale;
    Transform2D transform;
} Entity;

#endif