#ifndef CURSOR_GENERATOR_H
#define CURSOR_GENERATOR_H

#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <math.h>

// ============ ФУНКЦИИ ДЛЯ ГЕНЕРАЦИИ КУРСОРОВ ============

/* Рисует пиксель с антиалиасингом */
static void set_pixel_aa(unsigned char* pixels, int x, int y, int width, int height,
                         unsigned char r, unsigned char g, unsigned char b, float alpha) {
    if (x < 0 || x >= width || y < 0 || y >= height) return;
    int idx = (y * width + x) * 4;
    
    float existing_alpha = pixels[idx + 3] / 255.0f;
    float new_alpha = alpha;
    
    float out_alpha = new_alpha + existing_alpha * (1 - new_alpha);
    if (out_alpha > 0) {
        pixels[idx + 0] = (unsigned char)((r * new_alpha + pixels[idx + 0] * existing_alpha * (1 - new_alpha)) / out_alpha);
        pixels[idx + 1] = (unsigned char)((g * new_alpha + pixels[idx + 1] * existing_alpha * (1 - new_alpha)) / out_alpha);
        pixels[idx + 2] = (unsigned char)((b * new_alpha + pixels[idx + 2] * existing_alpha * (1 - new_alpha)) / out_alpha);
        pixels[idx + 3] = (unsigned char)(out_alpha * 255);
    }
}

/* Рисует линию с антиалиасингом (алгоритм Ву) */
static void draw_line_aa(unsigned char* pixels, int x0, int y0, int x1, int y1, int width, int height,
                         unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
    int steep = abs(y1 - y0) > abs(x1 - x0);
    if (steep) {
        int temp = x0; x0 = y0; y0 = temp;
        temp = x1; x1 = y1; y1 = temp;
    }
    if (x0 > x1) {
        int temp = x0; x0 = x1; x1 = temp;
        temp = y0; y0 = y1; y1 = temp;
    }
    
    int dx = x1 - x0;
    int dy = y1 - y0;
    float gradient = dy / (float)dx;
    
    float xend = round(x0);
    float yend = y0 + gradient * (xend - x0);
    float xgap = 1 - (x0 + 0.5f - floor(x0 + 0.5f));
    int xpxl1 = xend;
    int ypxl1 = floor(yend);
    if (steep) {
        set_pixel_aa(pixels, ypxl1, xpxl1, width, height, r, g, b, (1 - (yend - ypxl1)) * xgap * a / 255.0f);
        set_pixel_aa(pixels, ypxl1 + 1, xpxl1, width, height, r, g, b, (yend - ypxl1) * xgap * a / 255.0f);
    } else {
        set_pixel_aa(pixels, xpxl1, ypxl1, width, height, r, g, b, (1 - (yend - ypxl1)) * xgap * a / 255.0f);
        set_pixel_aa(pixels, xpxl1, ypxl1 + 1, width, height, r, g, b, (yend - ypxl1) * xgap * a / 255.0f);
    }
    
    float intery = yend + gradient;
    
    xend = round(x1);
    yend = y1 + gradient * (xend - x1);
    xgap = x1 + 0.5f - floor(x1 + 0.5f);
    int xpxl2 = xend;
    int ypxl2 = floor(yend);
    if (steep) {
        set_pixel_aa(pixels, ypxl2, xpxl2, width, height, r, g, b, (1 - (yend - ypxl2)) * xgap * a / 255.0f);
        set_pixel_aa(pixels, ypxl2 + 1, xpxl2, width, height, r, g, b, (yend - ypxl2) * xgap * a / 255.0f);
    } else {
        set_pixel_aa(pixels, xpxl2, ypxl2, width, height, r, g, b, (1 - (yend - ypxl2)) * xgap * a / 255.0f);
        set_pixel_aa(pixels, xpxl2, ypxl2 + 1, width, height, r, g, b, (yend - ypxl2) * xgap * a / 255.0f);
    }
    
    if (steep) {
        for (int x = xpxl1 + 1; x < xpxl2; x++) {
            set_pixel_aa(pixels, floor(intery), x, width, height, r, g, b, 1 - (intery - floor(intery)));
            set_pixel_aa(pixels, floor(intery) + 1, x, width, height, r, g, b, intery - floor(intery));
            intery += gradient;
        }
    } else {
        for (int x = xpxl1 + 1; x < xpxl2; x++) {
            set_pixel_aa(pixels, x, floor(intery), width, height, r, g, b, 1 - (intery - floor(intery)));
            set_pixel_aa(pixels, x, floor(intery) + 1, width, height, r, g, b, intery - floor(intery));
            intery += gradient;
        }
    }
}

