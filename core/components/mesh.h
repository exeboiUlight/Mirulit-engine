#pragma once

#include <glad/glad.h>
#include <stb_image/stb_image.h>
#include "../math/Vectors.h"
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <algorithm>

namespace MirulitComponencts {
    
    struct Texture {
        unsigned int id;
        std::string type;
        std::string path;
    };
    
    struct Vertex {
        MirulitMath::Vector3f Position;
        MirulitMath::Vector3f Normal;
        MirulitMath::Vector2f TexCoords;
        MirulitMath::Vector3f Tangent;
        MirulitMath::Vector3f Bitangent;
    };
    
    class Mesh3D {
        private:
            std::vector<Vertex> vertices;
            std::vector<unsigned int> indices;
            std::vector<Texture> textures;
            
            GLuint VAO, VBO, EBO;
            
            void setupMesh() {
                glGenVertexArrays(1, &VAO);
                glGenBuffers(1, &VBO);
                glGenBuffers(1, &EBO);
                
                glBindVertexArray(VAO);
                
                glBindBuffer(GL_ARRAY_BUFFER, VBO);
                glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
                
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
                
                glEnableVertexAttribArray(0);
                glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
                
                glEnableVertexAttribArray(1);
                glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
                
                glEnableVertexAttribArray(2);
                glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
                
                glEnableVertexAttribArray(3);
                glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
                
                glEnableVertexAttribArray(4);
                glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));
                
