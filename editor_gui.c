#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <stdlib.h>
#include <stdbool.h>

#define NK_IMPLEMENTATION
#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_BUTTON_TRIGGER_ON_RELEASE
#define NK_GLFW_GL3_IMPLEMENTATION

#include <nuklear.h>
#include "nuklear_glfw_gl3.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

// Наши заголовочные файлы
#include "engine/app_state.h"
#include "engine/cursor_generator.h"
#include "engine/window_manager.h"

// Определение глобального состояния
AppState g_state = {0};

// Функция для рисования стилизованной кнопки с наложением линий
static void draw_styled_button(struct nk_command_buffer* canvas, float x, float y, float w, float h,
                               struct nk_color base_color, int is_pressed, int is_hover, const char* text,
                               struct nk_user_font* font) {
    // Основной фон с градиентом для объёма
    struct nk_color grad_top = base_color;
    struct nk_color grad_bottom = nk_rgb(
        (unsigned char)(base_color.r * 0.7f),
        (unsigned char)(base_color.g * 0.7f),
        (unsigned char)(base_color.b * 0.7f)
    );
    
    // Вертикальный градиент
    for (int i = 0; i < (int)h; i++) {
        float t = (float)i / h;
        struct nk_color line_color = nk_rgb(
            (unsigned char)(grad_top.r * (1 - t) + grad_bottom.r * t),
            (unsigned char)(grad_top.g * (1 - t) + grad_bottom.g * t),
            (unsigned char)(grad_top.b * (1 - t) + grad_bottom.b * t)
        );
        nk_fill_rect(canvas, nk_rect(x, y + i, w, 1), 0, line_color);
    }
    
    // Эффект нажатия (сдвиг)
    if (is_pressed) {
        // Затемняем кнопку
        nk_fill_rect(canvas, nk_rect(x, y, w, h), 4, nk_rgba(0, 0, 0, 50));
    }
    
    // Стилизованное наложение линий (ретро-эффект)
    if (!is_pressed) {
        // Горизонтальные линии
        for (float ly = y + 4; ly < y + h - 4; ly += 4) {
            nk_stroke_line(canvas, x + 4, ly, x + w - 4, ly, 1.0f, nk_rgba(255, 255, 255, 30));
        }
        
        // Вертикальные линии
        for (float lx = x + 4; lx < x + w - 4; lx += 4) {
            nk_stroke_line(canvas, lx, y + 4, lx, y + h - 4, 1.0f, nk_rgba(255, 255, 255, 20));
        }
    }
    
    // Контур с фаской (объёмный эффект)
    if (is_pressed) {
        // Вдавленный эффект
        nk_stroke_rect(canvas, nk_rect(x, y, w, h), 4, 1.0f, nk_rgba(0, 0, 0, 150));
        nk_stroke_line(canvas, x + 1, y + 1, x + w - 2, y + 1, 1.0f, nk_rgba(100, 100, 100, 100));
        nk_stroke_line(canvas, x + 1, y + 1, x + 1, y + h - 2, 1.0f, nk_rgba(100, 100, 100, 100));
    } else {
        // Выпуклый эффект
        nk_stroke_rect(canvas, nk_rect(x, y, w, h), 4, 1.0f, nk_rgba(255, 255, 255, 150));
        nk_stroke_line(canvas, x + 1, y + h - 1, x + w - 1, y + h - 1, 1.0f, nk_rgba(0, 0, 0, 100));
        nk_stroke_line(canvas, x + w - 1, y + 1, x + w - 1, y + h - 1, 1.0f, nk_rgba(0, 0, 0, 100));
    }
    
    // Вычисляем ширину текста через функцию шрифта
    float total_width = 0;
    for (int i = 0; i < strlen(text); i++) {
        // Используем функцию шрифта для получения ширины каждого символа
        char ch[2] = {text[i], '\0'};
        total_width += font->width(font->userdata, font->height, ch, 1);
    }
    
    float start_x = x + (w - total_width) / 2;
    float text_y = y + (h - font->height) / 2;
    
    // Рисуем каждый символ отдельно
    for (int i = 0; i < strlen(text); i++) {
        char ch[2] = {text[i], '\0'};
        float char_width = font->width(font->userdata, font->height, ch, 1);
        float char_x = start_x;
        
        for (int j = 0; j < i; j++) {
            char prev_ch[2] = {text[j], '\0'};
            char_x += font->width(font->userdata, font->height, prev_ch, 1);
        }
        
        // Тень текста
        nk_draw_text(canvas, nk_rect(char_x + 1, text_y + 1, char_width, font->height),
                    ch, 1, font, nk_rgba(0, 0, 0, 100), nk_rgba(0, 0, 0, 0));
        
        // Основной текст
        nk_draw_text(canvas, nk_rect(char_x, text_y, char_width, font->height),
                    ch, 1, font, nk_rgb(255, 255, 255), nk_rgba(0, 0, 0, 0));
    }
}

