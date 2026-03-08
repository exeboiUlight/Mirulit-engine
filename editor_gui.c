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

#include "engine/pritevis.h"

bool MakeProject = false;

int main(void) {
    
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
    
    struct nk_color table[NK_COLOR_COUNT];
    table[NK_COLOR_TEXT] = nk_rgb(220, 220, 220);
    table[NK_COLOR_WINDOW] = nk_rgb(56, 56, 56);
    table[NK_COLOR_HEADER] = nk_rgb(41, 41, 41);
    table[NK_COLOR_BORDER] = nk_rgb(80, 80, 80);
    table[NK_COLOR_BUTTON] = nk_rgb(65, 65, 65);
    table[NK_COLOR_BUTTON_HOVER] = nk_rgb(85, 85, 85);
    table[NK_COLOR_BUTTON_ACTIVE] = nk_rgb(100, 100, 255);
    nk_style_from_table(ctx, table);
    
    while (!glfwWindowShouldClose(window)) {
        
        glfwPollEvents();
        
        int width, height;
        glfwGetWindowSize(window, &width, &height);
        
        nk_glfw3_new_frame(&glfw_state);
        
        glViewport(0, 0, width, height);
        glClearColor(0.1f, 0.1f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        #include "engine/title_bar.h"
        titlebar(ctx, width, height);
        
        #include "engine/hub.h"
        if (!MakeProject) Hub(ctx, width, height);
        if (MakeProject) CreateProjectWindow(ctx, width, height);
        
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