// mesh.h
#ifndef MESH_H
#define MESH_H

#include "../../utils/math/geometry.h"
#include "../shader.h"
#include "../texture.h"
#include <glad/glad.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define PI 3.14159265359f

// ==================== СТРУКТУРЫ ВЕРШИН ====================

// Структура для вершины с позицией и нормалью
typedef struct Vertex {
    Vec3 position;
    Vec3 normal;
    Vec2 texCoords;
    Vec3 color;
} Vertex;

// Структура для вершины с позицией и цветом (простая)
typedef struct SimpleVertex {
    Vec3 position;
    Vec3 color;
} SimpleVertex;

// Структура для вершины с позицией и текстурными координатами
typedef struct TexturedVertex {
    Vec3 position;
    Vec2 texCoords;
} TexturedVertex;

// ==================== СТРУКТУРА МЕША ====================

typedef struct Mesh {
    // Буферы OpenGL
    unsigned int VAO;      // Vertex Array Object
    unsigned int VBO;      // Vertex Buffer Object
    unsigned int EBO;      // Element Buffer Object
    
    // Данные меша
    Vertex* vertices;      // Массив вершин
    unsigned int* indices; // Массив индексов
    unsigned int vertexCount; // Количество вершин
    unsigned int indexCount;  // Количество индексов
    
    // Материал
    Vec3 color;           // Цвет меша (если нет текстуры)
    float shininess;      // Блеск материала
    int useTexture;       // Использовать ли текстуру
    
    // Текстура
    GLTexture* texture;   // Текстура меша
    int hasTexture;       // Флаг наличия текстуры
    
    // Трансформации
    Vec3 position;        // Позиция в мире
    Vec3 rotation;        // Вращение (в градусах)
    Vec3 scale;           // Масштаб
    
    // Имя меша (для отладки)
    char* name;
} Mesh;

// ==================== FORWARD DECLARATIONS ====================

// Операции с текстурой (объявляем раньше использования)
void mesh_set_texture(Mesh* mesh, GLTexture* texture);
void mesh_remove_texture(Mesh* mesh);

// ==================== ФУНКЦИИ СОЗДАНИЯ МЕША ====================

// Создание пустого меша
Mesh* mesh_create_empty(const char* name) {
    Mesh* mesh = (Mesh*)malloc(sizeof(Mesh));
    if (!mesh) return NULL;
    
    // Инициализация нулями
    memset(mesh, 0, sizeof(Mesh));
    
    // Имя меша
    if (name) {
        mesh->name = (char*)malloc(strlen(name) + 1);
        if (mesh->name) {
            strcpy(mesh->name, name);
        }
    }
    
    // Инициализация значений по умолчанию
    mesh->color = vec3_create(1.0f, 1.0f, 1.0f); // Белый цвет
    mesh->position = vec3_create(0.0f, 0.0f, 0.0f);
    mesh->rotation = vec3_create(0.0f, 0.0f, 0.0f);
    mesh->scale = vec3_create(1.0f, 1.0f, 1.0f);
    mesh->shininess = 32.0f;
    mesh->useTexture = 0;
    mesh->hasTexture = 0;
    mesh->texture = NULL;
    
    return mesh;
}

// Генерация буферов OpenGL
void mesh_generate_buffers(Mesh* mesh) {
    if (!mesh) return;
    
    // Генерируем VAO, VBO, EBO
    glGenVertexArrays(1, &mesh->VAO);
    glGenBuffers(1, &mesh->VBO);
    glGenBuffers(1, &mesh->EBO);
    
    // Привязываем VAO
    glBindVertexArray(mesh->VAO);
    
    // Копируем вершины в буфер
    glBindBuffer(GL_ARRAY_BUFFER, mesh->VBO);
    glBufferData(GL_ARRAY_BUFFER, mesh->vertexCount * sizeof(Vertex), mesh->vertices, GL_STATIC_DRAW);
    
    // Копируем индексы в буфер
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->indexCount * sizeof(unsigned int), mesh->indices, GL_STATIC_DRAW);
    
    // Устанавливаем указатели атрибутов вершин
    // Позиция (location = 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);
    
    // Нормаль (location = 1)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(1);
    
    // Текстурные координаты (location = 2)
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));
    glEnableVertexAttribArray(2);
    
    // Цвет (location = 3)
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));
    glEnableVertexAttribArray(3);
    
    // Отвязываем VAO
    glBindVertexArray(0);
}

