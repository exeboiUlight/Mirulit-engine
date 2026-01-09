#ifndef MOUSE_H
#define MOUSE_H

#include <GLFW/glfw3.h>

// Структура для хранения позиции мыши
typedef struct {
    double x;
    double y;
} MousePosition;

// Глобальные переменные для состояния мыши
static MousePosition currentMousePos = {0, 0};
static MousePosition previousMousePos = {0, 0};
static int mouseButtons[GLFW_MOUSE_BUTTON_LAST] = {0};
static int previousMouseButtons[GLFW_MOUSE_BUTTON_LAST] = {0};
static double scrollOffset = 0.0;
static int firstMouse = 1;

// Макрос для получения позиции мыши (в виде структуры)
#define GetMousePosition() currentMousePos

// Функция обратного вызова для перемещения мыши
static void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (firstMouse) {
        currentMousePos.x = xpos;
        currentMousePos.y = ypos;
        previousMousePos = currentMousePos;
        firstMouse = 0;
        return;
    }
    
    previousMousePos = currentMousePos;
    currentMousePos.x = xpos;
    currentMousePos.y = ypos;
}

// Функция обратного вызова для нажатия кнопок мыши
static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button >= 0 && button < GLFW_MOUSE_BUTTON_LAST) {
        previousMouseButtons[button] = mouseButtons[button];
        mouseButtons[button] = action;
    }
}

// Функция обратного вызова для прокрутки колесика
static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    scrollOffset = yoffset;
}

// Инициализация мыши
static void initMouse(GLFWwindow* window) {
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetScrollCallback(window, scroll_callback);
    
    // Скрываем курсор и захватываем его
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

// Проверка нажатия кнопки мыши
static int isMouseButtonPressed(int button) {
    if (button >= 0 && button < GLFW_MOUSE_BUTTON_LAST) {
        return mouseButtons[button] == GLFW_PRESS;
    }
    return 0;
}

// Проверка, была ли кнопка мыши только что нажата (одиночное нажатие)
static int isMouseButtonJustPressed(int button) {
    if (button >= 0 && button < GLFW_MOUSE_BUTTON_LAST) {
        return mouseButtons[button] == GLFW_PRESS && previousMouseButtons[button] == GLFW_RELEASE;
    }
    return 0;
}

// Получение смещения мыши с предыдущего кадра
static MousePosition getMouseOffset() {
    MousePosition offset = {
        currentMousePos.x - previousMousePos.x,
        previousMousePos.y - currentMousePos.y  // Инвертируем Y для удобства камеры
    };
    return offset;
}

// Получение смещения прокрутки
static double getMouseScroll() {
    double scroll = scrollOffset;
    scrollOffset = 0.0;  // Сбрасываем после чтения
    return scroll;
}

// Обновление состояния мыши (вызывать в конце каждого кадра)
static void updateMouse() {
    for (int i = 0; i < GLFW_MOUSE_BUTTON_LAST; i++) {
        previousMouseButtons[i] = mouseButtons[i];
    }
    previousMousePos = currentMousePos;
}

// Показать/скрыть курсор
static void setMouseCursorVisible(GLFWwindow* window, int visible) {
    glfwSetInputMode(window, GLFW_CURSOR, 
        visible ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
}

#endif