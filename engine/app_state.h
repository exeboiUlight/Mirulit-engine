#ifndef APP_STATE_H
#define APP_STATE_H

#include <GLFW/glfw3.h>
#include <time.h>

/* Глобальное состояние */
typedef struct {
    int show_build;
    char status_msg[256];
    int show_status;
    time_t status_time;
    
    /* Для кастомного title bar */
    int dragging;
    double drag_start_x;
    double drag_start_y;
    int window_x;
    int window_y;
    int window_width;
    int window_height;
    int maximized;
    int old_x, old_y, old_w, old_h;
    
    /* Для кнопок title bar */
    int close_hover;
    int maximize_hover;
    int minimize_hover;
    int menu_hover;
    
    int close_pressed;
    int maximize_pressed;
    int minimize_pressed;
    int menu_pressed;
    
    /* Для плавного перемещения */
    int drag_lock_x;
    int drag_lock_y;
    int drag_valid;
    
    /* Для кастомных курсоров */
    GLFWcursor* cursor_arrow;
    GLFWcursor* cursor_hand;
    GLFWcursor* cursor_move;
    GLFWcursor* cursor_text;
    GLFWcursor* cursor_crosshair;
    GLFWcursor* cursor_wait;
    GLFWcursor* cursor_help;
    int current_cursor;
} AppState;

extern AppState g_state;

#endif // APP_STATE_H