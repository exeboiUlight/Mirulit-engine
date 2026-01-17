#ifndef ASSET_LOADER_H
#define ASSET_LOADER_H

#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#endif

/* Типы ресурсов */
typedef enum {
    ASSET_TYPE_UNKNOWN = 0,
    ASSET_TYPE_TEXTURE,
    ASSET_TYPE_MODEL,
    ASSET_TYPE_SHADER,
    ASSET_TYPE_SOUND,
    ASSET_TYPE_FONT
} AssetType;

/* Структура ресурса */
typedef struct {
    void* data;           // Указатель на данные
    size_t size;          // Размер данных в байтах
    AssetType type;       // Тип ресурса
    char name[256];       // Имя ресурса (опционально)
    int refCount;         // Счетчик ссылок
} Asset;

/* Таблица ресурсов */
typedef struct {
    Asset** assets;       // Массив указателей на ресурсы
    int capacity;         // Вместимость таблицы
    int count;           // Текущее количество ресурсов
} AssetTable;

/* Функции для работы с ресурсами */
int assetLoaderInit(AssetTable* table, int initialCapacity);
void assetLoaderShutdown(AssetTable* table);

/* Загрузка ресурсов из файла */
Asset* assetLoadFromFile(AssetTable* table, const char* filename, AssetType type);
/* Загрузка ресурсов из памяти */
Asset* assetLoadFromMemory(AssetTable* table, void* data, size_t size, AssetType type, const char* name);

/* Поиск ресурса по имени */
Asset* assetFind(AssetTable* table, const char* name);
/* Освобождение ресурса */
void assetUnload(AssetTable* table, Asset* asset);
/* Принудительное освобождение всех ресурсов */
void assetUnloadAll(AssetTable* table);

/* Вспомогательные функции */
size_t assetGetFileSize(const char* filename);
void* assetReadFile(const char* filename, size_t* outSize);

/* Реализация */

#ifdef ASSET_LOADER_IMPLEMENTATION

/* Инициализация таблицы ресурсов */
int assetLoaderInit(AssetTable* table, int initialCapacity) {
    if (!table) return 0;
    
    table->assets = (Asset**)calloc(initialCapacity, sizeof(Asset*));
    if (!table->assets) return 0;
    
    table->capacity = initialCapacity;
    table->count = 0;
    return 1;
}

/* Освобождение таблицы ресурсов */
void assetLoaderShutdown(AssetTable* table) {
    if (!table) return;
    
    assetUnloadAll(table);
    free(table->assets);
    table->assets = NULL;
    table->capacity = 0;
    table->count = 0;
}

/* Получение размера файла */
size_t assetGetFileSize(const char* filename) {
#ifdef _WIN32
    WIN32_FILE_ATTRIBUTE_DATA fileInfo;
    if (GetFileAttributesExA(filename, GetFileExInfoStandard, &fileInfo)) {
        return ((size_t)fileInfo.nFileSizeHigh << 32) | fileInfo.nFileSizeLow;
    }
    return 0;
#else
    struct stat st;
    if (stat(filename, &st) == 0) {
        return (size_t)st.st_size;
    }
    return 0;
#endif
}

/* Чтение файла в память */
void* assetReadFile(const char* filename, size_t* outSize) {
    FILE* file = fopen(filename, "rb");
    if (!file) return NULL;
    
    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    void* buffer = malloc(fileSize + 1);
    if (!buffer) {
        fclose(file);
        return NULL;
    }
    
    size_t bytesRead = fread(buffer, 1, fileSize, file);
    fclose(file);
    
    if (bytesRead != (size_t)fileSize) {
        free(buffer);
        return NULL;
    }
    
    ((char*)buffer)[fileSize] = '\0'; // Для текстовых файлов
    
    if (outSize) *outSize = bytesRead;
    return buffer;
}

/* Загрузка ресурса из файла */
Asset* assetLoadFromFile(AssetTable* table, const char* filename, AssetType type) {
    size_t size;
    void* data = assetReadFile(filename, &size);
    if (!data) return NULL;
    
    Asset* asset = assetLoadFromMemory(table, data, size, type, filename);
    free(data); // Копия создается в LoadFromMemory
    
    return asset;
}

/* Загрузка ресурса из памяти */
Asset* assetLoadFromMemory(AssetTable* table, void* data, size_t size, AssetType type, const char* name) {
    if (!table || !data || size == 0) return NULL;
    
    /* Проверяем, есть ли уже такой ресурс */
    if (name) {
        Asset* existing = assetFind(table, name);
        if (existing) {
            existing->refCount++;
            return existing;
        }
    }
    
    /* Проверяем, нужно ли расширять таблицу */
    if (table->count >= table->capacity) {
        int newCapacity = table->capacity * 2;
        Asset** newAssets = (Asset**)realloc(table->assets, newCapacity * sizeof(Asset*));
        if (!newAssets) return NULL;
        
        table->assets = newAssets;
        table->capacity = newCapacity;
    }
    
    /* Создаем новый ресурс */
    Asset* asset = (Asset*)calloc(1, sizeof(Asset));
    if (!asset) return NULL;
    
    asset->data = malloc(size);
    if (!asset->data) {
        free(asset);
        return NULL;
    }
    
    memcpy(asset->data, data, size);
    asset->size = size;
    asset->type = type;
    asset->refCount = 1;
    
    if (name) {
        strncpy(asset->name, name, sizeof(asset->name) - 1);
    }
    
    /* Добавляем в таблицу */
    table->assets[table->count++] = asset;
    return asset;
}

/* Поиск ресурса по имени */
Asset* assetFind(AssetTable* table, const char* name) {
    if (!table || !name) return NULL;
    
    for (int i = 0; i < table->count; i++) {
        Asset* asset = table->assets[i];
        if (asset && strcmp(asset->name, name) == 0) {
            return asset;
        }
    }
    
    return NULL;
}

/* Освобождение ресурса */
void assetUnload(AssetTable* table, Asset* asset) {
    if (!table || !asset) return;
    
    asset->refCount--;
    if (asset->refCount > 0) return;
    
    /* Удаляем из таблицы */
    for (int i = 0; i < table->count; i++) {
        if (table->assets[i] == asset) {
            /* Сдвигаем оставшиеся элементы */
            for (int j = i; j < table->count - 1; j++) {
                table->assets[j] = table->assets[j + 1];
            }
            table->count--;
            break;
        }
    }
    
    /* Освобождаем память */
    if (asset->data) free(asset->data);
    free(asset);
}

/* Принудительное освобождение всех ресурсов */
void assetUnloadAll(AssetTable* table) {
    if (!table) return;
    
    for (int i = 0; i < table->count; i++) {
        Asset* asset = table->assets[i];
        if (asset) {
            if (asset->data) free(asset->data);
            free(asset);
        }
    }
    
    table->count = 0;
}

#endif /* ASSET_LOADER_IMPLEMENTATION */

#endif /* ASSET_LOADER_H */