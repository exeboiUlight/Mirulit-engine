// mirulit_c.h
#pragma once

#ifdef _WIN32
    #ifdef MIRULIT_EXPORTS
        #define MIRULIT_API __declspec(dllexport)
    #else
        #define MIRULIT_API __declspec(dllimport)
    #endif
#else
    #define MIRULIT_API __attribute__((visibility("default")))
#endif

#ifdef __cplusplus
extern "C" {
#endif

// Структуры для C интерфейса
typedef struct MirulitWindow MirulitWindow;
typedef struct MirulitEntity MirulitEntity;
typedef struct MirulitHub MirulitHub;

// Векторные структуры для C
typedef struct {
    float x, y;
} MirulitVector2f;

typedef struct {
    float x, y, z;
} MirulitVector3f;

// Преобразование структуры
typedef struct {
    MirulitVector2f position;
    MirulitVector2f scale;
    float rotate;
} MirulitTransform;

// Информация о проекте
typedef struct {
    const char* name;
    const char* path;
    const char* lastOpened;
    const char* author;
    const char* createdAt;
} MirulitProjectInfo;

// Инициализация и очистка
MIRULIT_API void Mirulit_Init(void);
MIRULIT_API void Mirulit_Shutdown(void);

// Окно
MIRULIT_API MirulitWindow* Mirulit_CreateWindow(int width, int height, const char* title);
MIRULIT_API void Mirulit_DestroyWindow(MirulitWindow* window);
MIRULIT_API int Mirulit_WindowShouldClose(MirulitWindow* window);
MIRULIT_API void Mirulit_WindowUpdate(MirulitWindow* window);
MIRULIT_API void Mirulit_WindowUpdateCallback(MirulitWindow* window, void (*callback)(void));
MIRULIT_API void* Mirulit_GetGLFWwindow(MirulitWindow* window);
MIRULIT_API int Mirulit_GetWindowWidth(MirulitWindow* window);
MIRULIT_API int Mirulit_GetWindowHeight(MirulitWindow* window);

// Сущность
MIRULIT_API MirulitEntity* Mirulit_CreateEntity(MirulitVector2f position, MirulitVector2f scale, float rotate);
MIRULIT_API void Mirulit_DestroyEntity(MirulitEntity* entity);
MIRULIT_API void Mirulit_EntitySetRect(MirulitEntity* entity);
MIRULIT_API void Mirulit_EntitySetPolygon(MirulitEntity* entity, const float* vertices, int vertexCount);
MIRULIT_API void Mirulit_EntitySetCircle(MirulitEntity* entity, int segments);
MIRULIT_API void Mirulit_EntityInitRender(MirulitEntity* entity, const char* texturePath);
MIRULIT_API void Mirulit_EntityDraw(MirulitEntity* entity);
MIRULIT_API void Mirulit_EntitySetPosition(MirulitEntity* entity, MirulitVector2f position);
MIRULIT_API void Mirulit_EntitySetRotation(MirulitEntity* entity, float rotation);
MIRULIT_API void Mirulit_EntitySetScale(MirulitEntity* entity, MirulitVector2f scale);
MIRULIT_API MirulitTransform Mirulit_EntityGetTransform(MirulitEntity* entity);

// GUI Hub
MIRULIT_API MirulitHub* Mirulit_CreateHub(void);
MIRULIT_API void Mirulit_DestroyHub(MirulitHub* hub);
MIRULIT_API void Mirulit_HubRender(MirulitHub* hub);
MIRULIT_API int Mirulit_HubIsLoading(MirulitHub* hub);
MIRULIT_API void Mirulit_HubSetDarkTheme(int dark);
MIRULIT_API void Mirulit_HubSetUIScale(float scale);
MIRULIT_API void Mirulit_HubSetDefaultAuthor(const char* author);
MIRULIT_API void Mirulit_HubOpenProject(MirulitHub* hub, const char* projectPath);
MIRULIT_API void Mirulit_HubCreateProject(MirulitHub* hub, const char* name, const char* path, const char* author);
MIRULIT_API const MirulitProjectInfo* Mirulit_HubGetRecentProjects(MirulitHub* hub, int* count);

// Вспомогательные функции
MIRULIT_API MirulitVector2f Mirulit_Vector2f(float x, float y);
MIRULIT_API MirulitVector3f Mirulit_Vector3f(float x, float y, float z);
MIRULIT_API float Mirulit_Vector2fLength(MirulitVector2f v);
MIRULIT_API MirulitVector2f Mirulit_Vector2fNormalize(MirulitVector2f v);

#ifdef __cplusplus
}
#endif