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

// Структуры для C интерфейса (opaque pointers)
typedef struct MirulitWindow MirulitWindow;
typedef struct MirulitEntity MirulitEntity;

// Векторные структуры для C
typedef struct {
    float x, y;
} MirulitVector2f;

typedef struct {
    float x, y, z;
} MirulitVector3f;

// Трансформация
typedef struct {
    MirulitVector2f position;
    MirulitVector2f scale;
    float rotate;
} MirulitTransform;

// Инициализация и очистка
MIRULIT_API void Mirulit_Init(void);
MIRULIT_API void Mirulit_Shutdown(void);

// Окно
MIRULIT_API MirulitWindow* Mirulit_CreateWindow(int width, int height, const char* title);
MIRULIT_API void Mirulit_DestroyWindow(MirulitWindow* window);
MIRULIT_API int Mirulit_WindowShouldClose(MirulitWindow* window);
MIRULIT_API void Mirulit_WindowUpdate(MirulitWindow* window);
MIRULIT_API void Mirulit_WindowUpdateCallback(MirulitWindow* window, void (*callback)(void));
MIRULIT_API void Mirulit_WindowSetClearColor(float r, float g, float b, float a);
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

// Утилиты для работы с векторами
MIRULIT_API MirulitVector2f Mirulit_Vector2f(float x, float y);
MIRULIT_API MirulitVector3f Mirulit_Vector3f(float x, float y, float z);
MIRULIT_API float Mirulit_Vector2fLength(MirulitVector2f v);
MIRULIT_API MirulitVector2f Mirulit_Vector2fNormalize(MirulitVector2f v);
MIRULIT_API float Mirulit_Vector2fDot(MirulitVector2f a, MirulitVector2f b);
MIRULIT_API MirulitVector2f Mirulit_Vector2fAdd(MirulitVector2f a, MirulitVector2f b);
MIRULIT_API MirulitVector2f Mirulit_Vector2fSub(MirulitVector2f a, MirulitVector2f b);
MIRULIT_API MirulitVector2f Mirulit_Vector2fMul(MirulitVector2f v, float scalar);

// Время
MIRULIT_API float Mirulit_GetTime(void);
MIRULIT_API void Mirulit_WaitEvents(void);
MIRULIT_API void Mirulit_PollEvents(void);

// Ввод
MIRULIT_API int Mirulit_GetKey(MirulitWindow* window, int key);
MIRULIT_API int Mirulit_GetMouseButton(MirulitWindow* window, int button);
MIRULIT_API void Mirulit_GetMousePos(MirulitWindow* window, double* x, double* y);

#ifdef __cplusplus
}
#endif