// Обновление буферов (после изменения вершин)
void mesh_update_buffers(Mesh* mesh) {
    if (!mesh) return;
    
    glBindBuffer(GL_ARRAY_BUFFER, mesh->VBO);
    glBufferData(GL_ARRAY_BUFFER, mesh->vertexCount * sizeof(Vertex), mesh->vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

// Создание меша из данных вершин
Mesh* mesh_create(Vertex* vertices, unsigned int vertexCount, 
                  unsigned int* indices, unsigned int indexCount,
                  const char* name) {
    Mesh* mesh = mesh_create_empty(name);
    if (!mesh) return NULL;
    
    // Копируем данные
    mesh->vertexCount = vertexCount;
    mesh->indexCount = indexCount;
    
    // Выделяем память и копируем вершины
    mesh->vertices = (Vertex*)malloc(sizeof(Vertex) * vertexCount);
    if (mesh->vertices && vertices) {
        memcpy(mesh->vertices, vertices, sizeof(Vertex) * vertexCount);
    }
    
    // Выделяем память и копируем индексы
    mesh->indices = (unsigned int*)malloc(sizeof(unsigned int) * indexCount);
    if (mesh->indices && indices) {
        memcpy(mesh->indices, indices, sizeof(unsigned int) * indexCount);
    }
    
    // Генерируем буферы
    mesh_generate_buffers(mesh);
    
    return mesh;
}

// Создание простого меша (только позиция и цвет)
Mesh* mesh_create_simple(SimpleVertex* vertices, unsigned int vertexCount,
                         unsigned int* indices, unsigned int indexCount,
                         const char* name) {
    // Конвертируем SimpleVertex в Vertex
    Vertex* convVertices = (Vertex*)malloc(sizeof(Vertex) * vertexCount);
    if (!convVertices) return NULL;
    
    for (unsigned int i = 0; i < vertexCount; i++) {
        convVertices[i].position = vertices[i].position;
        convVertices[i].color = vertices[i].color;
        convVertices[i].normal = vec3_create(0.0f, 0.0f, 1.0f);
        convVertices[i].texCoords = vec2_create(0.0f, 0.0f);
    }
    
    // Создаем меш
    Mesh* mesh = mesh_create(convVertices, vertexCount, indices, indexCount, name);
    
    free(convVertices);
    return mesh;
}

// Создание меша с текстурными координатами
Mesh* mesh_create_textured(TexturedVertex* vertices, unsigned int vertexCount,
                           unsigned int* indices, unsigned int indexCount,
                           const char* name) {
    // Конвертируем TexturedVertex в Vertex
    Vertex* convVertices = (Vertex*)malloc(sizeof(Vertex) * vertexCount);
    if (!convVertices) return NULL;
    
    for (unsigned int i = 0; i < vertexCount; i++) {
        convVertices[i].position = vertices[i].position;
        convVertices[i].texCoords = vertices[i].texCoords;
        convVertices[i].color = vec3_create(1.0f, 1.0f, 1.0f);
        convVertices[i].normal = vec3_create(0.0f, 0.0f, 1.0f);
    }
    
    // Создаем меш
    Mesh* mesh = mesh_create(convVertices, vertexCount, indices, indexCount, name);
    
    free(convVertices);
    return mesh;
}

// ==================== ПРИМИТИВЫ ====================

// Создание треугольника
Mesh* mesh_create_triangle(Vec3 color, const char* name) {
    SimpleVertex vertices[3] = {
        {{-0.5f, -0.5f, 0.0f}, {color.x, color.y, color.z}},
        {{ 0.5f, -0.5f, 0.0f}, {color.x, color.y, color.z}},
        {{ 0.0f,  0.5f, 0.0f}, {color.x, color.y, color.z}}
    };
    
    unsigned int indices[3] = {0, 1, 2};
    
    return mesh_create_simple(vertices, 3, indices, 3, name);
}

// Создание квадрата
Mesh* mesh_create_quad(float size, Vec3 color, const char* name) {
    float halfSize = size * 0.5f;
    
    SimpleVertex vertices[4] = {
        {{-halfSize, -halfSize, 0.0f}, {color.x, color.y, color.z}},
        {{ halfSize, -halfSize, 0.0f}, {color.x, color.y, color.z}},
        {{ halfSize,  halfSize, 0.0f}, {color.x, color.y, color.z}},
        {{-halfSize,  halfSize, 0.0f}, {color.x, color.y, color.z}}
    };
    
    unsigned int indices[6] = {0, 1, 2, 2, 3, 0};
    
    return mesh_create_simple(vertices, 4, indices, 6, name);
}

// Создание куба
Mesh* mesh_create_cube(float size, Vec3 color, const char* name) {
    float halfSize = size * 0.5f;
    
    Vertex vertices[24] = {
        // Front face
        {{-halfSize, -halfSize,  halfSize}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}, {color.x, color.y, color.z}},
        {{ halfSize, -halfSize,  halfSize}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}, {color.x, color.y, color.z}},
        {{ halfSize,  halfSize,  halfSize}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}, {color.x, color.y, color.z}},
        {{-halfSize,  halfSize,  halfSize}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}, {color.x, color.y, color.z}},
        
        // Back face
        {{-halfSize, -halfSize, -halfSize}, {0.0f, 0.0f, -1.0f}, {1.0f, 0.0f}, {color.x, color.y, color.z}},
        {{-halfSize,  halfSize, -halfSize}, {0.0f, 0.0f, -1.0f}, {1.0f, 1.0f}, {color.x, color.y, color.z}},
        {{ halfSize,  halfSize, -halfSize}, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f}, {color.x, color.y, color.z}},
        {{ halfSize, -halfSize, -halfSize}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f}, {color.x, color.y, color.z}},
        
        // Top face
        {{-halfSize,  halfSize, -halfSize}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}, {color.x, color.y, color.z}},
        {{-halfSize,  halfSize,  halfSize}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}, {color.x, color.y, color.z}},
        {{ halfSize,  halfSize,  halfSize}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}, {color.x, color.y, color.z}},
        {{ halfSize,  halfSize, -halfSize}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}, {color.x, color.y, color.z}},
        
        // Bottom face
        {{-halfSize, -halfSize, -halfSize}, {0.0f, -1.0f, 0.0f}, {1.0f, 1.0f}, {color.x, color.y, color.z}},
        {{ halfSize, -halfSize, -halfSize}, {0.0f, -1.0f, 0.0f}, {0.0f, 1.0f}, {color.x, color.y, color.z}},
        {{ halfSize, -halfSize,  halfSize}, {0.0f, -1.0f, 0.0f}, {0.0f, 0.0f}, {color.x, color.y, color.z}},
        {{-halfSize, -halfSize,  halfSize}, {0.0f, -1.0f, 0.0f}, {1.0f, 0.0f}, {color.x, color.y, color.z}},
        
        // Right face
        {{ halfSize, -halfSize, -halfSize}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}, {color.x, color.y, color.z}},
        {{ halfSize,  halfSize, -halfSize}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}, {color.x, color.y, color.z}},
        {{ halfSize,  halfSize,  halfSize}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}, {color.x, color.y, color.z}},
        {{ halfSize, -halfSize,  halfSize}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}, {color.x, color.y, color.z}},
        
        // Left face
        {{-halfSize, -halfSize, -halfSize}, {-1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}, {color.x, color.y, color.z}},
        {{-halfSize, -halfSize,  halfSize}, {-1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}, {color.x, color.y, color.z}},
        {{-halfSize,  halfSize,  halfSize}, {-1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}, {color.x, color.y, color.z}},
        {{-halfSize,  halfSize, -halfSize}, {-1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}, {color.x, color.y, color.z}}
    };
    
    unsigned int indices[36] = {
        0, 1, 2, 2, 3, 0,       // Front
        4, 5, 6, 6, 7, 4,       // Back
        8, 9, 10, 10, 11, 8,    // Top
        12, 13, 14, 14, 15, 12, // Bottom
        16, 17, 18, 18, 19, 16, // Right
        20, 21, 22, 22, 23, 20  // Left
    };
    
    return mesh_create(vertices, 24, indices, 36, name);
}

