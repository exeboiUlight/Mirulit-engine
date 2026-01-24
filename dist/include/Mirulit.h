// Mirulit Engine v1.0 - Pure C
// Single-header OpenGL/GLFW game engine
// License: MIT

#ifndef MIRULIT_H
#define MIRULIT_H

#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// ============================================================================
// Constants and Macros
// ============================================================================

#define MU_PI 3.14159265358979323846f
#define MU_RAD_TO_DEG (180.0f / MU_PI)
#define MU_DEG_TO_RAD (MU_PI / 180.0f)

#define MU_ASSERT(cond, msg) do { \
    if (!(cond)) { \
        fprintf(stderr, "Assertion failed: %s (%s:%d)\n", msg, __FILE__, __LINE__); \
        exit(1); \
    } \
} while(0)

// Key codes matching GLFW
#define MU_KEY_SPACE          32
#define MU_KEY_ESCAPE         256
#define MU_KEY_ENTER          257
#define MU_KEY_TAB            258
#define MU_KEY_BACKSPACE      259
#define MU_KEY_INSERT         260
#define MU_KEY_DELETE         261
#define MU_KEY_RIGHT          262
#define MU_KEY_LEFT           263
#define MU_KEY_DOWN           264
#define MU_KEY_UP             265
#define MU_KEY_A              65
#define MU_KEY_B              66
#define MU_KEY_C              67
#define MU_KEY_D              68
#define MU_KEY_E              69
#define MU_KEY_F              70
#define MU_KEY_G              71
#define MU_KEY_H              72
#define MU_KEY_I              73
#define MU_KEY_J              74
#define MU_KEY_K              75
#define MU_KEY_L              76
#define MU_KEY_M              77
#define MU_KEY_N              78
#define MU_KEY_O              79
#define MU_KEY_P              80
#define MU_KEY_Q              81
#define MU_KEY_R              82
#define MU_KEY_S              83
#define MU_KEY_T              84
#define MU_KEY_U              85
#define MU_KEY_V              86
#define MU_KEY_W              87
#define MU_KEY_X              88
#define MU_KEY_Y              89
#define MU_KEY_Z              90
#define MU_KEY_0              48
#define MU_KEY_1              49
#define MU_KEY_2              50
#define MU_KEY_3              51
#define MU_KEY_4              52
#define MU_KEY_5              53
#define MU_KEY_6              54
#define MU_KEY_7              55
#define MU_KEY_8              56
#define MU_KEY_9              57

#define MU_MOUSE_BUTTON_LEFT  0
#define MU_MOUSE_BUTTON_RIGHT 1
#define MU_MOUSE_BUTTON_MIDDLE 2

// ============================================================================
// Basic Types
// ============================================================================

typedef struct {
    float x, y;
} MU_Vec2;

typedef struct {
    float x, y, z;
} MU_Vec3;

typedef struct {
    float r, g, b, a;
} MU_Color;

typedef struct {
    float x, y, width, height;
} MU_Rect;

typedef struct {
    float m[16];
} MU_Mat4;

typedef struct MU_Window MU_Window;
typedef struct MU_Shader MU_Shader;
typedef struct MU_Texture2D MU_Texture2D;
typedef struct MU_Sprite MU_Sprite;
typedef struct MU_BatchRenderer MU_BatchRenderer;
typedef struct MU_Camera2D MU_Camera2D;
typedef struct MU_Application MU_Application;

// ============================================================================
// Vector and Matrix Functions
// ============================================================================

static inline MU_Vec2 mu_vec2(float x, float y) {
    MU_Vec2 v = {x, y};
    return v;
}

static inline MU_Vec2 mu_vec2_add(MU_Vec2 a, MU_Vec2 b) {
    return mu_vec2(a.x + b.x, a.y + b.y);
}

static inline MU_Vec2 mu_vec2_sub(MU_Vec2 a, MU_Vec2 b) {
    return mu_vec2(a.x - b.x, a.y - b.y);
}

static inline MU_Vec2 mu_vec2_mul(MU_Vec2 v, float s) {
    return mu_vec2(v.x * s, v.y * s);
}

static inline float mu_vec2_length(MU_Vec2 v) {
    return sqrtf(v.x * v.x + v.y * v.y);
}

