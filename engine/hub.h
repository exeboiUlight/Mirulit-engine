#ifndef HUB_H
#define HUB_H

#include <stdbool.h>
#include <string.h>
#include <stdio.h>

// Объявляем внешние переменные
extern bool MakeProject;

void Hub(struct nk_context *ctx, int width, int height) {
    if (nk_begin(ctx, "projects", nk_rect(5, 45, width - 10, height - 50), NK_WINDOW_BORDER | NK_WINDOW_TITLE)) {

        nk_layout_row_dynamic(ctx, 30, 2);
        if (nk_button_label(ctx, "New project")) {
            MakeProject = true;
            printf("New project button clicked\n");
        }
        if (nk_button_label(ctx, "Delete project")) {
        }
    }
    nk_end(ctx);
}

void CreateProjectWindow(struct nk_context *ctx, int width, int height) {
    static char project_name[256] = "test"; // Начальный текст для проверки
    static int active = 1;
    
    printf("CreateProjectWindow opened, MakeProject=%d\n", MakeProject);
    
    if (nk_begin(ctx, "create project", nk_rect(5, 45, width - 10, height - 50), 
                 NK_WINDOW_BORDER | NK_WINDOW_TITLE | NK_WINDOW_CLOSABLE)) {
        
        // Простой текст
        nk_layout_row_dynamic(ctx, 30, 1);
        nk_label(ctx, "Enter project name:", NK_TEXT_LEFT);
        
        // Поле ввода с визуальным выделением
        nk_layout_row_dynamic(ctx, 50, 1);
        
        struct nk_rect bounds = nk_widget_bounds(ctx);
        nk_layout_row_dynamic(ctx, 50, 1);
        
        nk_flags result = nk_edit_string_zero_terminated(ctx, NK_EDIT_FIELD, project_name, 255, nk_filter_default);
        
        // Показываем, что введено
        nk_layout_row_dynamic(ctx, 30, 1);
        char buffer[300];
        snprintf(buffer, sizeof(buffer), "Current text: '%s' (length: %zu)", project_name, strlen(project_name));
        nk_label(ctx, buffer, NK_TEXT_LEFT);
        
        // Кнопки
        nk_layout_row_dynamic(ctx, 40, 2);
        if (nk_button_label(ctx, "Create")) {
            printf("Create clicked with text: '%s'\n", project_name);
            if (strlen(project_name) > 0) {
                MakeProject = false;
            }
        }
        if (nk_button_label(ctx, "Cancel")) {
            printf("Cancel clicked\n");
            MakeProject = false;
        }
    } else {
        printf("Window closed\n");
        MakeProject = false;
    }
    nk_end(ctx);
}

#endif