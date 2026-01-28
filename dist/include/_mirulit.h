#ifndef MIRULIT_H
#define MIRULIT_H

// ==================== КОНФИГУРАЦИЯ ====================
// Раскомментируйте для дополнительных возможностей
// #define MIRULIT_ENABLE_SDL_IMAGE   // Требует SDL3_image
// #define MIRULIT_ENABLE_SDL_TTF     // Требует SDL3_ttf
// #define MIRULIT_ENABLE_PHYSICS     // Простая физика

#include <SDL3/SDL.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#ifdef MIRULIT_ENABLE_SDL_IMAGE
#include <SDL3_image/SDL_image.h>
#endif

#ifdef MIRULIT_ENABLE_SDL_TTF
#include <SDL3_ttf/SDL_ttf.h>
#endif

// ==================== ВЕРСИЯ И НАСТРОЙКИ ====================
#define MIRULIT_VERSION "1.0.0"
#define MIRULIT_MAX_ENTITIES 1024
#define MIRULIT_MAX_KEYS 512
#define MIRULIT_MAX_BUTTONS 8
#define MIRULIT_MAX_PARTICLES 1000
#define MIRULIT_DEFAULT_FPS 60

// ==================== ЦВЕТА (RGBA) ====================
#define MIR_COLOR_WHITE      (MIR_Color){255, 255, 255, 255}
#define MIR_COLOR_BLACK      (MIR_Color){0, 0, 0, 255}
#define MIR_COLOR_RED        (MIR_Color){255, 50, 50, 255}
#define MIR_COLOR_GREEN      (MIR_Color){50, 255, 50, 255}
#define MIR_COLOR_BLUE       (MIR_Color){50, 100, 255, 255}
#define MIR_COLOR_YELLOW     (MIR_Color){255, 255, 50, 255}
#define MIR_COLOR_CYAN       (MIR_Color){50, 255, 255, 255}
#define MIR_COLOR_MAGENTA    (MIR_Color){255, 50, 255, 255}
#define MIR_COLOR_ORANGE     (MIR_Color){255, 150, 50, 255}
#define MIR_COLOR_PURPLE     (MIR_Color){150, 50, 255, 255}
#define MIR_COLOR_GRAY       (MIR_Color){128, 128, 128, 255}
#define MIR_COLOR_DARKGRAY   (MIR_Color){64, 64, 64, 255}
#define MIR_COLOR_LIGHTGRAY  (MIR_Color){192, 192, 192, 255}
#define MIR_COLOR_BACKGROUND (MIR_Color){30, 30, 40, 255}

// ==================== СТРУКТУРЫ ДАННЫХ ====================

// Базовые типы
typedef struct { float x, y; } MIR_Vec2;
typedef struct { float x, y, z; } MIR_Vec3;
typedef struct { uint8_t r, g, b, a; } MIR_Color;
typedef struct { float x, y, w, h; } MIR_Rect;
typedef struct { float r, g, b, a; } MIR_Colorf;

// Компоненты сущности
typedef struct MIR_Transform {
    MIR_Vec2 position;
    MIR_Vec2 scale;
    float rotation;
    MIR_Vec2 velocity;
    MIR_Vec2 acceleration;
} MIR_Transform;

typedef struct MIR_Sprite {
    SDL_Texture* texture;
    MIR_Rect source_rect;
    MIR_Color color;
    int z_index;
    bool flip_x;
    bool flip_y;
} MIR_Sprite;

typedef struct MIR_Collider {
    MIR_Rect bounds;
    bool is_trigger;
    bool enabled;
    void (*on_collision)(struct MIR_Entity*, struct MIR_Entity*);
} MIR_Collider;

// Сущность (Entity)
typedef struct MIR_Entity {
    int id;
    char tag[32];
    bool active;
    bool visible;
    bool persistent;
    
    MIR_Transform transform;
    MIR_Sprite sprite;
    MIR_Collider collider;
    
    void* components[10];
    int component_count;
    
    void (*update)(struct MIR_Entity*, float);
    void (*draw)(struct MIR_Entity*);
    void (*on_click)(struct MIR_Entity*);
    void (*on_destroy)(struct MIR_Entity*);
    
    struct MIR_Entity* parent;
    struct MIR_Entity* children[20];
    int child_count;
    
    void* user_data;
} MIR_Entity;

// Частица
typedef struct {
    MIR_Vec2 position;
    MIR_Vec2 velocity;
    MIR_Vec2 acceleration;
    MIR_Color color;
    float size;
    float life;
    float max_life;
    bool active;
} MIR_Particle;

// Камера
typedef struct {
    MIR_Vec2 position;
    float zoom;
    float rotation;
    MIR_Vec2 target;
    float smooth_speed;
    MIR_Rect bounds;
} MIR_Camera;

// Анимация
typedef struct {
    MIR_Rect frames[32];
    int frame_count;
    int current_frame;
    float frame_time;
    float timer;
    bool loop;
    bool playing;
} MIR_Animation;