static inline MU_Vec2 mu_vec2_normalize(MU_Vec2 v) {
    float len = mu_vec2_length(v);
    if (len > 0.0f) {
        return mu_vec2_mul(v, 1.0f / len);
    }
    return mu_vec2(0.0f, 0.0f);
}

static inline MU_Color mu_color(float r, float g, float b, float a) {
    MU_Color c = {r, g, b, a};
    return c;
}

static inline MU_Rect mu_rect(float x, float y, float width, float height) {
    MU_Rect r = {x, y, width, height};
    return r;
}

static inline bool mu_rect_contains(MU_Rect rect, float x, float y) {
    return (x >= rect.x && x <= rect.x + rect.width &&
            y >= rect.y && y <= rect.y + rect.height);
}

static inline MU_Mat4 mu_mat4_identity(void) {
    MU_Mat4 mat = {0};
    mat.m[0] = mat.m[5] = mat.m[10] = mat.m[15] = 1.0f;
    return mat;
}

static inline MU_Mat4 mu_mat4_ortho(float left, float right, float bottom, float top, float near, float far) {
    MU_Mat4 mat = mu_mat4_identity();
    mat.m[0] = 2.0f / (right - left);
    mat.m[5] = 2.0f / (top - bottom);
    mat.m[10] = 2.0f / (far - near);
    mat.m[12] = -(right + left) / (right - left);
    mat.m[13] = -(top + bottom) / (top - bottom);
    mat.m[14] = -(far + near) / (far - near);
    return mat;
}

static inline MU_Mat4 mu_mat4_translate(float x, float y, float z) {
    MU_Mat4 mat = mu_mat4_identity();
    mat.m[12] = x;
    mat.m[13] = y;
    mat.m[14] = z;
    return mat;
}

static inline MU_Mat4 mu_mat4_scale(float x, float y, float z) {
    MU_Mat4 mat = mu_mat4_identity();
    mat.m[0] = x;
    mat.m[5] = y;
    mat.m[10] = z;
    return mat;
}

// ============================================================================
// Time System
// ============================================================================

typedef struct {
    double last_time;
    double delta_time;
    double start_time;
} MU_Time;

static MU_Time mu_time = {0};

static void mu_time_init(void) {
    mu_time.last_time = glfwGetTime();
    mu_time.start_time = mu_time.last_time;
}

static void mu_time_update(void) {
    double current_time = glfwGetTime();
    mu_time.delta_time = current_time - mu_time.last_time;
    mu_time.last_time = current_time;
}

static float mu_time_get_delta(void) {
    return (float)mu_time.delta_time;
}

static float mu_time_get(void) {
    return (float)(glfwGetTime() - mu_time.start_time);
}

// ============================================================================
// Window
// ============================================================================

struct MU_Window {
    GLFWwindow* handle;
    char title[256];
    int width;
    int height;
    bool vsync;
};

static MU_Window* mu_window_create(const char* title, int width, int height) {
    MU_Window* window = (MU_Window*)malloc(sizeof(MU_Window));
    if (!window) return NULL;
    
    memset(window, 0, sizeof(MU_Window));
    strncpy(window->title, title, sizeof(window->title) - 1);
    window->width = width;
    window->height = height;
    window->vsync = true;
    window->handle = NULL;
    
    return window;
}

static bool mu_window_init(MU_Window* window) {
    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return false;
    }
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    
    window->handle = glfwCreateWindow(window->width, window->height, 
                                       window->title, NULL, NULL);
    if (!window->handle) {
        fprintf(stderr, "Failed to create GLFW window\n");
        glfwTerminate();
        return false;
    }
    
    glfwMakeContextCurrent(window->handle);
    
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        fprintf(stderr, "Failed to initialize GLAD\n");
        return false;
    }
    
    glfwSwapInterval(window->vsync ? 1 : 0);
    
    glViewport(0, 0, window->width, window->height);
    
    printf("OpenGL %s, GLSL %s\n", 
           glGetString(GL_VERSION), 
           glGetString(GL_SHADING_LANGUAGE_VERSION));
    
    return true;
}

static void mu_window_destroy(MU_Window* window) {
    if (window) {
        if (window->handle) {
            glfwDestroyWindow(window->handle);
        }
        free(window);
    }
}

