// widget.h
#ifndef WIDGET_H
#define WIDGET_H

#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include "../utils/math/geometry.h"
#include "../graphics/rgb.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdio.h>

// ==================== СТРУКТУРЫ ДАННЫХ ====================

// Структура для прямоугольника
typedef struct {
    float x, y, width, height;
} WidgetRect;

// Структура для кнопки
typedef struct WidgetButton {
    WidgetRect rect;
    char* label;
    RGB normalColor;
    RGB hoverColor;
    RGB pressedColor;
    bool isPressed;
    bool isHovered;
    void (*onClick)(void);
    struct WidgetButton* next;
} WidgetButton;

// Структура для ползунка
typedef struct WidgetSlider {
    WidgetRect rect;
    char* label;
    float* value;
    float minValue;
    float maxValue;
    bool isDragging;
    RGB trackColor;
    RGB thumbColor;
    struct WidgetSlider* next;
} WidgetSlider;

// Структура для чекбокса
typedef struct WidgetCheckbox {
    WidgetRect rect;
    char* label;
    bool* value;
    RGB boxColor;
    RGB checkColor;
    struct WidgetCheckbox* next;
} WidgetCheckbox;

// Структура для текстового поля
typedef struct WidgetTextInput {
    WidgetRect rect;
    char* text;
    int maxLength;
    bool isFocused;
    RGB bgColor;
    RGB textColor;
    RGB borderColor;
    struct WidgetTextInput* next;
} WidgetTextInput;

// Структура для панели
typedef struct WidgetPanel {
    WidgetRect rect;
    char* title;
    RGB bgColor;
    RGB borderColor;
    struct WidgetPanel* next;
} WidgetPanel;

// Структура для метки
typedef struct WidgetLabel {
    WidgetRect rect;
    char* text;
    RGB color;
    struct WidgetLabel* next;
} WidgetLabel;

// Структура для менеджера виджетов
typedef struct {
    // Списки виджетов
    WidgetButton* firstButton;
    WidgetSlider* firstSlider;
    WidgetCheckbox* firstCheckbox;
    WidgetTextInput* firstTextInput;
    WidgetPanel* firstPanel;
    WidgetLabel* firstLabel;
    
    // Состояние ввода
    double mouseX, mouseY;
    bool mouseButtons[3]; // 0=левая, 1=правая, 2=средняя
    bool prevMouseButtons[3];
    bool keys[GLFW_KEY_LAST];
    bool prevKeys[GLFW_KEY_LAST];
    char inputBuffer[256];
    
    // Стиль
    RGB backgroundColor;
    RGB textColor;
    RGB primaryColor;
    RGB secondaryColor;
    RGB hoverColor;
    float padding;
    float margin;
    
    // Контекст
    GLFWwindow* window;
    bool isEnabled;
    double lastClickTime;
    int cursorPosition;
    
    // Шрифт (упрощенный)
    float fontScale;
} WidgetManager;

// ==================== ФУНКЦИИ ====================

// Создание менеджера виджетов
WidgetManager* widget_manager_create(GLFWwindow* window) {
    WidgetManager* manager = (WidgetManager*)malloc(sizeof(WidgetManager));
    if (!manager) return NULL;
    
    memset(manager, 0, sizeof(WidgetManager));
    
    manager->window = window;
    manager->isEnabled = true;
    
    // Настройки стиля по умолчанию
    manager->backgroundColor = rgb(45, 45, 48);
    manager->textColor = rgb(255, 255, 255);
    manager->primaryColor = rgb(0, 122, 204);
    manager->secondaryColor = rgb(60, 60, 60);
    manager->hoverColor = rgb(30, 30, 30);
    manager->padding = 5.0f;
    manager->margin = 10.0f;
    manager->fontScale = 1.0f;
    
    // Инициализация списков
    manager->firstButton = NULL;
    manager->firstSlider = NULL;
    manager->firstCheckbox = NULL;
    manager->firstTextInput = NULL;
    manager->firstPanel = NULL;
    manager->firstLabel = NULL;
    
    return manager;
}