// Основной движок
typedef struct {
    // SDL
    SDL_Window* window;
    SDL_Renderer* renderer;
    
    // Состояние
    int width;
    int height;
    char title[128];
    bool running;
    bool paused;
    
    // Сущности
    MIR_Entity* entities[MIRULIT_MAX_ENTITIES];
    int entity_count;
    int next_id;
    
    // Частицы
    MIR_Particle particles[MIRULIT_MAX_PARTICLES];
    
    // Камера
    MIR_Camera camera;
    
    // Ввод
    bool keys[MIRULIT_MAX_KEYS];
    bool keys_down[MIRULIT_MAX_KEYS];
    bool keys_up[MIRULIT_MAX_KEYS];
    
    bool mouse_buttons[MIRULIT_MAX_BUTTONS];
    bool mouse_down[MIRULIT_MAX_BUTTONS];
    bool mouse_up[MIRULIT_MAX_BUTTONS];
    MIR_Vec2 mouse_position;
    MIR_Vec2 mouse_world_position;
    float mouse_wheel;
    
    // Время
    float delta_time;
    float time_scale;
    uint64_t last_time;
    uint64_t start_time;
    int fps;
    int target_fps;
    
    // Статистика
    int draw_calls;
    int update_calls;
    int particle_count;
    
    // Ресурсы
    SDL_Texture* textures[100];
    int texture_count;
    
    // Состояние игры
    int score;
    int lives;
    int level;
    bool game_over;
    
} MIR_Engine;

// ==================== ГЛОБАЛЬНЫЕ ПЕРЕМЕННЫЕ ====================
static MIR_Engine* _mir = NULL;
static bool _mir_initialized = false;

// ==================== МАТЕМАТИЧЕСКИЕ ФУНКЦИИ ====================

static inline float MIR_Math_Lerp(float a, float b, float t) {
    return a + (b - a) * t;
}

static inline float MIR_Math_Clamp(float value, float min, float max) {
    if (value < min) return min;
    if (value > max) return max;
    return value;
}

static inline float MIR_Math_Distance(MIR_Vec2 a, MIR_Vec2 b) {
    float dx = a.x - b.x;
    float dy = a.y - b.y;
    return sqrtf(dx * dx + dy * dy);
}

static inline MIR_Vec2 MIR_Math_Normalize(MIR_Vec2 v) {
    float length = sqrtf(v.x * v.x + v.y * v.y);
    if (length > 0) {
        v.x /= length;
        v.y /= length;
    }
    return v;
}

static inline float MIR_Math_RandomRange(float min, float max) {
    return min + ((float)rand() / RAND_MAX) * (max - min);
}

static inline MIR_Vec2 MIR_Vec2_Add(MIR_Vec2 a, MIR_Vec2 b) {
    return (MIR_Vec2){a.x + b.x, a.y + b.y};
}

static inline MIR_Vec2 MIR_Vec2_Subtract(MIR_Vec2 a, MIR_Vec2 b) {
    return (MIR_Vec2){a.x - b.x, a.y - b.y};
}

static inline MIR_Vec2 MIR_Vec2_Multiply(MIR_Vec2 v, float s) {
    return (MIR_Vec2){v.x * s, v.y * s};
}

static inline float MIR_Vec2_Dot(MIR_Vec2 a, MIR_Vec2 b) {
    return a.x * b.x + a.y * b.y;
}

// ==================== ЯДРО ДВИЖКА ====================

bool MIR_Init(const char* title, int width, int height) {
    if (_mir_initialized) return true;
    
    // Инициализация SDL
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        printf("[MIRULIT] SDL_Init failed: %s\n", SDL_GetError());
        return false;
    }
    
    // Создание движка
    _mir = (MIR_Engine*)calloc(1, sizeof(MIR_Engine));
    if (!_mir) {
        printf("[MIRULIT] Memory allocation failed\n");
        SDL_Quit();
        return false;
    }
    
    // Создание окна
    _mir->window = SDL_CreateWindow(title, width, height, SDL_WINDOW_RESIZABLE);
    if (!_mir->window) {
        printf("[MIRULIT] Window creation failed: %s\n", SDL_GetError());
        free(_mir);
        SDL_Quit();
        return false;
    }
    
    // Создание рендерера
    _mir->renderer = SDL_CreateRenderer(_mir->window, NULL);
    if (!_mir->renderer) {
        printf("[MIRULIT] Renderer creation failed: %s\n", SDL_GetError());
        SDL_DestroyWindow(_mir->window);
        free(_mir);
        SDL_Quit();
        return false;
    }
    
    // Инициализация параметров
    _mir->width = width;
    _mir->height = height;
    _mir->running = true;
    _mir->paused = false;
    _mir->delta_time = 0.016f;
    _mir->time_scale = 1.0f;
    _mir->target_fps = MIRULIT_DEFAULT_FPS;
    _mir->next_id = 1;
    
    strncpy(_mir->title, title, sizeof(_mir->title) - 1);
    
    // Инициализация камеры
    _mir->camera.position = (MIR_Vec2){0, 0};
    _mir->camera.zoom = 1.0f;
    _mir->camera.rotation = 0.0f;
    _mir->camera.smooth_speed = 5.0f;
    _mir->camera.bounds = (MIR_Rect){-1000, -1000, 2000, 2000};
    
    // Инициализация времени
    _mir->start_time = SDL_GetTicks();
    _mir->last_time = _mir->start_time;
    
    // Инициализация рандома
    srand((unsigned int)time(NULL));
    
    printf("[MIRULIT] Engine v%s initialized: %dx%d\n", 
           MIRULIT_VERSION, width, height);
    printf("[MIRULIT] SDL3 version: %d.%d.%d\n",
           SDL_MAJOR_VERSION, SDL_MINOR_VERSION, SDL_MICRO_VERSION);
    
    _mir_initialized = true;
    return true;
}

