// object.h
#ifndef OBJECT_H
#define OBJECT_H

#include "mesh.h"
#include "../shader.h"
#include "../texture.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// ==================== СТРУКТУРА 3D ОБЪЕКТА ====================
typedef struct Object3D {
    Mesh* mesh;              // Меш объекта
    Shader* shader;          // Шейдер для рендеринга
    Vec3 position;           // Позиция в мире
    Vec3 rotation;           // Вращение (в градусах)
    Vec3 scale;              // Масштаб
    char* name;              // Имя объекта
    char* objFilePath;       // Путь к исходному OBJ файлу
    
    // Текстура
    GLTexture* texture;      // Текстура объекта
    int hasTexture;          // Флаг наличия текстуры
} Object3D;

// ==================== СТРУКТУРЫ ДЛЯ ПАРСИНГА OBJ ====================
typedef struct {
    float x, y, z;
} OBJ_Vec3;

typedef struct {
    float x, y;
} OBJ_Vec2;

typedef struct {
    int v_idx[3];      // Индексы вершин
    int vt_idx[3];     // Индексы текстурных координат
    int vn_idx[3];     // Индексы нормалей
    int material_idx;  // Индекс материала
} OBJ_Face;

typedef struct {
    OBJ_Vec3* vertices;      // Массив вершин
    OBJ_Vec2* texcoords;     // Массив текстурных координат
    OBJ_Vec3* normals;       // Массив нормалей
    OBJ_Face* faces;         // Массив граней
    int vertex_count;        // Количество вершин
    int texcoord_count;      // Количество текстурных координат
    int normal_count;        // Количество нормалей
    int face_count;          // Количество граней
    int vertices_capacity;   // Емкость массива вершин
    int texcoords_capacity;  // Емкость массива текстурных координат
    int normals_capacity;    // Емкость массива нормалей
    int faces_capacity;      // Емкость массива граней
} OBJ_Data;

// ==================== ПРОТОТИПЫ ФУНКЦИЙ ====================

// Создание и уничтожение объектов
Object3D* object_create(const char* name);
void object_destroy(Object3D* obj);

// Установка трансформаций
void object_set_position(Object3D* obj, Vec3 position);
void object_set_rotation(Object3D* obj, Vec3 rotation);
void object_set_scale(Object3D* obj, Vec3 scale);
void object_translate(Object3D* obj, Vec3 translation);
void object_rotate(Object3D* obj, Vec3 rotation);
void object_scale(Object3D* obj, Vec3 scaling);

// Операции с текстурой
void object_set_texture(Object3D* obj, GLTexture* texture);
void object_remove_texture(Object3D* obj);
int object_has_texture(Object3D* obj);
GLTexture* object_get_texture(Object3D* obj);

// Загрузка OBJ файлов
Object3D* object_load_from_obj(const char* filepath, const char* name, Shader* shader);
Object3D* object_load_from_obj_with_texture(const char* filepath, const char* name, 
                                           Shader* shader, const char* texturePath);
int obj_parse_file(const char* filepath, OBJ_Data* obj_data);
void obj_data_init(OBJ_Data* data);
void obj_data_cleanup(OBJ_Data* data);
Mesh* obj_data_to_mesh(OBJ_Data* data, const char* name);

// Рендеринг
void object_draw(Object3D* obj);
void object_draw_with_shader(Object3D* obj, Shader* shader);

// Информация
void object_print_info(Object3D* obj);

// ==================== РЕАЛИЗАЦИЯ ФУНКЦИЙ ====================

// Инициализация данных OBJ
void obj_data_init(OBJ_Data* data) {
    memset(data, 0, sizeof(OBJ_Data));
}

// Очистка данных OBJ
void obj_data_cleanup(OBJ_Data* data) {
    if (data->vertices) free(data->vertices);
    if (data->texcoords) free(data->texcoords);
    if (data->normals) free(data->normals);
    if (data->faces) free(data->faces);
    memset(data, 0, sizeof(OBJ_Data));
}

// Увеличение емкости массива вершин
static void obj_ensure_vertices_capacity(OBJ_Data* data, int required) {
    if (data->vertices_capacity < required) {
        int new_capacity = data->vertices_capacity == 0 ? 128 : data->vertices_capacity * 2;
        while (new_capacity < required) new_capacity *= 2;
        
        data->vertices = (OBJ_Vec3*)realloc(data->vertices, new_capacity * sizeof(OBJ_Vec3));
        data->vertices_capacity = new_capacity;
    }
}

