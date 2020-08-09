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
        if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT)
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

        // Set Sky colour
        setClearColor(51, 76, 76, 255);

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

        chunk_manager.generateChunks(3, 1, 3);
        chunk_manager.generateTerrain(0.24f, 1, CHUNK_SIZE - 4);
        printf("Created %d chunk(s)\n", chunk_manager.chunks.size());

        outline.setAttribute(0, "position");
        outline.createProgram("resources/shaders/outline_shader");

        outline.setUniformLocation("MVPMatrix");

        return true;
    }
    virtual bool Loop(float elapsed) override
    {
        // Update camera position and rotation
        camera.Update(m_window, GetFocus());
        camera.Movement(m_keys, elapsed);

        // Ray casting
        for (Math::Ray ray(camera.getPosition(), camera.getRotation()); ray.getLength() < 6; ray.step(0.05f))
        {
            glm::vec3 r = ray.getEnd();
            if (chunk_manager.getBlockGlobal(r.x, r.y, r.z) > 0)
            {
                // Save the first blocks position that was hit and break out of the loop
                lastRayPos = r;

                createCubeOutline(r.x, r.y, r.z, 5);
                break;
            }
            else lastRayPos = glm::vec3(INFINITY, 0, 0);
        }

        RenderEntity(cube, shader);

        // Render chunks in the chunk_manager
        for (auto& i : chunk_manager.chunks)
            RenderChunk(i->chunk, shader);

        RenderEntity(voxelOutline, outline, GL_LINES);

        return true;
    }

private:
    void RenderEntity(Entity& e, gl::Shader& s, GLenum mode = GL_TRIANGLES)
    {
        s.Bind();

        // Check if a texture exists and try to load it
        if (e.texture.texture != -1)
            e.texture.activateAndBind();

        s.loadMatrix(
            s.getUniformLocation("MVPMatrix"),
            Math::createMVPMatrix(glm::vec2(ScreenWidth(), ScreenHeight()), 90, 0.1f, 100.0f, camera, e)
        );

        e.VAO.Bind();
        glDrawElements(mode, e.EBO.size, GL_UNSIGNED_INT, 0);
        e.VAO.Unbind();

        s.Unbind();
    }

    void RenderChunk(Entity& e, gl::Shader& s)
    {
        s.Bind();

        chunk_manager.atlas.texture.activateAndBind();

        s.loadMatrix(
            s.getUniformLocation("MVPMatrix"),
            Math::createMVPMatrix(glm::vec2(ScreenWidth(), ScreenHeight()), 90, 0.1f, 100.0f, camera, e)
        );

        e.VAO.Bind();
        glDrawElements(GL_TRIANGLES, e.EBO.size, GL_UNSIGNED_INT, 0);
        e.VAO.Unbind();

        s.Unbind();
    }

    void createCubeOutline(float x, float y, float z, int width)
    {
        // Set to intiger space because the cubes are 1*1*1 in size
        x = (int)x;
        y = (int)y;
        z = (int)z;

        std::vector<float> vert = {
            x+0, y+1, z+1,
            x+0, y+1, z+0,
            x+1, y+1, z+0,
            x+1, y+1, z+1,

            x+0, y+0, z+1,
            x+0, y+0, z+0,
            x+1, y+0, z+0,
            x+1, y+0, z+1
        };

        // Lines are defined by 2 vertexes which index you get from the vert array
        std::vector<GLuint> indic = {
            0, 1,
            1, 2,
            2, 3,
            3, 0,
            4, 5,
            5, 6,
            6, 7,
            7, 4,
            0, 4,
            1, 5,
            2, 6,
            3, 7
        };

        // Set VBO creates a new VBO where update just changes the data
        if (voxelOutline.VBOs.empty()) voxelOutline.setVBO(vert, 0, 3);
        else                           voxelOutline.updateVBO(0, vert, 0, 3);
        voxelOutline.setEBO(indic);

        glLineWidth(width);
    }

private:
    Entity cube;
    gl::Shader shader;
    Camera camera;
    ChunkManager chunk_manager;
    glm::vec3 lastRayPos;

    Entity voxelOutline;
    gl::Shader outline;
};

int main(int argc, char* argv[])
{
    Woxel game("Woxel", 1280, 720);
    game.Run();
    return 0;
}