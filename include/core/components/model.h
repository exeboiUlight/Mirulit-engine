#pragma once

#include <glad/glad.h>
#include <vector>
#include <string>
#include <unordered_map>
#include <iostream>
#include "mesh.h"
#include "shader.h"

// Простой OBJ парсер (если у вас нет simple_obj.h)
namespace tinyobj {
    struct attrib_t {
        std::vector<float> vertices;
        std::vector<float> normals;
        std::vector<float> texcoords;
    };
    
    struct mesh_t {
        std::vector<unsigned int> indices;
    };
    
    struct shape_t {
        mesh_t mesh;
    };
    
    struct material_t {
    };
    
    bool LoadObj(attrib_t* attrib, std::vector<shape_t>* shapes, 
                 std::vector<material_t>* materials,
                 std::string* warn, std::string* err,
                 const char* filename) {
        // Простой парсер OBJ файла
        FILE* file = fopen(filename, "r");
        if (!file) {
            *err = "Cannot open file";
            return false;
        }
        
        char line[1024];
        while (fgets(line, sizeof(line), file)) {
            if (line[0] == 'v') {
                if (line[1] == ' ') {
                    // Вершина
                    float x, y, z;
                    sscanf(line, "v %f %f %f", &x, &y, &z);
                    attrib->vertices.push_back(x);
                    attrib->vertices.push_back(y);
                    attrib->vertices.push_back(z);
                }
                else if (line[1] == 'n') {
                    // Нормаль
                    float x, y, z;
                    sscanf(line, "vn %f %f %f", &x, &y, &z);
                    attrib->normals.push_back(x);
                    attrib->normals.push_back(y);
                    attrib->normals.push_back(z);
                }
                else if (line[1] == 't') {
                    // UV координата
                    float u, v;
                    sscanf(line, "vt %f %f", &u, &v);
                    attrib->texcoords.push_back(u);
                    attrib->texcoords.push_back(v);
                }
            }
            else if (line[0] == 'f') {
                // Грань
                unsigned int v1, v2, v3;
                unsigned int t1, t2, t3;
                unsigned int n1, n2, n3;
                
                int matches = sscanf(line, "f %d/%d/%d %d/%d/%d %d/%d/%d",
                    &v1, &t1, &n1, &v2, &t2, &n2, &v3, &t3, &n3);
                
                if (matches == 9) {
                    shapes->back().mesh.indices.push_back(v1 - 1);
                    shapes->back().mesh.indices.push_back(v2 - 1);
                    shapes->back().mesh.indices.push_back(v3 - 1);
                }
                else {
                    matches = sscanf(line, "f %d//%d %d//%d %d//%d",
                        &v1, &n1, &v2, &n2, &v3, &n3);
                    if (matches == 6) {
                        shapes->back().mesh.indices.push_back(v1 - 1);
                        shapes->back().mesh.indices.push_back(v2 - 1);
                        shapes->back().mesh.indices.push_back(v3 - 1);
                    }
                    else {
                        matches = sscanf(line, "f %d %d %d", &v1, &v2, &v3);
                        if (matches == 3) {
                            shapes->back().mesh.indices.push_back(v1 - 1);
                            shapes->back().mesh.indices.push_back(v2 - 1);
                            shapes->back().mesh.indices.push_back(v3 - 1);
                        }
                    }
                }
            }
            
            if (line[0] == 'o' || line[0] == 'g') {
                // Новая группа/объект
                shape_t newShape;
                shapes->push_back(newShape);
            }
        }
        
        if (shapes->empty() && !attrib->vertices.empty()) {
            shape_t defaultShape;
            for (size_t i = 0; i < attrib->vertices.size() / 3; i++) {
                defaultShape.mesh.indices.push_back(i);
            }
            shapes->push_back(defaultShape);
        }
        
        fclose(file);
        return true;
    }
}

namespace Engine {

class Model {
public:
    Model() {}
    
    Model(const std::string& path) {
        LoadModel(path);
    }
    
