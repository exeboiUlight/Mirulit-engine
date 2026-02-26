#ifndef HUB_H
#define HUB_H

typedef struct _project {
    char* name;
    int version[3];
    char* path;
    bool gitInit;
} _project;

void projectsWiev(struct nk_context *ctx) {
}

void Hub(struct nk_context *ctx, int width, int height) {
    if (nk_begin(ctx, "projects", nk_rect(5, 45, width - 10, height - 50), NK_WINDOW_BORDER | NK_WINDOW_TITLE)) {

        nk_layout_row_dynamic(ctx, 30, 2);
        if (nk_button_label(ctx, "New project")) {
        }
        if (nk_button_label(ctx, "Delate project")) {
        }
    }

    nk_end(ctx);
}

#endif