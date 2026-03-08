#ifndef WINDOW_H
#define WINDOW_H

DLL_EXPORT bool MirulitInit(int width, int height, const char* title, const char* icon) {

    if (!glfwInit())
        return false;

    _mir.window.width = width;
    _mir.window.height = height;
    _mir.window.title = title;
    _mir.window.icon = icon;

    _mir.window.window = glfwCreateWindow(_mir.window.width, _mir.window.height, _mir.window.title, NULL, NULL);

    if (!_mir.window.window)
    {
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(_mir.window.window);

    return true;
}

DLL_EXPORT void MirulitLoop(void *(update)()) {
    while (!glfwWindowShouldClose(_mir.window.window)) {
        glClear(GL_COLOR_BUFFER_BIT);

        /* Swap front and back buffers */
        glfwSwapBuffers(_mir.window.window);

        if (!update()) {
            glfwTerminate();
        }

        /* Poll for and process events */
        glfwPollEvents();
    }
    glfwTerminate();
}

#endif