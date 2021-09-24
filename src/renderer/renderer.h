#pragma once
#include "../gl/glObjects.h"

struct Entity;

class Renderer
{
private:
    // Singleton
    Renderer() = delete;

public:
    static void Render(gl::VertexArray& vao, gl::ElementArrayBuffer& ebo, gl::Texture& texture, gl::Material& material, GLenum mode = GL_TRIANGLES);
    static void RenderEntity(Entity& entity, gl::Material& material, GLenum mode = GL_TRIANGLES);
    static void RenderNoTexture(gl::VertexArray& vao, gl::ElementArrayBuffer& ebo, gl::Material& material, GLenum mode = GL_TRIANGLES);

    static int drawCalls;

private:
};