/* Рисует окружность с антиалиасингом */
static void draw_circle_aa(unsigned char* pixels, int cx, int cy, int radius, int width, int height,
                           unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
    int x = radius;
    int y = 0;
    int radiusError = 1 - x;
    
    while (x >= y) {
        float alpha;
        for (int i = -1; i <= 1; i++) {
            for (int j = -1; j <= 1; j++) {
                if (i == 0 && j == 0) {
                    alpha = 1.0f;
                } else {
                    float dist = sqrt(i*i + j*j);
                    alpha = 1.0f - (dist / 2.0f);
                    if (alpha < 0) alpha = 0;
                }
                
                set_pixel_aa(pixels, cx + x + i, cy + y + j, width, height, r, g, b, alpha * a / 255.0f);
                set_pixel_aa(pixels, cx + y + i, cy + x + j, width, height, r, g, b, alpha * a / 255.0f);
                set_pixel_aa(pixels, cx - y + i, cy + x + j, width, height, r, g, b, alpha * a / 255.0f);
                set_pixel_aa(pixels, cx - x + i, cy + y + j, width, height, r, g, b, alpha * a / 255.0f);
                set_pixel_aa(pixels, cx - x + i, cy - y + j, width, height, r, g, b, alpha * a / 255.0f);
                set_pixel_aa(pixels, cx - y + i, cy - x + j, width, height, r, g, b, alpha * a / 255.0f);
                set_pixel_aa(pixels, cx + y + i, cy - x + j, width, height, r, g, b, alpha * a / 255.0f);
                set_pixel_aa(pixels, cx + x + i, cy - y + j, width, height, r, g, b, alpha * a / 255.0f);
            }
        }
        
        y++;
        if (radiusError < 0) {
            radiusError += 2 * y + 1;
        } else {
            x--;
            radiusError += 2 * (y - x) + 1;
        }
    }
}

/* ОБЪЁМНАЯ СТРЕЛКА - с 3D эффектом */
static GLFWcursor* generate_arrow_cursor(int size) {
    unsigned char* pixels = calloc(size * size * 4, 1);
    
    // Рисуем объёмную стрелку
    for (int y = 0; y < size; y++) {
        for (int x = 0; x < size; x++) {
            if (x <= y && x < size * 0.6 && y < size * 0.8) {
                // Основная часть с градиентом для объёма
                float depth = 1.0f - (float)x / (size * 0.6f);
                int r = 200 + (int)(55 * depth);
                int g = 200 + (int)(55 * depth);
                int b = 200 + (int)(55 * depth);
                
                if (x == 0 || y == x || y == (int)(size * 0.8) - 1) {
                    // Яркий контур
                    set_pixel_aa(pixels, x, y, size, size, 255, 255, 255, 255);
                } else {
                    set_pixel_aa(pixels, x, y, size, size, r, g, b, 255);
                }
            }
        }
    }
    
    // Добавляем тень для объёма
    for (int y = 1; y < size; y++) {
        for (int x = 1; x < size; x++) {
            if (x <= y && x < size * 0.6 && y < size * 0.8) {
                set_pixel_aa(pixels, x+1, y+1, size, size, 0, 0, 0, 80);
            }
        }
    }
    
    // Блик
    for (int y = 0; y < size/4; y++) {
        for (int x = 0; x < size/8; x++) {
            if (x <= y && x < size * 0.6 && y < size * 0.8) {
                set_pixel_aa(pixels, x, y, size, size, 255, 255, 255, 150);
            }
        }
    }
    
    GLFWimage img = {size, size, pixels};
    GLFWcursor* cursor = glfwCreateCursor(&img, 0, 0);
    free(pixels);
    return cursor;
}

/* ОБЪЁМНАЯ РУКА - с 3D эффектом */
static GLFWcursor* generate_hand_cursor(int size) {
    unsigned char* pixels = calloc(size * size * 4, 1);
    
    int cx = size * 0.4;
    int cy = size * 0.5;
    
    // Ладонь с градиентом
    for (int y = 0; y < size; y++) {
        for (int x = 0; x < size; x++) {
            float dx = (x - cx) / 1.2f;
            float dy = (y - cy) / 1.5f;
            float dist = dx*dx + dy*dy;
            
            if (dist < size * size / 25.0f) {
                float intensity = 1.0f - sqrt(dist) / (size / 5.0f);
                int r = 255 - (int)(50 * (1 - intensity));
                int g = 230 - (int)(50 * (1 - intensity));
                int b = 200 - (int)(50 * (1 - intensity));
                set_pixel_aa(pixels, x, y, size, size, r, g, b, (unsigned char)(255 * intensity));
            }
        }
    }
    
    // Пальцы с градиентом
    int finger_pos[] = {-2, 0, 2};
    for (int i = 0; i < 3; i++) {
        int fx = cx + finger_pos[i] * size / 10;
        int fy = cy - size / 3;
        
        for (int y = 0; y < size/3; y++) {
            for (int x = 0; x < size/6; x++) {
                float dx = (x - size/12) / 1.0f;
                float dy = (y - size/6) / 2.0f;
                float dist = dx*dx + dy*dy;
                
                if (dist < size * size / 100.0f) {
                    float intensity = 1.0f - sqrt(dist) / (size / 10.0f);
                    int r = 255 - (int)(50 * (1 - intensity));
                    int g = 220 - (int)(50 * (1 - intensity));
                    int b = 180 - (int)(50 * (1 - intensity));
                    set_pixel_aa(pixels, fx + x, fy - y, size, size, r, g, b, (unsigned char)(255 * intensity));
                }
            }
        }
    }
    
    GLFWimage img = {size, size, pixels};
    GLFWcursor* cursor = glfwCreateCursor(&img, cx, cy);
    free(pixels);
    return cursor;
}