void MIR_Shutdown(void) {
    if (!_mir_initialized || !_mir) return;
    
    printf("[MIRULIT] Shutting down...\n");
    
    // Уничтожение всех сущностей
    for (int i = 0; i < _mir->entity_count; i++) {
        if (_mir->entities[i]) {
            if (_mir->entities[i]->on_destroy) {
                _mir->entities[i]->on_destroy(_mir->entities[i]);
            }
            
            // Освобождение компонентов
            for (int j = 0; j < _mir->entities[i]->component_count; j++) {
                free(_mir->entities[i]->components[j]);
            }
            
            // Освобождение текстуры
            if (_mir->entities[i]->sprite.texture) {
                SDL_DestroyTexture(_mir->entities[i]->sprite.texture);
            }
            
            free(_mir->entities[i]);
        }
    }
    
    // Освобождение загруженных текстур
    for (int i = 0; i < _mir->texture_count; i++) {
        if (_mir->textures[i]) {
            SDL_DestroyTexture(_mir->textures[i]);
        }
    }
    
    // Освобождение SDL
    SDL_DestroyRenderer(_mir->renderer);
    SDL_DestroyWindow(_mir->window);
    SDL_Quit();
    
    // Освобождение движка
    free(_mir);
    _mir = NULL;
    _mir_initialized = false;
    
    printf("[MIRULIT] Engine shutdown complete\n");
}

// ==================== ЦИКЛ ОБНОВЛЕНИЯ ====================

void MIR_ProcessEvents(void) {
    if (!_mir_initialized || !_mir) return;
    
    // Сброс состояний клавиш и мыши
    for (int i = 0; i < MIRULIT_MAX_KEYS; i++) {
        _mir->keys_down[i] = false;
        _mir->keys_up[i] = false;
    }
    
    for (int i = 0; i < MIRULIT_MAX_BUTTONS; i++) {
        _mir->mouse_down[i] = false;
        _mir->mouse_up[i] = false;
    }
    _mir->mouse_wheel = 0;
    
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_EVENT_QUIT:
                _mir->running = false;
                break;
                
            case SDL_EVENT_KEY_DOWN: {
                int key = event.key.key;
                if (key >= 0 && key < MIRULIT_MAX_KEYS) {
                    if (!_mir->keys[key]) {
                        _mir->keys_down[key] = true;
                    }
                    _mir->keys[key] = true;
                }
                break;
            }
                
            case SDL_EVENT_KEY_UP: {
                int key = event.key.key;
                if (key >= 0 && key < MIRULIT_MAX_KEYS) {
                    _mir->keys_up[key] = true;
                    _mir->keys[key] = false;
                }
                break;
            }
                
            case SDL_EVENT_MOUSE_BUTTON_DOWN: {
                int button = event.button.button;
                if (button >= 0 && button < MIRULIT_MAX_BUTTONS) {
                    if (!_mir->mouse_buttons[button]) {
                        _mir->mouse_down[button] = true;
                    }
                    _mir->mouse_buttons[button] = true;
                }
                break;
            }
                
            case SDL_EVENT_MOUSE_BUTTON_UP: {
                int button = event.button.button;
                if (button >= 0 && button < MIRULIT_MAX_BUTTONS) {
                    _mir->mouse_up[button] = true;
                    _mir->mouse_buttons[button] = false;
                }
                break;
            }
                
            case SDL_EVENT_MOUSE_MOTION:
                _mir->mouse_position.x = event.motion.x;
                _mir->mouse_position.y = event.motion.y;
                
                // Конвертация в мировые координаты
                _mir->mouse_world_position.x = 
                    (_mir->mouse_position.x - _mir->width / 2.0f) / _mir->camera.zoom + 
                    _mir->camera.position.x;
                _mir->mouse_world_position.y = 
                    (_mir->mouse_position.y - _mir->height / 2.0f) / _mir->camera.zoom + 
                    _mir->camera.position.y;
                break;
                
            case SDL_EVENT_MOUSE_WHEEL:
                _mir->mouse_wheel = event.wheel.y;
                break;
                
            case SDL_EVENT_WINDOW_RESIZED:
                _mir->width = event.window.data1;
                _mir->height = event.window.data2;
                break;
        }
    }
}

