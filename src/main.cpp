#include <core/utils/window.h>
#include <core/components/model.h>
#include <core/components/sprite.h>
#include <core/components/text.h>
#include <core/graphics/mesh.h>
#include <core/graphics/shader.h>
#include <iostream>

using namespace Engine;

int main() {
    // Создаем окно
    Window window(WindowProps("Engine Demo", 1200, 600, true));
    
    // Проверяем инициализацию окна
    if (window.GetNativeWindow() == nullptr) {
        std::cout << "Failed to create window!" << std::endl;
        return -1;
    }
    
    // Создаем шейдер для спрайтов
    Shader spriteShader;
    spriteShader.LoadFromSource(spriteVertexShaderSource, spriteFragmentShaderSource);
    
    // Создаем шейдер для текста
    Shader textShader;
    textShader.LoadFromSource(textVertexShaderSource, textFragmentShaderSource);
    
    // Создаем текстурный рендерер
    Texture2D texture;
    // Раскомментируйте, когда добавите текстуру
    // if (!texture.LoadFromFile("assets/textures/test.png")) {
    //     std::cout << "Warning: Could not load texture" << std::endl;
    // }
    
    // Создаем спрайт
    Sprite sprite;
    
    // Инициализируем текстовый рендерер
    TextRenderer textRenderer;
    // Раскомментируйте, когда добавите шрифт
    // if (!textRenderer.Init("assets/fonts/arial.ttf", 48)) {
    //     std::cout << "Warning: Could not load font" << std::endl;
    // }
    textRenderer.SetScreenSize(window.GetWidth(), window.GetHeight());
    
    // Загружаем 3D модель (опционально)
    Model model;
    // model.LoadModel("assets/models/cube.obj");
    
    // Переменная для вращения
    float rotation = 0.0f;
    
    // Главный цикл
    while (!window.ShouldClose()) {
        // Очищаем экран
        window.SetClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        window.Clear();
        
        // Рисуем спрайт (если текстура загружена)
        if (texture.GetID() != 0) {
            sprite.Draw(spriteShader, texture, 
                       100.0f, 100.0f,  // x, y
                       200.0f, 200.0f,  // width, height
                       rotation,        // rotation
                       1.0f, 1.0f, 1.0f); // color (white)
        } else {
            // Если текстура не загружена, рисуем квадрат с цветом
            // Для этого нужно создать простой шейдер или использовать Sprite с цветом
            std::cout << "Texture not loaded, skipping sprite drawing" << std::endl;
        }
        
        // Рисуем текст
        textRenderer.RenderText(textShader, "Hello, Engine!", 
                                50.0f, window.GetHeight() - 50.0f, 
                                1.0f, 1.0f, 0.0f, 0.0f); // Красный текст
        
        // Рисуем дополнительный текст с информацией
        std::string fpsText = "FPS: " + std::to_string(60); // В реальном коде нужно считать FPS
        textRenderer.RenderText(textShader, fpsText, 
                                50.0f, window.GetHeight() - 100.0f, 
                                0.8f, 1.0f, 1.0f, 1.0f); // Белый текст
        
        // Рисуем 3D модель (если загружена)
        if (model.IsLoaded()) {
            // Здесь нужно настроить матрицы view и projection для 3D
            // и вызвать model.Draw(shader);
        }
        
        rotation += 1.0f;
        if (rotation >= 360.0f) {
            rotation -= 360.0f;
        }
        
        window.OnUpdate();
    }
    
    // Шейдеры и ресурсы автоматически очистятся в деструкторах
    return 0;
}