// Увеличение емкости массива текстурных координат
static void obj_ensure_texcoords_capacity(OBJ_Data* data, int required) {
    if (data->texcoords_capacity < required) {
        int new_capacity = data->texcoords_capacity == 0 ? 128 : data->texcoords_capacity * 2;
        while (new_capacity < required) new_capacity *= 2;
        
        data->texcoords = (OBJ_Vec2*)realloc(data->texcoords, new_capacity * sizeof(OBJ_Vec2));
        data->texcoords_capacity = new_capacity;
    }
}

// Увеличение емкости массива нормалей
static void obj_ensure_normals_capacity(OBJ_Data* data, int required) {
    if (data->normals_capacity < required) {
        int new_capacity = data->normals_capacity == 0 ? 128 : data->normals_capacity * 2;
        while (new_capacity < required) new_capacity *= 2;
        
        data->normals = (OBJ_Vec3*)realloc(data->normals, new_capacity * sizeof(OBJ_Vec3));
        data->normals_capacity = new_capacity;
    }
}

// Увеличение емкости массива граней
static void obj_ensure_faces_capacity(OBJ_Data* data, int required) {
    if (data->faces_capacity < required) {
        int new_capacity = data->faces_capacity == 0 ? 128 : data->faces_capacity * 2;
        while (new_capacity < required) new_capacity *= 2;
        
        data->faces = (OBJ_Face*)realloc(data->faces, new_capacity * sizeof(OBJ_Face));
        data->faces_capacity = new_capacity;
    }
}

