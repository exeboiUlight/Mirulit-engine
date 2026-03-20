#pragma once

#include <glad/glad.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <string>
#include <unordered_map>
#include <vector>
#include <iostream>
#include "../graphics/mesh.h"
#include "../graphics/shader.h"

namespace Engine {

struct Character {
    unsigned int TextureID;   // ID текстуры глифа
    int SizeX, SizeY;         // Размер глифа
    int BearingX, BearingY;   // Смещение от基线 до левого/верхнего края
    unsigned int Advance;     // Смещение до следующего глифа
};

class TextRenderer {
public:
    TextRenderer() : m_VAO(0), m_VBO(0), m_FontSize(48), m_Width(0), m_Height(0) {}
    
    ~TextRenderer() {
        if (m_VAO) {
            glDeleteVertexArrays(1, &m_VAO);
            glDeleteBuffers(1, &m_VBO);
        }
        for (auto& pair : m_Characters) {
            if (pair.second.TextureID) {
                glDeleteTextures(1, &pair.second.TextureID);
            }
        }
        if (m_Face) {
            FT_Done_Face(m_Face);
        }
        if (m_FTLibrary) {
            FT_Done_FreeType(m_FTLibrary);
        }
    }
    
    bool Init(const std::string& fontPath, unsigned int fontSize = 48) {
        m_FontSize = fontSize;
        
        if (FT_Init_FreeType(&m_FTLibrary)) {
            std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
            return false;
        }
        
        if (FT_New_Face(m_FTLibrary, fontPath.c_str(), 0, &m_Face)) {
            std::cout << "ERROR::FREETYPE: Failed to load font: " << fontPath << std::endl;
            return false;
        }
        
        FT_Set_Pixel_Sizes(m_Face, 0, fontSize);
        
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        
        for (unsigned char c = 0; c < 128; c++) {
            if (FT_Load_Char(m_Face, c, FT_LOAD_RENDER)) {
                std::cout << "ERROR::FREETYPE: Failed to load Glyph: " << c << std::endl;
                continue;
            }
            
            unsigned int texture;
            glGenTextures(1, &texture);
            glBindTexture(GL_TEXTURE_2D, texture);
            glTexImage2D(
                GL_TEXTURE_2D,
                0,
                GL_RED,
                m_Face->glyph->bitmap.width,
                m_Face->glyph->bitmap.rows,
                0,
                GL_RED,
                GL_UNSIGNED_BYTE,
                m_Face->glyph->bitmap.buffer
            );
            
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            
            Character character = {
                texture,
                static_cast<int>(m_Face->glyph->bitmap.width),
                static_cast<int>(m_Face->glyph->bitmap.rows),
                m_Face->glyph->bitmap_left,
                m_Face->glyph->bitmap_top,
                static_cast<unsigned int>(m_Face->glyph->advance.x)
            };
            m_Characters.insert(std::pair<char, Character>(c, character));
        }
        
        glBindTexture(GL_TEXTURE_2D, 0);
        
        SetupRendering();
        
        return true;
    }
    
    void SetScreenSize(int width, int height) {
        m_Width = width;
        m_Height = height;
    }
    
    void SetupRendering() {
        glGenVertexArrays(1, &m_VAO);
        glGenBuffers(1, &m_VBO);
        glBindVertexArray(m_VAO);
        glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, nullptr, GL_DYNAMIC_DRAW);
        
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
        
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
    
    void RenderText(Shader& shader, const std::string& text, 
                    float x, float y, float scale, 
                    float r = 1.0f, float g = 1.0f, float b = 1.0f) {
        shader.Use();
        shader.SetVec3("textColor", r, g, b);
        shader.SetInt("textTexture", 0);
        
        // Устанавливаем ортографическую проекцию
        float left = 0.0f;
        float right = static_cast<float>(m_Width);
        float bottom = 0.0f;
        float top = static_cast<float>(m_Height);
        
        float projection[16] = {
            2.0f / (right - left), 0, 0, 0,
            0, 2.0f / (top - bottom), 0, 0,
            0, 0, -1, 0,
            -(right + left) / (right - left), -(top + bottom) / (top - bottom), 0, 1
        };
        
        shader.SetMat4("projection", projection);
        
        glActiveTexture(GL_TEXTURE0);
        glBindVertexArray(m_VAO);
        
        float startX = x;
        float startY = y;
        
        for (std::string::const_iterator c = text.begin(); c != text.end(); c++) {
            Character ch = m_Characters[*c];
            
            float xpos = startX + ch.BearingX * scale;
            float ypos = startY - (ch.SizeY - ch.BearingY) * scale;
            
            float w = ch.SizeX * scale;
            float h = ch.SizeY * scale;
            
            float vertices[6][4] = {
                { xpos,     ypos + h,   0.0f, 0.0f },
                { xpos,     ypos,       0.0f, 1.0f },
                { xpos + w, ypos,       1.0f, 1.0f },
                
                { xpos,     ypos + h,   0.0f, 0.0f },
                { xpos + w, ypos,       1.0f, 1.0f },
                { xpos + w, ypos + h,   1.0f, 0.0f }
            };
            
            glBindTexture(GL_TEXTURE_2D, ch.TextureID);
            glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            
            glDrawArrays(GL_TRIANGLES, 0, 6);
            
            startX += (ch.Advance >> 6) * scale;
        }
        
        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    
    void SetFontSize(unsigned int size) {
        if (size != m_FontSize && m_Face) {
            m_FontSize = size;
            FT_Set_Pixel_Sizes(m_Face, 0, size);
            RegenerateGlyphs();
        }
    }
    
private:
    void RegenerateGlyphs() {
        for (auto& pair : m_Characters) {
            if (pair.second.TextureID) {
                glDeleteTextures(1, &pair.second.TextureID);
            }
        }
        m_Characters.clear();
        
        for (unsigned char c = 0; c < 128; c++) {
            if (FT_Load_Char(m_Face, c, FT_LOAD_RENDER)) {
                continue;
            }
            
            unsigned int texture;
            glGenTextures(1, &texture);
            glBindTexture(GL_TEXTURE_2D, texture);
            glTexImage2D(
                GL_TEXTURE_2D, 0, GL_RED,
                m_Face->glyph->bitmap.width,
                m_Face->glyph->bitmap.rows,
                0, GL_RED, GL_UNSIGNED_BYTE,
                m_Face->glyph->bitmap.buffer
            );
            
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            
            Character character = {
                texture,
                static_cast<int>(m_Face->glyph->bitmap.width),
                static_cast<int>(m_Face->glyph->bitmap.rows),
                m_Face->glyph->bitmap_left,
                m_Face->glyph->bitmap_top,
                static_cast<unsigned int>(m_Face->glyph->advance.x)
            };
            m_Characters.insert(std::pair<char, Character>(c, character));
        }
    }
    
    FT_Library m_FTLibrary;
    FT_Face m_Face;
    unsigned int m_VAO, m_VBO;
    std::unordered_map<char, Character> m_Characters;
    unsigned int m_FontSize;
    int m_Width, m_Height;
};

// Вершинный шейдер для текста
const char* textVertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec4 vertex;
out vec2 TexCoords;
uniform mat4 projection;
void main() {
    gl_Position = projection * vec4(vertex.xy, 0.0, 1.0);
    TexCoords = vertex.zw;
}
)";

const char* textFragmentShaderSource = R"(
#version 330 core
in vec2 TexCoords;
out vec4 color;
uniform sampler2D textTexture;
uniform vec3 textColor;
void main() {
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(textTexture, TexCoords).r);
    color = vec4(textColor, 1.0) * sampled;
}
)";

}