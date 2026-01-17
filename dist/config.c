#define MIRULIT_IMPLEMENTATION

#include <Mirulit.h>

void Update() {}

int main() {
    
    MirulitWindow* window = MirulitEngineInit(1200, 600, "Mirulit game engine", (Vector3){100, 100, 100});
    if (!window) {
        return -1;
    }

    Object3D Scene1[0];
    int scene1Count = 0;

    MirulitEngineScene(window, Scene1, scene1Count, 0, 0, Update);

    MirulitEngineEndScene(Scene1, scene1Count);

    MirulitEngineDestroy(window);

    return 0;
}