                glBindVertexArray(0);
            }
            
            // Вспомогательная функция для разбиения строки
            std::vector<std::string> splitString(const std::string& s, char delimiter) const {
                std::vector<std::string> tokens;
                std::string token;
                std::istringstream tokenStream(s);
                while (std::getline(tokenStream, token, delimiter)) {
                    tokens.push_back(token);
                }
                return tokens;
            }
            
        public:
            Mesh3D(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures) 
                : vertices(vertices), indices(indices), textures(textures) {
                setupMesh();
            }
            
            Mesh3D(float* verticesData, int vertexCount, unsigned int* indicesData, int indexCount) {
                for (int i = 0; i < vertexCount; i += 8) {
                    Vertex vertex;
                    vertex.Position = MirulitMath::Vector3f(verticesData[i], verticesData[i+1], verticesData[i+2]);
                    vertex.Normal = MirulitMath::Vector3f(verticesData[i+3], verticesData[i+4], verticesData[i+5]);
                    vertex.TexCoords = MirulitMath::Vector2f(verticesData[i+6], verticesData[i+7]);
                    vertices.push_back(vertex);
                }
                
                for (int i = 0; i < indexCount; i++) {
                    indices.push_back(indicesData[i]);
                }
                
                setupMesh();
            }
            
            Mesh3D(float* vertices, int vertexCount) {
                for (int i = 0; i < vertexCount; i += 3) {
                    Vertex vertex;
                    vertex.Position = MirulitMath::Vector3f(vertices[i], vertices[i+1], vertices[i+2]);
                    vertex.Normal = MirulitMath::Vector3f(0.0f, 0.0f, 0.0f);
                    vertex.TexCoords = MirulitMath::Vector2f(0.0f, 0.0f);
                    this->vertices.push_back(vertex);
                }
                setupMesh();
            }
            
            // Конструктор для загрузки из OBJ файла
            Mesh3D(const std::string& objPath) {
                loadOBJ(objPath);
                setupMesh();
            }
            
            // Конструктор для загрузки из OBJ файла с текстурами
            Mesh3D(const std::string& objPath, const std::vector<Texture>& modelTextures) 
                : textures(modelTextures) {
                loadOBJ(objPath);
                setupMesh();
            }
            
            // Функция загрузки OBJ файла
            bool loadOBJ(const std::string& path) {
                std::vector<MirulitMath::Vector3f> tempPositions;
                std::vector<MirulitMath::Vector2f> tempTexCoords;
                std::vector<MirulitMath::Vector3f> tempNormals;
                
                std::vector<unsigned int> positionIndices, texCoordIndices, normalIndices;
                
                std::ifstream file(path);
                if (!file.is_open()) {
                    std::cerr << "Failed to open OBJ file: " << path << std::endl;
                    return false;
                }
                
                std::string line;
                while (std::getline(file, line)) {
                    // Пропускаем пустые строки и комментарии
                    if (line.empty() || line[0] == '#') continue;
                    
                    std::istringstream iss(line);
                    std::string prefix;
                    iss >> prefix;
                    
                    if (prefix == "v") {
                        // Вершина
                        MirulitMath::Vector3f pos;
                        iss >> pos.x >> pos.y >> pos.z;
                        tempPositions.push_back(pos);
                    }
                    else if (prefix == "vt") {
                        // Текстурная координата
                        MirulitMath::Vector2f tex;
                        iss >> tex.x >> tex.y;
                        tempTexCoords.push_back(tex);
                    }
                    else if (prefix == "vn") {
                        // Нормаль
                        MirulitMath::Vector3f norm;
                        iss >> norm.x >> norm.y >> norm.z;
                        tempNormals.push_back(norm);
                    }
                    else if (prefix == "f") {
                        // Грань
                        std::string vertex1, vertex2, vertex3;
                        iss >> vertex1 >> vertex2 >> vertex3;
                        
                        // Обрабатываем каждый vertex формата "pos/tex/norm" или "pos//norm"
                        std::vector<std::string> tokens1 = splitString(vertex1, '/');
                        std::vector<std::string> tokens2 = splitString(vertex2, '/');
                        std::vector<std::string> tokens3 = splitString(vertex3, '/');
                        
                        // Функция для обработки индекса
                        auto processIndex = [](const std::string& token) -> unsigned int {
                            if (token.empty()) return 0;
                            return std::stoi(token) - 1; // OBJ индексы начинаются с 1
                        };
                        
                        // Для каждого vertex добавляем индексы
                        positionIndices.push_back(processIndex(tokens1[0]));
                        positionIndices.push_back(processIndex(tokens2[0]));
                        positionIndices.push_back(processIndex(tokens3[0]));
                        
                        if (tokens1.size() > 1 && !tokens1[1].empty()) {
                            texCoordIndices.push_back(processIndex(tokens1[1]));
                            texCoordIndices.push_back(processIndex(tokens2[1]));
                            texCoordIndices.push_back(processIndex(tokens3[1]));
                        }
                        
                        if (tokens1.size() > 2 && !tokens1[2].empty()) {
                            normalIndices.push_back(processIndex(tokens1[2]));
                            normalIndices.push_back(processIndex(tokens2[2]));
                            normalIndices.push_back(processIndex(tokens3[2]));
                        }
                        
                        // Поддержка четырехугольников
                        std::string vertex4;
                        if (iss >> vertex4) {
                            std::vector<std::string> tokens4 = splitString(vertex4, '/');
                            
                            // Добавляем два треугольника для четырехугольника
                            positionIndices.push_back(processIndex(tokens1[0]));
                            positionIndices.push_back(processIndex(tokens3[0]));
                            positionIndices.push_back(processIndex(tokens4[0]));
                            
                            if (tokens1.size() > 1 && !tokens1[1].empty()) {
                                texCoordIndices.push_back(processIndex(tokens1[1]));
                                texCoordIndices.push_back(processIndex(tokens3[1]));
                                texCoordIndices.push_back(processIndex(tokens4[1]));
                            }
                            
                            if (tokens1.size() > 2 && !tokens1[2].empty()) {
                                normalIndices.push_back(processIndex(tokens1[2]));
                                normalIndices.push_back(processIndex(tokens3[2]));
                                normalIndices.push_back(processIndex(tokens4[2]));
                            }
                        }
                    }
                }
                
                file.close();
                
                // Создаем вершины
                bool hasTexCoords = !tempTexCoords.empty();
                bool hasNormals = !tempNormals.empty();
                
                // Используем map для избежания дублирования вершин
                std::map<std::tuple<unsigned int, unsigned int, unsigned int>, unsigned int> vertexCache;
                
                for (size_t i = 0; i < positionIndices.size(); i++) {
                    unsigned int posIndex = positionIndices[i];
                    unsigned int texIndex = hasTexCoords ? texCoordIndices[i] : 0;
                    unsigned int normIndex = hasNormals ? normalIndices[i] : 0;
                    
                    auto key = std::make_tuple(posIndex, texIndex, normIndex);
                    
                    // Проверяем, существует ли уже такая вершина
                    if (vertexCache.find(key) != vertexCache.end()) {
                        indices.push_back(vertexCache[key]);
                    } else {
                        Vertex vertex;
                        vertex.Position = tempPositions[posIndex];
                        
                        if (hasTexCoords && texIndex < tempTexCoords.size()) {
                            vertex.TexCoords = tempTexCoords[texIndex];
                        } else {
                            vertex.TexCoords = MirulitMath::Vector2f(0.0f, 0.0f);
                        }
                        
                        if (hasNormals && normIndex < tempNormals.size()) {
                            vertex.Normal = tempNormals[normIndex];
                        } else {
                            vertex.Normal = MirulitMath::Vector3f(0.0f, 0.0f, 0.0f);
                        }
                        
                        // Tangent и Bitangent будут вычислены позже при необходимости
                        vertex.Tangent = MirulitMath::Vector3f(0.0f, 0.0f, 0.0f);
                        vertex.Bitangent = MirulitMath::Vector3f(0.0f, 0.0f, 0.0f);
                        
                        vertices.push_back(vertex);
                        unsigned int newIndex = vertices.size() - 1;
                        vertexCache[key] = newIndex;
                        indices.push_back(newIndex);
                    }
                }
                
                // Если нет нормалей, вычисляем их
                if (!hasNormals) {
                    computeNormals();
                }
                
                std::cout << "OBJ loaded: " << path << std::endl;
                std::cout << "Vertices: " << vertices.size() << ", Indices: " << indices.size() << std::endl;
                
                return true;
            }
            
            // Вычисление нормалей, если их нет в файле
            void computeNormals() {
                for (size_t i = 0; i < indices.size(); i += 3) {
                    unsigned int i0 = indices[i];
                    unsigned int i1 = indices[i + 1];
                    unsigned int i2 = indices[i + 2];
                    
                    MirulitMath::Vector3f v0 = vertices[i0].Position;
                    MirulitMath::Vector3f v1 = vertices[i1].Position;
                    MirulitMath::Vector3f v2 = vertices[i2].Position;
                    
                    MirulitMath::Vector3f normal = (v1 - v0).cross(v2 - v0);
                    normal.normalize();
                    
                    vertices[i0].Normal = vertices[i0].Normal + normal;
                    vertices[i1].Normal = vertices[i1].Normal + normal;
                    vertices[i2].Normal = vertices[i2].Normal + normal;
                }
                
                // Нормализуем все нормали
                for (auto& vertex : vertices) {
                    vertex.Normal.normalize();
                }
            }
            
            void Draw(GLuint shaderID) const {
                unsigned int diffuseNr = 1;
                unsigned int specularNr = 1;
                unsigned int normalNr = 1;
                unsigned int heightNr = 1;
                
                for (unsigned int i = 0; i < textures.size(); i++) {
                    glActiveTexture(GL_TEXTURE0 + i);
                    
                    std::string number;
                    std::string name = textures[i].type;
                    
                    if (name == "texture_diffuse")
                        number = std::to_string(diffuseNr++);
                    else if (name == "texture_specular")
                        number = std::to_string(specularNr++);
                    else if (name == "texture_normal")
                        number = std::to_string(normalNr++);
                    else if (name == "texture_height")
                        number = std::to_string(heightNr++);
                    
                    glUniform1i(glGetUniformLocation(shaderID, (name + number).c_str()), i);
                    glBindTexture(GL_TEXTURE_2D, textures[i].id);
                }
                
                glBindVertexArray(VAO);
                if (indices.size() > 0) {
                    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
                } else {
                    glDrawArrays(GL_TRIANGLES, 0, vertices.size());
                }
                glBindVertexArray(0);
                
                glActiveTexture(GL_TEXTURE0);
            }
            
            // Функция для загрузки текстуры
            static Texture loadTexture(const char* path, const std::string& type) {
                Texture texture;
                texture.type = type;
                texture.path = path;
                
                glGenTextures(1, &texture.id);
                glBindTexture(GL_TEXTURE_2D, texture.id);
                
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                
                int width, height, nrChannels;
                unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);
                
                if (data) {
                    GLenum format;
                    if (nrChannels == 1)
                        format = GL_RED;
                    else if (nrChannels == 3)
                        format = GL_RGB;
                    else if (nrChannels == 4)
                        format = GL_RGBA;
                    
                    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
                    glGenerateMipmap(GL_TEXTURE_2D);
                } else {
                    std::cerr << "Failed to load texture: " << path << std::endl;
                }
                
                stbi_image_free(data);
                glBindTexture(GL_TEXTURE_2D, 0);
                
                return texture;
            }
            
            const std::vector<Vertex>& getVertices() const { return vertices; }
            const std::vector<unsigned int>& getIndices() const { return indices; }
            const std::vector<Texture>& getTextures() const { return textures; }
            
            ~Mesh3D() {
                glDeleteVertexArrays(1, &VAO);
                glDeleteBuffers(1, &VBO);
                glDeleteBuffers(1, &EBO);
            }
    };
}