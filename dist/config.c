#include <_Mirulit.h>

MIR_Entity* player = NULL;
MIR_Entity* enemy = NULL;
float spawn_timer = 0;
float shoot_cooldown = 0;
int score = 0;
int enemies_destroyed = 0;

// Структура для пули
typedef struct {
    MIR_Entity* entity;
    MIR_Vec2 direction;
    float speed;
    float lifetime;
} Bullet;

Bullet bullets[100];
int bullet_count = 0;
SDL_Texture* player_texture = NULL;  // Текстура игрока

// Глобальная переменная для скорости игрока (для правильного расчета партиклов)
static MIR_Vec2 player_velocity = {0, 0};
static MIR_Vec2 last_player_pos = {400, 300};

void PlayerUpdate(MIR_Entity* self, float dt) {
    float speed = 300.0f * dt;
    MIR_Vec2 move_dir = {0, 0};
    
    // Сохраняем позицию до движения для расчета скорости
    MIR_Vec2 prev_pos = self->transform.position;
    
    if (MIR_IsKeyDown(SDLK_W)) { 
        self->transform.position.y -= speed; 
        move_dir.y -= 1;
    }
    if (MIR_IsKeyDown(SDLK_S)) { 
        self->transform.position.y += speed; 
        move_dir.y += 1;
    }
    if (MIR_IsKeyDown(SDLK_A)) { 
        self->transform.position.x -= speed; 
        move_dir.x -= 1;
    }
    if (MIR_IsKeyDown(SDLK_D)) { 
        self->transform.position.x += speed; 
        move_dir.x += 1;
    }
    
    // Расчет реальной скорости игрока (в пикселях в секунду)
    if (dt > 0) {
        player_velocity = MIR_Vec2_Multiply(
            MIR_Vec2_Subtract(self->transform.position, prev_pos),
            1.0f / dt
        );
    }
    
    // Границы
    self->transform.position.x = MIR_Math_Clamp(self->transform.position.x, 40, 760);
    self->transform.position.y = MIR_Math_Clamp(self->transform.position.y, 40, 560);
    
    // Слежение камеры
    MIR_SetCameraTarget(self->transform.position);
    
    // Вращение в сторону мыши
    MIR_Vec2 mouse_world = MIR_GetMouseWorldPosition();
    MIR_Vec2 dir = MIR_Vec2_Subtract(mouse_world, self->transform.position);
    self->transform.rotation = atan2f(dir.y, dir.x) * 180.0f / 3.14159f + 90.0f;
    
    // Частицы двигателя - ИСПРАВЛЕННАЯ ВЕРСИЯ с синхронизированным ускорением
    // Создаём частицы только когда корабль движется
    float move_magnitude = sqrtf(move_dir.x * move_dir.x + move_dir.y * move_dir.y);
    if (move_magnitude > 0 || MIR_Math_Distance(player_velocity, (MIR_Vec2){0, 0}) > 10.0f) {
        // Направление взгляда корабля (куда смотрит нос)
        float angle_rad = (self->transform.rotation - 90.0f) * 3.14159f / 180.0f;
        MIR_Vec2 look_dir = {cosf(angle_rad), sinf(angle_rad)};
        
        // Задняя часть корабля в мировых координатах
        MIR_Vec2 engine_pos = MIR_Vec2_Subtract(
            self->transform.position,
            MIR_Vec2_Multiply(look_dir, 25.0f) // 25 пикселей от центра к хвосту
        );
        
        // Количество частиц зависит от скорости движения
        float speed_factor = MIR_Math_Clamp(MIR_Math_Distance(player_velocity, (MIR_Vec2){0, 0}) / 200.0f, 0.2f, 2.0f);
        int particle_count = (int)(5 * speed_factor);
        
        for (int i = 0; i < particle_count; i++) {
            // Случайное смещение от центра двигателя
            MIR_Vec2 offset = {
                MIR_Math_RandomRange(-4, 4),
                MIR_Math_RandomRange(-4, 4)
            };
            
            // Направление частиц - противоположно направлению взгляда (выхлоп)
            MIR_Vec2 particle_dir = {
                -look_dir.x * 0.9f + MIR_Math_RandomRange(-0.15f, 0.15f),
                -look_dir.y * 0.9f + MIR_Math_RandomRange(-0.15f, 0.15f)
            };
            
            // Нормализуем направление
            float dir_len = sqrtf(particle_dir.x * particle_dir.x + particle_dir.y * particle_dir.y);
            if (dir_len > 0) {
                particle_dir.x /= dir_len;
                particle_dir.y /= dir_len;
            }
            
            // Скорость партикла = скорость выхлопа + 100% скорости корабля для полной синхронизации
            float exhaust_speed = 80.0f + MIR_Math_RandomRange(-20, 20);
            MIR_Vec2 particle_vel = MIR_Vec2_Add(
                MIR_Vec2_Multiply(particle_dir, exhaust_speed),
                MIR_Vec2_Multiply(player_velocity, 1.0f) // 100% скорости корабля
            );
            
            // Цвет частиц двигателя (оранжевый/жёлтый) с вариацией
            float color_variation = MIR_Math_RandomRange(0.7f, 1.0f);
            MIR_Color engine_color = {
                (uint8_t)(255 * color_variation),
                (uint8_t)(150 * color_variation + MIR_Math_RandomRange(0, 50)),
                (uint8_t)(50 * color_variation),
                255
            };
            
            // Размер зависит от скорости
            float particle_size = MIR_Math_RandomRange(1.5f, 3.5f) * speed_factor;
            
            // Создаем частицу в мировых координатах
            MIR_EmitParticleEx(
                MIR_Vec2_Add(engine_pos, offset), // Мировая позиция
                particle_vel,                     // Скорость в мировых координатах
                (MIR_Vec2){0, 10},                // Легкая гравитация для частиц двигателя
                engine_color,
                particle_size,
                MIR_Math_RandomRange(0.3f, 0.6f)
            );
        }
    }
}

