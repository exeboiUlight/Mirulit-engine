#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>
#include <functional>
#include <unordered_map>
#include <iostream>

namespace Engine {
    
    struct WindowProps {
        std::string Title;
        unsigned int Width;
        unsigned int Height;
        bool VSync;
        
        WindowProps(const std::string& title = "Engine Window", 
                   unsigned int width = 1200, 
                   unsigned int height = 600,
                   bool vsync = true)
            : Title(title), Width(width), Height(height), VSync(vsync) {}
    };
    
    class Window {
    public:
        using EventCallbackFn = std::function<void(int key, int scancode, int action, int mods)>;
        using MouseButtonCallbackFn = std::function<void(int button, int action, int mods)>;
        using CursorPosCallbackFn = std::function<void(double xpos, double ypos)>;
        using ScrollCallbackFn = std::function<void(double xoffset, double yoffset)>;
        using WindowResizeCallbackFn = std::function<void(int width, int height)>;
        
        Window(const WindowProps& props = WindowProps()) {
            Init(props);

            glfwSetFramebufferSizeCallback(m_Window, [](GLFWwindow* window, int width, int height) {
                WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
                data.Width = width;
                data.Height = height;
                glViewport(0, 0, width, height);  // <-- Вот здесь уже есть glViewport!
                
                if (data.WindowResizeCallback) {
                    data.WindowResizeCallback(width, height);
                }
            });
        }
        
        ~Window() {
            Shutdown();
        }
        
        // Delete copy
        Window(const Window&) = delete;
        Window& operator=(const Window&) = delete;
        
        void Init(const WindowProps& props) {
            m_Data.Title = props.Title;
            m_Data.Width = props.Width;
            m_Data.Height = props.Height;
            m_Data.VSync = props.VSync;
            
            if (!glfwInit()) {
                std::cout << "Failed to initialize GLFW" << std::endl;
                return;
            }
            
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
            
            #ifdef __APPLE__
            glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
            #endif
            
            m_Window = glfwCreateWindow(m_Data.Width, m_Data.Height, m_Data.Title.c_str(), nullptr, nullptr);
            if (!m_Window) {
                std::cout << "Failed to create GLFW window" << std::endl;
                glfwTerminate();
                return;
            }
            
            glfwMakeContextCurrent(m_Window);
            
            if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
                std::cout << "Failed to initialize GLAD" << std::endl;
                return;
            }
            
            SetVSync(m_Data.VSync);
            
            // Set user pointer for callbacks
            glfwSetWindowUserPointer(m_Window, &m_Data);
            
            // Setup callbacks
            SetupCallbacks();
            
            std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
        }
        
        void Shutdown() {
            if (m_Window) {
                glfwDestroyWindow(m_Window);
                glfwTerminate();
                m_Window = nullptr;
            }
        }
        
        void OnUpdate() {
            glfwPollEvents();
            glfwSwapBuffers(m_Window);
        }
        
        void Clear() const {
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        }
        
        void SetClearColor(float r, float g, float b, float a = 1.0f) const {
            glClearColor(r, g, b, a);
        }
        
        bool ShouldClose() const {
            return glfwWindowShouldClose(m_Window);
        }
        
        void SetShouldClose(bool shouldClose) {
            glfwSetWindowShouldClose(m_Window, shouldClose);
        }
        
        unsigned int GetWidth() const { return m_Data.Width; }
        unsigned int GetHeight() const { return m_Data.Height; }
        float GetAspectRatio() const { return (float)m_Data.Width / (float)m_Data.Height; }
        
        GLFWwindow* GetNativeWindow() const { return m_Window; }
        
        void SetVSync(bool enabled) {
            glfwSwapInterval(enabled ? 1 : 0);
            m_Data.VSync = enabled;
        }
        
        bool IsVSync() const { return m_Data.VSync; }
        
        void SetTitle(const std::string& title) {
            m_Data.Title = title;
            glfwSetWindowTitle(m_Window, title.c_str());
        }
        
        void SetSize(unsigned int width, unsigned int height) {
            m_Data.Width = width;
            m_Data.Height = height;
            glfwSetWindowSize(m_Window, width, height);
        }
        
        // Event callbacks
        void SetEventCallback(const EventCallbackFn& callback) { m_Data.EventCallback = callback; }
        void SetMouseButtonCallback(const MouseButtonCallbackFn& callback) { m_Data.MouseButtonCallback = callback; }
        void SetCursorPosCallback(const CursorPosCallbackFn& callback) { m_Data.CursorPosCallback = callback; }
        void SetScrollCallback(const ScrollCallbackFn& callback) { m_Data.ScrollCallback = callback; }
        void SetWindowResizeCallback(const WindowResizeCallbackFn& callback) { m_Data.WindowResizeCallback = callback; }
        
        // Input helpers
        static bool IsKeyPressed(int keycode) {
            auto window = glfwGetCurrentContext();
            return glfwGetKey(window, keycode) == GLFW_PRESS;
        }
        
        static bool IsMouseButtonPressed(int button) {
            auto window = glfwGetCurrentContext();
            return glfwGetMouseButton(window, button) == GLFW_PRESS;
        }
        
        static void GetMousePos(double& xpos, double& ypos) {
            auto window = glfwGetCurrentContext();
            glfwGetCursorPos(window, &xpos, &ypos);
        }
        
    private:
        struct WindowData {
            std::string Title;
            unsigned int Width;
            unsigned int Height;
            bool VSync;
            
            EventCallbackFn EventCallback;
            MouseButtonCallbackFn MouseButtonCallback;
            CursorPosCallbackFn CursorPosCallback;
            ScrollCallbackFn ScrollCallback;
            WindowResizeCallbackFn WindowResizeCallback;
            
            WindowData()
                : Title(""), Width(0), Height(0), VSync(true) {}
        };
        
        GLFWwindow* m_Window = nullptr;
        WindowData m_Data;
        
        void SetupCallbacks() {
            // Key callback
            glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
                WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
                if (data.EventCallback) {
                    data.EventCallback(key, scancode, action, mods);
                }
            });
            
            // Mouse button callback
            glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int mods) {
                WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
                if (data.MouseButtonCallback) {
                    data.MouseButtonCallback(button, action, mods);
                }
            });
            
            // Cursor position callback
            glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xpos, double ypos) {
                WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
                if (data.CursorPosCallback) {
                    data.CursorPosCallback(xpos, ypos);
                }
            });
            
            // Scroll callback
            glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xoffset, double yoffset) {
                WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
                if (data.ScrollCallback) {
                    data.ScrollCallback(xoffset, yoffset);
                }
            });
            
            // Window resize callback
            glfwSetFramebufferSizeCallback(m_Window, [](GLFWwindow* window, int width, int height) {
                WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
                data.Width = width;
                data.Height = height;
                glViewport(0, 0, width, height);
                
                if (data.WindowResizeCallback) {
                    data.WindowResizeCallback(width, height);
                }
            });
        }
    };
    
}