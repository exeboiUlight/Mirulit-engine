#ifndef WINDOW_MANAGER_H
#define WINDOW_MANAGER_H

#include <GLFW/glfw3.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "app_state.h"

// ============ ФУНКЦИИ УПРАВЛЕНИЯ ОКНОМ ============

static void show_status(const char* msg) {
    strncpy(g_state.status_msg, msg, sizeof(g_state.status_msg) - 1);
    g_state.show_status = 1;
    g_state.status_time = time(NULL);
    printf("STATUS: %s\n", msg);
}

static void toggle_maximize(GLFWwindow* window) {
    if (g_state.maximized) {
        glfwSetWindowPos(window, g_state.old_x, g_state.old_y);
        glfwSetWindowSize(window, g_state.old_w, g_state.old_h);
        g_state.maximized = 0;
    } else {
        glfwGetWindowPos(window, &g_state.old_x, &g_state.old_y);
        glfwGetWindowSize(window, &g_state.old_w, &g_state.old_h);
        
        const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
        glfwSetWindowPos(window, 0, 0);
        glfwSetWindowSize(window, mode->width, mode->height);
        g_state.maximized = 1;
    }
}

static void update_cursor(GLFWwindow* window, double x, double y) {
    int width, height;
    glfwGetWindowSize(window, &width, &height);
    
    int new_cursor = 0;
    
    if (g_state.dragging) {
        new_cursor = 2;
    } else if (y <= 40) {
        if (x >= width - 130 && x <= width - 10) {
            new_cursor = 1;
        } else if (x >= 10 && x <= 40) {
            new_cursor = 1;
        } else if (x < width - 130) {
            new_cursor = 2;
        }
    }
    
    if (new_cursor != g_state.current_cursor) {
        switch (new_cursor) {
            case 0: glfwSetCursor(window, g_state.cursor_arrow); break;
            case 1: glfwSetCursor(window, g_state.cursor_hand); break;
            case 2: glfwSetCursor(window, g_state.cursor_move); break;
            case 3: glfwSetCursor(window, g_state.cursor_text); break;
            case 4: glfwSetCursor(window, g_state.cursor_crosshair); break;
            case 5: glfwSetCursor(window, g_state.cursor_wait); break;
            case 6: glfwSetCursor(window, g_state.cursor_help); break;
        }
        g_state.current_cursor = new_cursor;
    }
}

static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    double x, y;
    glfwGetCursorPos(window, &x, &y);
    int width, height;
    glfwGetWindowSize(window, &width, &height);
    
    if (y <= 40 && button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            if (x >= width - 50 && x <= width - 10) {
                g_state.close_pressed = 1;
                return;
            }
            if (x >= width - 90 && x <= width - 50) {
                g_state.maximize_pressed = 1;
                return;
            }
            if (x >= width - 130 && x <= width - 90) {
                g_state.minimize_pressed = 1;
                return;
            }
            if (x >= 10 && x <= 40) {
                g_state.menu_pressed = 1;
                return;
            }
            if (x < width - 130 && !g_state.maximized) {
                g_state.dragging = 1;
                g_state.drag_start_x = x;
                g_state.drag_start_y = y;
                glfwGetWindowPos(window, &g_state.window_x, &g_state.window_y);
                g_state.drag_lock_x = g_state.window_x;
                g_state.drag_lock_y = g_state.window_y;
                g_state.drag_valid = 1;
            }
        }
        else if (action == GLFW_RELEASE) {
            if (g_state.close_pressed) {
                if (x >= width - 50 && x <= width - 10) {
                    glfwSetWindowShouldClose(window, GLFW_TRUE);
                }
                g_state.close_pressed = 0;
            }
            if (g_state.maximize_pressed) {
                if (x >= width - 90 && x <= width - 50) {
                    toggle_maximize(window);
                }
                g_state.maximize_pressed = 0;
            }
            if (g_state.minimize_pressed) {
                if (x >= width - 130 && x <= width - 90) {
                    glfwIconifyWindow(window);
                }
                g_state.minimize_pressed = 0;
            }
            if (g_state.menu_pressed) {
                if (x >= 10 && x <= 40) {
                    show_status("Menu clicked");
                }
                g_state.menu_pressed = 0;
            }
            if (g_state.dragging) {
                g_state.dragging = 0;
                g_state.drag_valid = 0;
            }
        }
    } else {
        if (action == GLFW_RELEASE) {
            g_state.close_pressed = 0;
            g_state.maximize_pressed = 0;
            g_state.minimize_pressed = 0;
            g_state.menu_pressed = 0;
        }
    }
    
    update_cursor(window, x, y);
}

static void cursor_pos_callback(GLFWwindow* window, double x, double y) {
    int width, height;
    glfwGetWindowSize(window, &width, &height);
    
    if (g_state.dragging && g_state.drag_valid && !g_state.maximized) {
        int new_x = g_state.drag_lock_x + (int)(x - g_state.drag_start_x);
        int new_y = g_state.drag_lock_y + (int)(y - g_state.drag_start_y);
        
        const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
        
        if (new_x < 0) new_x = 0;
        if (new_y < 0) new_y = 0;
        if (new_x + width > mode->width) new_x = mode->width - width;
        if (new_y + height > mode->height) new_y = mode->height - height;
        
        glfwSetWindowPos(window, new_x, new_y);
        g_state.drag_lock_x = new_x;
        g_state.drag_lock_y = new_y;
    }
    
    if (y <= 40) {
        g_state.close_hover = (x >= width - 50 && x <= width - 10);
        g_state.maximize_hover = (x >= width - 90 && x <= width - 50);
        g_state.minimize_hover = (x >= width - 130 && x <= width - 90);
        g_state.menu_hover = (x >= 10 && x <= 40);
    } else {
        g_state.close_hover = 0;
        g_state.maximize_hover = 0;
        g_state.minimize_hover = 0;
        g_state.menu_hover = 0;
    }
    
    update_cursor(window, x, y);
}

#endif // WINDOW_MANAGER_H