#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    unsigned int ID; // ID шейдерной программы
    const char* vertexSource; // Исходный код вершинного шейдера
    const char* fragmentSource; // Исходный код фрагментного шейдера
} Shader;

/**
 * @brief Компилирует шейдер
 * 
 * @param type Тип шейдера (GL_VERTEX_SHADER, GL_FRAGMENT_SHADER, etc.)
 * @param source Исходный код шейдера
 * @return ID скомпилированного шейдера или 0 при ошибке
 */
static inline unsigned int shaderCompile(GLenum type, const char* source) {
    unsigned int shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);
    
    int success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        fprintf(stderr, "Shader compilation failed: %s\n", infoLog);
        glDeleteShader(shader);
        return 0;
    }
    
    return shader;
}

/**
 * @brief Создает и компилирует шейдерную программу
 * 
 * @param shader Указатель на структуру Shader
 * @param vertexSource Исходный код вершинного шейдера
 * @param fragmentSource Исходный код фрагментного шейдера
 * @return 1 при успехе, 0 при ошибке
 */
static inline int shaderCreate(Shader* shader, 
                               const char* vertexSource, 
                               const char* fragmentSource) {
    
    shader->vertexSource = vertexSource;
    shader->fragmentSource = fragmentSource;
    
    // Компиляция шейдеров
    unsigned int vertexShader = shaderCompile(GL_VERTEX_SHADER, vertexSource);
    if (!vertexShader) return 0;
    
    unsigned int fragmentShader = shaderCompile(GL_FRAGMENT_SHADER, fragmentSource);
    if (!fragmentShader) return 0;
    
    // Создание шейдерной программы
    shader->ID = glCreateProgram();
    glAttachShader(shader->ID, vertexShader);
    glAttachShader(shader->ID, fragmentShader);
    glLinkProgram(shader->ID);
    
    // Проверка линковки
    int success;
    char infoLog[512];
    glGetProgramiv(shader->ID, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shader->ID, 512, NULL, infoLog);
        fprintf(stderr, "Shader program linking failed: %s\n", infoLog);
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        glDeleteProgram(shader->ID);
        return 0;
    }
    
    // Удаление шейдеров (они уже прилинкованы)
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    
    return 1;
}

/**
 * @brief Создает стандартную шейдерную программу (вершинный + фрагментный)
 * 
 * @param shader Указатель на структуру Shader
 * @return 1 при успехе, 0 при ошибке
 */