/* ОБЪЁМНЫЙ КУРСОР ПЕРЕМЕЩЕНИЯ */
static GLFWcursor* generate_move_cursor(int size) {
    unsigned char* pixels = calloc(size * size * 4, 1);
    
    int cx = size/2;
    int cy = size/2;
    
    // Центральный шар с градиентом
    for (int y = 0; y < size; y++) {
        for (int x = 0; x < size; x++) {
            float dx = (x - cx) / (float)(size/3);
            float dy = (y - cy) / (float)(size/3);
            float dist = sqrt(dx*dx + dy*dy);
            
            if (dist < 1.0f) {
                float intensity = 1.0f - dist * 0.7f;
                set_pixel_aa(pixels, x, y, size, size, 
                            255 * intensity, 255 * intensity, 255 * intensity, 255);
            }
        }
    }
    
    // Объёмные стрелки
    for (int i = 0; i < 4; i++) {
        float angle = i * 3.14159f / 2;
        int tip_x = cx + (int)(cos(angle) * size * 0.4);
        int tip_y = cy + (int)(sin(angle) * size * 0.4);
        
        // Основная линия с градиентом
        draw_line_aa(pixels, cx, cy, tip_x, tip_y, size, size, 255, 255, 255, 200);
        
        // Толстая линия для объёма
        draw_line_aa(pixels, cx + (int)(sin(angle)), cy - (int)(cos(angle)), 
                    tip_x + (int)(sin(angle)), tip_y - (int)(cos(angle)), 
                    size, size, 255, 255, 255, 100);
        
        // Наконечник стрелки
        for (int a = -15; a <= 15; a += 5) {
            float angle2 = angle + a * 3.14159f / 180;
            int barb_x = tip_x - (int)(cos(angle2) * size/6);
            int barb_y = tip_y - (int)(sin(angle2) * size/6);
            draw_line_aa(pixels, tip_x, tip_y, barb_x, barb_y, size, size, 255, 255, 255, 200);
        }
    }
    
    GLFWimage img = {size, size, pixels};
    GLFWcursor* cursor = glfwCreateCursor(&img, cx, cy);
    free(pixels);
    return cursor;
}

/* ОБЪЁМНЫЙ ТЕКСТОВЫЙ КУРСОР */
static GLFWcursor* generate_text_cursor(int size) {
    unsigned char* pixels = calloc(size * size * 4, 1);
    
    int cx = size/2;
    int line_width = size/8;
    
    // Верхняя перекладина с градиентом
    for (int x = cx - size/4; x <= cx + size/4; x++) {
        for (int w = 0; w < line_width; w++) {
            float intensity = 1.0f - (float)w / line_width;
            set_pixel_aa(pixels, x, w, size, size, 255, 255, 255, 200 * intensity);
            set_pixel_aa(pixels, x, size - 1 - w, size, size, 255, 255, 255, 200 * intensity);
        }
    }
    
    // Вертикальная линия с градиентом
    for (int y = line_width; y < size - line_width; y++) {
        for (int w = -1; w <= 1; w++) {
            float intensity = 1.0f - (float)abs(w) / 2.0f;
            set_pixel_aa(pixels, cx + w, y, size, size, 255, 255, 255, 200 * intensity);
        }
    }
    
    // Свечение
    for (int y = 0; y < size; y++) {
        for (int w = -2; w <= 2; w++) {
            if (abs(w) > 1) {
                float intensity = 0.3f - (float)abs(w) * 0.1f;
                set_pixel_aa(pixels, cx + w, y, size, size, 100, 100, 255, 30 * intensity);
            }
        }
    }
    
    GLFWimage img = {size, size, pixels};
    GLFWcursor* cursor = glfwCreateCursor(&img, cx, size/2);
    free(pixels);
    return cursor;
}

