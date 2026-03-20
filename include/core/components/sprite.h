#pragma once

#include <glad/glad.h>
#include <string>
#include <vector>
#include <iostream>
#include "shader.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace Engine {

// Простые матричные операции
inline void MultiplyMatrix(float* result, const float* a, const float* b) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            result[i * 4 + j] = 0;
            for (int k = 0; k < 4; k++) {
                result[i * 4 + j] += a[i * 4 + k] * b[k * 4 + j];
            }
        }
    }
}

inline void TranslateMatrix(float* matrix, float x, float y, float z) {
    matrix[12] += matrix[0] * x + matrix[4] * y + matrix[8] * z;
    matrix[13] += matrix[1] * x + matrix[5] * y + matrix[9] * z;
    matrix[14] += matrix[2] * x + matrix[6] * y + matrix[10] * z;
    matrix[15] += matrix[3] * x + matrix[7] * y + matrix[11] * z;
}

inline void ScaleMatrix(float* matrix, float x, float y, float z) {
    for (int i = 0; i < 4; i++) {
        matrix[i * 4] *= x;
        matrix[i * 4 + 1] *= y;
        matrix[i * 4 + 2] *= z;
    }
}

inline void RotateMatrixZ(float* matrix, float angleDeg) {
    float rad = angleDeg * 3.14159f / 180.0f;
    float cosA = cos(rad);
    float sinA = sin(rad);
    
    float temp[16];
    for (int i = 0; i < 16; i++) temp[i] = matrix[i];
    
    matrix[0] = temp[0] * cosA + temp[4] * sinA;
    matrix[1] = temp[1] * cosA + temp[5] * sinA;
    matrix[2] = temp[2] * cosA + temp[6] * sinA;
    matrix[3] = temp[3] * cosA + temp[7] * sinA;
    
    matrix[4] = temp[4] * cosA - temp[0] * sinA;
    matrix[5] = temp[5] * cosA - temp[1] * sinA;
    matrix[6] = temp[6] * cosA - temp[2] * sinA;
    matrix[7] = temp[7] * cosA - temp[3] * sinA;
}

inline void IdentityMatrix(float* matrix) {
    for (int i = 0; i < 16; i++) matrix[i] = 0;
    matrix[0] = matrix[5] = matrix[10] = matrix[15] = 1.0f;
}

class Texture2D {
public:
    Texture2D() : m_RendererID(0), m_Width(0), m_Height(0) {}
    
    ~Texture2D() {
        if (m_RendererID) {
            glDeleteTextures(1, &m_RendererID);
        }
    }
    
    bool LoadFromFile(const std::string& path, bool flipVertically = true) {
        if (m_RendererID) {
            glDeleteTextures(1, &m_RendererID);
        }
        
        stbi_set_flip_vertically_on_load(flipVertically);
        
        int width, height, channels;
        unsigned char* data = stbi_load(path.c_str(), &width, &height, &channels, 0);
        
        if (!data) {
            std::cout << "Failed to load texture: " << path << std::endl;
            return false;
        }
        
        m_Width = width;
        m_Height = height;
        
        GLenum format = GL_RGB;
        if (channels == 1) format = GL_RED;
        else if (channels == 3) format = GL_RGB;
        else if (channels == 4) format = GL_RGBA;
        
        glGenTextures(1, &m_RendererID);
        glBindTexture(GL_TEXTURE_2D, m_RendererID);
        
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        
        stbi_image_free(data);
        
        return true;
    }
    
    void Bind(unsigned int slot = 0) const {
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_2D, m_RendererID);
    }
    
    void Unbind() const {
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    
    unsigned int GetID() const { return m_RendererID; }
    int GetWidth() const { return m_Width; }
    int GetHeight() const { return m_Height; }
    
private:
    unsigned int m_RendererID;
    int m_Width, m_Height;
};

class Sprite {
public:
    Sprite() : m_VAO(0), m_VBO(0), m_EBO(0) {
        SetupQuad();
    }
    
    ~Sprite() {
        if (m_VAO) {
            glDeleteVertexArrays(1, &m_VAO);
            glDeleteBuffers(1, &m_VBO);
            glDeleteBuffers(1, &m_EBO);
        }
    }
    
