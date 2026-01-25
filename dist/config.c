#include <mirulit/window.h>
#include <mirulit/maths.h>

/* Callback для обработки клавиш */
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_F1 && action == GLFW_PRESS) {
        printf("F1 pressed!\n");
    }
}

/* Callback для перемещения мыши */
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    static double lastX = 0, lastY = 0;
    
    if (lastX == 0 && lastY == 0) {
        lastX = xpos;
        lastY = ypos;
    }
    
    double xoffset = xpos - lastX;
    double yoffset = lastY - ypos; /* Обратный порядок для Y */
    
    lastX = xpos;
    lastY = ypos;
    
    /* Можно использовать xoffset и yoffset для камеры */
}

int main(void)
{
    /* Инициализация окна */
    if (!window_init(1024, 768, "My GLFW Window")) {
        return -1;
    }
    
    /* Настройка параметров окна */
    window_set_vsync(true);
    window_set_background_color(0.1f, 0.1f, 0.1f, 1.0f);
    
    /* Установка callback функций */
    window_set_key_callback(key_callback);
    window_set_mouse_callback(mouse_callback);
    
    /* Вывод информации */
    window_print_info();
    
    /* Основной цикл */
    while (!window_should_close()) {
        /* Обработка ввода */
        window_process_input();
        
        /* Очистка буферов */
        window_clear();
        
        /* Здесь ваша отрисовка */
        
        /* Обмен буферов и обработка событий */
        window_swap_buffers();
        window_poll_events();
    }
    
    /* Завершение работы */
    window_terminate();
    
    return 0;
}