// Уничтожение менеджера виджетов
void widget_manager_destroy(WidgetManager* manager) {
    if (!manager) return;
    
    // Очищаем все виджеты
    widget_clear_all(manager);
    free(manager);
}

// Проверка, находится ли точка внутри прямоугольника
bool widget_point_in_rect(float px, float py, WidgetRect rect) {
    return (px >= rect.x && px <= rect.x + rect.width &&
            py >= rect.y && py <= rect.y + rect.height);
}

// Создание кнопки
WidgetButton* widget_create_button(WidgetManager* manager, float x, float y, 
                                  float width, float height, const char* label,
                                  void (*onClick)(void)) {
    WidgetButton* button = (WidgetButton*)malloc(sizeof(WidgetButton));
    if (!button) return NULL;
    
    button->rect.x = x;
    button->rect.y = y;
    button->rect.width = width;
    button->rect.height = height;
    
    button->label = (char*)malloc(strlen(label) + 1);
    if (button->label) {
        strcpy(button->label, label);
    }
    
    button->normalColor = rgb(62, 62, 66);
    button->hoverColor = rgb(45, 45, 48);
    button->pressedColor = rgb(0, 122, 204);
    button->isPressed = false;
    button->isHovered = false;
    button->onClick = onClick;
    
    // Добавляем в список
    button->next = manager->firstButton;
    manager->firstButton = button;
    
    return button;
}

// Создание ползунка
WidgetSlider* widget_create_slider(WidgetManager* manager, float x, float y,
                                  float width, float height, const char* label,
                                  float* value, float minVal, float maxVal) {
    WidgetSlider* slider = (WidgetSlider*)malloc(sizeof(WidgetSlider));
    if (!slider) return NULL;
    
    slider->rect.x = x;
    slider->rect.y = y;
    slider->rect.width = width;
    slider->rect.height = height;
    
    slider->label = (char*)malloc(strlen(label) + 1);
    if (slider->label) {
        strcpy(slider->label, label);
    }
    
    slider->value = value;
    slider->minValue = minVal;
    slider->maxValue = maxVal;
    slider->isDragging = false;
    slider->trackColor = rgb(62, 62, 66);
    slider->thumbColor = rgb(0, 122, 204);
    
    // Инициализируем значение, если указатель не NULL
    if (value && minVal <= maxVal) {
        *value = (*value < minVal) ? minVal : 
                 (*value > maxVal) ? maxVal : *value;
    }
    
    // Добавляем в список
    slider->next = manager->firstSlider;
    manager->firstSlider = slider;
    
    return slider;
}

// Создание чекбокса
WidgetCheckbox* widget_create_checkbox(WidgetManager* manager, float x, float y,
                                      float size, const char* label, bool* value) {
    WidgetCheckbox* checkbox = (WidgetCheckbox*)malloc(sizeof(WidgetCheckbox));
    if (!checkbox) return NULL;
    
    checkbox->rect.x = x;
    checkbox->rect.y = y;
    checkbox->rect.width = size;
    checkbox->rect.height = size;
    
    checkbox->label = (char*)malloc(strlen(label) + 1);
    if (checkbox->label) {
        strcpy(checkbox->label, label);
    }
    
    checkbox->value = value;
    checkbox->boxColor = rgb(62, 62, 66);
    checkbox->checkColor = rgb(0, 122, 204);
    
    // Инициализируем значение, если указатель не NULL
    if (value) {
        *value = *value ? true : false;
    }
    
    // Добавляем в список
    checkbox->next = manager->firstCheckbox;
    manager->firstCheckbox = checkbox;
    
    return checkbox;
}

