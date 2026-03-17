#ifndef MESH_H
#define MESH_H
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "../math/Vector.h"
#include <glad/glad.h>

typedef struct Vertex {
    Vector3 position;
    Vector3 normal;
    Vector2 texCoords;
    Vector3 tangent;
    Vector3 bitangent;
} Vertex;

// Структура текстуры
typedef struct Texture {
    unsigned int id;
    char type[32];        // "diffuse", "specular", "normal", "height"
    char path[256];
} Texture;

// Структура меша
typedef struct Mesh {
    Vertex* vertices;
    unsigned int* indices;
    Texture* textures;
    
    int vertexCount;
    int indexCount;
    int textureCount;
    
    // Буферы OpenGL
    unsigned int VAO;
    unsigned int VBO;
    unsigned int EBO;
} Mesh;

// Инициализация меша
void Mesh_Init(Mesh* mesh) {
    if (!mesh) return;
    
    mesh->vertices = NULL;
    mesh->indices = NULL;
    mesh->textures = NULL;
    mesh->vertexCount = 0;
    mesh->indexCount = 0;
    mesh->textureCount = 0;
    mesh->VAO = 0;
    mesh->VBO = 0;
    mesh->EBO = 0;
}

// Создание и настройка меша
void Mesh_Create(Mesh* mesh, Vertex* vertices, int vertexCount, 
                 unsigned int* indices, int indexCount, 
                 Texture* textures, int textureCount) {
    
    if (!mesh) return;
    
    // Очищаем предыдущие данные если есть
    Mesh_Cleanup(mesh);
    
    // Копируем данные
    mesh->vertexCount = vertexCount;
    mesh->indexCount = indexCount;
    mesh->textureCount = textureCount;
    
    // Выделяем память и копируем вершины
    if (vertexCount > 0 && vertices) {
        mesh->vertices = (Vertex*)malloc(vertexCount * sizeof(Vertex));
        if (mesh->vertices) {
            memcpy(mesh->vertices, vertices, vertexCount * sizeof(Vertex));
        }
    }
    
    // Копируем индексы
    if (indexCount > 0 && indices) {
        mesh->indices = (unsigned int*)malloc(indexCount * sizeof(unsigned int));
        if (mesh->indices) {
            memcpy(mesh->indices, indices, indexCount * sizeof(unsigned int));
        }
    }
    
    // Копируем текстуры если есть
    if (textureCount > 0 && textures) {
        mesh->textures = (Texture*)malloc(textureCount * sizeof(Texture));
        if (mesh->textures) {
            memcpy(mesh->textures, textures, textureCount * sizeof(Texture));
        }
    }
    
    // Создаем буферы OpenGL
    glGenVertexArrays(1, &mesh->VAO);
    glGenBuffers(1, &mesh->VBO);
    glGenBuffers(1, &mesh->EBO);
    
    glBindVertexArray(mesh->VAO);
    
    // Заполняем VBO
    glBindBuffer(GL_ARRAY_BUFFER, mesh->VBO);
    glBufferData(GL_ARRAY_BUFFER, vertexCount * sizeof(Vertex), 
                 mesh->vertices, GL_STATIC_DRAW);
    
    // Заполняем EBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(unsigned int), 
                 mesh->indices, GL_STATIC_DRAW);
    
    // Настройка атрибутов вершин
    // Позиция (location = 0)
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    
    // Нормали (location = 1)
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 
                         (void*)offsetof(Vertex, normal));
    
    // Текстурные координаты (location = 2)
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), 
                         (void*)offsetof(Vertex, texCoords));
    
    // Тангенс (location = 3)
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 
                         (void*)offsetof(Vertex, tangent));
    
    // Битангес (location = 4)
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 
                         (void*)offsetof(Vertex, bitangent));
    
    glBindVertexArray(0);
}

