#pragma once

#include "components/renderer/opengl/mesh.h"
#include "components/renderer/opengl/shader.h"
#include "components/renderer/opengl/Texture.h"

namespace MirulitEngine {

    typedef struct transform {
        MirulitMath::Vector2f position;
        MirulitMath::Vector2f scale;
        float rotate;
    } transform;

    class Entity {
        public:

            MirulitComponets::Mesh* mesh = nullptr;
            MirulitComponets::Shader* shader = nullptr;

            MirulitEngine::transform transform;

            Entity(MirulitMath::Vector2f position, MirulitMath::Vector2f scale, float rotate) {
                transform.position = position;
                transform.scale = scale;
                transform.rotate = rotate;
            }

            ~Entity() {
                delete mesh;
                delete shader;
            }

            void Poligon(const std::vector<float>& vertices) {
                mesh = new MirulitComponets::Mesh(vertices);
            }

            void Rect() {
                float w = transform.scale.x / 2.0f;
                float h = transform.scale.y / 2.0f;
                float x = transform.position.x;
                float y = transform.position.y;
                
                std::vector<float> vertices = {
                    x - w, y - h,
                    x + w, y - h,
                    x + w, y + h,
                    
                    x + w, y + h,
                    x - w, y + h,
                    x - w, y - h
                };
                
                mesh = new MirulitComponets::Mesh(vertices);
            }

            void Circle(int segments = 32) {
                float radius = (transform.scale.x + transform.scale.y) / 4.0f;
                float x = transform.position.x;
                float y = transform.position.y;
                
                std::vector<float> vertices;
                
                for (int i = 0; i < segments; i++) {
                    float angle1 = 2.0f * 3.14159f * i / segments;
                    float angle2 = 2.0f * 3.14159f * (i + 1) / segments;
                    
                    vertices.push_back(x);
                    vertices.push_back(y);
                    
                    vertices.push_back(x + radius * cos(angle1));
                    vertices.push_back(y + radius * sin(angle1));
                    
                    vertices.push_back(x + radius * cos(angle2));
                    vertices.push_back(y + radius * sin(angle2));
                }
                
                mesh = new MirulitComponets::Mesh(vertices);
            }

            void initRender(const char* texture) {
                #ifdef OpenGL_Mirulit

                GLuint _texture = MirulitComponets::loadTexture(texture);

                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, _texture);

                shader = new MirulitComponets::Shader("assets/shaders/default.vert", "assets/shaders/default.frag", "file");

                shader->setInt("texture", 0);
                #endif
            }

            void draw() {
                if (shader && mesh) {
                    shader->use();
                    mesh->draw();
                }
            }

            void SetPosition(MirulitMath::Vector2f pos) {
                shader->setVec2("position", pos);
            }

            void SetRotate(float rot) {
                shader->setFloat("rotate", rot);
            }

            void SetScale(MirulitMath::Vector2f sca) {
                shader->setVec2("position", sca);
            }
    };
}