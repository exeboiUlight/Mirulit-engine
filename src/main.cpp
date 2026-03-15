// main.cpp
#include <iostream>
#include <functional>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image/stb_image.h>

#include "../core/core.h"
#include "../gui/window.h"
#include "../gui/hub.h"

using namespace MirulitEngine;
using namespace MirulitComponets;

int main() {
    MirulitEngine::Window window(1000, 700, "Mirulit Engine 2.0.0");
    
    MirulitGUI::init(window.getGLFWwindow());
    
    MirulitGUI::Hub hub;
    
    window.update([&]() {
        MirulitGUI::updateStart();
        
        hub.Render();
        
        MirulitGUI::updateEnd();
        
        if (!hub.IsLoading()) {
            
        }
    });
    
    MirulitGUI::CleanUp();
    return 0;
}