    void Draw(Shader& shader, const Texture2D& texture, 
              float x, float y, float width, float height,
              float rotation = 0.0f, float r = 1.0f, float g = 1.0f, float b = 1.0f) {
        shader.Use();
        
        // Создаем матрицу модели
        float model[16];
        IdentityMatrix(model);
        
        // Масштабирование
        ScaleMatrix(model, width, height, 1.0f);
        
        // Вращение (вокруг центра)
        float centerX = 0.5f;
        float centerY = 0.5f;
        TranslateMatrix(model, -centerX, -centerY, 0);
        RotateMatrixZ(model, rotation);
        TranslateMatrix(model, centerX, centerY, 0);
        
        // Позиция
        TranslateMatrix(model, x, y, 0);
        
        shader.SetMat4("model", model);
        shader.SetVec3("spriteColor", r, g, b);
        
        texture.Bind(0);
        shader.SetInt("texture1", 0);
        
        glBindVertexArray(m_VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }
    
    void DrawPart(Shader& shader, const Texture2D& texture,
                  float x, float y, float width, float height,
                  float texX, float texY, float texWidth, float texHeight,
                  float rotation = 0.0f) {
        shader.Use();
        
        // Создаем матрицу модели
        float model[16];
        IdentityMatrix(model);
        
        ScaleMatrix(model, width, height, 1.0f);
        
        float centerX = 0.5f;
        float centerY = 0.5f;
        TranslateMatrix(model, -centerX, -centerY, 0);
        RotateMatrixZ(model, rotation);
        TranslateMatrix(model, centerX, centerY, 0);
        
        TranslateMatrix(model, x, y, 0);
        
        shader.SetMat4("model", model);
        
        texture.Bind(0);
        shader.SetInt("texture1", 0);
        
        // Обновляем UV координаты для части текстуры
        UpdateUVs(texX, texY, texWidth, texHeight);
        
        glBindVertexArray(m_VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
        
        // Восстанавливаем стандартные UV
        ResetUVs();
    }
    
private:
    unsigned int m_VAO, m_VBO, m_EBO;
    std::vector<float> m_Vertices;
    
    void SetupQuad() {
        m_Vertices = {
            // positions        // texture coords
            0.0f, 1.0f, 0.0f,   0.0f, 1.0f,
            0.0f, 0.0f, 0.0f,   0.0f, 0.0f,
            1.0f, 0.0f, 0.0f,   1.0f, 0.0f,
            1.0f, 1.0f, 0.0f,   1.0f, 1.0f
        };
        
        unsigned int indices[] = {
            0, 1, 2,
            0, 2, 3
        };
        
        glGenVertexArrays(1, &m_VAO);
        glGenBuffers(1, &m_VBO);
        glGenBuffers(1, &m_EBO);
        
        glBindVertexArray(m_VAO);
        
        glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
        glBufferData(GL_ARRAY_BUFFER, m_Vertices.size() * sizeof(float), m_Vertices.data(), GL_DYNAMIC_DRAW);
        
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
        
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        
        glBindVertexArray(0);
    }
    
    void UpdateUVs(float u, float v, float w, float h) {
        std::vector<float> newVertices = {
            0.0f, 1.0f, 0.0f,   u, v + h,
            0.0f, 0.0f, 0.0f,   u, v,
            1.0f, 0.0f, 0.0f,   u + w, v,
            1.0f, 1.0f, 0.0f,   u + w, v + h
        };
        
        glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, newVertices.size() * sizeof(float), newVertices.data());
    }
    
    void ResetUVs() {
        glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, m_Vertices.size() * sizeof(float), m_Vertices.data());
    }
};

// Вершинный шейдер для спрайтов
const char* spriteVertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
out vec2 TexCoord;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
void main() {
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    TexCoord = aTexCoord;
}
)";

const char* spriteFragmentShaderSource = R"(
#version 330 core
in vec2 TexCoord;
out vec4 FragColor;
uniform sampler2D texture1;
uniform vec3 spriteColor;
void main() {
    FragColor = texture(texture1, TexCoord) * vec4(spriteColor, 1.0);
}
)";

}