void MIR_BeginFrame(void) {
    if (!_mir_initialized || !_mir || !_mir->running) return;
    
    // Расчет дельта-времени
    uint64_t current_time = SDL_GetTicks();
    _mir->delta_time = (current_time - _mir->last_time) / 1000.0f;
    _mir->last_time = current_time;
    
    // Ограничение дельта-времени (защита от рывков)
    if (_mir->delta_time > 0.1f) {
        _mir->delta_time = 0.1f;
    }
    
    // Применение time scale
    float scaled_dt = _mir->delta_time * _mir->time_scale;
    
    // Расчет FPS
    static int frame_counter = 0;
    static uint64_t fps_timer = 0;
    frame_counter++;
    
    if (current_time - fps_timer >= 1000) {
        _mir->fps = frame_counter;
        frame_counter = 0;
        fps_timer = current_time;
    }
    
    // Обновление камеры
    if (_mir->camera.smooth_speed > 0 && 
        (_mir->camera.target.x != 0 || _mir->camera.target.y != 0)) {
        float t = 1.0f - expf(-_mir->camera.smooth_speed * scaled_dt);
        _mir->camera.position.x = MIR_Math_Lerp(
            _mir->camera.position.x, _mir->camera.target.x, t);
        _mir->camera.position.y = MIR_Math_Lerp(
            _mir->camera.position.y, _mir->camera.target.y, t);
    }
    
    // Очистка экрана
    SDL_SetRenderDrawColor(_mir->renderer, 
                          MIR_COLOR_BACKGROUND.r,
                          MIR_COLOR_BACKGROUND.g,
                          MIR_COLOR_BACKGROUND.b,
                          MIR_COLOR_BACKGROUND.a);
    SDL_RenderClear(_mir->renderer);
    
    // Сброс статистики
    _mir->draw_calls = 0;
    _mir->update_calls = 0;
}

void MIR_EndFrame(void) {
    if (!_mir_initialized || !_mir) return;
    
    // Отображение
    SDL_RenderPresent(_mir->renderer);
    
    // Ограничение FPS
    if (_mir->target_fps > 0) {
        uint64_t frame_time = SDL_GetTicks() - _mir->last_time;
        uint64_t target_frame_time = 1000 / _mir->target_fps;
        
        if (frame_time < target_frame_time) {
            SDL_Delay((uint32_t)(target_frame_time - frame_time));
        }
    }
}

// ==================== РАБОТА С СУЩНОСТЯМИ ====================

MIR_Entity* MIR_CreateEntity(const char* tag) {
    if (!_mir_initialized || !_mir || _mir->entity_count >= MIRULIT_MAX_ENTITIES) {
        return NULL;
    }
    
    MIR_Entity* entity = (MIR_Entity*)calloc(1, sizeof(MIR_Entity));
    if (!entity) return NULL;
    
    entity->id = _mir->next_id++;
    entity->active = true;
    entity->visible = true;
    entity->persistent = false;
    
    if (tag) {
        strncpy(entity->tag, tag, sizeof(entity->tag) - 1);
    }
    
    // Инициализация трансформа
    entity->transform.position = (MIR_Vec2){0, 0};
    entity->transform.scale = (MIR_Vec2){1, 1};
    entity->transform.rotation = 0.0f;
    entity->transform.velocity = (MIR_Vec2){0, 0};
    entity->transform.acceleration = (MIR_Vec2){0, 0};
    
    // Инициализация спрайта
    entity->sprite.texture = NULL;
    entity->sprite.source_rect = (MIR_Rect){0, 0, 0, 0};
    entity->sprite.color = MIR_COLOR_WHITE;
    entity->sprite.z_index = 0;
    entity->sprite.flip_x = false;
    entity->sprite.flip_y = false;
    
    // Инициализация коллайдера
    entity->collider.bounds = (MIR_Rect){0, 0, 1, 1};
    entity->collider.is_trigger = false;
    entity->collider.enabled = true;
    entity->collider.on_collision = NULL;
    
    // Добавление в движок
    _mir->entities[_mir->entity_count++] = entity;
    
    return entity;
}

void MIR_DestroyEntity(MIR_Entity* entity) {
    if (!_mir_initialized || !_mir || !entity) return;
    
    // Вызов callback
    if (entity->on_destroy) {
        entity->on_destroy(entity);
    }
    
    // Удаление из списка детей родителя
    if (entity->parent) {
        for (int i = 0; i < entity->parent->child_count; i++) {
            if (entity->parent->children[i] == entity) {
                for (int j = i; j < entity->parent->child_count - 1; j++) {
                    entity->parent->children[j] = entity->parent->children[j + 1];
                }
                entity->parent->child_count--;
                break;
            }
        }
    }
    
    // Уничтожение детей
    for (int i = 0; i < entity->child_count; i++) {
        MIR_DestroyEntity(entity->children[i]);
    }
    
    // Освобождение компонентов
    for (int i = 0; i < entity->component_count; i++) {
        free(entity->components[i]);
    }
    
    // Освобождение текстуры
    if (entity->sprite.texture) {
        SDL_DestroyTexture(entity->sprite.texture);
    }
    
    // Удаление из массива движка
    for (int i = 0; i < _mir->entity_count; i++) {
        if (_mir->entities[i] == entity) {
            free(entity);
            for (int j = i; j < _mir->entity_count - 1; j++) {
                _mir->entities[j] = _mir->entities[j + 1];
            }
            _mir->entity_count--;
            break;
        }
    }
}

