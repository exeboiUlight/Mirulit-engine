// texture.h
#ifndef TEXTURE_H
#define TEXTURE_H

#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

// ==================== СТРУКТУРА ТЕКСТУРЫ ====================
// Переименуем структуру, чтобы избежать конфликта с mesh.h
typedef struct GLTexture {
    GLuint id;
    char* filePath;
} GLTexture;

// Структура для кэширования текстур
typedef struct TextureCacheEntry {
    char* filePath;
    GLuint textureID;
    struct TextureCacheEntry* next;
} TextureCacheEntry;

// ==================== ПУБЛИЧНЫЕ ФУНКЦИИ ====================

// Установка режима отладки
void Texture_SetDebugMode(bool mode);

// Создание текстуры из файла
GLTexture* Texture_Create(const char* file, bool flipVertically);

// Создание текстуры из существующего ID
GLTexture* Texture_CreateFromID(GLuint id);

// Создание пустой текстуры
GLTexture* Texture_CreateEmpty(void);

// Привязка текстуры
void Texture_Bind(const GLTexture* texture, GLenum textureUnit);

// Получение ID текстуры
GLuint Texture_GetID(const GLTexture* texture);

// Проверка валидности текстуры
bool Texture_IsValid(const GLTexture* texture);

// Уничтожение текстуры (не удаляет из OpenGL)
void Texture_Destroy(GLTexture* texture);

// Предзагрузка текстуры (возвращает только ID)
GLuint Texture_Preload(const char* filePath, bool flipVertically);

// Получение текстуры из кэша по пути
GLuint Texture_GetCached(const char* filePath);

// Отладка кэша текстур
void Texture_DebugCache(void);

// Очистка всех текстур
void Texture_CleanupAll(void);

// Установка параметров текстуры
void Texture_SetParameters(GLuint textureID, 
                          GLint minFilter, 
                          GLint magFilter, 
                          GLint wrapS, 
                          GLint wrapT);

// Создание текстуры из данных в памяти
GLTexture* Texture_CreateFromMemory(unsigned char* data, 
                                  int width, 
                                  int height, 
                                  int channels,
                                  const char* debugName);

// ==================== ГЛОБАЛЬНЫЕ ПЕРЕМЕННЫЕ ====================
static TextureCacheEntry* textureCache = NULL;
static GLuint* loadedTextures = NULL;
static size_t loadedTexturesCount = 0;
static size_t loadedTexturesCapacity = 0;
static bool textureDebugMode = false;

// ==================== ВСПОМОГАТЕЛЬНЫЕ ФУНКЦИИ ====================
static char* duplicate_string(const char* src) {
    if (!src) return NULL;
    char* dst = (char*)malloc(strlen(src) + 1);
    if (dst) strcpy(dst, src);
    return dst;
}

static void add_to_cache(const char* filePath, GLuint textureID) {
    TextureCacheEntry* entry = (TextureCacheEntry*)malloc(sizeof(TextureCacheEntry));
    if (!entry) return;
    
    entry->filePath = duplicate_string(filePath);
    entry->textureID = textureID;
    entry->next = textureCache;
    textureCache = entry;
}

static void add_to_loaded_textures(GLuint textureID) {
    if (loadedTexturesCount >= loadedTexturesCapacity) {
        loadedTexturesCapacity = loadedTexturesCapacity == 0 ? 16 : loadedTexturesCapacity * 2;
        loadedTextures = (GLuint*)realloc(loadedTextures, loadedTexturesCapacity * sizeof(GLuint));
    }
    loadedTextures[loadedTexturesCount++] = textureID;
}

// ==================== ПУБЛИЧНЫЕ ФУНКЦИИ ====================

// Установка режима отладки
void Texture_SetDebugMode(bool mode) {
    textureDebugMode = mode;
}

// Создание текстуры из файла
GLTexture* Texture_Create(const char* file, bool flipVertically) {
    GLTexture* texture = (GLTexture*)malloc(sizeof(GLTexture));
    if (!texture) return NULL;
    
    texture->id = 0;
    texture->filePath = duplicate_string(file);
    
    // Проверяем кэш
    TextureCacheEntry* current = textureCache;
    while (current) {
        if (strcmp(current->filePath, file) == 0) {
            texture->id = current->textureID;
            if (textureDebugMode) {
                printf("Using cached texture: %s (ID: %u)\n", file, texture->id);
            }
            return texture;
        }
        current = current->next;
    }
    
    // Загружаем изображение
    int width, height, channels;
    stbi_set_flip_vertically_on_load(flipVertically);
    unsigned char* data = stbi_load(file, &width, &height, &channels, STBI_rgb_alpha);
    
    if (!data) {
        if (textureDebugMode) {
            printf("Error loading image: %s\n", file);
        }
        free(texture->filePath);
        free(texture);
        return NULL;
    }
    
    // Генерируем текстуру
    glGenTextures(1, &texture->id);
    glBindTexture(GL_TEXTURE_2D, texture->id);
    
    // Загружаем данные текстуры
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 
                 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    
    // Устанавливаем параметры текстуры
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    // Генерируем mipmaps
    glGenerateMipmap(GL_TEXTURE_2D);
    
    // Освобождаем данные изображения
    stbi_image_free(data);
    
    // Добавляем в кэш и в массив загруженных текстур
    add_to_cache(file, texture->id);
    add_to_loaded_textures(texture->id);
    
    if (textureDebugMode) {
        printf("SUCCESS: Loaded texture: %s (ID: %u, Size: %dx%d, Channels: %d, Flipped: %s)\n", 
               file, texture->id, width, height, channels, 
               flipVertically ? "YES" : "NO");
    }
    
    return texture;
}

