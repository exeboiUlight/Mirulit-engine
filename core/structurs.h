#ifndef STRUCTURS_H
#define STRUCTURS_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

typedef struct Vertex {
    float x, y, z;
    float nx, ny, nz;
    float u, v;
} Vertex;

typedef struct ObjModel {} ObjModel;

typedef struct Scene {
    GLuint VAOdynamic, VBOdynamic;
    GLuint VAOstatic, VBOstatic;

    const char* name;
} Scene;

typedef struct Engine {
    GLFWwindow* window;
    char* current_scene;
} Engine;

#endif