MIR_Entity* MIR_FindEntityByTag(const char* tag) {
    if (!_mir_initialized || !_mir || !tag) return NULL;
    
    for (int i = 0; i < _mir->entity_count; i++) {
        if (_mir->entities[i] && strcmp(_mir->entities[i]->tag, tag) == 0) {
            return _mir->entities[i];
        }
    }
    return NULL;
}

MIR_Entity* MIR_FindEntityByID(int id) {
    if (!_mir_initialized || !_mir) return NULL;
    
    for (int i = 0; i < _mir->entity_count; i++) {
        if (_mir->entities[i] && _mir->entities[i]->id == id) {
            return _mir->entities[i];
        }
    }
    return NULL;
}

void MIR_UpdateEntities(void) {
    if (!_mir_initialized || !_mir || _mir->paused) return;
    
    float scaled_dt = _mir->delta_time * _mir->time_scale;
    
    for (int i = 0; i < _mir->entity_count; i++) {
        MIR_Entity* entity = _mir->entities[i];
        
        if (!entity || !entity->active) continue;
        
        _mir->update_calls++;
        
        // Обновление физики
        entity->transform.velocity = MIR_Vec2_Add(
            entity->transform.velocity,
            MIR_Vec2_Multiply(entity->transform.acceleration, scaled_dt)
        );
        
        entity->transform.position = MIR_Vec2_Add(
            entity->transform.position,
            MIR_Vec2_Multiply(entity->transform.velocity, scaled_dt)
        );
        
        // Вызов пользовательского обновления
        if (entity->update) {
            entity->update(entity, scaled_dt);
        }
        
        // Обновление коллайдера
        if (entity->collider.enabled) {
            entity->collider.bounds.x = entity->transform.position.x - 
                                       entity->collider.bounds.w / 2;
            entity->collider.bounds.y = entity->transform.position.y - 
                                       entity->collider.bounds.h / 2;
        }
    }
}

// ==================== СИСТЕМА ЧАСТИЦ ====================

void MIR_EmitParticleEx(MIR_Vec2 position, MIR_Vec2 velocity, MIR_Vec2 acceleration, 
                       MIR_Color color, float size, float life) {
    if (!_mir_initialized || !_mir) return;
    
    for (int i = 0; i < MIRULIT_MAX_PARTICLES; i++) {
        if (!_mir->particles[i].active) {
            _mir->particles[i].position = position;
            _mir->particles[i].velocity = velocity;
            _mir->particles[i].acceleration = acceleration;
            _mir->particles[i].color = color;
            _mir->particles[i].size = size;
            _mir->particles[i].life = life;
            _mir->particles[i].max_life = life;
            _mir->particles[i].active = true;
            _mir->particle_count++;
            break;
        }
    }
}

static inline void MIR_EmitParticle(MIR_Vec2 position, MIR_Vec2 velocity, 
                                   MIR_Color color, float size, float life) {
    MIR_EmitParticleEx(position, velocity, (MIR_Vec2){0, 50}, color, size, life);
}

void MIR_UpdateParticles(void) {
    if (!_mir_initialized || !_mir) return;
    
    float scaled_dt = _mir->delta_time * _mir->time_scale;
    
    for (int i = 0; i < MIRULIT_MAX_PARTICLES; i++) {
        if (!_mir->particles[i].active) continue;
        
        // Обновление физики
        _mir->particles[i].velocity = MIR_Vec2_Add(
            _mir->particles[i].velocity,
            MIR_Vec2_Multiply(_mir->particles[i].acceleration, scaled_dt)
        );
        
        _mir->particles[i].position = MIR_Vec2_Add(
            _mir->particles[i].position,
            MIR_Vec2_Multiply(_mir->particles[i].velocity, scaled_dt)
        );
        
        // Обновление жизни
        _mir->particles[i].life -= scaled_dt;
        if (_mir->particles[i].life <= 0) {
            _mir->particles[i].active = false;
            _mir->particle_count--;
        }
    }
}