// Создание текстуры из существующего ID
GLTexture* Texture_CreateFromID(GLuint id) {
    GLTexture* texture = (GLTexture*)malloc(sizeof(GLTexture));
    if (!texture) return NULL;
    
    texture->id = id;
    texture->filePath = NULL;
    
    return texture;
}

// Создание пустой текстуры
GLTexture* Texture_CreateEmpty(void) {
    GLTexture* texture = (GLTexture*)malloc(sizeof(GLTexture));
    if (!texture) return NULL;
    
    texture->id = 0;
    texture->filePath = NULL;
    
    return texture;
}

// Привязка текстуры
void Texture_Bind(const GLTexture* texture, GLenum textureUnit) {
    if (texture && texture->id != 0) {
        glActiveTexture(textureUnit);
        glBindTexture(GL_TEXTURE_2D, texture->id);
    }
}

// Получение ID текстуры
GLuint Texture_GetID(const GLTexture* texture) {
    return texture ? texture->id : 0;
}

// Проверка валидности текстуры
bool Texture_IsValid(const GLTexture* texture) {
    return texture && texture->id != 0;
}

// Уничтожение текстуры (не удаляет из OpenGL)
void Texture_Destroy(GLTexture* texture) {
    if (texture) {
        free(texture->filePath);
        free(texture);
    }
}

// Предзагрузка текстуры (возвращает только ID)
GLuint Texture_Preload(const char* filePath, bool flipVertically) {
    GLTexture* texture = Texture_Create(filePath, flipVertically);
    GLuint id = texture ? texture->id : 0;
    Texture_Destroy(texture);
    return id;
}

// Получение текстуры из кэша по пути
GLuint Texture_GetCached(const char* filePath) {
    TextureCacheEntry* current = textureCache;
    while (current) {
        if (strcmp(current->filePath, filePath) == 0) {
            return current->textureID;
        }
        current = current->next;
    }
    return 0;
}

// Отладка кэша текстур
void Texture_DebugCache(void) {
    if (!textureDebugMode) return;
    
    printf("=== Texture Cache Debug ===\n");
    
    int count = 0;
    TextureCacheEntry* current = textureCache;
    while (current) {
        count++;
        current = current->next;
    }
    
    printf("Cached textures: %d\n", count);
    
    current = textureCache;
    while (current) {
        printf("  %s -> ID: %u\n", current->filePath, current->textureID);
        current = current->next;
    }
    printf("=========================\n");
}

// Очистка всех текстур
void Texture_CleanupAll(void) {
    // Очищаем кэш
    TextureCacheEntry* current = textureCache;
    while (current) {
        TextureCacheEntry* next = current->next;
        free(current->filePath);
        free(current);
        current = next;
    }
    textureCache = NULL;
    
    // Удаляем текстуры из OpenGL
    if (loadedTexturesCount > 0) {
        glDeleteTextures(loadedTexturesCount, loadedTextures);
        loadedTexturesCount = 0;
    }
    
    free(loadedTextures);
    loadedTextures = NULL;
    loadedTexturesCapacity = 0;
}

// Установка параметров текстуры
void Texture_SetParameters(GLuint textureID, 
                          GLint minFilter, 
                          GLint magFilter, 
                          GLint wrapS, 
                          GLint wrapT) {
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapS);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapT);
    glBindTexture(GL_TEXTURE_2D, 0);
}

// Создание текстуры из данных в памяти
GLTexture* Texture_CreateFromMemory(unsigned char* data, 
                                  int width, 
                                  int height, 
                                  int channels,
                                  const char* debugName) {
    GLTexture* texture = (GLTexture*)malloc(sizeof(GLTexture));
    if (!texture) return NULL;
    
    texture->id = 0;
    texture->filePath = duplicate_string(debugName);
    
    // Генерируем текстуру
    glGenTextures(1, &texture->id);
    glBindTexture(GL_TEXTURE_2D, texture->id);
    
    // Определяем формат
    GLenum format = GL_RGB;
    if (channels == 4) format = GL_RGBA;
    else if (channels == 1) format = GL_RED;
    
    // Загружаем данные текстуры
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 
                 0, format, GL_UNSIGNED_BYTE, data);
    
    // Устанавливаем параметры по умолчанию
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    // Добавляем в массив загруженных текстур
    add_to_loaded_textures(texture->id);
    
    if (textureDebugMode) {
        printf("SUCCESS: Created texture from memory: %s (ID: %u, Size: %dx%d, Channels: %d)\n", 
               debugName, texture->id, width, height, channels);
    }
    
    return texture;
}
#endif