void EnemyUpdate(MIR_Entity* self, float dt) {
    // Движение к игроку
    if (player) {
        MIR_Vec2 dir = MIR_Vec2_Subtract(
            player->transform.position,
            self->transform.position
        );
        float dist = MIR_Math_Distance(
            player->transform.position,
            self->transform.position
        );
        
        if (dist > 0 && dist < 500) { // Только если игрок в радиусе 500 пикселей
            dir = MIR_Vec2_Multiply(MIR_Math_Normalize(dir), 80.0f * dt);
            self->transform.position = MIR_Vec2_Add(self->transform.position, dir);
        }
    }
    
    // Пульсация
    static float pulse_time = 0;
    pulse_time += dt;
    self->sprite.color.r = (uint8_t)(128 + sinf(pulse_time * 3) * 127);
}

void BulletUpdate(MIR_Entity* self, float dt) {
    // Находим связанную структуру пули
    for (int i = 0; i < bullet_count; i++) {
        if (bullets[i].entity == self) {
            // Движение пули
            self->transform.position = MIR_Vec2_Add(
                self->transform.position,
                MIR_Vec2_Multiply(bullets[i].direction, bullets[i].speed * dt)
            );
            
            // Уменьшение времени жизни
            bullets[i].lifetime -= dt;
            if (bullets[i].lifetime <= 0) {
                // Уничтожение пули
                self->active = false;
            }
            
            // Уничтожение при выходе за границы
            if (self->transform.position.x < -100 || self->transform.position.x > 900 ||
                self->transform.position.y < -100 || self->transform.position.y > 700) {
                self->active = false;
            }
            break;
        }
    }
}

