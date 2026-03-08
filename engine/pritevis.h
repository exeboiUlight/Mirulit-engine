#ifndef PRIMETIVES_H
#define PRIMETIVES_H

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


#endif