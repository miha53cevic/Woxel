#include "app.h"
#include "entity.h"
#include "math.h"
#include "cube.h"
#include "world.h"

class Woxel : public App
{
public:
    Woxel(const char* title, int width, int height)
        : App(title, width, height)
    {}

private:
    virtual bool Event(SDL_Event& e) override
    {
        // Left mouse click - destroy blocks
        if (e.type == SDL_MOUSEBUTTONDOWN)
        {
            // If the ray doesn't hit a block the X part gets set to INFINITY
            if (lastRayPos.x != INFINITY)
            {
                // Set the block to air if it's destroyed
                chunk_manager.setBlockGlobal(lastRayPos.x, lastRayPos.y, lastRayPos.z, 0);
                // Update the chunk mesh and the surrounding neighbours as well
                auto chunk = chunk_manager.getChunkFromGlobal(lastRayPos.x, lastRayPos.y, lastRayPos.z);
                chunk->Update();
                chunk->UpdateNeighbours();
            }
        }

        return true;
    }
    virtual bool Setup() override
    {
        // Enable back face CCW culling
        Culling(true);

        shader.setAttribute(0, "position");
        shader.setAttribute(1, "textureCoords");
        shader.createProgram("resources/shaders/shader");

        shader.setUniformLocation("MVPMatrix");

        // TODO - Remove cube, only used for testing
        cube.texture.loadTexture("resources/textures/plank_draft.png");
        cube.position = glm::vec3(-0.5f, -0.5f, -2);
        cube.setEBO(Cube::indicies);
        cube.setVBO(Cube::verticies, 0, 3);
        cube.setVBO(Cube::textureCoords, 1, 2);

        chunk_manager.generateChunks(2, 1, 1);
        printf("Created %d chunk(s)\n", chunk_manager.chunks.size());

        setClearColor(255, 255, 255, 255);

        return true;
    }
    virtual bool Loop(float elapsed) override
    {
        camera.Update(m_window, GetFocus());
        camera.Movement(m_keys, elapsed);

        // Ray casting
        for (Math::Ray ray(camera.getPosition(), camera.getRotation()); ray.getLength() < 6; ray.step(0.05f))
        {
            glm::vec3 r = ray.getEnd();
            if (chunk_manager.getBlockGlobal(r.x, r.y, r.z) > 0)
            {
                lastRayPos = r;
                break;
            }
            else lastRayPos = glm::vec3(INFINITY, 0, 0);
        }

        RenderEntity(cube, shader);

        // Render chunks in the chunk_manager
        for (auto& i : chunk_manager.chunks)
            RenderEntity(i->chunk, shader);

        return true;
    }

private:
    void RenderEntity(Entity& e, gl::Shader& s)
    {
        s.Bind();

        e.texture.activateAndBind();
        shader.loadMatrix(
            shader.getUniformLocation("MVPMatrix"),
            Math::createMVPMatrix(glm::vec2(ScreenWidth(), ScreenHeight()), 90, 0.1f, 100.0f, camera, e)
        );

        e.VAO.Bind();
        glDrawElements(GL_TRIANGLES, e.EBO.size, GL_UNSIGNED_INT, 0);
        e.VAO.Unbind();

        s.Unbind();
    }

private:
    Entity cube;
    gl::Shader shader;
    Camera camera;
    ChunkManager chunk_manager;
    glm::vec3 lastRayPos;
};

int main(int argc, char* argv[])
{
    Woxel game("Woxel", 1280, 720);
    game.Run();
    return 0;
}