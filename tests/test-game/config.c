#include <mirulit/window.h>
#include <mirulit/maths.h>

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_F1 && action == GLFW_PRESS) {
        printf("F1 pressed!\n");
    }
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    static double lastX = 0, lastY = 0;
    
    if (lastX == 0 && lastY == 0)
    {
        lastX = xpos;
        lastY = ypos;
    }
    
    double xoffset = xpos - lastX;
    double yoffset = lastY - ypos;
    
    lastX = xpos;
    lastY = ypos;
}

int main(void)
{
    if (!window_init(1024, 768, "Test game"))
        return -1;
    
    window_set_vsync(true);
    window_set_background_color(0.1f, 0.1f, 0.1f, 1.0f);
    
    window_set_key_callback(key_callback);
    window_set_mouse_callback(mouse_callback);
    
    window_print_info();
    
    while (!window_should_close())
    {
        window_process_input();
        
        window_clear();
        
        // Renderer

        window_swap_buffers();
        window_poll_events();
    }
    
    window_terminate();
    
    return 0;
}