void CreateBullet(MIR_Vec2 position, MIR_Vec2 direction) {
    if (bullet_count >= 100) return;
    
    MIR_Entity* bullet = MIR_CreateEntity("Bullet");
    if (!bullet) return;
    
    bullet->transform.position = position;
    bullet->transform.scale = (MIR_Vec2){12, 4}; // Пуля как прямоугольник
    bullet->sprite.color = MIR_COLOR_YELLOW;
    bullet->update = BulletUpdate;
    bullet->collider.bounds = (MIR_Rect){0, 0, 12, 4};
    bullet->collider.enabled = true;
    bullet->collider.is_trigger = true;
    
    // Начальная скорость пули с учетом скорости игрока
    MIR_Vec2 normalized_dir = MIR_Math_Normalize(direction);
    MIR_Vec2 bullet_velocity = MIR_Vec2_Add(
        MIR_Vec2_Multiply(normalized_dir, 800.0f),
        MIR_Vec2_Multiply(player_velocity, 0.5f) // Начальный импульс от скорости корабля
    );
    
    // Сохраняем пулю в массив
    bullets[bullet_count].entity = bullet;
    bullets[bullet_count].direction = MIR_Math_Normalize(bullet_velocity);
    bullets[bullet_count].speed = MIR_Math_Distance(bullet_velocity, (MIR_Vec2){0, 0});
    bullets[bullet_count].lifetime = 1.5f; // Меньше время жизни
    bullet_count++;
    
    // Эффект выстрела (из носа корабля) - ИСПРАВЛЕННАЯ ВЕРСИЯ
    float angle_rad = (player->transform.rotation - 90.0f) * 3.14159f / 180.0f;
    MIR_Vec2 look_dir = {cosf(angle_rad), sinf(angle_rad)};
    
    // Позиция выстрела в мировых координатах
    MIR_Vec2 gun_pos = MIR_Vec2_Add(
        position, 
        MIR_Vec2_Multiply(look_dir, 32.0f) // 32 пикселя от центра в сторону носа
    );
    
    for (int i = 0; i < 8; i++) {
        MIR_Vec2 offset = {
            MIR_Math_RandomRange(-3, 3),
            MIR_Math_RandomRange(-3, 3)
        };
        
        // Направление выстрела + небольшая случайность + скорость корабля
        MIR_Vec2 shot_dir = {
            direction.x + MIR_Math_RandomRange(-0.08f, 0.08f),
            direction.y + MIR_Math_RandomRange(-0.08f, 0.08f)
        };
        
        // Нормализуем
        float dir_len = sqrtf(shot_dir.x * shot_dir.x + shot_dir.y * shot_dir.y);
        if (dir_len > 0) {
            shot_dir.x /= dir_len;
            shot_dir.y /= dir_len;
        }
        
        // Скорость частиц в мировых координатах с учетом скорости корабля
        MIR_Vec2 vel = MIR_Vec2_Add(
            MIR_Vec2_Multiply(shot_dir, 120.0f + MIR_Math_RandomRange(-30, 30)),
            MIR_Vec2_Multiply(player_velocity, 0.8f) // Увеличено для лучшей синхронизации
        );
        
        // Цвета выстрела (белый/жёлтый/оранжевый)
        MIR_Color shot_color = {
            255,
            (uint8_t)(220 + MIR_Math_RandomRange(0, 35)),
            (uint8_t)(150 + MIR_Math_RandomRange(0, 50)),
            255
        };
        
        // Создаем частицу в мировых координатах
        MIR_EmitParticleEx(
            MIR_Vec2_Add(gun_pos, offset), // Мировая позиция
            vel,                           // Скорость в мировых координатах
            (MIR_Vec2){0, 0},              // Без гравитации для частиц выстрела
            shot_color,
            MIR_Math_RandomRange(1.5f, 2.5f),
            MIR_Math_RandomRange(0.1f, 0.2f)
        );
    }
}

void CheckCollisions() {
    // Проверяем коллизии пуль с врагами
    for (int i = 0; i < bullet_count; i++) {
        if (!bullets[i].entity || !bullets[i].entity->active) continue;
        
        for (int j = 0; j < _mir->entity_count; j++) {
            MIR_Entity* enemy_entity = _mir->entities[j];
            if (!enemy_entity || !enemy_entity->active || 
                strcmp(enemy_entity->tag, "Enemy") != 0) continue;
            
            if (MIR_CheckCollision(bullets[i].entity, enemy_entity)) {
                // Уничтожаем пулю и врага
                bullets[i].entity->active = false;
                enemy_entity->active = false;
                score += 100;
                enemies_destroyed++;
                
                // Эффект взрыва (с гравитацией)
                for (int k = 0; k < 20; k++) {
                    // Базовая скорость взрыва + влияние скорости игрока
                    MIR_Vec2 base_vel = {
                        MIR_Math_RandomRange(-300, 300),
                        MIR_Math_RandomRange(-300, 300)
                    };
                    MIR_Vec2 vel = MIR_Vec2_Add(
                        base_vel,
                        MIR_Vec2_Multiply(player_velocity, 0.3f) // Добавляем влияние скорости игрока
                    );
                    
                    MIR_Color explosion_color = {
                        (uint8_t)MIR_Math_RandomRange(200, 255),
                        (uint8_t)MIR_Math_RandomRange(50, 150),
                        50,
                        255
                    };
                    // Используем исправленный EmitParticleEx для мировых координат
                    MIR_EmitParticleEx(
                        enemy_entity->transform.position, // Мировая позиция
                        vel,
                        (MIR_Vec2){0, 50}, // Гравитация вниз
                        explosion_color,
                        MIR_Math_RandomRange(4.0f, 10.0f),
                        MIR_Math_RandomRange(0.5f, 1.5f)
                    );
                }
            }
        }
    }
    
    // Проверяем коллизии игрока с врагами
    if (player && player->active) {
        for (int i = 0; i < _mir->entity_count; i++) {
            MIR_Entity* enemy_entity = _mir->entities[i];
            if (!enemy_entity || !enemy_entity->active || 
                strcmp(enemy_entity->tag, "Enemy") != 0) continue;
            
            if (MIR_CheckCollision(player, enemy_entity)) {
                // Уничтожаем врага и наносим урон
                enemy_entity->active = false;
                
                // Эффект столкновения (с гравитацией)
                for (int k = 0; k < 30; k++) {
                    MIR_Vec2 vel = MIR_Vec2_Add(
                        (MIR_Vec2){
                            MIR_Math_RandomRange(-400, 400),
                            MIR_Math_RandomRange(-400, 400)
                        },
                        MIR_Vec2_Multiply(player_velocity, 0.3f) // Добавляем влияние скорости игрока
                    );
                    MIR_Color collision_color = {
                        255,
                        100,
                        100,
                        255
                    };
                    MIR_EmitParticleEx(
                        enemy_entity->transform.position,
                        vel,
                        (MIR_Vec2){0, 30}, // Гравитация
                        collision_color,
                        MIR_Math_RandomRange(3.0f, 8.0f),
                        MIR_Math_RandomRange(0.3f, 0.8f)
                    );
                }
                
                // Можно добавить систему жизней здесь
            }
        }
    }
    
    // Удаляем неактивные пули
    for (int i = bullet_count - 1; i >= 0; i--) {
        if (!bullets[i].entity || !bullets[i].entity->active) {
            // Удаляем сущность
            if (bullets[i].entity) {
                MIR_DestroyEntity(bullets[i].entity);
            }
            
            // Сдвигаем массив
            for (int j = i; j < bullet_count - 1; j++) {
                bullets[j] = bullets[j + 1];
            }
            bullet_count--;
        }
    }
    
    // Удаляем неактивных врагов
    for (int i = _mir->entity_count - 1; i >= 0; i--) {
        if (_mir->entities[i] && !_mir->entities[i]->active && 
            strcmp(_mir->entities[i]->tag, "Enemy") == 0) {
            MIR_DestroyEntity(_mir->entities[i]);
        }
    }
}

