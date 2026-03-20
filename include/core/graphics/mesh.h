#pragma once

#include <glad/glad.h>
#include <vector>
#include <string>

namespace Engine {
    
    struct Vector3 {
        float x, y, z;
        
        Vector3() : x(0), y(0), z(0) {}
        Vector3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}
    };
    
    struct Vector2 {
        float x, y;
        
        Vector2() : x(0), y(0) {}
        Vector2(float _x, float _y) : x(_x), y(_y) {}
    };
    
    struct Vertex {
        Vector3 Position;
        Vector3 Normal;
        Vector2 TexCoords;
        
        // Для использования в unordered_map
        bool operator==(const Vertex& other) const {
            return Position.x == other.Position.x &&
                   Position.y == other.Position.y &&
                   Position.z == other.Position.z &&
                   Normal.x == other.Normal.x &&
                   Normal.y == other.Normal.y &&
                   Normal.z == other.Normal.z &&
                   TexCoords.x == other.TexCoords.x &&
                   TexCoords.y == other.TexCoords.y;
        }
    };
    
    class Mesh {
    public:
        Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices) 
            : m_VertexCount(static_cast<unsigned int>(vertices.size())), 
              m_IndexCount(static_cast<unsigned int>(indices.size()))
        {
            glGenVertexArrays(1, &VAO);
            glGenBuffers(1, &VBO);
            glGenBuffers(1, &EBO);
            
            glBindVertexArray(VAO);
            
            // Vertex buffer
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
            
            // Index buffer
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
            
            // Position attribute
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Position));
            glEnableVertexAttribArray(0);
            
            // Normal attribute
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
            glEnableVertexAttribArray(1);
            
            // Texture coordinates attribute
            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
            glEnableVertexAttribArray(2);
            
            glBindVertexArray(0);
        }
        
        ~Mesh() {
            if (VAO) {
                glDeleteVertexArrays(1, &VAO);
                glDeleteBuffers(1, &VBO);
                glDeleteBuffers(1, &EBO);
            }
        }
        
        // Move constructor
        Mesh(Mesh&& other) noexcept
            : VAO(other.VAO), VBO(other.VBO), EBO(other.EBO),
              m_VertexCount(other.m_VertexCount), m_IndexCount(other.m_IndexCount)
        {
            other.VAO = 0;
            other.VBO = 0;
            other.EBO = 0;
        }
        
        // Move assignment
        Mesh& operator=(Mesh&& other) noexcept {
            if (this != &other) {
                if (VAO) {
                    glDeleteVertexArrays(1, &VAO);
                    glDeleteBuffers(1, &VBO);
                    glDeleteBuffers(1, &EBO);
                }
                
                VAO = other.VAO;
                VBO = other.VBO;
                EBO = other.EBO;
                m_VertexCount = other.m_VertexCount;
                m_IndexCount = other.m_IndexCount;
                
                other.VAO = 0;
                other.VBO = 0;
                other.EBO = 0;
            }
            return *this;
        }
        
        void Draw() const {
            glBindVertexArray(VAO);
            glDrawElements(GL_TRIANGLES, m_IndexCount, GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);
        }
        
        // Getters
        unsigned int GetVertexCount() const { return m_VertexCount; }
        unsigned int GetIndexCount() const { return m_IndexCount; }
        
    private:
        unsigned int VAO = 0, VBO = 0, EBO = 0;
        unsigned int m_VertexCount = 0;
        unsigned int m_IndexCount = 0;
    };
    
}