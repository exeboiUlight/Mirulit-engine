#pragma once

#include "../math/Vectors.h"
#include "shader.h"
#include <vector>
#include <iostream>

namespace MirulitComponenets {
    using MirulitMath::Vector2f;
    
    class Mesh {
        private:
            GLuint VAO, VBO;
            GLsizei vertexCount;
            bool isInitialized;
            
        public:

            Mesh() {}

            Mesh(float* vertices, int numVertices) {
                isInitialized = false;
                
                if (!vertices || numVertices <= 0) {
                    std::cerr << "Mesh error: Invalid vertex data" << std::endl;
                    return;
                }
                
                vertexCount = numVertices;
                
                glGenVertexArrays(1, &VAO);
                glGenBuffers(1, &VBO);
                
                glBindVertexArray(VAO);
                
                glBindBuffer(GL_ARRAY_BUFFER, VBO);
                glBufferData(GL_ARRAY_BUFFER, numVertices * 2 * sizeof(float), vertices, GL_STATIC_DRAW);
                
                glEnableVertexAttribArray(0);
                glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
                
                glBindBuffer(GL_ARRAY_BUFFER, 0);
                glBindVertexArray(0);
                
                isInitialized = true;
            }
            
            Mesh(const std::vector<float>& vertices) {
                isInitialized = false;
                
                if (vertices.empty() || vertices.size() % 2 != 0) {
                    std::cerr << "Mesh error: Invalid vertex vector (size must be multiple of 2)" << std::endl;
                    return;
                }
                
                vertexCount = vertices.size() / 2;
                
                glGenVertexArrays(1, &VAO);
                glGenBuffers(1, &VBO);
                
                glBindVertexArray(VAO);
                
                glBindBuffer(GL_ARRAY_BUFFER, VBO);
                glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
                
                glEnableVertexAttribArray(0);
                glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
                
                glBindBuffer(GL_ARRAY_BUFFER, 0);
                glBindVertexArray(0);
                
                isInitialized = true;
            }
            
            Mesh(const std::vector<Vector2f>& vertices) {
                isInitialized = false;
                
                if (vertices.empty()) {
                    std::cerr << "Mesh error: Empty vertex vector" << std::endl;
                    return;
                }
                
                vertexCount = vertices.size();
                
                std::vector<float> vertexData;
                vertexData.reserve(vertices.size() * 2);
                for (const auto& v : vertices) {
                    vertexData.push_back(v.x);
                    vertexData.push_back(v.y);
                }
                
                glGenVertexArrays(1, &VAO);
                glGenBuffers(1, &VBO);
                
                glBindVertexArray(VAO);
                
                glBindBuffer(GL_ARRAY_BUFFER, VBO);
                glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(float), vertexData.data(), GL_STATIC_DRAW);
                
                glEnableVertexAttribArray(0);
                glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
                
                glBindBuffer(GL_ARRAY_BUFFER, 0);
                glBindVertexArray(0);
                
                isInitialized = true;
            }
            
            ~Mesh() {
                if (isInitialized) {
                    glDeleteVertexArrays(1, &VAO);
                    glDeleteBuffers(1, &VBO);
                }
            }
            
            void draw() {
                if (!isInitialized) {
                    std::cerr << "Mesh not initialized!" << std::endl;
                    return;
                }
                
                glBindVertexArray(VAO);
                glDrawArrays(GL_TRIANGLES, 0, vertexCount);
                glBindVertexArray(0);
            }
            
            void draw(GLenum mode) {
                if (!isInitialized) {
                    std::cerr << "Mesh not initialized!" << std::endl;
                    return;
                }
                
                glBindVertexArray(VAO);
                glDrawArrays(mode, 0, vertexCount);
                glBindVertexArray(0);
            }
            
            void updateData(const std::vector<float>& newVertices) {
                if (!isInitialized || newVertices.empty()) return;
                
                vertexCount = newVertices.size() / 2;
                
                glBindBuffer(GL_ARRAY_BUFFER, VBO);
                glBufferData(GL_ARRAY_BUFFER, newVertices.size() * sizeof(float), newVertices.data(), GL_STATIC_DRAW);
                glBindBuffer(GL_ARRAY_BUFFER, 0);
            }
            
