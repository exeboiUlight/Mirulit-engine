#define MIRULIT_IMPLEMENTATION
#include "utils.h"

#include <GL/gl.h>
#include <stdio.h>

void errorCallback(int code, const char* desc) {
    printf("Mirulit Error %d: %s\n", code, desc);
}

void keyCallback(int key, int action) {
    if (key == MIRULIT_KEY_ESCAPE && action == MIRULIT_PRESS) {
        printf("Escape pressed\n");
    }
}

int main(void) {
    if (!mirulitInit()) {
        return 1;
    }
    
    MirulitWindow* window = mirulitCreateWindow(800, 600, "Mirulit Test", NULL, NULL);
    if (!window) {
        mirulitTerminate();
        return 1;
    }
    
    mirulitMakeContextCurrent(window);
    mirulitSetErrorCallback(errorCallback);
    mirulitSetKeyCallback(window, keyCallback);
    
    printf("Mirulit version: %s\n", mirulitGetVersionString());
    
    glClearColor(0.2f, 0.3f, 0.4f, 1.0f);
    
    while (!mirulitWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);
        
        /* Простой треугольник */
        glBegin(GL_TRIANGLES);
        glColor3f(1.0f, 0.0f, 0.0f);
        glVertex2f(-0.5f, -0.5f);
        glColor3f(0.0f, 1.0f, 0.0f);
        glVertex2f(0.5f, -0.5f);
        glColor3f(0.0f, 0.0f, 1.0f);
        glVertex2f(0.0f, 0.5f);
        glEnd();
        
        mirulitSwapBuffers(window);
        mirulitPollEvents();
    }
    
    mirulitDestroyWindow(window);
    mirulitTerminate();
    
    return 0;
}