static void mu_window_set_vsync(MU_Window* window, bool enabled) {
    window->vsync = enabled;
    glfwSwapInterval(window->vsync ? 1 : 0);
}

static void mu_window_swap_buffers(MU_Window* window) {
    glfwSwapBuffers(window->handle);
}

static void mu_window_poll_events(void) {
    glfwPollEvents();
}

static bool mu_window_should_close(MU_Window* window) {
    return glfwWindowShouldClose(window->handle);
}

static void mu_window_close(MU_Window* window) {
    glfwSetWindowShouldClose(window->handle, GLFW_TRUE);
}

static void mu_window_set_title(MU_Window* window, const char* title) {
    strncpy(window->title, title, sizeof(window->title) - 1);
    glfwSetWindowTitle(window->handle, title);
}

// ============================================================================
// Input System
// ============================================================================

typedef struct {
    MU_Window* window;
    double mouse_x, mouse_y;
    double last_mouse_x, last_mouse_y;
    bool first_mouse;
} MU_Input;

static MU_Input mu_input = {0};

static void mu_input_init(MU_Window* window) {
    mu_input.window = window;
    mu_input.mouse_x = mu_input.mouse_y = 0.0;
    mu_input.last_mouse_x = mu_input.last_mouse_y = 0.0;
    mu_input.first_mouse = true;
    
    glfwSetCursorPosCallback(window->handle, [](GLFWwindow* win, double x, double y) {
        mu_input.last_mouse_x = mu_input.mouse_x;
        mu_input.last_mouse_y = mu_input.mouse_y;
        mu_input.mouse_x = x;
        mu_input.mouse_y = y;
        mu_input.first_mouse = false;
    });
}

static bool mu_input_is_key_pressed(int keycode) {
    MU_ASSERT(mu_input.window, "Input not initialized");
    int state = glfwGetKey(mu_input.window->handle, keycode);
    return state == GLFW_PRESS || state == GLFW_REPEAT;
}

static bool mu_input_is_mouse_button_pressed(int button) {
    MU_ASSERT(mu_input.window, "Input not initialized");
    int state = glfwGetMouseButton(mu_input.window->handle, button);
    return state == GLFW_PRESS;
}

static MU_Vec2 mu_input_get_mouse_position(void) {
    return mu_vec2((float)mu_input.mouse_x, (float)mu_input.mouse_y);
}

static MU_Vec2 mu_input_get_mouse_delta(void) {
    if (mu_input.first_mouse) return mu_vec2(0.0f, 0.0f);
    return mu_vec2((float)(mu_input.mouse_x - mu_input.last_mouse_x),
                   (float)(mu_input.mouse_y - mu_input.last_mouse_y));
}

// ============================================================================
// Shader
// ============================================================================

struct MU_Shader {
    GLuint program_id;
};

static GLuint mu_shader_compile(const char* source, GLenum type) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);
    
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char info_log[512];
        glGetShaderInfoLog(shader, 512, NULL, info_log);
        fprintf(stderr, "Shader compilation failed: %s\n", info_log);
        glDeleteShader(shader);
        return 0;
    }
    return shader;
}

static MU_Shader* mu_shader_create(void) {
    MU_Shader* shader = (MU_Shader*)malloc(sizeof(MU_Shader));
    if (!shader) return NULL;
    shader->program_id = 0;
    return shader;
}

