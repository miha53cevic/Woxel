#pragma once
#include "../gl/glObjects.h"
#include <memory>

class Camera;

struct Entity
{
    Entity();

    glm::vec3 position, rotation, scale;

    gl::VertexArray VAO;
    gl::ElementArrayBuffer EBO;

    void setEBO(const std::vector<GLuint>& indicies, int DrawMode = GL_STATIC_DRAW);
    void setVBO(const std::vector<GLfloat>& data, int attributeID, int size, int DrawMode = GL_STATIC_DRAW);

    void updateVBO(int index, const std::vector<GLfloat>& data, int attributeID, int size, int DrawMode = GL_STATIC_DRAW);

    void freeVBOs();

    std::vector<std::unique_ptr<gl::VertexBufferObject>> VBOs;

    gl::Texture texture;
};