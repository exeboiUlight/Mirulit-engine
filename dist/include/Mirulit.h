#ifndef MIRULIT_H
#define MIRULIT_H

#include <core/graphics/objects/object.h>
#include <core/graphics/objects/mesh.h>
#include <core/graphics/shader.h>
#include <core/graphics/rgb.h>
#include <windows.h>

typedef struct {
    void (*Start)();
    void (*Update)();
} MirulitScene;

#endif