    void LoadModel(const std::string& path) {
        m_Path = path;
        
        // Определяем тип файла по расширению
        size_t dotPos = path.find_last_of(".");
        if (dotPos != std::string::npos) {
            std::string ext = path.substr(dotPos + 1);
            if (ext == "obj") {
                LoadOBJ(path);
            } else {
                std::cout << "Unsupported model format: " << path << std::endl;
            }
        } else {
            std::cout << "Unsupported model format: " << path << std::endl;
        }
    }
    
    void Draw(Shader& shader) const {
        for (const auto& mesh : m_Meshes) {
            mesh.Draw();
        }
    }
    
    const std::vector<Mesh>& GetMeshes() const { return m_Meshes; }
    bool IsLoaded() const { return !m_Meshes.empty(); }
    
private:
    std::vector<Mesh> m_Meshes;
    std::string m_Path;
    
    void LoadOBJ(const std::string& path) {
        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string warn, err;
        
        bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, path.c_str());
        
        if (!warn.empty()) {
            std::cout << "OBJ Warning: " << warn << std::endl;
        }
        
        if (!err.empty()) {
            std::cout << "OBJ Error: " << err << std::endl;
        }
        
        if (!ret) {
            std::cout << "Failed to load OBJ: " << path << std::endl;
            return;
        }
        
        // Если нет форм, создаем одну
        if (shapes.empty()) {
            tinyobj::shape_t defaultShape;
            for (size_t i = 0; i < attrib.vertices.size() / 3; i++) {
                defaultShape.mesh.indices.push_back(i);
            }
            shapes.push_back(defaultShape);
        }
        
        // Обрабатываем каждую форму (mesh)
        for (const auto& shape : shapes) {
            std::vector<Vertex> vertices;
            std::vector<unsigned int> indices;
            std::unordered_map<size_t, unsigned int> uniqueVertices;
            
            for (size_t i = 0; i < shape.mesh.indices.size(); i++) {
                unsigned int idx = shape.mesh.indices[i];
                
                Vertex vertex;
                
                // Позиции
                if (idx * 3 + 2 < attrib.vertices.size()) {
                    vertex.Position.x = attrib.vertices[idx * 3 + 0];
                    vertex.Position.y = attrib.vertices[idx * 3 + 1];
                    vertex.Position.z = attrib.vertices[idx * 3 + 2];
                }
                
                // Нормали
                if (idx * 3 + 2 < attrib.normals.size()) {
                    vertex.Normal.x = attrib.normals[idx * 3 + 0];
                    vertex.Normal.y = attrib.normals[idx * 3 + 1];
                    vertex.Normal.z = attrib.normals[idx * 3 + 2];
                } else {
                    vertex.Normal = Vector3(0.0f, 0.0f, 0.0f);
                }
                
                // UV координаты
                if (idx * 2 + 1 < attrib.texcoords.size()) {
                    vertex.TexCoords.x = attrib.texcoords[idx * 2 + 0];
                    vertex.TexCoords.y = attrib.texcoords[idx * 2 + 1];
                } else {
                    vertex.TexCoords = Vector2(0.0f, 0.0f);
                }
                
                // Дедупликация вершин
                size_t hash = (size_t)(vertex.Position.x * 1000) ^ 
                             ((size_t)(vertex.Position.y * 1000) << 10) ^
                             ((size_t)(vertex.Position.z * 1000) << 20);
                
                if (uniqueVertices.find(hash) == uniqueVertices.end()) {
                    uniqueVertices[hash] = static_cast<unsigned int>(vertices.size());
                    vertices.push_back(vertex);
                }
                
                indices.push_back(uniqueVertices[hash]);
            }
            
            if (!vertices.empty() && !indices.empty()) {
                // Создаем меш
                m_Meshes.emplace_back(vertices, indices);
            }
        }
        
        std::cout << "Loaded OBJ: " << path << " with " << m_Meshes.size() << " meshes" << std::endl;
        std::cout << "Total vertices: ";
        size_t totalVerts = 0;
        for (const auto& mesh : m_Meshes) {
            totalVerts += mesh.m_VertexCount;
        }
        std::cout << totalVerts << std::endl;
    }
};

}