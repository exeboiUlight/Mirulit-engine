#include <iostream>
#include <functional>

#include "../core/core.h"

using namespace MirulitEngine;
using namespace MirulitComponenets;

Mesh* test = nullptr;
Shader* shader = nullptr;

int main() {
    Window window(1200, 600, "Hello mirulit");

    // Создаем объекты после инициализации OpenGL
    test = new Mesh({
        0.0f, 0.0f,
        0.5f, 0.0f,
        0.0f, 0.5f
    });

    // Простой шейдер для отрисовки
    const char* vertexShaderSource = R"(
        #version 330 core
        layout (location = 0) in vec2 aPos;
        void main() {
            gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);
        }
    )";

    const char* fragmentShaderSource = R"(
        #version 330 core
        out vec4 FragColor;
        void main() {
            FragColor = vec4(1.0, 1.0, 1.0, 1.0);
        }
    )";

    shader = new Shader(vertexShaderSource, fragmentShaderSource, true);

    window.update([&]() {
        glClearColor(0.4078f, 0.83137f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        if (shader && test) {
            shader->use();
            test->draw();
        }
    });

    delete test;
    delete shader;
    return 0;
}