// Создание текстового поля
WidgetTextInput* widget_create_text_input(WidgetManager* manager, float x, float y,
                                         float width, float height, char* text,
                                         int maxLength) {
    WidgetTextInput* input = (WidgetTextInput*)malloc(sizeof(WidgetTextInput));
    if (!input) return NULL;
    
    input->rect.x = x;
    input->rect.y = y;
    input->rect.width = width;
    input->rect.height = height;
    
    input->text = text;
    input->maxLength = maxLength;
    input->isFocused = false;
    input->bgColor = rgb(37, 37, 38);
    input->textColor = rgb(255, 255, 255);
    input->borderColor = rgb(62, 62, 66);
    
    // Добавляем в список
    input->next = manager->firstTextInput;
    manager->firstTextInput = input;
    
    return input;
}

// Создание панели
WidgetPanel* widget_create_panel(WidgetManager* manager, float x, float y,
                                float width, float height, const char* title) {
    WidgetPanel* panel = (WidgetPanel*)malloc(sizeof(WidgetPanel));
    if (!panel) return NULL;
    
    panel->rect.x = x;
    panel->rect.y = y;
    panel->rect.width = width;
    panel->rect.height = height;
    
    panel->title = (char*)malloc(strlen(title) + 1);
    if (panel->title) {
        strcpy(panel->title, title);
    }
    
    panel->bgColor = rgb(45, 45, 48);
    panel->borderColor = rgb(62, 62, 66);
    
    // Добавляем в список
    panel->next = manager->firstPanel;
    manager->firstPanel = panel;
    
    return panel;
}

// Создание метки
WidgetLabel* widget_create_label(WidgetManager* manager, float x, float y,
                                const char* text) {
    WidgetLabel* label = (WidgetLabel*)malloc(sizeof(WidgetLabel));
    if (!label) return NULL;
    
    label->rect.x = x;
    label->rect.y = y;
    label->rect.width = 0; // Ширина вычисляется при рендеринге
    label->rect.height = 20; // Высота по умолчанию
    
    label->text = (char*)malloc(strlen(text) + 1);
    if (label->text) {
        strcpy(label->text, text);
    }
    
    label->color = manager->textColor;
    
    // Добавляем в список
    label->next = manager->firstLabel;
    manager->firstLabel = label;
    
    return label;
}

// Получение текущего активного текстового поля
WidgetTextInput* widget_get_focused_text_input(WidgetManager* manager) {
    WidgetTextInput* input = manager->firstTextInput;
    while (input) {
        if (input->isFocused) {
            return input;
        }
        input = input->next;
    }
    return NULL;
}

// Обновление состояния виджетов
void widget_update(WidgetManager* manager) {
    if (!manager || !manager->isEnabled) return;
    
    // Обновляем позицию мыши
    glfwGetCursorPos(manager->window, &manager->mouseX, &manager->mouseY);
    
    // Конвертируем в координаты OpenGL (0..width, 0..height)
    int width, height;
    glfwGetWindowSize(manager->window, &width, &height);
    float mouseX = (float)manager->mouseX;
    float mouseY = (float)manager->mouseY;
    
    // Проверяем hover для кнопок
    WidgetButton* button = manager->firstButton;
    while (button) {
        button->isHovered = widget_point_in_rect(mouseX, mouseY, button->rect);
        button = button->next;
    }
    
    // Проверяем hover для чекбоксов
    WidgetCheckbox* checkbox = manager->firstCheckbox;
    while (checkbox) {
        // Просто обновляем без hover состояния
        checkbox = checkbox->next;
    }
    
    // Проверяем hover для текстовых полей
    WidgetTextInput* textInput = manager->firstTextInput;
    while (textInput) {
        // Просто обновляем без hover состояния
        textInput = textInput->next;
    }
    
    // Обновляем ползунки, если идет перетаскивание
    WidgetSlider* slider = manager->firstSlider;
    while (slider) {
        if (slider->isDragging) {
            if (manager->mouseButtons[0]) { // Левая кнопка мыши нажата
                float normalized = (mouseX - slider->rect.x) / slider->rect.width;
                normalized = fmaxf(0.0f, fminf(1.0f, normalized));
                
                if (slider->value) {
                    *slider->value = slider->minValue + 
                                    normalized * (slider->maxValue - slider->minValue);
                }
            } else {
                slider->isDragging = false;
            }
        }
        slider = slider->next;
    }
    
    // Сохраняем предыдущие состояния
    memcpy(manager->prevMouseButtons, manager->mouseButtons, sizeof(manager->mouseButtons));
    memcpy(manager->prevKeys, manager->keys, sizeof(manager->keys));
}

