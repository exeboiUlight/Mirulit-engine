#include <Mirulit.h>

MU_Texture2D* texture = NULL;
MU_Sprite* sprite = NULL;

void update(float delta_time) {
    if (MU_IsKeyPressed(MU_KEY_ESCAPE)) {
        MU_QUIT();
    }
    
    if (MU_IsKeyPressed(MU_KEY_W)) {
        MU_Vec2 pos = sprite->position;
        pos.y -= 100.0f * delta_time;
        mu_sprite_set_position(sprite, pos);
    }
    if (MU_IsKeyPressed(MU_KEY_S)) {
        MU_Vec2 pos = sprite->position;
        pos.y += 100.0f * delta_time;
        mu_sprite_set_position(sprite, pos);
    }
}

void render(void) {
    mu_batch_renderer_draw_sprite(MU_RENDERER(), sprite);
}

int main(void) {
    // Инициализация приложения
    MU_INIT("Mirulit Game", 800, 600);
    
    // Создание текстур и спрайтов
    texture = mu_texture2d_create();
    mu_texture2d_load(texture, "texture.png");
    
    sprite = mu_sprite_create_with_texture(texture);
    mu_sprite_set_position(sprite, mu_vec2(400, 300));
    mu_sprite_set_size(sprite, mu_vec2(200, 200));
    mu_sprite_set_color(sprite, mu_color(1.0f, 0.5f, 0.5f, 1.0f));
    
    // Запуск игрового цикла
    MU_RUN(update, render);
    
    // Очистка
    mu_sprite_destroy(sprite);
    mu_texture2d_destroy(texture);
    
    return 0;
}