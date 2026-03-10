#include <iostream>

#include "../core/core.h"

void update() {
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
}

int main() {

    using namespace MirulitEngine;

    Window window(1200, 600, "Hello mirulit");

    window.update(&update);

}