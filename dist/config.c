#define MIRULIT_WINDOW_IMPLEMENTATION
#include <mirulit/window.h>

#include "stb_image/stb_image.h"
#include <stdio.h>

int main() {
    windowInit();

    WindowHandle window = windowCreate(1200, 600, "Hello, world!");

    windowMakeContextCurrent(window);

    while (!windowShouldClose(window)) {
        windowPollEvents();
        windowSwapBuffers(window);
    }

    windowDestroy(window);
    windowTerminate();

    return 0;
}