#pragma once

#include <glad/glad.h>
#include <string>
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <iostream>

namespace Engine {
    
    class Shader {
    public:
        Shader() : m_RendererID(0) {}
        
        Shader(const std::string& vertexPath, const std::string& fragmentPath) 
            : m_RendererID(0)
        {
            LoadFromFile(vertexPath, fragmentPath);
        }
        
        ~Shader() {
            if (m_RendererID) {
                glDeleteProgram(m_RendererID);
            }
        }
        
        // Move constructor
        Shader(Shader&& other) noexcept 
            : m_RendererID(other.m_RendererID), m_UniformLocationCache(std::move(other.m_UniformLocationCache))
        {
            other.m_RendererID = 0;
        }
        
        // Move assignment
        Shader& operator=(Shader&& other) noexcept {
            if (this != &other) {
                if (m_RendererID) {
                    glDeleteProgram(m_RendererID);
                }
                
                m_RendererID = other.m_RendererID;
                m_UniformLocationCache = std::move(other.m_UniformLocationCache);
                
                other.m_RendererID = 0;
            }
            return *this;
        }
        
        void LoadFromFile(const std::string& vertexPath, const std::string& fragmentPath) {
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
            }
            catch (std::ifstream::failure& e) {
                std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ: " << e.what() << std::endl;
                return;
            }
            
            LoadFromSource(vertexCode, fragmentCode);
        }
        
        void LoadFromSource(const std::string& vertexSource, const std::string& fragmentSource) {
            if (m_RendererID) {
                glDeleteProgram(m_RendererID);
            }
            
            unsigned int vertex = glCreateShader(GL_VERTEX_SHADER);
            const char* vSrc = vertexSource.c_str();
            glShaderSource(vertex, 1, &vSrc, nullptr);
            glCompileShader(vertex);
            
            int success;
            char infoLog[512];
            glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
            if (!success) {
                glGetShaderInfoLog(vertex, 512, nullptr, infoLog);
                std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
            }
            
            unsigned int fragment = glCreateShader(GL_FRAGMENT_SHADER);
            const char* fSrc = fragmentSource.c_str();
            glShaderSource(fragment, 1, &fSrc, nullptr);
            glCompileShader(fragment);
            
            glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
            if (!success) {
                glGetShaderInfoLog(fragment, 512, nullptr, infoLog);
                std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
            }
            
            m_RendererID = glCreateProgram();
            glAttachShader(m_RendererID, vertex);
            glAttachShader(m_RendererID, fragment);
            glLinkProgram(m_RendererID);
            
            glGetProgramiv(m_RendererID, GL_LINK_STATUS, &success);
            if (!success) {
                glGetProgramInfoLog(m_RendererID, 512, nullptr, infoLog);
                std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
            }
            
            glDeleteShader(vertex);
            glDeleteShader(fragment);
        }
        
        void Use() const {
            glUseProgram(m_RendererID);
        }
        
        void Unuse() const {
            glUseProgram(0);
        }
        
        unsigned int GetID() const { return m_RendererID; }
        
        void SetInt(const std::string& name, int value) const {
            glUniform1i(GetUniformLocation(name), value);
        }
        
        void SetFloat(const std::string& name, float value) const {
            glUniform1f(GetUniformLocation(name), value);
        }
        
        void SetVec2(const std::string& name, float x, float y) const {
            glUniform2f(GetUniformLocation(name), x, y);
        }
        
        void SetVec3(const std::string& name, float x, float y, float z) const {
            glUniform3f(GetUniformLocation(name), x, y, z);
        }
        
        void SetVec4(const std::string& name, float x, float y, float z, float w) const {
            glUniform4f(GetUniformLocation(name), x, y, z, w);
        }
        
        void SetMat4(const std::string& name, const float* value) const {
            glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, value);
        }
        
        void SetBool(const std::string& name, bool value) const {
            glUniform1i(GetUniformLocation(name), (int)value);
        }
        
    private:
        unsigned int m_RendererID;
        mutable std::unordered_map<std::string, int> m_UniformLocationCache;
        
        int GetUniformLocation(const std::string& name) const {
            auto it = m_UniformLocationCache.find(name);
            if (it != m_UniformLocationCache.end()) {
                return it->second;
            }
            
            int location = glGetUniformLocation(m_RendererID, name.c_str());
            if (location == -1) {
                std::cout << "Warning: Uniform '" << name << "' doesn't exist!" << std::endl;
            }
            
            m_UniformLocationCache[name] = location;
            return location;
        }
    };
    
}