// Обработка событий мыши
void widget_handle_mouse(WidgetManager* manager, int button, int action) {
    if (!manager || !manager->isEnabled) return;
    
    if (button >= 0 && button < 3) {
        manager->mouseButtons[button] = (action == GLFW_PRESS);
    }
    
    if (action == GLFW_PRESS && button == 0) { // Левая кнопка мыши нажата
        float mouseX = (float)manager->mouseX;
        float mouseY = (float)manager->mouseY;
        
        // Обработка кнопок
        WidgetButton* btn = manager->firstButton;
        while (btn) {
            if (widget_point_in_rect(mouseX, mouseY, btn->rect)) {
                btn->isPressed = true;
                if (btn->onClick) {
                    btn->onClick();
                }
            }
            btn = btn->next;
        }
        
        // Обработка чекбоксов
        WidgetCheckbox* checkbox = manager->firstCheckbox;
        while (checkbox) {
            if (widget_point_in_rect(mouseX, mouseY, checkbox->rect)) {
                if (checkbox->value) {
                    *checkbox->value = !(*checkbox->value);
                }
            }
            checkbox = checkbox->next;
        }
        
        // Обработка текстовых полей
        WidgetTextInput* textInput = manager->firstTextInput;
        while (textInput) {
            bool wasFocused = textInput->isFocused;
            textInput->isFocused = widget_point_in_rect(mouseX, mouseY, textInput->rect);
            
            // Если кликнули вне текстового поля, снимаем фокус
            if (wasFocused && !textInput->isFocused) {
                textInput->isFocused = false;
            }
            
            textInput = textInput->next;
        }
        
        // Обработка ползунков
        WidgetSlider* slider = manager->firstSlider;
        while (slider) {
            if (widget_point_in_rect(mouseX, mouseY, slider->rect)) {
                slider->isDragging = true;
            }
            slider = slider->next;
        }
    }
    
    if (action == GLFW_RELEASE && button == 0) { // Левая кнопка мыши отпущена
        // Сбрасываем состояние нажатия у кнопок
        WidgetButton* btn = manager->firstButton;
        while (btn) {
            btn->isPressed = false;
            btn = btn->next;
        }
    }
}

// Обработка событий клавиатуры
void widget_handle_keyboard(WidgetManager* manager, int key, int action) {
    if (!manager || !manager->isEnabled) return;
    
    if (key >= 0 && key < GLFW_KEY_LAST) {
        manager->keys[key] = (action == GLFW_PRESS || action == GLFW_REPEAT);
    }
    
    // Обработка текстового ввода
    WidgetTextInput* focusedInput = widget_get_focused_text_input(manager);
    if (focusedInput && focusedInput->text) {
        if (action == GLFW_PRESS || action == GLFW_REPEAT) {
            // Обработка Backspace
            if (key == GLFW_KEY_BACKSPACE) {
                int len = (int)strlen(focusedInput->text);
                if (len > 0) {
                    focusedInput->text[len - 1] = '\0';
                }
            }
            // Обработка Enter (снимаем фокус)
            else if (key == GLFW_KEY_ENTER || key == GLFW_KEY_ESCAPE) {
                focusedInput->isFocused = false;
            }
        }
    }
}

