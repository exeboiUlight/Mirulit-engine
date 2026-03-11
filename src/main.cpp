#include <iostream>

#include "../core/core.h"

using namespace MirulitEngine;

void update() {
    glClearColor(104/255.0f, 212/255.0f, 1.0f, 1.0f);
}

int main() {

    Window window(1200, 600, "Hello mirulit");

    window.update(&update);

}