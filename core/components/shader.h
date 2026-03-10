#pragma once

#include <glad/glad.h>
#include <../math/Vectors.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

namespace MirulitComponencts {
    class Shader {
        private:
            unsigned int m_ID;
            const char* codeShader;
            
            void checkCompileErrors(unsigned int shader, const std::string& type) {
                int success;
                char infoLog[1024];
                
                if (type != "PROGRAM") {
                    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
                    if (!success) {
                        glGetShaderInfoLog(shader, 1024, NULL, infoLog);
                        std::cerr << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << std::endl;
                    }
                } else {
                    glGetProgramiv(shader, GL_LINK_STATUS, &success);
                    if (!success) {
                        glGetProgramInfoLog(shader, 1024, NULL, infoLog);
                        std::cerr << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << std::endl;
                    }
                }
            }
            
        public:
            Shader(const char* vertexCode, const char* fragmentCode) {
                unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
                glShaderSource(vertexShader, 1, &vertexCode, NULL);
                glCompileShader(vertexShader);
                checkCompileErrors(vertexShader, "VERTEX");
                
                unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
                glShaderSource(fragmentShader, 1, &fragmentCode, NULL);
                glCompileShader(fragmentShader);
                checkCompileErrors(fragmentShader, "FRAGMENT");
                
                // Создаем шейдерную программу
                m_ID = glCreateProgram();
                glAttachShader(m_ID, vertexShader);
                glAttachShader(m_ID, fragmentShader);
                glLinkProgram(m_ID);
                checkCompileErrors(m_ID, "PROGRAM");
                
                glDeleteShader(vertexShader);
                glDeleteShader(fragmentShader);
            }
            
            Shader(const char* vertexPath, const char* fragmentPath) {
                std::string vertexCode;
                std::string fragmentCode;
                std::ifstream vShaderFile;
                std::ifstream fShaderFile;
                
                vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
                fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
                
                try {
                    vShaderFile.open(vertexPath);
                    fShaderFile.open(fragmentPath);
                    std::stringstream vShaderStream, fShaderStream;
                    
                    vShaderStream << vShaderFile.rdbuf();
                    fShaderStream << fShaderFile.rdbuf();
                    
                    vShaderFile.close();
                    fShaderFile.close();
                    
                    vertexCode = vShaderStream.str();
                    fragmentCode = fShaderStream.str();
                } catch (std::ifstream::failure& e) {
                    std::cerr << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << e.what() << std::endl;
                }
                
                const char* vShaderCode = vertexCode.c_str();
                const char* fShaderCode = fragmentCode.c_str();
                
                unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
                glShaderSource(vertexShader, 1, &vShaderCode, NULL);
                glCompileShader(vertexShader);
                checkCompileErrors(vertexShader, "VERTEX");
                
                unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
                glShaderSource(fragmentShader, 1, &fShaderCode, NULL);
                glCompileShader(fragmentShader);
                checkCompileErrors(fragmentShader, "FRAGMENT");
                
                m_ID = glCreateProgram();
                glAttachShader(m_ID, vertexShader);
                glAttachShader(m_ID, fragmentShader);
                glLinkProgram(m_ID);
                checkCompileErrors(m_ID, "PROGRAM");
                
                glDeleteShader(vertexShader);
                glDeleteShader(fragmentShader);
            }
            
            void use() const {
                glUseProgram(m_ID);
            }
            
            unsigned int getID() const {
                return m_ID;
            }
            
            void setBool(const std::string& name, bool value) const {
                glUniform1i(glGetUniformLocation(m_ID, name.c_str()), (int)value);
            }
            
            void setInt(const std::string& name, int value) const {
                glUniform1i(glGetUniformLocation(m_ID, name.c_str()), value);
            }
            
            void setFloat(const std::string& name, float value) const {
                glUniform1f(glGetUniformLocation(m_ID, name.c_str()), value);
            }
            
            void setVec2(const std::string& name, const MirulitMath::Vector2f& value) const {
                glUniform2f(glGetUniformLocation(m_ID, name.c_str()), value.x, value.y);
            }
            
            void setVec2(const std::string& name, float x, float y) const {
                glUniform2f(glGetUniformLocation(m_ID, name.c_str()), x, y);
            }
            
            void setVec3(const std::string& name, const MirulitMath::Vector3f& value) const {
                glUniform3f(glGetUniformLocation(m_ID, name.c_str()), value.x, value.y, value.z);
            }
            
            void setVec3(const std::string& name, float x, float y, float z) const {
                glUniform3f(glGetUniformLocation(m_ID, name.c_str()), x, y, z);
            }
            
            void setMat4(const std::string& name, const float* value) const {
                glUniformMatrix4fv(glGetUniformLocation(m_ID, name.c_str()), 1, GL_FALSE, value);
            }
            
            ~Shader() {
                glDeleteProgram(m_ID);
            }
    };
}