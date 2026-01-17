#ifndef MESH_H
#define MESH_H

#include <glad/glad.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    unsigned int VAO;      // Vertex Array Object
    unsigned int VBO;      // Vertex Buffer Object
    unsigned int EBO;      // Element Buffer Object (опционально)
    unsigned int vertexCount; // Количество вершин
    unsigned int indexCount;  // Количество индексов (если используется EBO)
    float* vertices;       // Массив вершинных данных
    unsigned int* indices; // Массив индексов (опционально)
    unsigned int vertexSize; // Размер всех вершинных данных в байтах
} Mesh;

/**
 * @brief Инициализирует меш с вершинными данными
 */
static inline void meshInit(Mesh* mesh, 
                           float* vertices, 
                           unsigned int vertexCount,
                           unsigned int vertexSize,
                           unsigned int* indices, 
                           unsigned int indexCount,
                           GLenum usage) {
    
    // Инициализация полей
    mesh->vertexCount = vertexCount;
    mesh->indexCount = indexCount;
    mesh->vertexSize = vertexSize;
    mesh->vertices = NULL;
    mesh->indices = NULL;
    mesh->EBO = 0;
    
    // Копирование вершинных данных
    unsigned int floatCount = vertexSize / sizeof(float) * vertexCount;
    mesh->vertices = (float*)malloc(sizeof(float) * floatCount);
    if (mesh->vertices) {
        memcpy(mesh->vertices, vertices, sizeof(float) * floatCount);
    }
    
    // Копирование индексов (если есть)
    if (indices && indexCount > 0) {
        mesh->indices = (unsigned int*)malloc(sizeof(unsigned int) * indexCount);
        if (mesh->indices) {
            memcpy(mesh->indices, indices, sizeof(unsigned int) * indexCount);
        }
    }
    
    // Генерация OpenGL объектов
    glGenVertexArrays(1, &mesh->VAO);
    glGenBuffers(1, &mesh->VBO);
    
    // Настройка VAO и VBO
    glBindVertexArray(mesh->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, mesh->VBO);
    glBufferData(GL_ARRAY_BUFFER, 
                 vertexSize * vertexCount, 
                 vertices, 
                 usage);
    
    // Настройка EBO (если есть индексы)
    if (mesh->indices && mesh->indexCount > 0) {
        glGenBuffers(1, &mesh->EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, 
                     sizeof(unsigned int) * indexCount, 
                     indices, 
                     usage);
    }
    
    // Отвязывание буферов
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    
    if (mesh->EBO) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }
}

/**
 * @brief Настраивает вершинные атрибуты
 */
static inline void meshSetAttribute(Mesh* mesh, 
                                   unsigned int index,
                                   int size, 
                                   GLenum type, 
                                   GLboolean normalized,
                                   unsigned int stride, 
                                   const void* offset) {
    
    if (!mesh->VAO) return;
    
    glBindVertexArray(mesh->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, mesh->VBO);
    
    glVertexAttribPointer(index, size, type, normalized, stride, offset);
    glEnableVertexAttribArray(index);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

/**
 * @brief Создает меш для треугольника (удобная функция-обертка)
 */
static inline void meshCreateTriangle(Mesh* mesh) {
    float vertices[] = {
        -0.5f, -0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
         0.0f,  0.5f, 0.0f
    };
    
    meshInit(mesh, 
             vertices, 
             3,                     // 3 вершины
             sizeof(vertices),      // общий размер данных
             NULL,                  // без индексов
             0,                     // 0 индексов
             GL_STATIC_DRAW);       // статичное использование
    
    // Настройка атрибута позиции (location = 0)
    meshSetAttribute(mesh, 0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
}

/**
 * @brief Создает меш для прямоугольника
 */
static inline void meshCreateQuad(Mesh* mesh) {
    float vertices[] = {
        // Позиции          // Текстурные координаты (опционально)
        -0.5f,  0.5f, 0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
         0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
         0.5f,  0.5f, 0.0f, 1.0f, 1.0f
    };
    
    unsigned int indices[] = {
        0, 1, 2,
        0, 2, 3
    };
    
    meshInit(mesh, 
             vertices, 
             4,                     // 4 вершины
             sizeof(vertices),      // общий размер данных
             indices,               // с индексами
             6,                     // 6 индексов
             GL_STATIC_DRAW);       // статичное использование
    
    // Настройка атрибута позиции (location = 0)
    meshSetAttribute(mesh, 0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    // Настройка текстурных координат (location = 1)
    meshSetAttribute(mesh, 1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
}

/**
 * @brief Рисует меш
 */
static inline void meshDraw(Mesh* mesh, GLenum mode) {
    if (!mesh->VAO) return;
    
    glBindVertexArray(mesh->VAO);
    
    if (mesh->indices && mesh->indexCount > 0) {
        glDrawElements(mode, mesh->indexCount, GL_UNSIGNED_INT, 0);
    } else {
        glDrawArrays(mode, 0, mesh->vertexCount);
    }
    
    glBindVertexArray(0);
}

/**
 * @brief Обновляет данные вершин в буфере
 */
static inline void meshUpdateVertices(Mesh* mesh, float* data, unsigned int size) {
    if (!mesh->VBO || size == 0) return;
    
    // Обновляем локальную копию
    if (mesh->vertices) {
        free(mesh->vertices);
    }
    
    unsigned int floatCount = size / sizeof(float);
    mesh->vertices = (float*)malloc(size);
    if (mesh->vertices) {
        memcpy(mesh->vertices, data, size);
        mesh->vertexSize = size;
        mesh->vertexCount = floatCount / 3; // Предполагаем 3 компонента на вершину
    }
    
    // Обновляем буфер OpenGL
    glBindBuffer(GL_ARRAY_BUFFER, mesh->VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

/**
 * @brief Освобождает ресурсы меша
 */
static inline void meshFree(Mesh* mesh) {
    if (mesh->VAO) {
        glDeleteVertexArrays(1, &mesh->VAO);
        mesh->VAO = 0;
    }
    
    if (mesh->VBO) {
        glDeleteBuffers(1, &mesh->VBO);
        mesh->VBO = 0;
    }
    
    if (mesh->EBO) {
        glDeleteBuffers(1, &mesh->EBO);
        mesh->EBO = 0;
    }
    
    if (mesh->vertices) {
        free(mesh->vertices);
        mesh->vertices = NULL;
    }
    
    if (mesh->indices) {
        free(mesh->indices);
        mesh->indices = NULL;
    }
    
    mesh->vertexCount = 0;
    mesh->indexCount = 0;
    mesh->vertexSize = 0;
}

#endif