// Создание плоскости
Mesh* mesh_create_plane(float width, float height, Vec3 color, const char* name) {
    float halfWidth = width * 0.5f;
    float halfHeight = height * 0.5f;
    
    Vertex vertices[4] = {
        {{-halfWidth, 0.0f, -halfHeight}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}, {color.x, color.y, color.z}},
        {{ halfWidth, 0.0f, -halfHeight}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}, {color.x, color.y, color.z}},
        {{ halfWidth, 0.0f,  halfHeight}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}, {color.x, color.y, color.z}},
        {{-halfWidth, 0.0f,  halfHeight}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}, {color.x, color.y, color.z}}
    };
    
    unsigned int indices[6] = {0, 1, 2, 2, 3, 0};
    
    return mesh_create(vertices, 4, indices, 6, name);
}

// Создание пирамиды
Mesh* mesh_create_pyramid(float baseSize, float height, Vec3 color, const char* name) {
    float halfBase = baseSize * 0.5f;
    
    Vertex vertices[5] = {
        // Основание
        {{-halfBase, 0.0f, -halfBase}, {0.0f, -1.0f, 0.0f}, {0.0f, 0.0f}, {color.x, color.y, color.z}},
        {{ halfBase, 0.0f, -halfBase}, {0.0f, -1.0f, 0.0f}, {1.0f, 0.0f}, {color.x, color.y, color.z}},
        {{ halfBase, 0.0f,  halfBase}, {0.0f, -1.0f, 0.0f}, {1.0f, 1.0f}, {color.x, color.y, color.z}},
        {{-halfBase, 0.0f,  halfBase}, {0.0f, -1.0f, 0.0f}, {0.0f, 1.0f}, {color.x, color.y, color.z}},
        
        // Вершина
        {{ 0.0f, height, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.5f, 0.5f}, {color.x, color.y, color.z}}
    };
    
    unsigned int indices[18] = {
        // Основание
        0, 1, 2,
        0, 2, 3,
        
        // Боковые грани
        0, 1, 4,
        1, 2, 4,
        2, 3, 4,
        3, 0, 4
    };
    
    Mesh* mesh = mesh_create(vertices, 5, indices, 18, name);
    
    // Пересчитываем нормали для боковых граней
    for (int i = 12; i < 18; i += 3) {
        unsigned int i1 = mesh->indices[i];
        unsigned int i2 = mesh->indices[i + 1];
        unsigned int i3 = mesh->indices[i + 2];
        
        Vec3 v1 = mesh->vertices[i1].position;
        Vec3 v2 = mesh->vertices[i2].position;
        Vec3 v3 = mesh->vertices[i3].position;
        
        Vec3 edge1 = vec3_sub(&v2, &v1);
        Vec3 edge2 = vec3_sub(&v3, &v1);
        Vec3 normal = vec3_cross(&edge1, &edge2);
        normal = vec3_normalized(&normal);
        
        mesh->vertices[i1].normal = normal;
        mesh->vertices[i2].normal = normal;
        mesh->vertices[i3].normal = normal;
    }
    
    mesh_update_buffers(mesh);
    return mesh;
}

