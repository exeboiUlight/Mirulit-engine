#ifndef WINDOW_H
#define WINDOW_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

bool window_init(int width, int height, const char* title);
void window_terminate(void);

void window_process_input(void);
bool window_should_close(void);
void window_swap_buffers(void);
void window_poll_events(void);

GLFWwindow* window_get_handle(void);
int window_get_width(void);
int window_get_height(void);
float window_get_aspect_ratio(void);
double window_get_time(void);

void window_set_title(const char* title);
void window_set_size(int width, int height);
void window_set_vsync(bool enabled);
void window_set_background_color(float r, float g, float b, float a);
void window_set_clear_color_enabled(bool enabled);

void window_set_framebuffer_size_callback(GLFWframebuffersizefun callback);
void window_set_key_callback(GLFWkeyfun callback);
void window_set_mouse_callback(GLFWcursorposfun callback);
void window_set_scroll_callback(GLFWscrollfun callback);

void window_clear(void);
void window_set_clear_flags(GLbitfield flags);
void window_print_info(void);

static GLFWwindow* main_window = NULL;
static int window_width = 800;
static int window_height = 600;
static const char* window_title = "Mirulit Window";
static bool clear_color_enabled = true;
static float bg_color[4] = {0.2f, 0.3f, 0.3f, 1.0f};
static GLbitfield clear_flags = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT;

static void default_framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    (void)window;
    glViewport(0, 0, width, height);
    window_width = width;
    window_height = height;
}

static void error_callback(int error, const char* description)
{
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

bool window_init(int width, int height, const char* title)
{
    glfwSetErrorCallback(error_callback);

    if (!glfwInit())
    {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return false;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    #ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif

    window_width = width;
    window_height = height;
    window_title = title;

    main_window = glfwCreateWindow(width, height, title, NULL, NULL);
    if (main_window == NULL)
    {
        fprintf(stderr, "Failed to create GLFW window\n");
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(main_window);

    glfwSetFramebufferSizeCallback(main_window, default_framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        fprintf(stderr, "Failed to initialize GLAD\n");
        glfwTerminate();
        return false;
    }

    glEnable(GL_DEPTH_TEST);

    glViewport(0, 0, width, height);

    printf("Window initialized successfully\n");
    printf("OpenGL version: %s\n", glGetString(GL_VERSION));
    printf("GLSL version: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
    printf("Renderer: %s\n", glGetString(GL_RENDERER));
    printf("Vendor: %s\n", glGetString(GL_VENDOR));

    return true;
}

void window_terminate(void)
{
    if (main_window)
    {
        glfwDestroyWindow(main_window);
        main_window = NULL;
    }
    glfwTerminate();
    printf("Window terminated\n");
}

void window_process_input(void)
{
    if (main_window == NULL) return;

    if (glfwGetKey(main_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(main_window, true);
}

bool window_should_close(void)
{
    if (main_window == NULL) return true;
    return glfwWindowShouldClose(main_window);
}

void window_swap_buffers(void)
{
    if (main_window)
        glfwSwapBuffers(main_window);
}

void window_poll_events(void)
{
    glfwPollEvents();
}

GLFWwindow* window_get_handle(void)
{
    return main_window;
}

int window_get_width(void)
{
    return window_width;
}

int window_get_height(void)
{
    return window_height;
}

float window_get_aspect_ratio(void)
{
    if (window_height == 0) return 1.0f;
    return (float)window_width / (float)window_height;
}

double window_get_time(void)
{
    return glfwGetTime();
}

void window_set_title(const char* title)
{
    if (main_window && title)
    {
        window_title = title;
        glfwSetWindowTitle(main_window, title);
    }
}

void window_set_size(int width, int height)
{
    if (main_window && width > 0 && height > 0)
    {
        window_width = width;
        window_height = height;
        glfwSetWindowSize(main_window, width, height);
    }
}

void window_set_vsync(bool enabled)
{
    if (main_window)
        glfwSwapInterval(enabled ? 1 : 0);
}

void window_set_background_color(float r, float g, float b, float a)
{
    bg_color[0] = r;
    bg_color[1] = g;
    bg_color[2] = b;
    bg_color[3] = a;

    if (clear_color_enabled)
        glClearColor(r, g, b, a);
}

void window_set_clear_color_enabled(bool enabled)
{
    clear_color_enabled = enabled;
}

void window_set_framebuffer_size_callback(GLFWframebuffersizefun callback)
{
    if (main_window)
    {
        if (callback == NULL)
            glfwSetFramebufferSizeCallback(main_window, default_framebuffer_size_callback);
        else
            glfwSetFramebufferSizeCallback(main_window, callback);
    }
}

void window_set_key_callback(GLFWkeyfun callback)
{
    if (main_window)
        glfwSetKeyCallback(main_window, callback);
}

void window_set_mouse_callback(GLFWcursorposfun callback)
{
    if (main_window)
        glfwSetCursorPosCallback(main_window, callback);
}

void window_set_scroll_callback(GLFWscrollfun callback)
{
    if (main_window)
        glfwSetScrollCallback(main_window, callback);
}

void window_clear(void)
{
    glClear(clear_flags);
}

void window_set_clear_flags(GLbitfield flags)
{
    clear_flags = flags;
}

void window_print_info(void)
{
    printf("=== Window Information ===\n");
    printf("Size: %dx%d\n", window_width, window_height);
    printf("Aspect ratio: %.2f\n", window_get_aspect_ratio());
    printf("Title: %s\n", window_title);
    printf("Background color: [%.2f, %.2f, %.2f, %.2f]\n",  bg_color[0], bg_color[1], bg_color[2], bg_color[3]);
    printf("Clear color enabled: %s\n", clear_color_enabled ? "true" : "false");
    printf("==========================\n");
}

#endif