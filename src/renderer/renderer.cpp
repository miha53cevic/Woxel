#include "renderer.h"
#include "../util/entity.h"

void Renderer::Render(gl::VertexArray & vao, gl::ElementArrayBuffer & ebo, gl::Texture & texture, gl::Material & material, GLenum mode)
{
    material.shader->Bind();

    // Check if a texture exists and try to load it
    if (texture.texture != -1)
        texture.activateAndBind();
    else printf("[Renderer]: Could not bind texture!\n");

    vao.Bind();
    gl::glClearErrors();
    glDrawElements(mode, ebo.size, GL_UNSIGNED_INT, nullptr);
    gl::glCheckError(__FILE__, __LINE__);
    drawCalls++;
    vao.Unbind();

    material.shader->Unbind();
}

void Renderer::RenderEntity(Entity & entity, gl::Material & material, GLenum mode)
{
    material.shader->Bind();

    // Check if a texture exists and try to load it
    if (entity.texture.texture != -1)
        entity.texture.activateAndBind();
    else printf("[Renderer]: Could not bind texture!\n");

    entity.VAO.Bind();
    gl::glClearErrors();
    glDrawElements(mode, entity.EBO.size, GL_UNSIGNED_INT, nullptr);
    gl::glCheckError(__FILE__, __LINE__);
    drawCalls++;
    entity.VAO.Unbind();

    material.shader->Unbind();
}

void Renderer::RenderNoTexture(gl::VertexArray & vao, gl::ElementArrayBuffer & ebo, gl::Material & material, GLenum mode)
{
    material.shader->Bind();

    vao.Bind();
    gl::glClearErrors();
    glDrawElements(mode, ebo.size, GL_UNSIGNED_INT, nullptr);
    gl::glCheckError(__FILE__, __LINE__);
    drawCalls++;
    vao.Unbind();

    material.shader->Unbind();
}

int Renderer::drawCalls = 0;