// Обработка ввода текста
void widget_handle_text_input(WidgetManager* manager, unsigned int codepoint) {
    if (!manager || !manager->isEnabled) return;
    
    WidgetTextInput* focusedInput = widget_get_focused_text_input(manager);
    if (focusedInput && focusedInput->text) {
        int len = (int)strlen(focusedInput->text);
        if (len < focusedInput->maxLength - 1) {
            // Конвертируем кодпоинт в символ (упрощенно)
            char c = (char)codepoint;
            if (c >= 32 && c <= 126) { // Только печатные символы
                focusedInput->text[len] = c;
                focusedInput->text[len + 1] = '\0';
            }
        }
    }
}

// Рендеринг прямоугольника (упрощенный - без шейдеров)
void widget_render_rect(WidgetManager* manager, WidgetRect rect, RGB color) {
    glBegin(GL_QUADS);
    glColor3f(color.R, color.G, color.B);
    glVertex2f(rect.x, rect.y);
    glVertex2f(rect.x + rect.width, rect.y);
    glVertex2f(rect.x + rect.width, rect.y + rect.height);
    glVertex2f(rect.x, rect.y + rect.height);
    glEnd();
}

// Рендеринг текста (упрощенный - через glBitmap или примитивы)
void widget_render_text(WidgetManager* manager, float x, float y, 
                       const char* text, RGB color) {
    // Упрощенный рендеринг текста - используем точки для демонстрации
    // В реальном проекте лучше использовать FreeType или bitmap-шрифты
    
    glColor3f(color.R, color.G, color.B);
    glRasterPos2f(x, y + 15); // Небольшое смещение для лучшего отображения
    
    // Простой вывод через glutBitmapCharacter (если есть GLUT)
    // Для простоты просто не выводим текст или используем точки
    
    // Альтернатива: рисуем прямоугольники для каждого символа
    float charWidth = 8.0f * manager->fontScale;
    float charHeight = 12.0f * manager->fontScale;
    
    for (int i = 0; text[i] != '\0'; i++) {
        // Пропускаем сложный рендеринг символов
        // В реальном проекте здесь должен быть код отрисовки символов
    }
}

// Рендеринг границы
void widget_render_border(WidgetManager* manager, WidgetRect rect, 
                         RGB color, float thickness) {
    // Верхняя граница
    WidgetRect top = {rect.x, rect.y, rect.width, thickness};
    widget_render_rect(manager, top, color);
    
    // Нижняя граница
    WidgetRect bottom = {rect.x, rect.y + rect.height - thickness, 
                        rect.width, thickness};
    widget_render_rect(manager, bottom, color);
    
    // Левая граница
    WidgetRect left = {rect.x, rect.y, thickness, rect.height};
    widget_render_rect(manager, left, color);
    
    // Правая граница
    WidgetRect right = {rect.x + rect.width - thickness, rect.y, 
                       thickness, rect.height};
    widget_render_rect(manager, right, color);
}