// Функция для рисования стилизованной иконки без специальных символов
static void draw_icon(struct nk_command_buffer* canvas, float x, float y, float w, float h,
                      const char* icon_type, struct nk_color color, int is_pressed, int is_hover) {
    // Рисуем круглую подложку
    struct nk_color bg_color = color;
    if (is_pressed) {
        bg_color = nk_rgb(100, 100, 255);
    } else if (is_hover) {
        bg_color = nk_rgb(85, 85, 85);
    }
    
    nk_fill_circle(canvas, nk_rect(x, y, w, h), bg_color);
    
    // Добавляем объёмный эффект
    nk_stroke_circle(canvas, nk_rect(x, y, w, h), 1.0f, nk_rgba(255, 255, 255, 100));
    nk_stroke_circle(canvas, nk_rect(x + 1, y + 1, w - 2, h - 2), 1.0f, nk_rgba(0, 0, 0, 50));
    
    // Рисуем простые геометрические фигуры вместо символов
    float cx = x + w/2;
    float cy = y + h/2;
    
    if (strcmp(icon_type, "close") == 0) {
        // Крестик из двух линий
        nk_stroke_line(canvas, cx - 4, cy - 4, cx + 4, cy + 4, 2.0f, nk_rgb(255, 255, 255));
        nk_stroke_line(canvas, cx + 4, cy - 4, cx - 4, cy + 4, 2.0f, nk_rgb(255, 255, 255));
    } else if (strcmp(icon_type, "maximize") == 0) {
        // Квадрат для максимизации
        nk_stroke_rect(canvas, nk_rect(cx - 5, cy - 5, 10, 10), 2, 2.0f, nk_rgb(255, 255, 255));
    } else if (strcmp(icon_type, "minimize") == 0) {
        // Горизонтальная линия для минимизации
        nk_stroke_line(canvas, cx - 6, cy, cx + 6, cy, 2.0f, nk_rgb(255, 255, 255));
    } else if (strcmp(icon_type, "menu") == 0) {
        // Три горизонтальные линии для меню
        nk_stroke_line(canvas, cx - 4, cy - 3, cx + 4, cy - 3, 2.0f, nk_rgb(255, 255, 255));
        nk_stroke_line(canvas, cx - 4, cy, cx + 4, cy, 2.0f, nk_rgb(255, 255, 255));
        nk_stroke_line(canvas, cx - 4, cy + 3, cx + 4, cy + 3, 2.0f, nk_rgb(255, 255, 255));
    }
}

