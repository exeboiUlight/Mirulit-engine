#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ==================== СТРУКТУРА ШЕЙДЕРА ====================
typedef struct Shader {
    unsigned int ID;  // ID шейдерной программы
    char* name;       // Имя шейдера (для отладки)
} Shader;

// ==================== ВСПОМОГАТЕЛЬНЫЕ ФУНКЦИИ ====================

// Загрузка текста из файла
static char* shader_load_file(const char* filepath) {
    FILE* file = fopen(filepath, "rb");
    if (!file) {
        printf("ERROR::SHADER::FILE_NOT_FOUND: %s\n", filepath);
        return NULL;
    }
    
    // Определяем размер файла
    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    // Выделяем память и читаем файл
    char* buffer = (char*)malloc(length + 1);
    if (!buffer) {
        fclose(file);
        return NULL;
    }
    
    fread(buffer, 1, length, file);
    buffer[length] = '\0';
    
    fclose(file);
    return buffer;
}

// Проверка ошибок компиляции шейдера
static int shader_check_compile_status(unsigned int shader, const char* type) {
    int success;
    char infoLog[1024];
    
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, sizeof(infoLog), NULL, infoLog);
        printf("ERROR::SHADER::%s::COMPILATION_FAILED\n", type);
        printf("%s\n", infoLog);
        return 0;
    }
    return 1;
}

// Проверка ошибок линковки шейдерной программы
static int shader_check_link_status(unsigned int program, const char* name) {
    int success;
    char infoLog[1024];
    
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program, sizeof(infoLog), NULL, infoLog);
        printf("ERROR::SHADER::PROGRAM::LINKING_FAILED\n");
        printf("Shader: %s\n", name);
        printf("%s\n", infoLog);
        return 0;
    }
    return 1;
}

// ==================== ПУБЛИЧНЫЕ ФУНКЦИИ ====================

// Создание шейдера из исходного кода
Shader* shader_create(const char* vertexSource, const char* fragmentSource, const char* name) {
    // Создаем объект шейдера
    Shader* shader = (Shader*)malloc(sizeof(Shader));
    if (!shader) return NULL;
    
    shader->name = (char*)malloc(strlen(name) + 1);
    if (shader->name) {
        strcpy(shader->name, name);
    }
    
    // Компилируем вершинный шейдер
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexSource, NULL);
    glCompileShader(vertexShader);
    
    if (!shader_check_compile_status(vertexShader, "VERTEX")) {
        glDeleteShader(vertexShader);
        free(shader->name);
        free(shader);
        return NULL;
    }
    
    // Компилируем фрагментный шейдер
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
    glCompileShader(fragmentShader);
    
    if (!shader_check_compile_status(fragmentShader, "FRAGMENT")) {
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        free(shader->name);
        free(shader);
        return NULL;
    }
    
    // Создаем шейдерную программу и линкуем шейдеры
    shader->ID = glCreateProgram();
    glAttachShader(shader->ID, vertexShader);
    glAttachShader(shader->ID, fragmentShader);
    glLinkProgram(shader->ID);
    
    if (!shader_check_link_status(shader->ID, name)) {
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        free(shader->name);
        free(shader);
        return NULL;
    }
    
    // Удаляем шейдеры, они больше не нужны
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    
    printf("SUCCESS: Shader '%s' created (ID: %u)\n", name, shader->ID);
    return shader;
}

// Создание шейдера из файлов
Shader* shader_create_from_files(const char* vertexPath, const char* fragmentPath, const char* name) {
    // Загружаем исходный код из файлов
    char* vertexSource = shader_load_file(vertexPath);
    if (!vertexSource) {
        printf("ERROR::SHADER::VERTEX_FILE_NOT_LOADED: %s\n", vertexPath);
        return NULL;
    }
    
    char* fragmentSource = shader_load_file(fragmentPath);
    if (!fragmentSource) {
        printf("ERROR::SHADER::FRAGMENT_FILE_NOT_LOADED: %s\n", fragmentPath);
        free(vertexSource);
        return NULL;
    }
    
    // Создаем шейдер
    Shader* shader = shader_create(vertexSource, fragmentSource, name);
    
    // Освобождаем память
    free(vertexSource);
    free(fragmentSource);
    
    return shader;
}

// Активация шейдера
void shader_use(const Shader* shader) {
    if (shader && shader->ID) {
        glUseProgram(shader->ID);
    }
}

// Деактивация шейдера
void shader_unuse(void) {
    glUseProgram(0);
}

// Установка uniform-переменных
void shader_set_bool(const Shader* shader, const char* name, int value) {
    if (shader && shader->ID) {
        glUniform1i(glGetUniformLocation(shader->ID, name), value);
    }
}