static bool mu_shader_load_default(MU_Shader* shader) {
    const char* vertex_source = 
        "#version 330 core\n"
        "layout(location = 0) in vec2 aPos;\n"
        "layout(location = 1) in vec2 aTexCoord;\n"
        "layout(location = 2) in vec4 aColor;\n"
        "out vec2 vTexCoord;\n"
        "out vec4 vColor;\n"
        "uniform mat4 uProjection;\n"
        "uniform mat4 uModel;\n"
        "void main() {\n"
        "    gl_Position = uProjection * uModel * vec4(aPos, 0.0, 1.0);\n"
        "    vTexCoord = aTexCoord;\n"
        "    vColor = aColor;\n"
        "}\n";
    
    const char* fragment_source = 
        "#version 330 core\n"
        "in vec2 vTexCoord;\n"
        "in vec4 vColor;\n"
        "out vec4 FragColor;\n"
        "uniform sampler2D uTexture;\n"
        "uniform int uUseTexture;\n"
        "void main() {\n"
        "    if (uUseTexture == 1) {\n"
        "        FragColor = texture(uTexture, vTexCoord) * vColor;\n"
        "    } else {\n"
        "        FragColor = vColor;\n"
        "    }\n"
        "}\n";
    
    GLuint vertex_shader = mu_shader_compile(vertex_source, GL_VERTEX_SHADER);
    if (!vertex_shader) return false;
    
    GLuint fragment_shader = mu_shader_compile(fragment_source, GL_FRAGMENT_SHADER);
    if (!fragment_shader) {
        glDeleteShader(vertex_shader);
        return false;
    }
    
    shader->program_id = glCreateProgram();
    glAttachShader(shader->program_id, vertex_shader);
    glAttachShader(shader->program_id, fragment_shader);
    glLinkProgram(shader->program_id);
    
    GLint success;
    glGetProgramiv(shader->program_id, GL_LINK_STATUS, &success);
    if (!success) {
        char info_log[512];
        glGetProgramInfoLog(shader->program_id, 512, NULL, info_log);
        fprintf(stderr, "Shader linking failed: %s\n", info_log);
        glDeleteProgram(shader->program_id);
        shader->program_id = 0;
        return false;
    }
    
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
    
    return true;
}

static void mu_shader_destroy(MU_Shader* shader) {
    if (shader) {
        if (shader->program_id) {
            glDeleteProgram(shader->program_id);
        }
        free(shader);
    }
}

static void mu_shader_bind(MU_Shader* shader) {
    glUseProgram(shader->program_id);
}

static void mu_shader_unbind(void) {
    glUseProgram(0);
}

static void mu_shader_set_int(MU_Shader* shader, const char* name, int value) {
    GLint location = glGetUniformLocation(shader->program_id, name);
    if (location != -1) {
        glUniform1i(location, value);
    }
}

static void mu_shader_set_float(MU_Shader* shader, const char* name, float value) {
    GLint location = glGetUniformLocation(shader->program_id, name);
    if (location != -1) {
        glUniform1f(location, value);
    }
}

static void mu_shader_set_vec2(MU_Shader* shader, const char* name, MU_Vec2 value) {
    GLint location = glGetUniformLocation(shader->program_id, name);
    if (location != -1) {
        glUniform2f(location, value.x, value.y);
    }
}

static void mu_shader_set_color(MU_Shader* shader, const char* name, MU_Color color) {
    GLint location = glGetUniformLocation(shader->program_id, name);
    if (location != -1) {
        glUniform4f(location, color.r, color.g, color.b, color.a);
    }
}

static void mu_shader_set_mat4(MU_Shader* shader, const char* name, MU_Mat4 matrix) {
    GLint location = glGetUniformLocation(shader->program_id, name);
    if (location != -1) {
        glUniformMatrix4fv(location, 1, GL_FALSE, matrix.m);
    }
}

// ============================================================================
// Texture2D
// ============================================================================

struct MU_Texture2D {
    GLuint texture_id;
    int width;
    int height;
    int channels;
};

static MU_Texture2D* mu_texture2d_create(void) {
    MU_Texture2D* texture = (MU_Texture2D*)malloc(sizeof(MU_Texture2D));
    if (!texture) return NULL;
    texture->texture_id = 0;
    texture->width = 0;
    texture->height = 0;
    texture->channels = 0;
    return texture;
}

static bool mu_texture2d_load(MU_Texture2D* texture, const char* path) {
    // Для простоты создаём тестовую текстуру
    // В реальном движке используйте stb_image.h
    unsigned char dummy_data[] = {
        255, 0, 0, 255,   0, 255, 0, 255,
        0, 0, 255, 255,   255, 255, 255, 255
    };
    
    texture->width = 2;
    texture->height = 2;
    texture->channels = 4;
    
    glGenTextures(1, &texture->texture_id);
    glBindTexture(GL_TEXTURE_2D, texture->texture_id);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture->width, texture->height, 
                 0, GL_RGBA, GL_UNSIGNED_BYTE, dummy_data);
    glGenerateMipmap(GL_TEXTURE_2D);
    
    glBindTexture(GL_TEXTURE_2D, 0);
    
    printf("Loaded texture (dummy): %s\n", path);
    return true;
}

