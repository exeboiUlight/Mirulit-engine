#include <iostream>
#include <functional>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image/stb_image.h>

#include "../core/core.h"

using namespace MirulitEngine;
using namespace MirulitComponets;

int main() {
    Window window(600, 600, "Hello mirulit");

    Entity test({0, 0}, {1, 1}, 0);

    test.Rect();
    test.initRender("assets/textures/cube.png");

    window.update([&]() {
        glClearColor(0.4078f, 0.83137f, 1.0f, 1.0f);
        test.draw();
    });

    return 0;
}