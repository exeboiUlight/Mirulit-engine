#ifndef CAMERA_H
#define CAMERA_H

#include "../matrix.h"
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif

typedef struct {
    // Позиция камеры
    float position[3];
    
    // Направление (углы Эйлера в градусах)
    float yaw;      // поворот влево-вправо
    float pitch;    // наклон вверх-вниз
    
    // Векторы ориентации
    float front[3];
    float right[3];
    float up[3];
    
    // Параметры проекции
    float fov;
    float aspect_ratio;
    float near_plane;
    float far_plane;
    
    // Матрицы
    float view_matrix[16];
    float projection_matrix[16];
    
} Camera;

// Инициализация камеры
void camera_init(Camera* cam, float pos_x, float pos_y, float pos_z, 
                 float yaw, float pitch, float fov, float aspect);

// Обновление векторов камеры на основе углов
void camera_update_vectors(Camera* cam);

// Обновление матрицы вида
void camera_update_view_matrix(Camera* cam);

// Обновление матрицы проекции
void camera_update_projection_matrix(Camera* cam, float aspect);

// Управление движением
void camera_move_forward(Camera* cam, float speed);
void camera_move_backward(Camera* cam, float speed);
void camera_move_right(Camera* cam, float speed);
void camera_move_left(Camera* cam, float speed);
void camera_move_up(Camera* cam, float speed);
void camera_move_down(Camera* cam, float speed);

// Управление вращением
void camera_rotate(Camera* cam, float delta_yaw, float delta_pitch);

// Получение матрицы вида
float* camera_get_view_matrix(Camera* cam);

// Получение матрицы проекции
float* camera_get_projection_matrix(Camera* cam);

void camera_init(Camera* cam, float pos_x, float pos_y, float pos_z, 
                 float yaw, float pitch, float fov, float aspect) {
    // Инициализация позиции
    cam->position[0] = pos_x;
    cam->position[1] = pos_y;
    cam->position[2] = pos_z;
    
    // Инициализация углов
    cam->yaw = yaw;
    cam->pitch = pitch;
    
    // Инициализация параметров проекции
    cam->fov = fov;
    cam->aspect_ratio = aspect;
    cam->near_plane = 0.1f;
    cam->far_plane = 100.0f;
    
    // Инициализация векторов
    camera_update_vectors(cam);
    
    // Создание матриц
    camera_update_view_matrix(cam);
    camera_update_projection_matrix(cam, aspect);
}

void camera_update_vectors(Camera* cam) {
    // Преобразуем углы Эйлера в радианы
    float yaw_rad = cam->yaw * M_PI / 180.0f;
    float pitch_rad = cam->pitch * M_PI / 180.0f;
    
    // Вычисляем направление взгляда (front vector)
    cam->front[0] = cosf(yaw_rad) * cosf(pitch_rad);
    cam->front[1] = sinf(pitch_rad);
    cam->front[2] = sinf(yaw_rad) * cosf(pitch_rad);
    
    // Нормализуем вектор направления
    float length = sqrtf(cam->front[0]*cam->front[0] + 
                         cam->front[1]*cam->front[1] + 
                         cam->front[2]*cam->front[2]);
    cam->front[0] /= length;
    cam->front[1] /= length;
    cam->front[2] /= length;
    
    // Вычисляем правый вектор (right vector) как cross(front, world_up)
    float world_up[3] = {0.0f, 1.0f, 0.0f};
    
    cam->right[0] = cam->front[1] * world_up[2] - cam->front[2] * world_up[1];
    cam->right[1] = cam->front[2] * world_up[0] - cam->front[0] * world_up[2];
    cam->right[2] = cam->front[0] * world_up[1] - cam->front[1] * world_up[0];
    
    // Нормализуем правый вектор
    length = sqrtf(cam->right[0]*cam->right[0] + 
                   cam->right[1]*cam->right[1] + 
                   cam->right[2]*cam->right[2]);
    cam->right[0] /= length;
    cam->right[1] /= length;
    cam->right[2] /= length;
    
    // Вычисляем вектор вверх (up vector) как cross(right, front)
    cam->up[0] = cam->right[1] * cam->front[2] - cam->right[2] * cam->front[1];
    cam->up[1] = cam->right[2] * cam->front[0] - cam->right[0] * cam->front[2];
    cam->up[2] = cam->right[0] * cam->front[1] - cam->right[1] * cam->front[0];
    
    // Нормализуем вектор вверх
    length = sqrtf(cam->up[0]*cam->up[0] + 
                   cam->up[1]*cam->up[1] + 
                   cam->up[2]*cam->up[2]);
    cam->up[0] /= length;
    cam->up[1] /= length;
    cam->up[2] /= length;
}

void camera_update_view_matrix(Camera* cam) {
    // Вычисляем точку, в которую смотрим
    float target[3];
    target[0] = cam->position[0] + cam->front[0];
    target[1] = cam->position[1] + cam->front[1];
    target[2] = cam->position[2] + cam->front[2];
    
    // Используем существующую функцию create_view_matrix
    create_view_matrix(cam->view_matrix,
                       cam->position[0], cam->position[1], cam->position[2],
                       target[0], target[1], target[2],
                       cam->up[0], cam->up[1], cam->up[2]);
}

void camera_update_projection_matrix(Camera* cam, float aspect) {
    cam->aspect_ratio = aspect;
    create_perspective_matrix(cam->projection_matrix,
                              cam->fov, cam->aspect_ratio,
                              cam->near_plane, cam->far_plane);
}

void camera_move_forward(Camera* cam, float speed) {
    cam->position[0] += cam->front[0] * speed;
    cam->position[1] += cam->front[1] * speed;
    cam->position[2] += cam->front[2] * speed;
    camera_update_view_matrix(cam);
}

void camera_move_backward(Camera* cam, float speed) {
    cam->position[0] -= cam->front[0] * speed;
    cam->position[1] -= cam->front[1] * speed;
    cam->position[2] -= cam->front[2] * speed;
    camera_update_view_matrix(cam);
}

void camera_move_right(Camera* cam, float speed) {
    cam->position[0] += cam->right[0] * speed;
    cam->position[1] += cam->right[1] * speed;
    cam->position[2] += cam->right[2] * speed;
    camera_update_view_matrix(cam);
}

void camera_move_left(Camera* cam, float speed) {
    cam->position[0] -= cam->right[0] * speed;
    cam->position[1] -= cam->right[1] * speed;
    cam->position[2] -= cam->right[2] * speed;
    camera_update_view_matrix(cam);
}

void camera_move_up(Camera* cam, float speed) {
    cam->position[1] += speed;  // Просто двигаем по мировой оси Y
    camera_update_view_matrix(cam);
}

void camera_move_down(Camera* cam, float speed) {
    cam->position[1] -= speed;  // Просто двигаем по мировой оси Y
    camera_update_view_matrix(cam);
}

void camera_rotate(Camera* cam, float delta_yaw, float delta_pitch) {
    cam->yaw += delta_yaw;
    cam->pitch += delta_pitch;
    
    // Ограничиваем угол наклона (pitch), чтобы не было переворота
    if (cam->pitch > 89.0f) cam->pitch = 89.0f;
    if (cam->pitch < -89.0f) cam->pitch = -89.0f;
    
    // Обновляем векторы и матрицу
    camera_update_vectors(cam);
    camera_update_view_matrix(cam);
}

float* camera_get_view_matrix(Camera* cam) {
    return cam->view_matrix;
}

float* camera_get_projection_matrix(Camera* cam) {
    return cam->projection_matrix;
}

#endif