void MIR_DrawParticles(void) {
    if (!_mir_initialized || !_mir) return;
    
    for (int i = 0; i < MIRULIT_MAX_PARTICLES; i++) {
        if (!_mir->particles[i].active) continue;
        
        // Интерполяция цвета по времени жизни
        float t = _mir->particles[i].life / _mir->particles[i].max_life;
        MIR_Color color = _mir->particles[i].color;
        color.a = (uint8_t)(color.a * t);
        
        // Конвертация мировых координат в экранные (с учетом камеры)
        float screen_x = (_mir->particles[i].position.x - _mir->camera.position.x) * 
                        _mir->camera.zoom + _mir->width / 2.0f;
        float screen_y = (_mir->particles[i].position.y - _mir->camera.position.y) * 
                        _mir->camera.zoom + _mir->height / 2.0f;
        
        // Масштабирование размера частицы с учетом зума камеры
        float screen_size = _mir->particles[i].size * t * _mir->camera.zoom;
        
        // Отрисовка частицы
        SDL_SetRenderDrawColor(_mir->renderer, 
                              color.r, color.g, color.b, color.a);
        
        SDL_FRect rect = {
            screen_x - screen_size / 2,
            screen_y - screen_size / 2,
            screen_size, screen_size
        };
        SDL_RenderFillRect(_mir->renderer, &rect);
        _mir->draw_calls++;
    }
}

// ==================== РЕНДЕРИНГ ====================

void MIR_DrawEntity(MIR_Entity* entity) {
    if (!_mir_initialized || !_mir || !entity || !entity->visible) return;
    
    _mir->draw_calls++;
    
    // Мировые координаты с учётом камеры
    float world_x = (entity->transform.position.x - _mir->camera.position.x) * 
                    _mir->camera.zoom + _mir->width / 2.0f;
    float world_y = (entity->transform.position.y - _mir->camera.position.y) * 
                    _mir->camera.zoom + _mir->height / 2.0f;
    
    if (entity->sprite.texture) {
        // Отрисовка текстуры
        SDL_FRect dest_rect = {
            world_x - entity->transform.scale.x * _mir->camera.zoom / 2,
            world_y - entity->transform.scale.y * _mir->camera.zoom / 2,
            entity->transform.scale.x * _mir->camera.zoom,
            entity->transform.scale.y * _mir->camera.zoom
        };
        
        SDL_SetTextureColorMod(entity->sprite.texture,
                              entity->sprite.color.r,
                              entity->sprite.color.g,
                              entity->sprite.color.b);
        SDL_SetTextureAlphaMod(entity->sprite.texture, entity->sprite.color.a);
        
        SDL_RenderTexture(_mir->renderer, entity->sprite.texture, NULL, &dest_rect);
    } else {
        // Отрисовка цветного прямоугольника
        SDL_SetRenderDrawColor(_mir->renderer,
                              entity->sprite.color.r,
                              entity->sprite.color.g,
                              entity->sprite.color.b,
                              entity->sprite.color.a);
        
        SDL_FRect rect = {
            world_x - entity->transform.scale.x * _mir->camera.zoom / 2,
            world_y - entity->transform.scale.y * _mir->camera.zoom / 2,
            entity->transform.scale.x * _mir->camera.zoom,
            entity->transform.scale.y * _mir->camera.zoom
        };
        
        SDL_RenderFillRect(_mir->renderer, &rect);
        
        // Рамка
        SDL_SetRenderDrawColor(_mir->renderer,
                              entity->sprite.color.r / 2,
                              entity->sprite.color.g / 2,
                              entity->sprite.color.b / 2,
                              entity->sprite.color.a);
        SDL_RenderRect(_mir->renderer, &rect);
    }
    
    // Пользовательская отрисовка
    if (entity->draw) {
        entity->draw(entity);
    }
}

void MIR_DrawEntities(void) {
    if (!_mir_initialized || !_mir) return;
    
    // Сортировка по z-index
    for (int i = 0; i < _mir->entity_count - 1; i++) {
        for (int j = 0; j < _mir->entity_count - i - 1; j++) {
            if (_mir->entities[j]->sprite.z_index > 
                _mir->entities[j + 1]->sprite.z_index) {
                MIR_Entity* temp = _mir->entities[j];
                _mir->entities[j] = _mir->entities[j + 1];
                _mir->entities[j + 1] = temp;
            }
        }
    }
    
    // Отрисовка
    for (int i = 0; i < _mir->entity_count; i++) {
        MIR_DrawEntity(_mir->entities[i]);
    }
}

void MIR_DrawRect(MIR_Rect rect, MIR_Color color) {
    if (!_mir_initialized || !_mir) return;
    
    SDL_SetRenderDrawColor(_mir->renderer, color.r, color.g, color.b, color.a);
    SDL_FRect sdl_rect = {rect.x, rect.y, rect.w, rect.h};
    SDL_RenderFillRect(_mir->renderer, &sdl_rect);
    _mir->draw_calls++;
}