static void mu_texture2d_destroy(MU_Texture2D* texture) {
    if (texture) {
        if (texture->texture_id) {
            glDeleteTextures(1, &texture->texture_id);
        }
        free(texture);
    }
}

static void mu_texture2d_bind(MU_Texture2D* texture, int slot) {
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, texture->texture_id);
}

static void mu_texture2d_unbind(void) {
    glBindTexture(GL_TEXTURE_2D, 0);
}

// ============================================================================
// Sprite
// ============================================================================

struct MU_Sprite {
    MU_Vec2 position;
    MU_Vec2 size;
    float rotation;
    MU_Color color;
    MU_Texture2D* texture;
    MU_Rect texture_rect;
};

static MU_Sprite* mu_sprite_create(void) {
    MU_Sprite* sprite = (MU_Sprite*)malloc(sizeof(MU_Sprite));
    if (!sprite) return NULL;
    
    sprite->position = mu_vec2(0.0f, 0.0f);
    sprite->size = mu_vec2(100.0f, 100.0f);
    sprite->rotation = 0.0f;
    sprite->color = mu_color(1.0f, 1.0f, 1.0f, 1.0f);
    sprite->texture = NULL;
    sprite->texture_rect = mu_rect(0.0f, 0.0f, 1.0f, 1.0f);
    
    return sprite;
}

static MU_Sprite* mu_sprite_create_with_texture(MU_Texture2D* texture) {
    MU_Sprite* sprite = mu_sprite_create();
    if (!sprite) return NULL;
    
    sprite->texture = texture;
    if (texture) {
        sprite->size = mu_vec2((float)texture->width, (float)texture->height);
    }
    
    return sprite;
}

static void mu_sprite_destroy(MU_Sprite* sprite) {
    free(sprite);
}

static void mu_sprite_set_position(MU_Sprite* sprite, MU_Vec2 position) {
    sprite->position = position;
}

static void mu_sprite_set_size(MU_Sprite* sprite, MU_Vec2 size) {
    sprite->size = size;
}

static void mu_sprite_set_rotation(MU_Sprite* sprite, float rotation) {
    sprite->rotation = rotation;
}

static void mu_sprite_set_color(MU_Sprite* sprite, MU_Color color) {
    sprite->color = color;
}

static void mu_sprite_set_texture(MU_Sprite* sprite, MU_Texture2D* texture) {
    sprite->texture = texture;
}

static MU_Rect mu_sprite_get_bounds(MU_Sprite* sprite) {
    return mu_rect(sprite->position.x - sprite->size.x * 0.5f,
                   sprite->position.y - sprite->size.y * 0.5f,
                   sprite->size.x, sprite->size.y);
}

// ============================================================================
// Batch Renderer
// ============================================================================

typedef struct {
    MU_Vec2 position;
    MU_Vec2 tex_coord;
    MU_Color color;
} MU_Vertex;

struct MU_BatchRenderer {
    GLuint vao, vbo, ebo;
    MU_Shader* shader;
    MU_Mat4 projection;
};

static MU_BatchRenderer* mu_batch_renderer_create(void) {
    MU_BatchRenderer* renderer = (MU_BatchRenderer*)malloc(sizeof(MU_BatchRenderer));
    if (!renderer) return NULL;
    
    renderer->vao = 0;
    renderer->vbo = 0;
    renderer->ebo = 0;
    renderer->shader = NULL;
    
    return renderer;
}

static bool mu_batch_renderer_init(MU_BatchRenderer* renderer, int width, int height) {
    renderer->shader = mu_shader_create();
    if (!renderer->shader) return false;
    
    if (!mu_shader_load_default(renderer->shader)) {
        fprintf(stderr, "Failed to load default shader\n");
        return false;
    }
    
    renderer->projection = mu_mat4_ortho(0.0f, (float)width, (float)height, 0.0f, -1.0f, 1.0f);
    
    glGenVertexArrays(1, &renderer->vao);
    glGenBuffers(1, &renderer->vbo);
    glGenBuffers(1, &renderer->ebo);
    
    glBindVertexArray(renderer->vao);
    
    glBindBuffer(GL_ARRAY_BUFFER, renderer->vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(MU_Vertex) * 4, NULL, GL_DYNAMIC_DRAW);
    
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(MU_Vertex), (void*)0);
    glEnableVertexAttribArray(0);
    
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(MU_Vertex), (void*)offsetof(MU_Vertex, tex_coord));
    glEnableVertexAttribArray(1);
    
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(MU_Vertex), (void*)offsetof(MU_Vertex, color));
    glEnableVertexAttribArray(2);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    
    return true;
}

