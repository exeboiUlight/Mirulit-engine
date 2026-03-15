// mirulit_dll.cpp
#define MIRULIT_EXPORTS
#define STB_IMAGE_IMPLEMENTATION

#include <stb_image/stb_image.h>
#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <mutex>

// Подключаем оригинальные заголовки
#include "../core/Entity.h"
#include "../core/graphics/Window.h"
#include "../gui/hub.h"

// Подключаем C интерфейс
#include "mirulit_c.h"

// Глобальное состояние
static bool g_Initialized = false;
static std::mutex g_Mutex;

// ==================== Классы-обертки для C интерфейса ====================

struct MirulitWindow {
    std::unique_ptr<MirulitEngine::Window> impl;
    void (*updateCallback)(void) = nullptr;
    
    MirulitWindow(int width, int height, const char* title) 
        : impl(std::make_unique<MirulitEngine::Window>(width, height, title)) {}
};

struct MirulitEntity {
    std::unique_ptr<MirulitEngine::Entity> impl;
    MirulitMath::Vector2f lastPosition;
    MirulitMath::Vector2f lastScale;
    float lastRotate;
    
    MirulitEntity(MirulitMath::Vector2f position, MirulitMath::Vector2f scale, float rotate)
        : impl(std::make_unique<MirulitEngine::Entity>(position, scale, rotate))
        , lastPosition(position), lastScale(scale), lastRotate(rotate) {}
};

struct MirulitHub {
    std::unique_ptr<MirulitGUI::Hub> impl;
    std::vector<MirulitProjectInfo> recentProjectsCache;
    std::vector<std::string> stringStorage;
    
    MirulitHub() : impl(std::make_unique<MirulitGUI::Hub>()) {
        updateRecentProjectsCache();
    }
    
    void updateRecentProjectsCache() {
        // Очищаем кеш
        recentProjectsCache.clear();
        stringStorage.clear();
        
        // Здесь нужно получить последние проекты из Hub
        // Это зависит от реализации Hub::GetRecentProjects()
        // Для примера добавим заглушку
    }
};

// ==================== Реализация C интерфейса ====================

