// main.cpp
#include <iostream>
#include <functional>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image/stb_image.h>

#include "../core/core.h"
#include "../gui/window.h"
#include "../gui/windows/filemenanger.h"
#include "../gui/editor.h"

using namespace MirulitEngine;
using namespace MirulitComponets;

int main() {
    Window window(1200, 600, "Mirulit Engine Editor");
    
    MirulitGUI::init(window.window);
    
    // Создаем редактор
    MirulitGUI::Editor editor;
    
    Entity test({0, 0}, {0.2, 0.2}, 0);
    test.Circle(64);
    test.initRender("assets/textures/cube.png");
    
    window.update([&]() {
        MirulitGUI::updateStart();
        
        // Рендерим интерфейс редактора
        editor.Render();
        
        // Ваша сцена (может рендериться в отдельную текстуру)
        glClearColor(0.4078f, 0.83137f, 1.0f, 1.0f);
        test.draw();
        
        MirulitGUI::updateEnd();
    });
    
    MirulitGUI::CleanUp();
    return 0;
}