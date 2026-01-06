#ifndef TIME_H
#define TIME_H

#include <GLFW/glfw3.h>
#include <stdio.h>

// ==================== СТРУКТУРА ТАЙМЕРА ====================
typedef struct Timer {
    double lastTime;      // Время последнего кадра
    double currentTime;   // Текущее время
    double deltaTime;     // Время между кадрами (в секундах)
    float deltaTimef;     // Время между кадрами (в секундах, как float)
    
    // Для FPS
    double lastFPSTime;   // Время последнего расчета FPS
    int frameCount;       // Счетчик кадров
    int fps;             // Текущее значение FPS
    double frameTime;     // Время отрисовки одного кадра (в мс)
} Timer;

// ==================== ФУНКЦИИ ====================

// Инициализация таймера
Timer* timer_create(void) {
    Timer* timer = (Timer*)malloc(sizeof(Timer));
    if (!timer) return NULL;
    
    timer->lastTime = glfwGetTime();
    timer->currentTime = timer->lastTime;
    timer->deltaTime = 0.0;
    timer->deltaTimef = 0.0f;
    
    timer->lastFPSTime = timer->lastTime;
    timer->frameCount = 0;
    timer->fps = 0;
    timer->frameTime = 0.0;
    
    return timer;
}

// Обновление таймера (вызывать в начале каждого кадра)
void timer_update(Timer* timer) {
    if (!timer) return;
    
    timer->currentTime = glfwGetTime();
    timer->deltaTime = timer->currentTime - timer->lastTime;
    timer->deltaTimef = (float)timer->deltaTime;
    timer->lastTime = timer->currentTime;
    
    // Расчет времени отрисовки кадра
    timer->frameTime = timer->deltaTime * 1000.0; // в миллисекундах
}

// Обновление FPS (вызывать каждый кадр)
void timer_update_fps(Timer* timer) {
    if (!timer) return;
    
    timer->frameCount++;
    
    // Если прошла 1 секунда
    double timeSinceLastFPS = timer->currentTime - timer->lastFPSTime;
    if (timeSinceLastFPS >= 1.0) {
        timer->fps = (int)(timer->frameCount / timeSinceLastFPS);
        timer->frameCount = 0;
        timer->lastFPSTime = timer->currentTime;
    }
}

// Получение FPS
int timer_get_fps(Timer* timer) {
    return timer ? timer->fps : 0;
}

// Получение delta time (в секундах)
double timer_get_delta_time(Timer* timer) {
    return timer ? timer->deltaTime : 0.0;
}

// Получение delta time как float
float timer_get_delta_timef(Timer* timer) {
    return timer ? timer->deltaTimef : 0.0f;
}

// Получение времени отрисовки кадра (в мс)
double timer_get_frame_time(Timer* timer) {
    return timer ? timer->frameTime : 0.0;
}

// Получение общего времени работы приложения (в секундах)
double timer_get_total_time(Timer* timer) {
    return timer ? timer->currentTime : 0.0;
}

// Форматирование строки для отображения в заголовке окна
void timer_format_window_title(const Timer* timer, char* buffer, size_t bufferSize, 
                              const char* baseTitle) {
    if (!timer || !buffer) return;
    
    snprintf(buffer, bufferSize, "%s | FPS: %d | Frame: %.2fms", 
             baseTitle ? baseTitle : "Application",
             timer->fps, timer->frameTime);
}

// Форматирование строки с расширенной информацией
void timer_format_debug_info(const Timer* timer, char* buffer, size_t bufferSize) {
    if (!timer || !buffer) return;
    
    snprintf(buffer, bufferSize, 
             "FPS: %d | Frame: %.2fms | Delta: %.4fs | Total: %.2fs",
             timer->fps, timer->frameTime, timer->deltaTime, timer->currentTime);
}

// Ограничение FPS (синхронизация с заданным значением)
void timer_limit_fps(Timer* timer, int targetFPS) {
    if (!timer || targetFPS <= 0) return;
    
    double targetFrameTime = 1.0 / targetFPS;
    double currentFrameTime = glfwGetTime() - timer->lastTime;
    
    if (currentFrameTime < targetFrameTime) {
        double sleepTime = targetFrameTime - currentFrameTime;
        // Используем busy-wait для точности
        double endTime = glfwGetTime() + sleepTime;
        while (glfwGetTime() < endTime) {
            // Ничего не делаем - просто ждем
        }
    }
}

// Сброс таймера
void timer_reset(Timer* timer) {
    if (!timer) return;
    
    timer->lastTime = glfwGetTime();
    timer->currentTime = timer->lastTime;
    timer->deltaTime = 0.0;
    timer->deltaTimef = 0.0f;
    timer->frameCount = 0;
    timer->fps = 0;
    timer->frameTime = 0.0;
}

// Уничтожение таймера
void timer_destroy(Timer* timer) {
    if (timer) free(timer);
}

// ==================== МАКРОСЫ ДЛЯ БЫСТРОГО ДОСТУПА ====================

// Макрос для использования delta time в вычислениях движения
#define TIMER_DT(timer) (timer_get_delta_timef(timer))

// Макрос для расчета скорости с учетом delta time
#define TIMER_SPEED(speed, timer) ((speed) * TIMER_DT(timer))

// Макрос для проверки интервала времени
#define TIMER_INTERVAL(timer, interval) (timer_get_total_time(timer) - (interval))

#endif // TIME_H