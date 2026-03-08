#ifndef MIRULIT_H
#define MIRULIT_H

#ifdef BUILD_DLL
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT __declspec(dllimport)
#endif

// limited

#define LimitadeEntitys2D 2048
#define LimitadeEntitys3D 2048
#define LimitadeParticls2D 2048
#define LimitadeParticls3D 2048

// libery

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image/stb_image.h>


typedef struct window {
    GLFWwindow* window;
    int width;
    int height;
    const char* title;
    const char* icon;
} Window;

typedef struct MirulitEngine {
    Window window;
} MirulitEngine;

MirulitEngine _mir;

#include "graphics/window.h"
#include "Math.h"
#include "Entity.h"

#endif