// Парсинг OBJ файла
int obj_parse_file(const char* filepath, OBJ_Data* obj_data) {
    FILE* file = fopen(filepath, "r");
    if (!file) {
        printf("ERROR: Cannot open OBJ file: %s\n", filepath);
        return 0;
    }
    
    char line[1024];
    int line_num = 0;
    
    // Инициализируем данные
    obj_data_init(obj_data);
    
    // Предварительный проход для подсчета количества элементов
    int vertex_count = 0, texcoord_count = 0, normal_count = 0, face_count = 0;
    
    while (fgets(line, sizeof(line), file)) {
        line_num++;
        
        // Пропускаем пустые строки и комментарии
        if (line[0] == '\n' || line[0] == '#' || line[0] == '\r') {
            continue;
        }
        
        // Обрезаем символ новой строки
        line[strcspn(line, "\n")] = 0;
        line[strcspn(line, "\r")] = 0;
        
        if (strlen(line) == 0) continue;
        
        // Анализируем первую лексему
        if (strncmp(line, "v ", 2) == 0) {
            vertex_count++;
        } else if (strncmp(line, "vt ", 3) == 0) {
            texcoord_count++;
        } else if (strncmp(line, "vn ", 3) == 0) {
            normal_count++;
        } else if (strncmp(line, "f ", 2) == 0) {
            face_count++;
        }
    }
    
    // Выделяем память с запасом
    obj_ensure_vertices_capacity(obj_data, vertex_count + 1);
    obj_ensure_texcoords_capacity(obj_data, texcoord_count + 1);
    obj_ensure_normals_capacity(obj_data, normal_count + 1);
    obj_ensure_faces_capacity(obj_data, face_count + 1);
    
    // Возвращаемся к началу файла для реального парсинга
    rewind(file);
    line_num = 0;
    
    // Сбрасываем счетчики
    obj_data->vertex_count = 0;
    obj_data->texcoord_count = 0;
    obj_data->normal_count = 0;
    obj_data->face_count = 0;
    
    while (fgets(line, sizeof(line), file)) {
        line_num++;
        
        // Пропускаем пустые строки и комментарии
        if (line[0] == '\n' || line[0] == '#' || line[0] == '\r') {
            continue;
        }
        
        // Обрезаем символ новой строки
        line[strcspn(line, "\n")] = 0;
        line[strcspn(line, "\r")] = 0;
        
        if (strlen(line) == 0) continue;
        
        // Парсинг вершин
        if (strncmp(line, "v ", 2) == 0) {
            OBJ_Vec3 vertex;
            int count = sscanf(line + 2, "%f %f %f", 
                             &vertex.x, &vertex.y, &vertex.z);
            
            if (count == 3) {
                obj_data->vertices[obj_data->vertex_count++] = vertex;
            } else {
                printf("WARNING: Line %d: Invalid vertex format\n", line_num);
            }
        }
        // Парсинг текстурных координат
        else if (strncmp(line, "vt ", 3) == 0) {
            OBJ_Vec2 texcoord;
            int count = sscanf(line + 3, "%f %f", 
                             &texcoord.x, &texcoord.y);
            
            if (count >= 2) {
                obj_data->texcoords[obj_data->texcoord_count++] = texcoord;
            } else {
                printf("WARNING: Line %d: Invalid texture coordinate format\n", line_num);
            }
        }
        // Парсинг нормалей
        else if (strncmp(line, "vn ", 3) == 0) {
            OBJ_Vec3 normal;
            int count = sscanf(line + 3, "%f %f %f", 
                             &normal.x, &normal.y, &normal.z);
            
            if (count == 3) {
                obj_data->normals[obj_data->normal_count++] = normal;
            } else {
                printf("WARNING: Line %d: Invalid normal format\n", line_num);
            }
        }
        // Парсинг граней (самая сложная часть)
        else if (strncmp(line, "f ", 2) == 0) {
            OBJ_Face face;
            memset(&face, -1, sizeof(OBJ_Face)); // -1 означает отсутствие индекса
            
            char* token = strtok(line + 2, " \t\r\n");
            int vertex_index = 0;
            
            while (token && vertex_index < 3) {
                // Формат может быть: v, v/vt, v//vn, v/vt/vn
                int v_idx = -1, vt_idx = -1, vn_idx = -1;
                
                // Пытаемся разобрать формат v/vt/vn
                if (sscanf(token, "%d/%d/%d", &v_idx, &vt_idx, &vn_idx) == 3) {
                    // формат v/vt/vn
                } 
                // Формат v//vn
                else if (sscanf(token, "%d//%d", &v_idx, &vn_idx) == 2) {
                    // формат v//vn
                }
                // Формат v/vt
                else if (sscanf(token, "%d/%d", &v_idx, &vt_idx) == 2) {
                    // формат v/vt
                }
                // Просто v
                else if (sscanf(token, "%d", &v_idx) == 1) {
                    // только вершина
                }
                else {
                    printf("WARNING: Line %d: Invalid face vertex format: %s\n", 
                           line_num, token);
                    break;
                }
                
                // Индексы в OBJ начинаются с 1, нам нужно с 0
                if (v_idx > 0) {
                    face.v_idx[vertex_index] = v_idx - 1;
                }
                if (vt_idx > 0) {
                    face.vt_idx[vertex_index] = vt_idx - 1;
                }
                if (vn_idx > 0) {
                    face.vn_idx[vertex_index] = vn_idx - 1;
                }
                
                vertex_index++;
                token = strtok(NULL, " \t\r\n");
            }
            
            // Проверяем, что получили как минимум 3 вершины
            if (vertex_index >= 3) {
                // Если это квад (4 вершины), разбиваем на 2 треугольника
                if (vertex_index == 4) {
                    // Первый треугольник (0, 1, 2)
                    obj_data->faces[obj_data->face_count++] = face;
                    
                    // Второй треугольник (0, 2, 3)
                    OBJ_Face second_face;
                    second_face.v_idx[0] = face.v_idx[0];
                    second_face.v_idx[1] = face.v_idx[2];
                    second_face.v_idx[2] = face.v_idx[3];
                    
                    second_face.vt_idx[0] = face.vt_idx[0];
                    second_face.vt_idx[1] = face.vt_idx[2];
                    second_face.vt_idx[2] = face.vt_idx[3];
                    
                    second_face.vn_idx[0] = face.vn_idx[0];
                    second_face.vn_idx[1] = face.vn_idx[2];
                    second_face.vn_idx[2] = face.vn_idx[3];
                    
                    obj_data->faces[obj_data->face_count++] = second_face;
                } else {
                    // Просто треугольник
                    obj_data->faces[obj_data->face_count++] = face;
                }
            }
        }
        // Пропускаем другие команды (mtllib, usemtl и т.д.)
        else if (strncmp(line, "mtllib", 6) == 0 ||
                 strncmp(line, "usemtl", 6) == 0 ||
                 strncmp(line, "s ", 2) == 0 ||
                 strncmp(line, "o ", 2) == 0 ||
                 strncmp(line, "g ", 2) == 0) {
            // Пропускаем, но можно добавить поддержку материалов позже
        }
    }
    
    fclose(file);
    
    printf("OBJ Parser: Loaded %s\n", filepath);
    printf("  Vertices: %d\n", obj_data->vertex_count);
    printf("  TexCoords: %d\n", obj_data->texcoord_count);
    printf("  Normals: %d\n", obj_data->normal_count);
    printf("  Faces: %d (triangles: %d)\n", 
           obj_data->face_count, obj_data->face_count);
    
    return 1;
}