/* ОБЪЁМНЫЙ ПРИЦЕЛ */
static GLFWcursor* generate_crosshair_cursor(int size) {
    unsigned char* pixels = calloc(size * size * 4, 1);
    
    int cx = size/2;
    int cy = size/2;
    int gap = size/8;
    
    // Внешнее кольцо с градиентом
    for (int r = size/4; r <= size/3; r++) {
        float intensity = 1.0f - (float)(r - size/4) / (size/12);
        draw_circle_aa(pixels, cx, cy, r, size, size, 255, 255, 255, 150 * intensity);
    }
    
    // Линии с градиентом
    draw_line_aa(pixels, cx, 0, cx, cy - gap, size, size, 255, 255, 255, 200);
    draw_line_aa(pixels, cx, cy + gap, cx, size-1, size, size, 255, 255, 255, 200);
    draw_line_aa(pixels, 0, cy, cx - gap, cy, size, size, 255, 255, 255, 200);
    draw_line_aa(pixels, cx + gap, cy, size-1, cy, size, size, 255, 255, 255, 200);
    
    // Точка в центре с ореолом
    for (int r = 0; r <= 2; r++) {
        draw_circle_aa(pixels, cx, cy, r, size, size, 255, 0, 0, 255 - r * 50);
    }
    
    GLFWimage img = {size, size, pixels};
    GLFWcursor* cursor = glfwCreateCursor(&img, cx, cy);
    free(pixels);
    return cursor;
}

/* ОБЪЁМНЫЙ КУРСОР ОЖИДАНИЯ (песочные часы) */
static GLFWcursor* generate_wait_cursor(int size) {
    unsigned char* pixels = calloc(size * size * 4, 1);
    
    int cx = size/2;
    int cy = size/2;
    
    for (int y = 0; y < size; y++) {
        for (int x = 0; x < size; x++) {
            // Верхний треугольник с градиентом
            if (y < size/2 && abs(x - cx) < (y * size/32)) {
                float intensity = 1.0f - y / (float)size;
                set_pixel_aa(pixels, x, y, size, size, 
                            255, 200 - (int)(100 * intensity), 100, 
                            (unsigned char)(200 * intensity));
            }
            
            // Нижний треугольник с градиентом
            if (y > size/2 && abs(x - cx) < ((size - y) * size/32)) {
                float intensity = (y - size/2) / (float)(size/2);
                set_pixel_aa(pixels, x, y, size, size, 
                            100, 200 - (int)(100 * intensity), 255,
                            (unsigned char)(200 * intensity));
            }
            
            // Талия с подсветкой
            if (abs(x - cx) < 2 && abs(y - size/2) < 3) {
                set_pixel_aa(pixels, x, y, size, size, 255, 255, 255, 255);
            }
        }
    }
    
    GLFWimage img = {size, size, pixels};
    GLFWcursor* cursor = glfwCreateCursor(&img, cx, cy);
    free(pixels);
    return cursor;
}

/* ОБЪЁМНЫЙ КУРСОР ПОМОЩИ */
static GLFWcursor* generate_help_cursor(int size) {
    unsigned char* pixels = calloc(size * size * 4, 1);
    
    int cx = size/2;
    int cy = size/2;
    
    // Сфера подсветки
    for (int y = 0; y < size; y++) {
        for (int x = 0; x < size; x++) {
            float dx = (x - cx) / 4.0f;
            float dy = (y - cy) / 4.0f;
            float dist = dx*dx + dy*dy;
            
            if (dist < 1.0f) {
                float intensity = 1.0f - dist;
                set_pixel_aa(pixels, x, y, size, size, 
                            100, 150, 255, (unsigned char)(50 * intensity));
            }
        }
    }
    
    // Знак вопроса с объёмом
    draw_circle_aa(pixels, cx, cy - size/6, size/6, size, size, 255, 255, 255, 255);
    
    for (int r = size/8; r <= size/6; r++) {
        float intensity = 1.0f - (float)(r - size/8) / (size/24);
        draw_circle_aa(pixels, cx, cy - size/6, r, size, size, 255, 255, 255, 100 * intensity);
    }
    
    for (int y = cy - size/12; y <= cy + size/6; y++) {
        for (int w = -1; w <= 1; w++) {
            set_pixel_aa(pixels, cx + w, y, size, size, 255, 255, 255, 255);
            set_pixel_aa(pixels, cx + w, y-1, size, size, 200, 200, 200, 100);
        }
    }
    
    for (int dy = -1; dy <= 1; dy++) {
        for (int dx = -1; dx <= 1; dx++) {
            set_pixel_aa(pixels, cx + dx, cy + size/4 + dy, size, size, 255, 255, 255, 255);
        }
    }
    
    GLFWimage img = {size, size, pixels};
    GLFWcursor* cursor = glfwCreateCursor(&img, cx, cy);
    free(pixels);
    return cursor;
}

#endif // CURSOR_GENERATOR_H