            void updateData(const std::vector<Vector2f>& newVertices) {
                if (!isInitialized || newVertices.empty()) return;
                
                vertexCount = newVertices.size();
                
                std::vector<float> vertexData;
                vertexData.reserve(newVertices.size() * 2);
                for (const auto& v : newVertices) {
                    vertexData.push_back(v.x);
                    vertexData.push_back(v.y);
                }
                
                glBindBuffer(GL_ARRAY_BUFFER, VBO);
                glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(float), vertexData.data(), GL_STATIC_DRAW);
                glBindBuffer(GL_ARRAY_BUFFER, 0);
            }
            
            GLuint getVAO() const { return VAO; }
            GLuint getVBO() const { return VBO; }
            GLsizei getVertexCount() const { return vertexCount; }
            bool isValid() const { return isInitialized; }
    };
    
    namespace MeshFactory {
        using MirulitMath::Vector2f;
        
        inline Mesh createTriangle() {
            std::vector<float> vertices = {
                -0.5f, -0.5f,
                 0.5f, -0.5f,
                 0.0f,  0.5f
            };
            return Mesh(vertices);
        }
        
        inline Mesh createQuad() {
            std::vector<float> vertices = {
                -0.5f, -0.5f,
                 0.5f, -0.5f,
                 0.5f,  0.5f,
                 0.5f,  0.5f,
                -0.5f,  0.5f,
                -0.5f, -0.5f
            };
            return Mesh(vertices);
        }
        
        inline Mesh createRectangle(float width, float height) {
            float w = width / 2.0f;
            float h = height / 2.0f;
            
            std::vector<float> vertices = {
                -w, -h,  w, -h,  w,  h,
                 w,  h, -w,  h, -w, -h
            };
            return Mesh(vertices);
        }
        
        inline Mesh createCircle(float radius, int segments = 32) {
            std::vector<float> vertices;
            
            for (int i = 0; i < segments; i++) {
                float angle1 = 2.0f * 3.14159f * i / segments;
                float angle2 = 2.0f * 3.14159f * (i + 1) / segments;
                
                vertices.push_back(0.0f);
                vertices.push_back(0.0f);
                
                vertices.push_back(radius * cos(angle1));
                vertices.push_back(radius * sin(angle1));
                
                vertices.push_back(radius * cos(angle2));
                vertices.push_back(radius * sin(angle2));
            }
            
            return Mesh(vertices);
        }
        
        inline Mesh createLine(const Vector2f& start, const Vector2f& end) {
            std::vector<float> vertices = {
                start.x, start.y,
                end.x, end.y
            };
            return Mesh(vertices);
        }
        
        inline Mesh createGrid(int cellsX, int cellsY, float size = 1.0f) {
            std::vector<float> vertices;
            float step = size / cellsX;
            
            for (int i = 0; i <= cellsX; i++) {
                float x = -size/2 + i * step;
                vertices.push_back(x);
                vertices.push_back(-size/2);
                vertices.push_back(x);
                vertices.push_back(size/2);
            }
            
            for (int i = 0; i <= cellsY; i++) {
                float y = -size/2 + i * step;
                vertices.push_back(-size/2);
                vertices.push_back(y);
                vertices.push_back(size/2);
                vertices.push_back(y);
            }
            
            return Mesh(vertices);
        }
        
        inline Mesh createStar(float radius, int points = 5) {
            std::vector<float> vertices;
            float angle = 3.14159f / points;
            
            for (int i = 0; i < points * 2; i++) {
                float r = (i % 2 == 0) ? radius : radius * 0.5f;
                float a = i * angle;
                vertices.push_back(r * cos(a));
                vertices.push_back(r * sin(a));
            }
            
            std::vector<float> triangles;
            for (int i = 0; i < points * 2; i += 2) {
                int next = (i + 2) % (points * 2);
                int center = (i + 1) % (points * 2);
                
                triangles.push_back(vertices[i * 2]);
                triangles.push_back(vertices[i * 2 + 1]);
                triangles.push_back(vertices[center * 2]);
                triangles.push_back(vertices[center * 2 + 1]);
                triangles.push_back(vertices[next * 2]);
                triangles.push_back(vertices[next * 2 + 1]);
            }
            
            return Mesh(triangles);
        }
    }
}