// Конвертация OBJ данных в меш
Mesh* obj_data_to_mesh(OBJ_Data* data, const char* name) {
    if (data->vertex_count == 0 || data->face_count == 0) {
        printf("ERROR: No vertices or faces in OBJ data\n");
        return NULL;
    }
    
    // Создаем массив вершин для меша
    Vertex* vertices = (Vertex*)malloc(sizeof(Vertex) * data->face_count * 3);
    unsigned int* indices = (unsigned int*)malloc(sizeof(unsigned int) * data->face_count * 3);
    
    if (!vertices || !indices) {
        printf("ERROR: Memory allocation failed for mesh\n");
        if (vertices) free(vertices);
        if (indices) free(indices);
        return NULL;
    }
    
    // Заполняем вершины и индексы
    for (int face_idx = 0; face_idx < data->face_count; face_idx++) {
        OBJ_Face* face = &data->faces[face_idx];
        
        for (int vertex_idx = 0; vertex_idx < 3; vertex_idx++) {
            int vertex_index = face_idx * 3 + vertex_idx;
            
            // Заполняем позицию
            if (face->v_idx[vertex_idx] >= 0 && 
                face->v_idx[vertex_idx] < data->vertex_count) {
                OBJ_Vec3* pos = &data->vertices[face->v_idx[vertex_idx]];
                vertices[vertex_index].position = vec3_create(pos->x, pos->y, pos->z);
            } else {
                vertices[vertex_index].position = vec3_create(0, 0, 0);
            }
            
            // Заполняем нормаль
            if (face->vn_idx[vertex_idx] >= 0 && 
                face->vn_idx[vertex_idx] < data->normal_count) {
                OBJ_Vec3* norm = &data->normals[face->vn_idx[vertex_idx]];
                vertices[vertex_index].normal = vec3_create(norm->x, norm->y, norm->z);
            } else {
                // Если нет нормалей, вычисляем их позже
                vertices[vertex_index].normal = vec3_create(0, 0, 1);
            }
            
            // Заполняем текстурные координаты
            if (face->vt_idx[vertex_idx] >= 0 && 
                face->vt_idx[vertex_idx] < data->texcoord_count) {
                OBJ_Vec2* tex = &data->texcoords[face->vt_idx[vertex_idx]];
                vertices[vertex_index].texCoords = vec2_create(tex->x, tex->y);
            } else {
                vertices[vertex_index].texCoords = vec2_create(0, 0);
            }
            
            // Устанавливаем цвет по умолчанию (белый)
            vertices[vertex_index].color = vec3_create(1.0f, 1.0f, 1.0f);
            
            // Индекс
            indices[vertex_index] = vertex_index;
        }
    }
    
    // Если нормали отсутствовали, вычисляем их
    if (data->normal_count == 0) {
        printf("Calculating normals...\n");
        for (int i = 0; i < data->face_count * 3; i += 3) {
            Vec3 v0 = vertices[i].position;
            Vec3 v1 = vertices[i + 1].position;
            Vec3 v2 = vertices[i + 2].position;
            
            Vec3 edge1 = vec3_sub(&v1, &v0);
            Vec3 edge2 = vec3_sub(&v2, &v0);
            Vec3 normal = vec3_cross(&edge1, &edge2);
            normal = vec3_normalized(&normal);
            
            vertices[i].normal = normal;
            vertices[i + 1].normal = normal;
            vertices[i + 2].normal = normal;
        }
    }
    
    // Создаем меш
    Mesh* mesh = mesh_create(vertices, data->face_count * 3, 
                            indices, data->face_count * 3, name);
    
    // Освобождаем временные массивы
    free(vertices);
    free(indices);
    
    return mesh;
}

