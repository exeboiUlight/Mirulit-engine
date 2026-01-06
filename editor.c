#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#endif

// инклюды для imgui
#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>

// базовые инклюды
#include "core/graphics/window.h"
#include <stdio.h>
#include <time.h>
#include <math.h>

int main(void) {
    MirulitWindow* window = NewWindow(1200, 600, "Mirulit game engine");

    glfwSwapInterval(0);
    
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    
    
    while (!isClose(window)) {
        glClearColor(0.2f, 0.3f, 0.4f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        updateWindow(window);
    }
    
    Texture_CleanupAll();
    
    quitWindow(window);
    return 0;
}