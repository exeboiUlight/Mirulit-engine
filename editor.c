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
#include <stdio.h>
#include <time.h>
#include <math.h>
#include "core/graphics/matrix.h"

int main(void) {
    MirulitWindow* window = NewWindow(1200, 600, "Mirulit - Textured 3D Model");
    
    // Инициализация OpenGL
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    
    // Включаем режим отладки текстур
    Texture_SetDebugMode(true);
    
    // Создаем текстурированный шейдер
    Shader* texturedShader = shader_create_from_files(
        "engine/shaders/standart/textured.vert",
        "engine/shaders/standart/textured.frag",
        "TestShader"
    );
    if (!texturedShader) {
        printf("ERROR: Failed to create textured shader!\n");
        quitWindow(window);
        return 1;
    }
    
    // Загружаем текстуру
    GLTexture* woodTexture = Texture_Create("engine/textures/wood.jpg", true);
    if (!woodTexture) {
        printf("WARNING: Failed to load wood texture. Using fallback texture.\n");
        // Создаем простую текстуру программно как запасной вариант
        unsigned char brickData[] = {
            255, 100, 100,   200, 50, 50,
            200, 50, 50,     255, 100, 100
        };
        woodTexture = Texture_CreateFromMemory(brickData, 2, 2, 3, "FallbackBrick");
    }
    
    // Вариант 1: Создаем куб с текстурой
    printf("Creating textured cube...\n");
    Vec3 cubeColor = rgb_to_vec3(255, 255, 255);
    Mesh* cubeMesh = mesh_create_cube(1.0f, cubeColor, "TexturedCube");
    
    if (cubeMesh && woodTexture) {
        mesh_set_texture(cubeMesh, woodTexture);
        printf("Texture applied to cube.\n");
    }
    
    Object3D* cube = object_create("TexturedCube");
    if (cube) {
        cube->mesh = cubeMesh;
        cube->shader = texturedShader;
        object_set_position(cube, vec3_create(-2.0f, 0.0f, -5.0f));
    }
    
    // Вариант 2: Создаем плоскость с текстурой
    printf("Creating textured plane...\n");
    Mesh* planeMesh = mesh_create_plane(3.0f, 3.0f, cubeColor, "TexturedPlane");
    
    if (planeMesh && woodTexture) {
        // Можно использовать ту же текстуру или загрузить другую
        mesh_set_texture(planeMesh, woodTexture);
    }
    
    Object3D* plane = object_create("TexturedPlane");
    if (plane) {
        plane->mesh = planeMesh;
        plane->shader = texturedShader;
        object_set_position(plane, vec3_create(2.0f, 0.0f, -5.0f));
        object_set_rotation(plane, vec3_create(-90.0f, 0.0f, 0.0f));
    }
    
    // Матрицы
    float projection[16];
    float view[16];
    float model[16];
    
    float aspect = 1200.0f / 600.0f;
    create_perspective_matrix(projection, 45.0f, aspect, 0.1f, 100.0f);
    create_view_matrix(view, 0.0f, 2.0f, 8.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f);
    
    // Основной цикл
    float rotationAngle = 0.0f;
    while (!isClose(window)) {
        // Очистка
        glClearColor(0.2f, 0.3f, 0.4f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // Обновляем вращение
        rotationAngle += 1.0f;
        
        // Рисуем куб с текстурой
        if (cube) {
            shader_use(texturedShader);
            
            // Устанавливаем матрицы
            create_model_matrix(model, 
                              cube->position.x, cube->position.y, cube->position.z,
                              rotationAngle, rotationAngle * 0.7f, 0.0f,
                              1.0f, 1.0f, 1.0f);
            
            shader_set_mat4(texturedShader, "projection", projection);
            shader_set_mat4(texturedShader, "view", view);
            shader_set_mat4(texturedShader, "model", model);
            
            // Настройки освещения
            shader_set_vec3(texturedShader, "light.position", 3.0f, 3.0f, 3.0f);
            shader_set_vec3(texturedShader, "light.ambient", 0.2f, 0.2f, 0.2f);
            shader_set_vec3(texturedShader, "light.diffuse", 0.8f, 0.8f, 0.8f);
            shader_set_vec3(texturedShader, "light.specular", 1.0f, 1.0f, 1.0f);
            shader_set_vec3(texturedShader, "viewPos", 0.0f, 2.0f, 8.0f);
            shader_set_vec3(texturedShader, "material.specular", 0.5f, 0.5f, 0.5f);
            shader_set_float(texturedShader, "material.shininess", 32.0f);
            
            // Рисуем объект
            object_draw(cube);
        }
        
        // Рисуем плоскость с текстурой
        if (plane) {
            shader_use(texturedShader);
            
            create_model_matrix(model,
                              plane->position.x, plane->position.y, plane->position.z,
                              plane->rotation.x, plane->rotation.y, plane->rotation.z,
                              1.0f, 1.0f, 1.0f);
            
            shader_set_mat4(texturedShader, "model", model);
            
            // Рисуем объект
            object_draw(plane);
        }
        
        // Обновление окна
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    // Очистка
    if (cube) object_destroy(cube);
    if (plane) object_destroy(plane);
    if (texturedShader) shader_destroy(texturedShader);
    if (woodTexture) Texture_Destroy(woodTexture);
    
    Texture_CleanupAll(); // Очищаем все текстуры
    
    quitWindow(window);
    return 0;
}