// ==================== ПРИМИТИВЫ С ТЕКСТУРАМИ ====================

// Создание куба с текстурой
Mesh* mesh_create_cube_textured(float size, GLTexture* texture, const char* name) {
    Vec3 white = vec3_create(1.0f, 1.0f, 1.0f);
    Mesh* mesh = mesh_create_cube(size, white, name);
    if (mesh && texture) {
        mesh_set_texture(mesh, texture);
    }
    return mesh;
}

// Создание плоскости с текстурой
Mesh* mesh_create_plane_textured(float width, float height, GLTexture* texture, const char* name) {
    Vec3 white = vec3_create(1.0f, 1.0f, 1.0f);
    Mesh* mesh = mesh_create_plane(width, height, white, name);
    if (mesh && texture) {
        mesh_set_texture(mesh, texture);
    }
    return mesh;
}

// Создание квадрата с текстурой
Mesh* mesh_create_quad_textured(float size, GLTexture* texture, const char* name) {
    Vec3 white = vec3_create(1.0f, 1.0f, 1.0f);
    
    float halfSize = size * 0.5f;
    
    TexturedVertex vertices[4] = {
        {{-halfSize, -halfSize, 0.0f}, {0.0f, 0.0f}},
        {{ halfSize, -halfSize, 0.0f}, {1.0f, 0.0f}},
        {{ halfSize,  halfSize, 0.0f}, {1.0f, 1.0f}},
        {{-halfSize,  halfSize, 0.0f}, {0.0f, 1.0f}}
    };
    
    unsigned int indices[6] = {0, 1, 2, 2, 3, 0};
    
    Mesh* mesh = mesh_create_textured(vertices, 4, indices, 6, name);
    if (mesh && texture) {
        mesh_set_texture(mesh, texture);
    }
    return mesh;
}