static inline int shaderCreateStandard(Shader* shader) {
    const char* vertexSource = 
        "#version 330 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "void main() {\n"
        "    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
        "}\0";
    
    const char* fragmentSource = 
        "#version 330 core\n"
        "out vec4 FragColor;\n"
        "void main() {\n"
        "    FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
        "}\0";
    
    return shaderCreate(shader, vertexSource, fragmentSource);
}

/**
 * @brief Создает цветную шейдерную программу с возможностью задания цвета
 * 
 * @param shader Указатель на структуру Shader
 * @return 1 при успехе, 0 при ошибке
 */
static inline int shaderCreateColored(Shader* shader) {
    const char* vertexSource = 
        "#version 330 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "uniform vec3 uColor;\n"
        "out vec3 vertexColor;\n"
        "void main() {\n"
        "    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
        "    vertexColor = uColor;\n"
        "}\0";
    
    const char* fragmentSource = 
        "#version 330 core\n"
        "in vec3 vertexColor;\n"
        "out vec4 FragColor;\n"
        "void main() {\n"
        "    FragColor = vec4(vertexColor, 1.0);\n"
        "}\0";
    
    return shaderCreate(shader, vertexSource, fragmentSource);
}

/**
 * @brief Создает шейдерную программу с текстурой
 * 
 * @param shader Указатель на структуру Shader
 * @return 1 при успехе, 0 при ошибке
 */
static inline int shaderCreateTextured(Shader* shader) {
    const char* vertexSource = 
        "#version 330 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "layout (location = 1) in vec2 aTexCoord;\n"
        "out vec2 TexCoord;\n"
        "void main() {\n"
        "    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
        "    TexCoord = aTexCoord;\n"
        "}\0";
    
    const char* fragmentSource = 
        "#version 330 core\n"
        "in vec2 TexCoord;\n"
        "out vec4 FragColor;\n"
        "uniform sampler2D texture1;\n"
        "void main() {\n"
        "    FragColor = texture(texture1, TexCoord);\n"
        "}\0";
    
    return shaderCreate(shader, vertexSource, fragmentSource);
}

/**
 * @brief Использует шейдерную программу
 * 
 * @param shader Указатель на структуру Shader
 */
static inline void shaderUse(Shader* shader) {
    if (shader->ID) {
        glUseProgram(shader->ID);
    }
}

/**
 * @brief Устанавливает uniform значение типа float
 * 
 * @param shader Указатель на структуру Shader
 * @param name Имя uniform переменной
 * @param value Значение
 */
static inline void shaderSetFloat(Shader* shader, const char* name, float value) {
    int location = glGetUniformLocation(shader->ID, name);
    if (location != -1) {
        glUniform1f(location, value);
    }
}

/**
 * @brief Устанавливает uniform значение типа int
 * 
 * @param shader Указатель на структуру Shader
 * @param name Имя uniform переменной
 * @param value Значение
 */
static inline void shaderSetInt(Shader* shader, const char* name, int value) {
    int location = glGetUniformLocation(shader->ID, name);
    if (location != -1) {
        glUniform1i(location, value);
    }
}

/**
 * @brief Устанавливает uniform значение типа vec3
 * 
 * @param shader Указатель на структуру Shader
 * @param name Имя uniform переменной
 * @param x Значение X
 * @param y Значение Y
 * @param z Значение Z
 */
static inline void shaderSetVec3(Shader* shader, const char* name, float x, float y, float z) {
    int location = glGetUniformLocation(shader->ID, name);
    if (location != -1) {
        glUniform3f(location, x, y, z);
    }
}

/**
 * @brief Устанавливает uniform значение типа vec4
 * 
 * @param shader Указатель на структуру Shader
 * @param name Имя uniform переменной
 * @param x Значение X
 * @param y Значение Y
 * @param z Значение Z
 * @param w Значение W
 */
static inline void shaderSetVec4(Shader* shader, const char* name, float x, float y, float z, float w) {
    int location = glGetUniformLocation(shader->ID, name);
    if (location != -1) {
        glUniform4f(location, x, y, z, w);
    }
}

/**
 * @brief Устанавливает uniform значение типа mat4
 * 
 * @param shader Указатель на структуру Shader
 * @param name Имя uniform переменной
 * @param matrix Указатель на матрицу (16 значений float)
 */
static inline void shaderSetMat4(Shader* shader, const char* name, float* matrix) {
    int location = glGetUniformLocation(shader->ID, name);
    if (location != -1) {
        glUniformMatrix4fv(location, 1, GL_FALSE, matrix);
    }
}

/**
 * @brief Освобождает ресурсы шейдера
 * 
 * @param shader Указатель на структуру Shader
 */
static inline void shaderFree(Shader* shader) {
    if (shader->ID) {
        glDeleteProgram(shader->ID);
        shader->ID = 0;
    }
    shader->vertexSource = NULL;
    shader->fragmentSource = NULL;
}

/**
 * @brief Перекомпилирует шейдер с новыми исходниками
 * 
 * @param shader Указатель на структуру Shader
 * @param vertexSource Новый исходный код вершинного шейдера
 * @param fragmentSource Новый исходный код фрагментного шейдера
 * @return 1 при успехе, 0 при ошибке
 */
static inline int shaderRecompile(Shader* shader, 
                                  const char* vertexSource, 
                                  const char* fragmentSource) {
    shaderFree(shader);
    return shaderCreate(shader, vertexSource, fragmentSource);
}

#endif // SHADER_H