// Рендеринг всех виджетов
void widget_render(WidgetManager* manager) {
    if (!manager || !manager->isEnabled) return;
    
    // Сохраняем состояние OpenGL
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    
    // Устанавливаем ортографическую проекцию
    int width, height;
    glfwGetWindowSize(manager->window, &width, &height);
    glOrtho(0, width, height, 0, -1, 1);
    
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    
    // Отключаем тесты глубины для 2D рендеринга
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // Рендерим панели
    WidgetPanel* panel = manager->firstPanel;
    while (panel) {
        // Фон панели
        widget_render_rect(manager, panel->rect, panel->bgColor);
        
        // Граница панели
        widget_render_border(manager, panel->rect, panel->borderColor, 1.0f);
        
        // Заголовок панели
        if (panel->title) {
            widget_render_text(manager, panel->rect.x + 10, panel->rect.y + 5, 
                             panel->title, manager->textColor);
        }
        
        panel = panel->next;
    }
    
    // Рендерим метки
    WidgetLabel* label = manager->firstLabel;
    while (label) {
        if (label->text) {
            widget_render_text(manager, label->rect.x, label->rect.y, 
                             label->text, label->color);
        }
        label = label->next;
    }
    
    // Рендерим кнопки
    WidgetButton* button = manager->firstButton;
    while (button) {
        // Определяем цвет кнопки
        RGB buttonColor = button->normalColor;
        if (button->isPressed) {
            buttonColor = button->pressedColor;
        } else if (button->isHovered) {
            buttonColor = button->hoverColor;
        }
        
        // Фон кнопки
        widget_render_rect(manager, button->rect, buttonColor);
        
        // Граница кнопки
        widget_render_border(manager, button->rect, rgb(100, 100, 100), 1.0f);
        
        // Текст кнопки (центрированный)
        if (button->label) {
            float textX = button->rect.x + button->rect.width / 2 - 
                         (strlen(button->label) * 4); // Приблизительное центрирование
            float textY = button->rect.y + button->rect.height / 2 - 6;
            widget_render_text(manager, textX, textY, button->label, manager->textColor);
        }
        
        button = button->next;
    }
    
    // Рендерим ползунки
    WidgetSlider* slider = manager->firstSlider;
    while (slider) {
        // Трек ползунка
        WidgetRect trackRect = {
            slider->rect.x,
            slider->rect.y + slider->rect.height / 2 - 2,
            slider->rect.width,
            4
        };
        widget_render_rect(manager, trackRect, slider->trackColor);
        
        // Ползунок (thumb)
        if (slider->value) {
            float normalized = (*slider->value - slider->minValue) / 
                             (slider->maxValue - slider->minValue);
            normalized = fmaxf(0.0f, fminf(1.0f, normalized));
            
            float thumbX = slider->rect.x + normalized * slider->rect.width - 5;
            WidgetRect thumbRect = {
                thumbX,
                slider->rect.y,
                10,
                slider->rect.height
            };
            widget_render_rect(manager, thumbRect, slider->thumbColor);
        }
        
        // Метка ползунка
        if (slider->label) {
            widget_render_text(manager, slider->rect.x, slider->rect.y - 15, 
                             slider->label, manager->textColor);
        }
        
        // Значение ползунка
        if (slider->value) {
            char valueText[32];
            snprintf(valueText, sizeof(valueText), "%.2f", *slider->value);
            widget_render_text(manager, slider->rect.x + slider->rect.width + 10, 
                             slider->rect.y, valueText, manager->textColor);
        }
        
        slider = slider->next;
    }
    
    // Рендерим чекбоксы
    WidgetCheckbox* checkbox = manager->firstCheckbox;
    while (checkbox) {
        // Фон чекбокса
        widget_render_rect(manager, checkbox->rect, checkbox->boxColor);
        
        // Граница чекбокса
        widget_render_border(manager, checkbox->rect, rgb(100, 100, 100), 1.0f);
        
        // Галочка
        if (checkbox->value && *checkbox->value) {
            WidgetRect checkRect = {
                checkbox->rect.x + 2,
                checkbox->rect.y + 2,
                checkbox->rect.width - 4,
                checkbox->rect.height - 4
            };
            widget_render_rect(manager, checkRect, checkbox->checkColor);
        }
        
        // Метка чекбокса
        if (checkbox->label) {
            widget_render_text(manager, checkbox->rect.x + checkbox->rect.width + 5, 
                             checkbox->rect.y, checkbox->label, manager->textColor);
        }
        
        checkbox = checkbox->next;
    }
    
    // Рендерим текстовые поля
    WidgetTextInput* textInput = manager->firstTextInput;
    while (textInput) {
        // Фон текстового поля
        widget_render_rect(manager, textInput->rect, textInput->bgColor);
        
        // Граница (толще если поле в фокусе)
        RGB borderColor = textInput->isFocused ? rgb(0, 122, 204) : textInput->borderColor;
        float borderWidth = textInput->isFocused ? 2.0f : 1.0f;
        widget_render_border(manager, textInput->rect, borderColor, borderWidth);
        
        // Текст
        if (textInput->text) {
            widget_render_text(manager, textInput->rect.x + 5, 
                             textInput->rect.y + 5, textInput->text, textInput->textColor);
        }
        
        // Курсор (если поле в фокусе)
        if (textInput->isFocused) {
            static double cursorBlinkTime = 0;
            cursorBlinkTime += 0.01; // Упрощенная анимация
            
            if (fmod(cursorBlinkTime, 1.0) < 0.5) {
                float cursorX = textInput->rect.x + 5;
                if (textInput->text) {
                    cursorX += strlen(textInput->text) * 8; // Приблизительная ширина символа
                }
                
                WidgetRect cursorRect = {
                    cursorX,
                    textInput->rect.y + 3,
                    2,
                    textInput->rect.height - 6
                };
                widget_render_rect(manager, cursorRect, rgb(255, 255, 255));
            }
        }
        
        textInput = textInput->next;
    }
    
    // Восстанавливаем состояние OpenGL
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glPopAttrib();
    
    // Включаем тест глубины обратно
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
}