// Создание 3D объекта
Object3D* object_create(const char* name) {
    Object3D* obj = (Object3D*)malloc(sizeof(Object3D));
    if (!obj) return NULL;
    
    memset(obj, 0, sizeof(Object3D));
    
    if (name) {
        obj->name = (char*)malloc(strlen(name) + 1);
        if (obj->name) {
            strcpy(obj->name, name);
        }
    }
    
    // Инициализация значений по умолчанию
    obj->position = vec3_create(0.0f, 0.0f, 0.0f);
    obj->rotation = vec3_create(0.0f, 0.0f, 0.0f);
    obj->scale = vec3_create(1.0f, 1.0f, 1.0f);
    obj->mesh = NULL;
    obj->shader = NULL;
    obj->objFilePath = NULL;
    obj->texture = NULL;
    obj->hasTexture = 0;
    
    return obj;
}

// Уничтожение 3D объекта
void object_destroy(Object3D* obj) {
    if (!obj) return;
    
    if (obj->name) free(obj->name);
    if (obj->objFilePath) free(obj->objFilePath);
    
    // Удаляем текстуру
    object_remove_texture(obj);
    
    // Меш и шейдер не уничтожаем здесь, так как они могут использоваться другими объектами
    // (это ответственность вызывающего кода)
    
    free(obj);
}

// Загрузка объекта из OBJ файла
Object3D* object_load_from_obj(const char* filepath, const char* name, Shader* shader) {
    OBJ_Data obj_data;
    
    // Парсим OBJ файл
    if (!obj_parse_file(filepath, &obj_data)) {
        printf("ERROR: Failed to parse OBJ file: %s\n", filepath);
        return NULL;
    }
    
    // Конвертируем в меш
    Mesh* mesh = obj_data_to_mesh(&obj_data, name ? name : "OBJ_Mesh");
    if (!mesh) {
        printf("ERROR: Failed to convert OBJ data to mesh\n");
        obj_data_cleanup(&obj_data);
        return NULL;
    }
    
    // Очищаем OBJ данные
    obj_data_cleanup(&obj_data);
    
    // Создаем объект
    Object3D* obj = object_create(name);
    if (!obj) {
        mesh_destroy(mesh);
        return NULL;
    }
    
    // Сохраняем путь к файлу
    obj->objFilePath = (char*)malloc(strlen(filepath) + 1);
    if (obj->objFilePath) {
        strcpy(obj->objFilePath, filepath);
    }
    
    // Устанавливаем меш и шейдер
    obj->mesh = mesh;
    obj->shader = shader;
    
    printf("SUCCESS: Loaded OBJ object: %s from %s\n", 
           name ? name : "Unnamed", filepath);
    
    return obj;
}

// Загрузка объекта из OBJ файла с текстурой
Object3D* object_load_from_obj_with_texture(const char* filepath, const char* name, 
                                           Shader* shader, const char* texturePath) {
    // Сначала загружаем объект
    Object3D* obj = object_load_from_obj(filepath, name, shader);
    if (!obj) return NULL;
    
    // Если указан путь к текстуре, загружаем ее
    if (texturePath) {
        GLTexture* texture = Texture_Create(texturePath, true); // flipVertically = true
        if (texture) {
            object_set_texture(obj, texture);
            printf("Texture loaded for object '%s': %s\n", name, texturePath);
        } else {
            printf("WARNING: Failed to load texture for object '%s': %s\n", name, texturePath);
        }
    }
    
    return obj;
}

// Установка позиции объекта
void object_set_position(Object3D* obj, Vec3 position) {
    if (obj) {
        obj->position = position;
        if (obj->mesh) {
            mesh_set_position(obj->mesh, position);
        }
    }
}

// Установка вращения объекта
void object_set_rotation(Object3D* obj, Vec3 rotation) {
    if (obj) {
        obj->rotation = rotation;
        if (obj->mesh) {
            mesh_set_rotation(obj->mesh, rotation);
        }
    }
}

