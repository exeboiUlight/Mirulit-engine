#pragma once

#include <glad/glad.h>
#include <stb_image/stb_image.h>

namespace MirulitComponencts {
    class Mesh3D {
        private:
            GLuint VBO, VAO;

            Mesh3D(float* vertices) {
                glGenVertexArrays(1, &VAO);
                glGenBuffers(1, &VBO);
            }
    };
}