// Очистка всех виджетов
void widget_clear_all(WidgetManager* manager) {
    if (!manager) return;
    
    // Очищаем кнопки
    WidgetButton* button = manager->firstButton;
    while (button) {
        WidgetButton* next = button->next;
        free(button->label);
        free(button);
        button = next;
    }
    manager->firstButton = NULL;
    
    // Очищаем ползунки
    WidgetSlider* slider = manager->firstSlider;
    while (slider) {
        WidgetSlider* next = slider->next;
        free(slider->label);
        free(slider);
        slider = next;
    }
    manager->firstSlider = NULL;
    
    // Очищаем чекбоксы
    WidgetCheckbox* checkbox = manager->firstCheckbox;
    while (checkbox) {
        WidgetCheckbox* next = checkbox->next;
        free(checkbox->label);
        free(checkbox);
        checkbox = next;
    }
    manager->firstCheckbox = NULL;
    
    // Очищаем текстовые поля
    WidgetTextInput* textInput = manager->firstTextInput;
    while (textInput) {
        WidgetTextInput* next = textInput->next;
        free(textInput);
        textInput = next;
    }
    manager->firstTextInput = NULL;
    
    // Очищаем панели
    WidgetPanel* panel = manager->firstPanel;
    while (panel) {
        WidgetPanel* next = panel->next;
        free(panel->title);
        free(panel);
        panel = next;
    }
    manager->firstPanel = NULL;
    
    // Очищаем метки
    WidgetLabel* label = manager->firstLabel;
    while (label) {
        WidgetLabel* next = label->next;
        free(label->text);
        free(label);
        label = next;
    }
    manager->firstLabel = NULL;
}

// Включение/отключение виджетов
void widget_set_enabled(WidgetManager* manager, bool enabled) {
    if (manager) {
        manager->isEnabled = enabled;
    }
}

// Установка позиции мыши (вызывается извне)
void widget_set_mouse_position(WidgetManager* manager, double x, double y) {
    if (manager) {
        manager->mouseX = x;
        manager->mouseY = y;
    }
}

// Установка состояния кнопки мыши
void widget_set_mouse_button(WidgetManager* manager, int button, bool pressed) {
    if (manager && button >= 0 && button < 3) {
        manager->mouseButtons[button] = pressed;
    }
}

// Установка состояния клавиши
void widget_set_key(WidgetManager* manager, int key, bool pressed) {
    if (manager && key >= 0 && key < GLFW_KEY_LAST) {
        manager->keys[key] = pressed;
    }
}

#endif // WIDGET_H