void shader_set_int(const Shader* shader, const char* name, int value) {
    if (shader && shader->ID) {
        glUniform1i(glGetUniformLocation(shader->ID, name), value);
    }
}

void shader_set_float(const Shader* shader, const char* name, float value) {
    if (shader && shader->ID) {
        glUniform1f(glGetUniformLocation(shader->ID, name), value);
    }
}

void shader_set_vec2(const Shader* shader, const char* name, float x, float y) {
    if (shader && shader->ID) {
        glUniform2f(glGetUniformLocation(shader->ID, name), x, y);
    }
}

void shader_set_vec3(const Shader* shader, const char* name, float x, float y, float z) {
    if (shader && shader->ID) {
        glUniform3f(glGetUniformLocation(shader->ID, name), x, y, z);
    }
}

void shader_set_vec4(const Shader* shader, const char* name, float x, float y, float z, float w) {
    if (shader && shader->ID) {
        glUniform4f(glGetUniformLocation(shader->ID, name), x, y, z, w);
    }
}

void shader_set_mat4(const Shader* shader, const char* name, const float* matrix) {
    if (shader && shader->ID) {
        glUniformMatrix4fv(glGetUniformLocation(shader->ID, name), 1, GL_FALSE, matrix);
    }
}

// Установка цвета (RGB структура)
void shader_set_color(const Shader* shader, const char* name, float r, float g, float b) {
    shader_set_vec3(shader, name, r, g, b);
}

// Получение ID шейдерной программы
unsigned int shader_get_id(const Shader* shader) {
    return shader ? shader->ID : 0;
}

// Получение имени шейдера
const char* shader_get_name(const Shader* shader) {
    return shader ? shader->name : "NULL";
}

// Уничтожение шейдера
void shader_destroy(Shader* shader) {
    if (shader) {
        if (shader->ID) {
            glDeleteProgram(shader->ID);
        }
        free(shader->name);
        free(shader);
    }
}

// ==================== ПРЕДОПРЕДЕЛЕННЫЕ ШЕЙДЕРЫ ====================

// Простой цветной шейдер (для разноцветного треугольника)
static const char* SIMPLE_VERTEX_SHADER = 
    "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "layout (location = 1) in vec3 aColor;\n"
    "out vec3 ourColor;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos, 1.0);\n"
    "   ourColor = aColor;\n"
    "}\0";

static const char* SIMPLE_FRAGMENT_SHADER = 
    "#version 330 core\n"
    "out vec4 FragColor;\n"
    "in vec3 ourColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(ourColor, 1.0);\n"
    "}\0";

// Шейдер для мешей с текстурами
static const char* MESH_VERTEX_SHADER = 
    "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "layout (location = 1) in vec3 aNormal;\n"
    "layout (location = 2) in vec2 aTexCoords;\n"
    "layout (location = 3) in vec3 aTangent;\n"
    "layout (location = 4) in vec3 aBitangent;\n"
    "\n"
    "out vec3 FragPos;\n"
    "out vec3 Normal;\n"
    "out vec2 TexCoords;\n"
    "\n"
    "uniform mat4 model;\n"
    "uniform mat4 view;\n"
    "uniform mat4 projection;\n"
    "\n"
    "void main()\n"
    "{\n"
    "   FragPos = vec3(model * vec4(aPos, 1.0));\n"
    "   Normal = mat3(transpose(inverse(model))) * aNormal;\n"
    "   TexCoords = aTexCoords;\n"
    "   \n"
    "   gl_Position = projection * view * model * vec4(aPos, 1.0);\n"
    "}\0";

static const char* MESH_FRAGMENT_SHADER = 
    "#version 330 core\n"
    "out vec4 FragColor;\n"
    "\n"
    "in vec3 FragPos;\n"
    "in vec3 Normal;\n"
    "in vec2 TexCoords;\n"
    "\n"
    "struct Material {\n"
    "    vec3 ambient;\n"
    "    vec3 diffuse;\n"
    "    vec3 specular;\n"
    "    float shininess;\n"
    "};\n"
    "\n"
    "uniform Material material;\n"
    "uniform sampler2D texture_diffuse1;\n"
    "uniform sampler2D texture_specular1;\n"
    "\n"
    "void main()\n"
    "{\n"
    "   // Простой вывод текстуры\n"
    "   vec4 texColor = texture(texture_diffuse1, TexCoords);\n"
    "   FragColor = texColor;\n"
    "}\0";

// Создание простого цветного шейдера
Shader* shader_create_simple_color(void) {
    return shader_create(SIMPLE_VERTEX_SHADER, SIMPLE_FRAGMENT_SHADER, "SimpleColorShader");
}

// Создание шейдера для мешей
Shader* shader_create_mesh(void) {
    return shader_create(MESH_VERTEX_SHADER, MESH_FRAGMENT_SHADER, "MeshShader");
}

#endif // SHADER_H