// Установка масштаба объекта
void object_set_scale(Object3D* obj, Vec3 scale) {
    if (obj) {
        obj->scale = scale;
        if (obj->mesh) {
            mesh_set_scale(obj->mesh, scale);
        }
    }
}

// Перемещение объекта
void object_translate(Object3D* obj, Vec3 translation) {
    if (obj) {
        obj->position = vec3_add(&obj->position, &translation);
        if (obj->mesh) {
            mesh_translate(obj->mesh, translation);
        }
    }
}

// Вращение объекта
void object_rotate(Object3D* obj, Vec3 rotation) {
    if (obj) {
        obj->rotation = vec3_add(&obj->rotation, &rotation);
        if (obj->mesh) {
            mesh_rotate(obj->mesh, rotation);
        }
    }
}

// Масштабирование объекта
void object_scale(Object3D* obj, Vec3 scaling) {
    if (obj) {
        obj->scale = vec3_add(&obj->scale, &scaling);
        if (obj->mesh) {
            mesh_scale_mesh(obj->mesh, scaling);
        }
    }
}

// ==================== ОПЕРАЦИИ С ТЕКСТУРОЙ ====================

// Установка текстуры для объекта
void object_set_texture(Object3D* obj, GLTexture* texture) {
    if (!obj) return;
    
    // Удаляем старую текстуру
    if (obj->texture && obj->hasTexture) {
        Texture_Destroy(obj->texture);
    }
    
    obj->texture = texture;
    obj->hasTexture = texture != NULL;
    
    // Если у объекта есть меш, устанавливаем текстуру и в меш
    if (obj->mesh && texture) {
        mesh_set_texture(obj->mesh, texture);
    }
}

// Удаление текстуры
void object_remove_texture(Object3D* obj) {
    if (!obj) return;
    
    if (obj->texture && obj->hasTexture) {
        Texture_Destroy(obj->texture);
        obj->texture = NULL;
        obj->hasTexture = 0;
    }
    
    // Также удаляем текстуру из меша
    if (obj->mesh) {
        mesh_remove_texture(obj->mesh);
    }
}

// Проверка наличия текстуры
int object_has_texture(Object3D* obj) {
    return obj ? obj->hasTexture : 0;
}

// Получение текстуры
GLTexture* object_get_texture(Object3D* obj) {
    return obj ? obj->texture : NULL;
}

// ==================== РЕНДЕРИНГ ====================

// Рендеринг объекта с его шейдером
void object_draw(Object3D* obj) {
    if (!obj || !obj->mesh) return;
    
    if (obj->shader) {
        mesh_draw(obj->mesh, obj->shader);
    } else {
        mesh_draw_simple(obj->mesh);
    }
}

// Рендеринг объекта с указанным шейдером
void object_draw_with_shader(Object3D* obj, Shader* shader) {
    if (!obj || !obj->mesh) return;
    
    if (shader) {
        mesh_draw(obj->mesh, shader);
    } else {
        object_draw(obj);
    }
}

// Вывод информации об объекте
void object_print_info(Object3D* obj) {
    if (!obj) {
        printf("Object3D: NULL\n");
        return;
    }
    
    printf("=== Object3D Info ===\n");
    printf("Name: %s\n", obj->name ? obj->name : "Unnamed");
    printf("OBJ File: %s\n", obj->objFilePath ? obj->objFilePath : "None");
    printf("Position: (%.2f, %.2f, %.2f)\n", 
           obj->position.x, obj->position.y, obj->position.z);
    printf("Rotation: (%.2f, %.2f, %.2f)\n", 
           obj->rotation.x, obj->rotation.y, obj->rotation.z);
    printf("Scale: (%.2f, %.2f, %.2f)\n", 
           obj->scale.x, obj->scale.y, obj->scale.z);
    printf("Has Texture: %s\n", obj->hasTexture ? "YES" : "NO");
    printf("Has Mesh: %s\n", obj->mesh ? "YES" : "NO");
    printf("Has Shader: %s\n", obj->shader ? "YES" : "NO");
    
    if (obj->mesh) {
        mesh_print_info(obj->mesh);
    }
    printf("====================\n");
}

#endif // OBJECT_H