extern "C" {

// Инициализация и очистка
MIRULIT_API void Mirulit_Init(void) {
    std::lock_guard<std::mutex> lock(g_Mutex);
    if (!g_Initialized) {
        if (!glfwInit()) {
            std::cerr << "Failed to initialize GLFW" << std::endl;
            return;
        }
        g_Initialized = true;
    }
}

MIRULIT_API void Mirulit_Shutdown(void) {
    std::lock_guard<std::mutex> lock(g_Mutex);
    if (g_Initialized) {
        glfwTerminate();
        g_Initialized = false;
    }
}

// Окно
MIRULIT_API MirulitWindow* Mirulit_CreateWindow(int width, int height, const char* title) {
    std::lock_guard<std::mutex> lock(g_Mutex);
    try {
        Mirulit_Init(); // Убеждаемся, что GLFW инициализирован
        return new MirulitWindow(width, height, title);
    } catch (const std::exception& e) {
        std::cerr << "Failed to create window: " << e.what() << std::endl;
        return nullptr;
    }
}

MIRULIT_API void Mirulit_DestroyWindow(MirulitWindow* window) {
    std::lock_guard<std::mutex> lock(g_Mutex);
    delete window;
}

MIRULIT_API int Mirulit_WindowShouldClose(MirulitWindow* window) {
    return window && window->impl ? window->impl->shouldClose() : 1;
}

MIRULIT_API void Mirulit_WindowUpdate(MirulitWindow* window) {
    if (window && window->impl) {
        window->impl->update();
    }
}

MIRULIT_API void Mirulit_WindowUpdateCallback(MirulitWindow* window, void (*callback)(void)) {
    if (window && window->impl) {
        window->updateCallback = callback;
        window->impl->update([window]() {
            if (window->updateCallback) {
                window->updateCallback();
            }
        });
    }
}

MIRULIT_API void* Mirulit_GetGLFWwindow(MirulitWindow* window) {
    return window && window->impl ? window->impl->getGLFWwindow() : nullptr;
}

MIRULIT_API int Mirulit_GetWindowWidth(MirulitWindow* window) {
    return window && window->impl ? window->impl->getWidth() : 0;
}

MIRULIT_API int Mirulit_GetWindowHeight(MirulitWindow* window) {
    return window && window->impl ? window->impl->getHeight() : 0;
}

// Сущность
MIRULIT_API MirulitEntity* Mirulit_CreateEntity(MirulitVector2f position, MirulitVector2f scale, float rotate) {
    try {
        MirulitMath::Vector2f pos(position.x, position.y);
        MirulitMath::Vector2f sc(scale.x, scale.y);
        return new MirulitEntity(pos, sc, rotate);
    } catch (const std::exception& e) {
        std::cerr << "Failed to create entity: " << e.what() << std::endl;
        return nullptr;
    }
}

MIRULIT_API void Mirulit_DestroyEntity(MirulitEntity* entity) {
    delete entity;
}

MIRULIT_API void Mirulit_EntitySetRect(MirulitEntity* entity) {
    if (entity && entity->impl) {
        entity->impl->Rect();
    }
}

MIRULIT_API void Mirulit_EntitySetPolygon(MirulitEntity* entity, const float* vertices, int vertexCount) {
    if (entity && entity->impl && vertices && vertexCount > 0) {
        std::vector<float> verts(vertices, vertices + vertexCount * 2);
        entity->impl->Poligon(verts);
    }
}

MIRULIT_API void Mirulit_EntitySetCircle(MirulitEntity* entity, int segments) {
    if (entity && entity->impl) {
        entity->impl->Circle(segments);
    }
}

MIRULIT_API void Mirulit_EntityInitRender(MirulitEntity* entity, const char* texturePath) {
    if (entity && entity->impl && texturePath) {
        entity->impl->initRender(texturePath);
    }
}

MIRULIT_API void Mirulit_EntityDraw(MirulitEntity* entity) {
    if (entity && entity->impl) {
        entity->impl->draw();
    }
}

MIRULIT_API void Mirulit_EntitySetPosition(MirulitEntity* entity, MirulitVector2f position) {
    if (entity && entity->impl) {
        entity->lastPosition = MirulitMath::Vector2f(position.x, position.y);
        entity->impl->transform.position = entity->lastPosition;
        // Обновляем шейдер если нужно
        if (entity->impl->shader) {
            entity->impl->SetPosition(entity->lastPosition);
        }
    }
}

MIRULIT_API void Mirulit_EntitySetRotation(MirulitEntity* entity, float rotation) {
    if (entity && entity->impl) {
        entity->lastRotate = rotation;
        entity->impl->transform.rotate = rotation;
        if (entity->impl->shader) {
            entity->impl->SetRotate(rotation);
        }
    }
}

MIRULIT_API void Mirulit_EntitySetScale(MirulitEntity* entity, MirulitVector2f scale) {
    if (entity && entity->impl) {
        entity->lastScale = MirulitMath::Vector2f(scale.x, scale.y);
        entity->impl->transform.scale = entity->lastScale;
        if (entity->impl->shader) {
            entity->impl->SetScale(entity->lastScale);
        }
    }
}

MIRULIT_API MirulitTransform Mirulit_EntityGetTransform(MirulitEntity* entity) {
    MirulitTransform transform = {{0,0}, {1,1}, 0};
    if (entity && entity->impl) {
        transform.position.x = entity->impl->transform.position.x;
        transform.position.y = entity->impl->transform.position.y;
        transform.scale.x = entity->impl->transform.scale.x;
        transform.scale.y = entity->impl->transform.scale.y;
        transform.rotate = entity->impl->transform.rotate;
    }
    return transform;
}

// GUI Hub
MIRULIT_API MirulitHub* Mirulit_CreateHub(void) {
    try {
        return new MirulitHub();
    } catch (const std::exception& e) {
        std::cerr << "Failed to create hub: " << e.what() << std::endl;
        return nullptr;
    }
}

MIRULIT_API void Mirulit_DestroyHub(MirulitHub* hub) {
    delete hub;
}

MIRULIT_API void Mirulit_HubRender(MirulitHub* hub) {
    if (hub && hub->impl) {
        hub->impl->Render();
    }
}

MIRULIT_API int Mirulit_HubIsLoading(MirulitHub* hub) {
    return hub && hub->impl ? hub->impl->IsLoading() : 0;
}

MIRULIT_API void Mirulit_HubSetDarkTheme(int dark) {
    // Эта функция должна быть добавлена в класс Hub
    // Для примера используем существующую
    if (dark) {
        MirulitGUI::SetDarkStyle();
    }
}

MIRULIT_API void Mirulit_HubSetUIScale(float scale) {
    ImGui::GetIO().FontGlobalScale = scale;
}

MIRULIT_API void Mirulit_HubSetDefaultAuthor(const char* author) {
    // Нужно добавить в класс Hub
}

MIRULIT_API void Mirulit_HubOpenProject(MirulitHub* hub, const char* projectPath) {
    if (hub && hub->impl && projectPath) {
        // Нужно добавить метод OpenProject в класс Hub
        // hub->impl->OpenProject(projectPath);
    }
}

MIRULIT_API void Mirulit_HubCreateProject(MirulitHub* hub, const char* name, const char* path, const char* author) {
    if (hub && hub->impl && name && path) {
        // Нужно добавить метод CreateProject в класс Hub
    }
}

MIRULIT_API const MirulitProjectInfo* Mirulit_HubGetRecentProjects(MirulitHub* hub, int* count) {
    if (!hub || !count) return nullptr;
    hub->updateRecentProjectsCache();
    *count = static_cast<int>(hub->recentProjectsCache.size());
    return hub->recentProjectsCache.data();
}

// Вспомогательные функции
MIRULIT_API MirulitVector2f Mirulit_Vector2f(float x, float y) {
    MirulitVector2f v = {x, y};
    return v;
}

MIRULIT_API MirulitVector3f Mirulit_Vector3f(float x, float y, float z) {
    MirulitVector3f v = {x, y, z};
    return v;
}

MIRULIT_API float Mirulit_Vector2fLength(MirulitVector2f v) {
    return std::sqrt(v.x * v.x + v.y * v.y);
}

MIRULIT_API MirulitVector2f Mirulit_Vector2fNormalize(MirulitVector2f v) {
    float len = std::sqrt(v.x * v.x + v.y * v.y);
    if (len > 0) {
        v.x /= len;
        v.y /= len;
    }
    return v;
}

} // extern "C"

// ==================== Точка входа для DLL ====================

#ifdef _WIN32
#include <windows.h>

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call) {
        case DLL_PROCESS_ATTACH:
            // Инициализация при загрузке DLL
            break;
        case DLL_THREAD_ATTACH:
        case DLL_THREAD_DETACH:
        case DLL_PROCESS_DETACH:
            // Очистка при выгрузке DLL
            if (g_Initialized) {
                glfwTerminate();
                g_Initialized = false;
            }
            break;
    }
    return TRUE;
}

#endif