// Отрисовка меша
void Mesh_Draw(Mesh* mesh, unsigned int shaderProgram) {
    if (!mesh || mesh->VAO == 0 || mesh->indexCount == 0) return;
    
    // Привязываем текстуры
    for (int i = 0; i < mesh->textureCount; i++) {
        glActiveTexture(GL_TEXTURE0 + i);
        
        // Определяем тип текстуры для шейдера
        char uniformName[64];
        if (strcmp(mesh->textures[i].type, "diffuse") == 0) {
            sprintf(uniformName, "material.diffuse[%d]", i);
        } else if (strcmp(mesh->textures[i].type, "specular") == 0) {
            sprintf(uniformName, "material.specular[%d]", i);
        } else if (strcmp(mesh->textures[i].type, "normal") == 0) {
            sprintf(uniformName, "material.normal[%d]", i);
        } else if (strcmp(mesh->textures[i].type, "height") == 0) {
            sprintf(uniformName, "material.height[%d]", i);
        } else {
            sprintf(uniformName, "material.texture[%d]", i);
        }
        
        // Устанавливаем сэмплер в шейдере
        glUniform1i(glGetUniformLocation(shaderProgram, uniformName), i);
        
        // Привязываем текстуру
        glBindTexture(GL_TEXTURE_2D, mesh->textures[i].id);
    }
    
    // Рендерим меш
    glBindVertexArray(mesh->VAO);
    glDrawElements(GL_TRIANGLES, mesh->indexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    
    // Сбрасываем активную текстуру
    glActiveTexture(GL_TEXTURE0);
}

// Очистка ресурсов меша
void Mesh_Cleanup(Mesh* mesh) {
    if (!mesh) return;
    
    // Освобождаем память
    if (mesh->vertices) {
        free(mesh->vertices);
        mesh->vertices = NULL;
    }
    
    if (mesh->indices) {
        free(mesh->indices);
        mesh->indices = NULL;
    }
    
    if (mesh->textures) {
        free(mesh->textures);
        mesh->textures = NULL;
    }
    
    // Удаляем буферы OpenGL
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
    
    mesh->vertexCount = 0;
    mesh->indexCount = 0;
    mesh->textureCount = 0;
}

// Вспомогательные функции для создания текстур
Texture Texture_CreateDiffuse(unsigned int id, const char* path) {
    Texture tex;
    tex.id = id;
    strcpy(tex.type, "diffuse");
    if (path) {
        strcpy(tex.path, path);
    } else {
        tex.path[0] = '\0';
    }
    return tex;
}

Texture Texture_CreateSpecular(unsigned int id, const char* path) {
    Texture tex;
    tex.id = id;
    strcpy(tex.type, "specular");
    if (path) {
        strcpy(tex.path, path);
    } else {
        tex.path[0] = '\0';
    }
    return tex;
}

Texture Texture_CreateNormal(unsigned int id, const char* path) {
    Texture tex;
    tex.id = id;
    strcpy(tex.type, "normal");
    if (path) {
        strcpy(tex.path, path);
    } else {
        tex.path[0] = '\0';
    }
    return tex;
}

Texture Texture_CreateHeight(unsigned int id, const char* path) {
    Texture tex;
    tex.id = id;
    strcpy(tex.type, "height");
    if (path) {
        strcpy(tex.path, path);
    } else {
        tex.path[0] = '\0';
    }
    return tex;
}

// Функции для работы с вершинами
Vertex Vertex_Create(Vector3 pos, Vector3 normal, Vector2 texCoords) {
    Vertex v;
    v.position = pos;
    v.normal = normal;
    v.texCoords = texCoords;
    v.tangent.x = v.tangent.y = v.tangent.z = 0;
    v.bitangent.x = v.bitangent.y = v.bitangent.z = 0;
    return v;
}

void Vertex_SetTangent(Vertex* vertex, Vector3 tangent) {
    if (vertex) {
        vertex->tangent = tangent;
    }
}

void Vertex_SetBitangent(Vertex* vertex, Vector3 bitangent) {
    if (vertex) {
        vertex->bitangent = bitangent;
    }
}

#endif