// ==================== ОПЕРАЦИИ С МЕШЕМ ====================

// Установка цвета меша
void mesh_set_color(Mesh* mesh, Vec3 color) {
    if (!mesh) return;
    
    mesh->color = color;
    
    // Обновляем цвет всех вершин
    for (unsigned int i = 0; i < mesh->vertexCount; i++) {
        mesh->vertices[i].color = color;
    }
    
    mesh_update_buffers(mesh);
}

// Установка позиции
void mesh_set_position(Mesh* mesh, Vec3 position) {
    if (mesh) mesh->position = position;
}

// Установка вращения
void mesh_set_rotation(Mesh* mesh, Vec3 rotation) {
    if (mesh) mesh->rotation = rotation;
}

// Установка масштаба
void mesh_set_scale(Mesh* mesh, Vec3 scale) {
    if (mesh) mesh->scale = scale;
}

// Перемещение
void mesh_translate(Mesh* mesh, Vec3 translation) {
    if (mesh) {
        mesh->position = vec3_add(&mesh->position, &translation);
    }
}

// Вращение
void mesh_rotate(Mesh* mesh, Vec3 rotation) {
    if (mesh) {
        mesh->rotation = vec3_add(&mesh->rotation, &rotation);
    }
}

// Масштабирование меша
void mesh_scale_mesh(Mesh* mesh, Vec3 scaling) {
    if (mesh) {
        mesh->scale = vec3_add(&mesh->scale, &scaling);
    }
}

// ==================== ОПЕРАЦИИ С ТЕКСТУРОЙ ====================

// Установка текстуры для меша
void mesh_set_texture(Mesh* mesh, GLTexture* texture) {
    if (!mesh) return;
    
    // Удаляем старую текстуру, если есть
    if (mesh->texture && mesh->hasTexture) {
        Texture_Destroy(mesh->texture);
    }
    
    mesh->texture = texture;
    mesh->hasTexture = texture != NULL;
    mesh->useTexture = texture != NULL ? 1 : 0;
    
    // Если есть текстура, устанавливаем белый цвет
    if (mesh->hasTexture) {
        mesh_set_color(mesh, vec3_create(1.0f, 1.0f, 1.0f));
    }
}

// Удаление текстуры
void mesh_remove_texture(Mesh* mesh) {
    if (!mesh) return;
    
    if (mesh->texture && mesh->hasTexture) {
        Texture_Destroy(mesh->texture);
        mesh->texture = NULL;
        mesh->hasTexture = 0;
        mesh->useTexture = 0;
    }
}

