#include "Entity.h"

Entity::Entity()
    : position(0, 0, 0)
    , rotation(0, 0, 0)
    , scale(1, 1, 1)
{
}

void Entity::setEBO(const std::vector<GLuint>& indicies, int DrawMode)
{
    VAO.Bind();
    EBO.setData(indicies, DrawMode);
    VAO.Unbind();
}

void Entity::setVBO(const std::vector<GLfloat>& data, int attributeID, int size, int DrawMode)
{
    VAO.Bind();
    auto VBO = std::make_unique<gl::VertexBufferObject>();
    VBO->setData(data, attributeID, size, DrawMode);
    VBOs.push_back(std::move(VBO));
    VAO.Unbind();
}