void MIR_DrawCircle(MIR_Vec2 center, float radius, MIR_Color color, int segments) {
    if (!_mir_initialized || !_mir || radius <= 0) return;
    
    if (segments < 8) segments = 8;
    if (segments > 64) segments = 64;
    
    SDL_SetRenderDrawColor(_mir->renderer, color.r, color.g, color.b, color.a);
    
    for (int i = 0; i < segments; i++) {
        float angle1 = (float)i / segments * 2 * 3.14159265f;
        float angle2 = (float)(i + 1) / segments * 2 * 3.14159265f;
        
        SDL_RenderLine(_mir->renderer,
                      center.x + cosf(angle1) * radius,
                      center.y + sinf(angle1) * radius,
                      center.x + cosf(angle2) * radius,
                      center.y + sinf(angle2) * radius);
    }
    _mir->draw_calls++;
}

SDL_Texture* MIR_LoadTexture(const char* filepath) {
    if (!_mir_initialized || !_mir) return NULL;
    
    // Создаем поверхность из BMP файла (SDL3 пока не поддерживает PNG без SDL_image)
    SDL_Surface* surface = SDL_LoadBMP(filepath);
    if (!surface) {
        printf("[MIRULIT] Failed to load texture: %s\n", SDL_GetError());
        return NULL;
    }
    
    // Создаем текстуру из поверхности
    SDL_Texture* texture = SDL_CreateTextureFromSurface(_mir->renderer, surface);
    SDL_DestroySurface(surface);
    
    if (!texture) {
        printf("[MIRULIT] Failed to create texture: %s\n", SDL_GetError());
        return NULL;
    }
    
    printf("[MIRULIT] Texture loaded: %s\n", filepath);
    return texture;
}

void MIR_DrawLine(MIR_Vec2 start, MIR_Vec2 end, MIR_Color color, float thickness) {
    if (!_mir_initialized || !_mir) return;
    
    SDL_SetRenderDrawColor(_mir->renderer, color.r, color.g, color.b, color.a);
    
    if (thickness <= 1.0f) {
        SDL_RenderLine(_mir->renderer, start.x, start.y, end.x, end.y);
    } else {
        // Простая реализация толстой линии через прямоугольник
        MIR_Vec2 dir = MIR_Vec2_Subtract(end, start);
        float length = sqrtf(dir.x * dir.x + dir.y * dir.y);
        if (length > 0) {
            dir.x /= length;
            dir.y /= length;
            
            // Перпендикуляр
            MIR_Vec2 perp = { -dir.y, dir.x };
            perp = MIR_Vec2_Multiply(perp, thickness / 2);
            
            // Углы прямоугольника
            MIR_Vec2 p1 = MIR_Vec2_Add(start, perp);
            MIR_Vec2 p2 = MIR_Vec2_Subtract(start, perp);
            MIR_Vec2 p3 = MIR_Vec2_Subtract(end, perp);
            MIR_Vec2 p4 = MIR_Vec2_Add(end, perp);
            
            // Отрисовка как заполненного четырёхугольника
            SDL_Vertex vertices[4] = {
                { {p1.x, p1.y}, {color.r, color.g, color.b, color.a}, {0, 0} },
                { {p2.x, p2.y}, {color.r, color.g, color.b, color.a}, {0, 0} },
                { {p4.x, p4.y}, {color.r, color.g, color.b, color.a}, {0, 0} },
                { {p3.x, p3.y}, {color.r, color.g, color.b, color.a}, {0, 0} }
            };
            
            int indices[6] = {0, 1, 2, 1, 3, 2};
            SDL_RenderGeometry(_mir->renderer, NULL, vertices, 4, indices, 6);
        }
    }
    _mir->draw_calls++;
}

// ==================== КАМЕРА ====================

void MIR_SetCameraTarget(MIR_Vec2 target) {
    if (!_mir_initialized || !_mir) return;
    _mir->camera.target = target;
}

void MIR_SetCameraZoom(float zoom) {
    if (!_mir_initialized || !_mir) return;
    _mir->camera.zoom = MIR_Math_Clamp(zoom, 0.1f, 5.0f);
}

void MIR_CameraShake(float intensity, float duration) {
    if (!_mir_initialized || !_mir) return;
    
    static float shake_timer = 0;
    static float shake_intensity = 0;
    
    if (intensity > 0) {
        shake_timer = duration;
        shake_intensity = intensity;
    }
    
    if (shake_timer > 0) {
        shake_timer -= _mir->delta_time;
        
        float offset_x = MIR_Math_RandomRange(-shake_intensity, shake_intensity);
        float offset_y = MIR_Math_RandomRange(-shake_intensity, shake_intensity);
        
        _mir->camera.position.x += offset_x;
        _mir->camera.position.y += offset_y;
        
        // Затухание
        shake_intensity *= 0.9f;
    }
}

// ==================== КОЛЛИЗИИ ====================

bool MIR_CheckCollision(MIR_Entity* a, MIR_Entity* b) {
    if (!a || !b || !a->collider.enabled || !b->collider.enabled) return false;
    
    MIR_Rect rectA = a->collider.bounds;
    MIR_Rect rectB = b->collider.bounds;
    
    return (rectA.x < rectB.x + rectB.w &&
            rectA.x + rectA.w > rectB.x &&
            rectA.y < rectB.y + rectB.h &&
            rectA.y + rectA.h > rectB.y);
}

