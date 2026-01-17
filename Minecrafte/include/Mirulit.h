#ifndef MIRULIT_H
#define MIRULIT_H

#include <mirulit/utils.h>
#include <mirulit/mesh.h>
#include <mirulit/math.h>
#include <stdio.h>
#include <GL/gl.h>

#define _SceneNumber main

typedef struct {
    Mesh mesh;
    GLenum ID;
} Object3D;

MirulitWindow* MirulitEngineInit(int width, int height, char* title, Vector3 BGcolor) {
    if (!mirulitInit()) {
        fprintf(stderr, "Failed to initialize Mirulit\n");
        return NULL;
    }

    MirulitWindow* window = mirulitCreateWindow(width, height, title, NULL, NULL);
    if (!window) {
        fprintf(stderr, "Не удалось создать окно\n");
        mirulitTerminate();
        return NULL;
    }

    mirulitMakeContextCurrent(window);
    
    mirulitSwapInterval(0);

    if (!gladLoadGL()) {
        fprintf(stderr, "Failed to initialize GLAD\n");
        mirulitDestroyWindow(window);
        mirulitTerminate();
        return NULL;
    }

    Vector3 bg = ColorRGB(BGcolor.x, BGcolor.y, BGcolor.z);

    glClearColor(bg.x, bg.y, bg.z, 1.0f);
    
    return window;
}

Object3D* MirulitEngineScene(MirulitWindow* window, Object3D* objects, int objectsCount, int sceneID, int curentScene, void (*updateFunction)()) {
    if (curentScene != sceneID) {
        return objects;
    }

    while (!mirulitWindowShouldClose(window)) {
        mirulitPollEvents();
        glClear(GL_COLOR_BUFFER_BIT);

        if (updateFunction) {
            updateFunction();
        }

        for (int i = 0; i < objectsCount; i++) {
            meshDraw(&objects[i].mesh, objects[i].ID);
        }

        mirulitSwapBuffers(window);
    }

    return objects;
}

void MirulitEngineEndScene(Object3D* Scene, int objectsCount) {
    for (int i = 0; i < objectsCount; i++) {
        meshFree(&Scene[i].mesh);
    }
}

void MirulitEngineDestroy(MirulitWindow* window) {
    mirulitDestroyWindow(window);
    mirulitTerminate();
}

#endif