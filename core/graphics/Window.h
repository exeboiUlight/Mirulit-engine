#pragma once

#include <functional>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

namespace MirulitEngine {
    class Window {
        private:
            int width;
            int height;
            const char* title;
            GLFWwindow* window = nullptr;

        public:
            Window(int width, int height, const char* title) {
                this->width = width;
                this->height = height;
                this->title = title;

                if (!glfwInit()) {
                    std::cerr << "Failed to initialize GLFW" << std::endl;
                    return;
                }

                glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
                glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
                glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

                window = glfwCreateWindow(width, height, title, nullptr, nullptr);

                if (!window) {
                    std::cerr << "Failed to create GLFW window" << std::endl;
                    glfwTerminate();
                    return;
                }

                glfwMakeContextCurrent(window);
                glfwSwapInterval(0);

                if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
                    std::cerr << "Failed to initialize GLAD" << std::endl;
                    glfwTerminate();
                    return;
                }

                glViewport(0, 0, width, height);
            }

            ~Window() {
                if (window) {
                    glfwDestroyWindow(window);
                }
                glfwTerminate();
            }

            void update() {
                if (!window) {
                    std::cerr << "Window is not valid" << std::endl;
                    return;
                }

                while (!glfwWindowShouldClose(window)) {
                    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                    glfwSwapBuffers(window);
                    glfwPollEvents();
                }
            }

            void update(std::function<void()> updateCallback) {
                if (!window) {
                    std::cerr << "Window is not valid" << std::endl;
                    return;
                }
            
                while (!glfwWindowShouldClose(window)) {
                    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            
                    if (updateCallback) {
                        updateCallback();
                    }
            
                    glfwSwapBuffers(window);
                    glfwPollEvents();
                }
            }

            bool shouldClose() const {
                return glfwWindowShouldClose(window);
            }

            GLFWwindow* getGLFWwindow() const { return window; }
            int getWidth() const { return width; }
            int getHeight() const { return height; }
    };
}