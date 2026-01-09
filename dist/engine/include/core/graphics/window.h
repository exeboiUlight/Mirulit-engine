#ifndef WINDOW_H
#define WINDOW_H

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include "texture.h"

#define MirulitWindow GLFWwindow

MirulitWindow* NewWindow(int width, int height, const char* title) {
    glfwInit();
    
    MirulitWindow* window = glfwCreateWindow(width, height, title, NULL, NULL);
    glfwMakeContextCurrent(window);
    
    gladLoadGL();

    glfwSwapInterval(0);
    
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    
    return window;
}

void SetWindowIcon(MirulitWindow* window, const char* iconPath) {
    int width, height, channels;
    unsigned char* data = stbi_load(iconPath, &width, &height, &channels, 0);
    
    if (data) {
        GLFWimage icon;
        icon.width = width;
        icon.height = height;
        icon.pixels = data;
        
        glfwSetWindowIcon(window, 1, &icon);
        
        stbi_image_free(data);
    } else {
        printf("Failed to load icon: %s\n", iconPath);
    }
}

void SetWindowIcons(MirulitWindow* window, const char** iconPaths, int count) {
    GLFWimage* icons = (GLFWimage*)malloc(sizeof(GLFWimage) * count);
    
    for (int i = 0; i < count; i++) {
        int width, height, channels;
        
        // Загружаем изображение с 4 каналами (RGBA) для прозрачности
        unsigned char* data = stbi_load(iconPaths[i], &width, &height, &channels, STBI_rgb_alpha);
        
        if (data && width > 0 && height > 0) {
            printf("Successfully loaded icon %d: %s (size: %dx%d, channels: %d)\n", 
                   i, iconPaths[i], width, height, channels);
            
            icons[i].width = width;
            icons[i].height = height;
            icons[i].pixels = data;
        } else {
            printf("ERROR: Failed to load icon %d: %s\n", i, iconPaths[i]);
            printf("STBI error: %s\n", stbi_failure_reason());
            
            icons[i].width = 0;
            icons[i].height = 0;
            icons[i].pixels = NULL;
        }
    }
    
    glfwSetWindowIcon(window, count, icons);
    
    // Освобождаем память
    for (int i = 0; i < count; i++) {
        if (icons[i].pixels) {
            stbi_image_free(icons[i].pixels);
        }
    }
    
    free(icons);
}

int isClose(MirulitWindow* window) {
    return glfwWindowShouldClose(window);
}

void GetEvent() {
    glfwPollEvents();
}

void updateWindow(MirulitWindow* window) {
    glClear(GL_COLOR_BUFFER_BIT);
    glfwSwapBuffers(window);
}

void quitWindow(MirulitWindow* window) {
    glfwDestroyWindow(window);
    glfwTerminate();
}

#endif