static void mu_batch_renderer_destroy(MU_BatchRenderer* renderer) {
    if (renderer) {
        if (renderer->shader) {
            mu_shader_destroy(renderer->shader);
        }
        if (renderer->vbo) glDeleteBuffers(1, &renderer->vbo);
        if (renderer->ebo) glDeleteBuffers(1, &renderer->ebo);
        if (renderer->vao) glDeleteVertexArrays(1, &renderer->vao);
        free(renderer);
    }
}

static void mu_batch_renderer_begin(MU_BatchRenderer* renderer) {
    mu_shader_bind(renderer->shader);
    mu_shader_set_mat4(renderer->shader, "uProjection", renderer->projection);
    glBindVertexArray(renderer->vao);
}

static void mu_batch_renderer_end(MU_BatchRenderer* renderer) {
    glBindVertexArray(0);
    mu_shader_unbind();
}

static void mu_batch_renderer_draw_sprite(MU_BatchRenderer* renderer, MU_Sprite* sprite) {
    if (!renderer->shader) return;
    
    MU_Mat4 model = mu_mat4_translate(sprite->position.x, sprite->position.y, 0.0f);
    model = mu_mat4_scale(sprite->size.x * 0.5f, sprite->size.y * 0.5f, 1.0f);
    
    mu_shader_set_mat4(renderer->shader, "uModel", model);
    mu_shader_set_int(renderer->shader, "uUseTexture", sprite->texture ? 1 : 0);
    
    if (sprite->texture) {
        mu_texture2d_bind(sprite->texture, 0);
    }
    
    MU_Vertex vertices[4];
    MU_Rect tex_rect = sprite->texture_rect;
    
    vertices[0].position = mu_vec2(-1.0f, 1.0f);
    vertices[0].tex_coord = mu_vec2(tex_rect.x, tex_rect.y + tex_rect.height);
    vertices[0].color = sprite->color;
    
    vertices[1].position = mu_vec2(1.0f, 1.0f);
    vertices[1].tex_coord = mu_vec2(tex_rect.x + tex_rect.width, tex_rect.y + tex_rect.height);
    vertices[1].color = sprite->color;
    
    vertices[2].position = mu_vec2(1.0f, -1.0f);
    vertices[2].tex_coord = mu_vec2(tex_rect.x + tex_rect.width, tex_rect.y);
    vertices[2].color = sprite->color;
    
    vertices[3].position = mu_vec2(-1.0f, -1.0f);
    vertices[3].tex_coord = mu_vec2(tex_rect.x, tex_rect.y);
    vertices[3].color = sprite->color;
    
    unsigned int indices[6] = {0, 1, 2, 2, 3, 0};
    
    glBindBuffer(GL_ARRAY_BUFFER, renderer->vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderer->ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    
    if (sprite->texture) {
        mu_texture2d_unbind();
    }
}

static void mu_batch_renderer_clear(MU_BatchRenderer* renderer, MU_Color color) {
    glClearColor(color.r, color.g, color.b, color.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

// ============================================================================
// Camera2D
// ============================================================================

struct MU_Camera2D {
    MU_Vec2 position;
    float rotation;
    float zoom;
};

static MU_Camera2D* mu_camera2d_create(void) {
    MU_Camera2D* camera = (MU_Camera2D*)malloc(sizeof(MU_Camera2D));
    if (!camera) return NULL;
    
    camera->position = mu_vec2(0.0f, 0.0f);
    camera->rotation = 0.0f;
    camera->zoom = 1.0f;
    
    return camera;
}

static void mu_camera2d_destroy(MU_Camera2D* camera) {
    free(camera);
}

static void mu_camera2d_set_position(MU_Camera2D* camera, MU_Vec2 position) {
    camera->position = position;
}

static MU_Mat4 mu_camera2d_get_view_matrix(MU_Camera2D* camera) {
    return mu_mat4_translate(-camera->position.x, -camera->position.y, 0.0f);
}

// ============================================================================
// Application Framework
// ============================================================================

typedef void (*MU_UpdateFunc)(float delta_time);
typedef void (*MU_RenderFunc)(void);

struct MU_Application {
    MU_Window* window;
    MU_BatchRenderer* renderer;
    MU_Camera2D* camera;
    
    MU_UpdateFunc update_func;
    MU_RenderFunc render_func;
    
    char title[256];
    int width, height;
    bool running;
};

static MU_Application* mu_app = NULL;

static MU_Application* mu_application_create(const char* title, int width, int height) {
    MU_Application* app = (MU_Application*)malloc(sizeof(MU_Application));
    if (!app) return NULL;
    
    memset(app, 0, sizeof(MU_Application));
    strncpy(app->title, title, sizeof(app->title) - 1);
    app->width = width;
    app->height = height;
    app->running = false;
    
    return app;
}

static bool mu_application_init(MU_Application* app) {
    mu_app = app;
    
    app->window = mu_window_create(app->title, app->width, app->height);
    if (!app->window || !mu_window_init(app->window)) {
        return false;
    }
    
    mu_input_init(app->window);
    
    app->renderer = mu_batch_renderer_create();
    if (!app->renderer || !mu_batch_renderer_init(app->renderer, app->width, app->height)) {
        return false;
    }
    
    app->camera = mu_camera2d_create();
    if (!app->camera) {
        return false;
    }
    
    mu_time_init();
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    printf("Mirulit Engine initialized successfully!\n");
    return true;
}

static void mu_application_run(MU_Application* app) {
    app->running = true;
    
    while (app->running && !mu_window_should_close(app->window)) {
        mu_time_update();
        
        if (app->update_func) {
            app->update_func(mu_time_get_delta());
        }
        
        mu_batch_renderer_begin(app->renderer);
        mu_batch_renderer_clear(app->renderer, mu_color(0.1f, 0.1f, 0.1f, 1.0f));
        
        if (app->render_func) {
            app->render_func();
        }
        
        mu_batch_renderer_end(app->renderer);
        
        mu_window_swap_buffers(app->window);
        mu_window_poll_events();
    }
}

static void mu_application_shutdown(MU_Application* app) {
    app->running = false;
    
    if (app->camera) mu_camera2d_destroy(app->camera);
    if (app->renderer) mu_batch_renderer_destroy(app->renderer);
    if (app->window) mu_window_destroy(app->window);
    
    glfwTerminate();
    
    free(app);
    mu_app = NULL;
}

static void mu_application_quit(void) {
    if (mu_app) {
        mu_app->running = false;
    }
}

// ============================================================================
// Public API Macros
// ============================================================================

#define MU_INIT(app_title, width, height) \
    do { \
        MU_Application* app = mu_application_create(app_title, width, height); \
        if (!app || !mu_application_init(app)) { \
            fprintf(stderr, "Failed to initialize application\n"); \
            return 1; \
        } \
    } while(0)

#define MU_RUN(update_func, render_func) \
    do { \
        if (mu_app) { \
            mu_app->update_func = update_func; \
            mu_app->render_func = render_func; \
            mu_application_run(mu_app); \
            mu_application_shutdown(mu_app); \
        } \
    } while(0)

#define MU_QUIT() mu_application_quit()

#define MU_WINDOW() mu_app->window
#define MU_RENDERER() mu_app->renderer
#define MU_CAMERA() mu_app->camera

// ============================================================================
// Public API Functions
// ============================================================================

static inline bool MU_IsKeyPressed(int keycode) {
    return mu_input_is_key_pressed(keycode);
}

static inline bool MU_IsMouseButtonPressed(int button) {
    return mu_input_is_mouse_button_pressed(button);
}

static inline MU_Vec2 MU_GetMousePosition(void) {
    return mu_input_get_mouse_position();
}

static inline MU_Vec2 MU_GetMouseDelta(void) {
    return mu_input_get_mouse_delta();
}

static inline float MU_GetDeltaTime(void) {
    return mu_time_get_delta();
}

static inline float MU_GetTime(void) {
    return mu_time_get();
}

#ifdef __cplusplus
}
#endif

#endif // MIRULIT_H