// Проверка наличия текстуры
int mesh_has_texture(Mesh* mesh) {
    return mesh ? mesh->hasTexture : 0;
}

// Получение текстуры
GLTexture* mesh_get_texture(Mesh* mesh) {
    return mesh ? mesh->texture : NULL;
}

// ==================== РЕНДЕРИНГ ====================

// Рендеринг меша с шейдером
void mesh_draw(Mesh* mesh, Shader* shader) {
    if (!mesh || !shader) return;
    
    shader_use(shader);
    
    // Если есть текстура - устанавливаем ее
    if (mesh->hasTexture && mesh->texture) {
        Texture_Bind(mesh->texture, GL_TEXTURE0);
        shader_set_int(shader, "texture_diffuse1", 0);
        shader_set_int(shader, "useTexture", 1);
    } else {
        shader_set_int(shader, "useTexture", 0);
        // Устанавливаем цвет меша (если нет текстуры)
        shader_set_color(shader, "objectColor", mesh->color.x, mesh->color.y, mesh->color.z);
    }
    
    // Устанавливаем другие параметры материала
    shader_set_float(shader, "material.shininess", mesh->shininess);
    
    // Привязываем VAO
    glBindVertexArray(mesh->VAO);
    
    // Рисуем меш
    glDrawElements(GL_TRIANGLES, mesh->indexCount, GL_UNSIGNED_INT, 0);
    
    // Отвязываем VAО
    glBindVertexArray(0);
}

// Рендеринг меша с простым цветным шейдером
void mesh_draw_simple(Mesh* mesh) {
    if (!mesh) return;
    
    static Shader* simpleShader = NULL;
    if (!simpleShader) {
        simpleShader = shader_create_simple_color();
    }
    
    mesh_draw(mesh, simpleShader);
}

// Освобождение памяти
void mesh_destroy(Mesh* mesh) {
    if (!mesh) return;
    
    // Удаляем текстуру
    mesh_remove_texture(mesh);
    
    // Удаляем OpenGL буферы
    if (mesh->VAO) glDeleteVertexArrays(1, &mesh->VAO);
    if (mesh->VBO) glDeleteBuffers(1, &mesh->VBO);
    if (mesh->EBO) glDeleteBuffers(1, &mesh->EBO);
    
    // Освобождаем память
    if (mesh->vertices) free(mesh->vertices);
    if (mesh->indices) free(mesh->indices);
    if (mesh->name) free(mesh->name);
    
    free(mesh);
}

// Вывод информации о меше
void mesh_print_info(Mesh* mesh) {
    if (!mesh) {
        printf("Mesh: NULL\n");
        return;
    }
    
    printf("=== Mesh Info ===\n");
    printf("Name: %s\n", mesh->name ? mesh->name : "Unnamed");
    printf("Vertices: %u\n", mesh->vertexCount);
    printf("Indices: %u\n", mesh->indexCount);
    printf("Position: (%.2f, %.2f, %.2f)\n", 
           mesh->position.x, mesh->position.y, mesh->position.z);
    printf("Rotation: (%.2f, %.2f, %.2f)\n", 
           mesh->rotation.x, mesh->rotation.y, mesh->rotation.z);
    printf("Scale: (%.2f, %.2f, %.2f)\n", 
           mesh->scale.x, mesh->scale.y, mesh->scale.z);
    printf("Color: (%.2f, %.2f, %.2f)\n", 
           mesh->color.x, mesh->color.y, mesh->color.z);
    printf("Has Texture: %s\n", mesh->hasTexture ? "YES" : "NO");
    printf("Use Texture: %s\n", mesh->useTexture ? "YES" : "NO");
    printf("Shininess: %.1f\n", mesh->shininess);
    printf("VAO: %u, VBO: %u, EBO: %u\n", 
           mesh->VAO, mesh->VBO, mesh->EBO);
    printf("=================\n");
}

#endif