void OnRestartButtonClick(void) {
    printf("Restart button clicked!\n");
    MIR_SetTimeScale(1.0f);
    score = 0;
    enemies_destroyed = 0;
    player_velocity = (MIR_Vec2){0, 0};
    
    // Очистка всех врагов и пуль
    for (int i = _mir->entity_count - 1; i >= 0; i--) {
        if (_mir->entities[i] && strcmp(_mir->entities[i]->tag, "Enemy") == 0) {
            MIR_DestroyEntity(_mir->entities[i]);
        }
    }
    
    for (int i = 0; i < bullet_count; i++) {
        if (bullets[i].entity) {
            MIR_DestroyEntity(bullets[i].entity);
        }
    }
    bullet_count = 0;
}

int main(void) {
    // Инициализация движка
    if (!MIR_Init("Mirulit Engine - Space Shooter", 800, 600)) {
        return 1;
    }
    
    // Загрузка текстуры игрока
    const char* texture_paths = "engine/icons/64px.png";
    
    player_texture = MIR_LoadTexture(texture_paths);
    
    if (!player_texture) {
        printf("[MIRULIT] WARNING: Could not load player texture. Using colored ship instead.\n");
    }
    
    // Создание игрока
    player = MIR_CreateEntity("Player");
    player->transform.position = (MIR_Vec2){400, 300};
    player->transform.scale = (MIR_Vec2){64, 64};
    last_player_pos = player->transform.position;
    
    if (player_texture) {
        player->sprite.texture = player_texture;
        player->sprite.color = MIR_COLOR_WHITE;
    } else {
        // Альтернативный спрайт из примитивов (будет отрисован в draw)
        player->sprite.color = MIR_COLOR_CYAN;
        player->draw = NULL; // Можно добавить кастомную отрисовку
    }
    
    player->update = PlayerUpdate;
    player->collider.bounds = (MIR_Rect){0, 0, 40, 40};
    player->collider.enabled = true;
    
    // Создание врага
    enemy = MIR_CreateEntity("Enemy");
    enemy->transform.position = (MIR_Vec2){200, 200};
    enemy->transform.scale = (MIR_Vec2){50, 50};
    enemy->sprite.color = MIR_COLOR_RED;
    enemy->update = EnemyUpdate;
    enemy->collider.bounds = (MIR_Rect){0, 0, 50, 50};
    enemy->collider.enabled = true;
    
    // Создание UI кнопки
    MIR_CreateButton(
        (MIR_Rect){650, 20, 120, 40},
        "RESTART",
        (MIR_Color){50, 150, 50, 255},
        MIR_COLOR_WHITE,
        OnRestartButtonClick
    );
    
    // Главный игровой цикл
    while (MIR_IsRunning()) {
        // Обработка ввода
        MIR_ProcessEvents();
        
        // Выход по Escape
        if (MIR_IsKeyPressed(SDLK_ESCAPE)) {
            break;
        }
        
        // Замедление времени по Tab
        if (MIR_IsKeyPressed(SDLK_TAB)) {
            MIR_SetTimeScale(0.2f);
        }
        if (MIR_IsKeyReleased(SDLK_TAB)) {
            MIR_SetTimeScale(1.0f);
        }
        
        // Стрельба по левой кнопке мыши
        shoot_cooldown -= MIR_GetDeltaTime();
        if (MIR_IsMouseButtonDown(0) && shoot_cooldown <= 0) {
            if (player) {
                MIR_Vec2 mouse_world = MIR_GetMouseWorldPosition();
                MIR_Vec2 direction = MIR_Vec2_Subtract(mouse_world, player->transform.position);
                
                // Создаем пулю
                CreateBullet(player->transform.position, direction);
                
                shoot_cooldown = 0.15f; // Скорость стрельбы
            }
        }
        
        // Начало кадра
        MIR_BeginFrame();
        
        // Спавн врагов по таймеру
        spawn_timer += MIR_GetDeltaTime();
        if (spawn_timer > 2.0f && _mir->entity_count < 20) { // Не больше 20 врагов
            MIR_Entity* new_enemy = MIR_CreateEntity("Enemy");
            new_enemy->transform.position = (MIR_Vec2){
                MIR_Math_RandomRange(50, 750),
                MIR_Math_RandomRange(50, 550)
            };
            new_enemy->transform.scale = (MIR_Vec2){
                MIR_Math_RandomRange(35, 65),
                MIR_Math_RandomRange(35, 65)
            };
            new_enemy->sprite.color = (MIR_Color){
                (uint8_t)MIR_Math_RandomRange(150, 255),
                (uint8_t)MIR_Math_RandomRange(50, 100),
                (uint8_t)MIR_Math_RandomRange(50, 100),
                255
            };
            new_enemy->update = EnemyUpdate;
            new_enemy->collider.bounds = (MIR_Rect){0, 0, 
                new_enemy->transform.scale.x, new_enemy->transform.scale.y};
            new_enemy->collider.enabled = true;
            spawn_timer = 0;
        }
        
        // Обновление
        if (!MIR_IsPaused()) {
            MIR_UpdateEntities();
            MIR_UpdateParticles();
            CheckCollisions();
        }
        
        // Отрисовка прицела
        MIR_Vec2 mouse_pos = MIR_GetMousePosition();
        MIR_DrawLine(
            (MIR_Vec2){mouse_pos.x - 12, mouse_pos.y},
            (MIR_Vec2){mouse_pos.x + 12, mouse_pos.y},
            (MIR_Color){255, 100, 100, 220},
            2.0f
        );
        MIR_DrawLine(
            (MIR_Vec2){mouse_pos.x, mouse_pos.y - 12},
            (MIR_Vec2){mouse_pos.x, mouse_pos.y + 12},
            (MIR_Color){255, 100, 100, 220},
            2.0f
        );
        
        // Отрисовка
        MIR_DrawEntities();
        MIR_DrawParticles();
        
        // Отрисовка UI панели
        MIR_DrawRect((MIR_Rect){10, 10, 250, 90}, (MIR_Color){0, 0, 0, 180});
        
        // Отрисовка счета и статистики
        char score_text[64];
        snprintf(score_text, sizeof(score_text), "SCORE: %d", score);
        
        char enemies_text[64];
        snprintf(enemies_text, sizeof(enemies_text), "ENEMIES: %d", enemies_destroyed);
        
        char fps_text[64];
        snprintf(fps_text, sizeof(fps_text), "FPS: %d", MIR_GetFPS());
        
        // Здесь могла бы быть отрисовка текста, но для простоты используем дебаг вывод
        printf("\rSCORE: %6d | ENEMIES: %3d | FPS: %3d | BULLETS: %3d | PLAYER VEL: %.1f,%.1f", 
               score, enemies_destroyed, MIR_GetFPS(), bullet_count,
               player_velocity.x, player_velocity.y);
        fflush(stdout);
        
        // Конец кадра
        MIR_EndFrame();
    }
    
    // Завершение
    MIR_Shutdown();
    printf("\nGame Over! Final Score: %d\n", score);
    return 0;
}