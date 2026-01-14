#define MIRULIT_IMPLEMENTATION

#include <stdio.h>
#include <string.h>
#include <Mirulit.h>
#include <mirulit/utils.h>
#include <GL/gl.h>

int main() {

    int a;

    scanf("%d", &a);


    if (!mirulitInit()) {
        return -1;
    }

    MirulitWindow* window = mirulitCreateWindow(1200, 600, "Mirulit game engine", NULL, NULL);
    if (!window) {
        fprintf(stderr, "Не удалось создать окно\n");
        mirulitTerminate();
        return -1;
    }

    mirulitMakeContextCurrent(window);

    GLuint test;

    while (!mirulitWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);

        glPushMatrix();

        glBegin(GL_TRIANGLES);
        glColor3f(1.0f, 0.0f, 0.0f);
        glVertex2f(-0.5f, -0.5f);

        glColor3f(0.0f, 1.0f, 0.0f);
        glVertex2f(0.5f, -0.5f);

        glColor3f(0.0f, 0.0f, 1.0f);
        glVertex2f(0.0f, 0.5f);
        glEnd();

        glPopMatrix();

        mirulitPollEvents();
        mirulitSwapBuffers(window);
        mirulitWaitEventsTimeout(0.016);
    }

    mirulitDestroyWindow(window);
    mirulitTerminate();

    return 0;
}