MIR_Entity* MIR_PointCollision(MIR_Vec2 point) {
    if (!_mir_initialized || !_mir) return NULL;
    
    for (int i = 0; i < _mir->entity_count; i++) {
        MIR_Entity* entity = _mir->entities[i];
        if (!entity || !entity->collider.enabled) continue;
        
        MIR_Rect bounds = entity->collider.bounds;
        if (point.x >= bounds.x && point.x <= bounds.x + bounds.w &&
            point.y >= bounds.y && point.y <= bounds.y + bounds.h) {
            return entity;
        }
    }
    return NULL;
}

void MIR_ResolveCollisions(void) {
    if (!_mir_initialized || !_mir) return;
    
    for (int i = 0; i < _mir->entity_count; i++) {
        for (int j = i + 1; j < _mir->entity_count; j++) {
            if (MIR_CheckCollision(_mir->entities[i], _mir->entities[j])) {
                if (_mir->entities[i]->collider.on_collision) {
                    _mir->entities[i]->collider.on_collision(
                        _mir->entities[i], _mir->entities[j]);
                }
                if (_mir->entities[j]->collider.on_collision) {
                    _mir->entities[j]->collider.on_collision(
                        _mir->entities[j], _mir->entities[i]);
                }
            }
        }
    }
}

// ==================== УТИЛИТЫ ====================

float MIR_GetDeltaTime(void) {
    return _mir_initialized && _mir ? _mir->delta_time : 0.016f;
}

float MIR_GetTime(void) {
    return _mir_initialized && _mir ? 
           (SDL_GetTicks() - _mir->start_time) / 1000.0f : 0.0f;
}

int MIR_GetFPS(void) {
    return _mir_initialized && _mir ? _mir->fps : 0;
}

bool MIR_IsKeyDown(int key) {
    return _mir_initialized && _mir && key >= 0 && key < MIRULIT_MAX_KEYS ? 
           _mir->keys[key] : false;
}

bool MIR_IsKeyPressed(int key) {
    return _mir_initialized && _mir && key >= 0 && key < MIRULIT_MAX_KEYS ? 
           _mir->keys_down[key] : false;
}

bool MIR_IsKeyReleased(int key) {
    return _mir_initialized && _mir && key >= 0 && key < MIRULIT_MAX_KEYS ? 
           _mir->keys_up[key] : false;
}

bool MIR_IsMouseButtonDown(int button) {
    return _mir_initialized && _mir && button >= 0 && button < MIRULIT_MAX_BUTTONS ? 
           _mir->mouse_buttons[button] : false;
}

bool MIR_IsMouseButtonPressed(int button) {
    return _mir_initialized && _mir && button >= 0 && button < MIRULIT_MAX_BUTTONS ? 
           _mir->mouse_down[button] : false;
}

MIR_Vec2 MIR_GetMousePosition(void) {
    return _mir_initialized && _mir ? _mir->mouse_position : (MIR_Vec2){0, 0};
}

MIR_Vec2 MIR_GetMouseWorldPosition(void) {
    return _mir_initialized && _mir ? _mir->mouse_world_position : (MIR_Vec2){0, 0};
}

float MIR_GetMouseWheel(void) {
    return _mir_initialized && _mir ? _mir->mouse_wheel : 0.0f;
}

int MIR_GetScreenWidth(void) {
    return _mir_initialized && _mir ? _mir->width : 0;
}

int MIR_GetScreenHeight(void) {
    return _mir_initialized && _mir ? _mir->height : 0;
}

void MIR_SetTimeScale(float scale) {
    if (_mir_initialized && _mir) {
        _mir->time_scale = MIR_Math_Clamp(scale, 0.0f, 5.0f);
    }
}

void MIR_SetTargetFPS(int fps) {
    if (_mir_initialized && _mir) {
        _mir->target_fps = fps > 0 ? fps : 0;
    }
}

void MIR_Pause(void) {
    if (_mir_initialized && _mir) {
        _mir->paused = true;
    }
}

void MIR_Resume(void) {
    if (_mir_initialized && _mir) {
        _mir->paused = false;
    }
}

bool MIR_IsPaused(void) {
    return _mir_initialized && _mir ? _mir->paused : false;
}

bool MIR_IsRunning(void) {
    return _mir_initialized && _mir ? _mir->running : false;
}

void MIR_Quit(void) {
    if (_mir_initialized && _mir) {
        _mir->running = false;
    }
}

// ==================== ДЕБАГ И СТАТИСТИКА ====================

void MIR_DrawDebugInfo(void) {
    if (!_mir_initialized || !_mir) return;
    
    // Вывод в консоль
    static int last_fps = 0;
    if (_mir->fps != last_fps) {
        printf("\r[MIRULIT] FPS: %3d | Entities: %3d | Particles: %3d | Draws: %3d", 
               _mir->fps, _mir->entity_count, _mir->particle_count, _mir->draw_calls);
        fflush(stdout);
        last_fps = _mir->fps;
    }
}

#endif // MIRULIT_H