int main(void) {
    printf("Starting application...\n");
    
    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return -1;
    }
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Mirulit Game Engine", NULL, NULL);
    if (!window) {
        fprintf(stderr, "Failed to create GLFW window\n");
        glfwTerminate();
        return -1;
    }
    
    const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    if (mode) {
        int x = (mode->width - 1280) / 2;
        int y = (mode->height - 720) / 2;
        glfwSetWindowPos(window, x, y);
    }
    
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    
    /* ГЕНЕРИРУЕМ ОБЪЁМНЫЕ КУРСОРЫ */
    printf("Generating 3D custom cursors...\n");
    
    int cursor_size = 32;
    
    g_state.cursor_arrow = generate_arrow_cursor(cursor_size);
    g_state.cursor_hand = generate_hand_cursor(cursor_size);
    g_state.cursor_move = generate_move_cursor(cursor_size);
    g_state.cursor_text = generate_text_cursor(cursor_size);
    g_state.cursor_crosshair = generate_crosshair_cursor(cursor_size);
    g_state.cursor_wait = generate_wait_cursor(cursor_size);
    g_state.cursor_help = generate_help_cursor(cursor_size);
    
    glfwSetCursor(window, g_state.cursor_arrow);
    g_state.current_cursor = 0;
    
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, cursor_pos_callback);
    
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        fprintf(stderr, "Failed to initialize GLAD\n");
        glfwTerminate();
        return -1;
    }
    
    struct nk_glfw glfw_state = {0};
    struct nk_context* ctx = nk_glfw3_init(&glfw_state, window, NK_GLFW3_DEFAULT);
    if (!ctx) {
        fprintf(stderr, "Failed to initialize Nuklear\n");
        glfwTerminate();
        return -1;
    }
    
    struct nk_font_atlas* atlas;
    nk_glfw3_font_stash_begin(&glfw_state, &atlas);
    
    struct nk_font_config config = nk_font_config(13);
    config.range = nk_font_default_glyph_ranges();
    struct nk_font* font = nk_font_atlas_add_default(atlas, 13, &config);
    
    nk_glfw3_font_stash_end(&glfw_state);
    
    if (font) {
        nk_style_set_font(ctx, &font->handle);
    }
    
    // Кастомная тема в стиле 2000-х
    struct nk_color table[NK_COLOR_COUNT];
    table[NK_COLOR_TEXT] = nk_rgb(220, 220, 220);
    table[NK_COLOR_WINDOW] = nk_rgb(56, 56, 56);
    table[NK_COLOR_HEADER] = nk_rgb(41, 41, 41);
    table[NK_COLOR_BORDER] = nk_rgb(80, 80, 80);
    table[NK_COLOR_BUTTON] = nk_rgb(65, 65, 65);
    table[NK_COLOR_BUTTON_HOVER] = nk_rgb(85, 85, 85);
    table[NK_COLOR_BUTTON_ACTIVE] = nk_rgb(100, 100, 255);
    nk_style_from_table(ctx, table);
    
    printf("Application started with 3D interface and styled buttons!\n");
    
    while (!glfwWindowShouldClose(window)) {
        
        glfwPollEvents();
        
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        
        nk_glfw3_new_frame(&glfw_state);
        
        glViewport(0, 0, width, height);
        glClearColor(0.1f, 0.1f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        #include "engine/title_bar.h"
        titlebar(ctx, width, height);
        
        #include "engine/hub.h"
        Hub(ctx, width, height);
        
        nk_glfw3_render(&glfw_state, NK_ANTI_ALIASING_ON, 1024*1024, 256*1024);
        glfwSwapBuffers(window);
    }
    
    // Очищаем курсоры
    if (g_state.cursor_arrow) glfwDestroyCursor(g_state.cursor_arrow);
    if (g_state.cursor_hand) glfwDestroyCursor(g_state.cursor_hand);
    if (g_state.cursor_move) glfwDestroyCursor(g_state.cursor_move);
    if (g_state.cursor_text) glfwDestroyCursor(g_state.cursor_text);
    if (g_state.cursor_crosshair) glfwDestroyCursor(g_state.cursor_crosshair);
    if (g_state.cursor_wait) glfwDestroyCursor(g_state.cursor_wait);
    if (g_state.cursor_help) glfwDestroyCursor(g_state.cursor_help);
    
    nk_glfw3_shutdown(&glfw_state);
    glfwDestroyWindow(window);
    glfwTerminate();
    
    return 0;
}