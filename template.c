// editor.c
#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#endif

#include "core/graphics/window.h"
#include "core/graphics/objects/mesh.h"
#include "core/graphics/objects/object.h"
#include "core/graphics/rgb.h"
#include "core/graphics/shader.h"
#include "core/graphics/texture.h"
#include "core/utils/math/time.h"
#include <stdio.h>
#include <time.h>
#include <math.h>
#include "core/graphics/matrix.h"
#include "core/utils/json.h"

int main(void) {
    MirulitWindow* window = NewWindow(1200, 600, "Mirulit game engine");

    glfwSwapInterval(0);
    
    // Инициализация OpenGL
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    
    // Инициализация таймера
    Timer* timer = timer_create();
    if (!timer) {
        printf("ERROR: Failed to create timer!\n");
        quitWindow(window);
        return 1;
    }
    
    // Включаем режим отладки текстур
    Texture_SetDebugMode(false);
    
    // Создаем текстурированный шейдер
    Shader* texturedShader = shader_create_from_files(
        "engine/shaders/standart/textured.vert",
        "engine/shaders/standart/textured.frag",
        "CubeShader"
    );
    if (!texturedShader) {
        printf("ERROR: Failed to create textured shader!\n");
        timer_destroy(timer);
        quitWindow(window);
        return 1;
    }
    
    // Загружаем текстуру для куба
    GLTexture* cubeTexture = Texture_Create("engine/icons/16px.png", true);
    if (!cubeTexture) {
        printf("WARNING: Failed to load texture. Creating simple colored cube instead.\n");
    }
    
    // Создаем куб
    printf("Creating rotating cube...\n");
    Vec3 cubeColor = rgb_to_vec3(200, 100, 50); // Оранжевый цвет
    Mesh* cubeMesh = mesh_create_cube(1.0f, cubeColor, "RotatingCube");
    
    // Если есть текстура - применяем ее, иначе используем цвет
    if (cubeMesh && cubeTexture) {
        mesh_set_texture(cubeMesh, cubeTexture);
        printf("Texture applied to cube.\n");
    }
    
    Object3D* cube = object_create("RotatingCube");
    if (cube) {
        cube->mesh = cubeMesh;
        cube->shader = texturedShader;
        object_set_position(cube, vec3_create(0.0f, 0.0f, -5.0f));
    }
    
    // Матрицы
    float projection[16];
    float view[16];
    float model[16];
    
    float aspect = 1200.0f / 600.0f;
    create_perspective_matrix(projection, 45.0f, aspect, 0.1f, 100.0f);
    create_view_matrix(view, 0.0f, 2.0f, 8.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f);
    
    // Для заголовка окна
    char windowTitle[256];
    
    // Основной цикл
    float rotationAngle = 0.0f;
    while (!isClose(window)) {
        // Обновляем таймер
        timer_update(timer);
        timer_update_fps(timer);
        
        // Обновляем заголовок окна с FPS
        timer_format_window_title(timer, windowTitle, sizeof(windowTitle), 
                                 "Mirulit game engine");
        glfwSetWindowTitle(window, windowTitle);
        
        // Очистка экрана
        glClearColor(0.2f, 0.3f, 0.4f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // Обновляем вращение с учетом delta time
        rotationAngle += 50.0f * timer_get_delta_timef(timer); // 50 градусов в секунду
        
        // Рисуем куб
        if (cube) {
            shader_use(texturedShader);
            
            // Создаем матрицу модели с вращением
            create_model_matrix(model, 
                              cube->position.x, cube->position.y, cube->position.z,
                              rotationAngle, rotationAngle * 0.5f, rotationAngle * 0.3f,
                              1.0f, 1.0f, 1.0f);
            
            // Устанавливаем матрицы в шейдер
            shader_set_mat4(texturedShader, "projection", projection);
            shader_set_mat4(texturedShader, "view", view);
            shader_set_mat4(texturedShader, "model", model);
            
            // Настройки освещения (для лучшего вида)
            shader_set_vec3(texturedShader, "light.position", 3.0f, 3.0f, 3.0f);
            shader_set_vec3(texturedShader, "light.ambient", 0.2f, 0.2f, 0.2f);
            shader_set_vec3(texturedShader, "light.diffuse", 0.8f, 0.8f, 0.8f);
            shader_set_vec3(texturedShader, "light.specular", 1.0f, 1.0f, 1.0f);
            shader_set_vec3(texturedShader, "viewPos", 0.0f, 2.0f, 8.0f);
            
            // Рисуем объект
            object_draw(cube);
        }
        
        // Обновление окна
        glfwSwapBuffers(window);
        glfwPollEvents();
        
        // Опционально: ограничение FPS
        timer_limit_fps(timer, 60);
    }
    
    // Очистка
    if (cube) object_destroy(cube);
    if (texturedShader) shader_destroy(texturedShader);
    if (cubeTexture) Texture_Destroy(cubeTexture);
    
    Texture_CleanupAll();
    timer_destroy(timer);
    
    quitWindow(window);
    return 0;
}