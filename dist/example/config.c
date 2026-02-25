#include <mirulit.h>

MIR_Entity* player = NULL;

void UpdatePlayer(MIR_Entity* entity, float delta) {
    float speed = 200.0f;
    
    if (MIR_IsKeyDown(SDLK_W))
        entity->transform.position.y -= speed * delta;
    
    if (MIR_IsKeyDown(SDLK_S))
        entity->transform.position.y += speed * delta;
    
    if (MIR_IsKeyDown(SDLK_A))
        entity->transform.position.x -= speed * delta;
    
    if (MIR_IsKeyDown(SDLK_D))
        entity->transform.position.x += speed * delta;
}

int main() {
    if (!MIR_Init("My Game", 800, 600, false, false))
        return 1;
    
    player = MIR_CreateEntity("player");
    player->transform.position = (MIR_Vec2){400, 300};
    player->transform.scale = (MIR_Vec2){50, 50};
    player->sprite.color = MIR_COLOR_GREEN;
    player->update = UpdatePlayer;
    
    while (MIR_IsRunning()) {
        MIR_ProcessEvents();
        MIR_BeginFrame();
        
        MIR_UpdateEntities();
        MIR_UpdateParticles();
        
        MIR_DrawEntities();
        MIR_DrawParticles();
        
        MIR_EndFrame();
    }
    
    MIR_Shutdown();
    return 0;
}