#ifndef TITLEBAR_H
#define TITLEBAR_H

void titlebar(struct nk_context *ctx, int width, int height) {
    if (nk_begin(ctx, "TitleBar", nk_rect(0, 0, width, 40),
        NK_WINDOW_NO_SCROLLBAR | NK_WINDOW_NO_INPUT)) {
        
        struct nk_command_buffer* canvas = nk_window_get_canvas(ctx);

        // Градиентный фон title bar
        for (int i = 0; i < 40; i++) {
            float t = (float)i / 40;
            struct nk_color grad_color = nk_rgb(
                30 + (int)(20 * t),
                30 + (int)(20 * t),
                30 + (int)(20 * t)
            );
            nk_fill_rect(canvas, nk_rect(0, i, width, 1), 0, grad_color);
        }

        // Нижняя граница с подсветкой
        nk_fill_rect(canvas, nk_rect(0, 38, width, 2), 0, nk_rgb(80, 80, 100));

        // Логотип (стилизованная кнопка)
        draw_styled_button(canvas, 8, 6, 30, 28, 
                          nk_rgb(60, 60, 80),
                          g_state.menu_pressed, g_state.menu_hover, "M",
                          &font->handle);

        // Рисуем текст ПОСЛЕ фона, но ДО кнопок управления
        nk_draw_text(canvas, nk_rect(46, 12, 400, 20), 
                    "Mirulit Game Engine", 20, &font->handle, 
                    nk_rgb(255, 255, 255), nk_rgba(0, 0, 0, 0));

        // Кнопки управления со стилизованными иконками (рисуются ПОВЕРХ текста)
        draw_icon(canvas, width - 48, 8, 24, 24, "close",
                 nk_rgb(200, 60, 60), g_state.close_pressed, g_state.close_hover);

        draw_icon(canvas, width - 88, 8, 24, 24, "maximize",
                 nk_rgb(60, 60, 200), g_state.maximize_pressed, g_state.maximize_hover);

        draw_icon(canvas, width - 128, 8, 24, 24, "minimize",
                 nk_rgb(60, 200, 60), g_state.minimize_pressed, g_state.minimize_hover);
    }
    nk_end(ctx);
}

#endif