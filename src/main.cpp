#include "app.h"
#include "entity.h"
#include "math.h"
#include "world.h"

#define PLAYER_HEIGHT 2

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

        // Right mouse click - add blocks
        if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_RIGHT)
        {
            if (lastRayPos.x != INFINITY)
            {
                glm::vec3 temp_ray = lastRayPos;
                // Keep looping until we either find an AIR block or get out of bounds and get -1
                while (chunk_manager.getBlockGlobal(temp_ray.x, temp_ray.y, temp_ray.z) != 0 &&
                       chunk_manager.getBlockGlobal(temp_ray.x, temp_ray.y, temp_ray.z) != -1
                    )
                {
                    temp_ray += lastUnitRay * 0.05f;
                }

                // We still have to check if we got -1(OutOfBounds) or 0(AIR)
                if (chunk_manager.getBlockGlobal(temp_ray.x, temp_ray.y, temp_ray.z) == 0)
                {
                    // Set the block and update the chunk and it's neighbours
                    chunk_manager.setBlockGlobal(temp_ray.x, temp_ray.y, temp_ray.z, 1);

                    auto chunk = chunk_manager.getChunkFromGlobal(lastRayPos.x, lastRayPos.y, lastRayPos.z);
                    chunk->Update();
                    chunk->UpdateNeighbours();
                }
            }
        }

        return true;
    }
    virtual bool Setup() override
    {
        // Enable back face CCW culling
        Culling(true);

        // Set Sky colour
        setClearColor(64, 191, 255, 255);

        shader.setAttribute(0, "position");
        shader.setAttribute(1, "textureCoords");
        shader.createProgram("resources/shaders/shader");

        shader.setUniformLocation("MVPMatrix");

        chunk_manager.generateChunks(4, 4, 4);
        chunk_manager.generateTerrain(0.25f, 1, CHUNK_SIZE * 3);
        //chunk_manager.generateFlatTerrain(CHUNK_SIZE + 2);
        printf("Created %d chunk(s)\n", chunk_manager.chunks.size());

        outline.setAttribute(0, "position");
        outline.createProgram("resources/shaders/outline_shader");

        outline.setUniformLocation("MVPMatrix");

        camera.updatePosition({ 5, CHUNK_SIZE * 2, 5 });

        return true;
    }
    virtual bool Loop(float elapsed) override
    {
        // Update camera position and rotation
        camera.Update(m_window, GetFocus());
        //camera.Movement(m_keys, elapsed);
        CollisionMovement(10, elapsed);

        // Ray casting
        for (Math::Ray ray(camera.getPosition(), camera.getRotation()); ray.getLength() < 6; ray.step(0.05f))
        {
            glm::vec3 r = ray.getEnd();
            if (chunk_manager.getBlockGlobal(r.x, r.y, r.z) > 0)
            {
                // Save the first blocks position that was hit and break out of the loop
                lastRayPos = r;

                // Save the unit vector of the ray as well
                lastUnitRay.x = glm::cos(glm::radians(camera.getRotation().y + 90));
                lastUnitRay.z = glm::sin(glm::radians(camera.getRotation().y + 90));
                lastUnitRay.y = glm::tan(glm::radians(camera.getRotation().x));

                createCubeOutline(r.x, r.y, r.z, 5);
                break;
            }
            else lastRayPos = glm::vec3(INFINITY, 0, 0);
        }

        // Render chunks in the chunk_manager
        for (auto& i : chunk_manager.chunks)
            RenderChunk(i->chunk, shader);

        // Render selected block outline
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
            Math::createMVPMatrix(glm::vec2(ScreenWidth(), ScreenHeight()), 90, 0.1f, 1000.0f, camera, e)
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
            Math::createMVPMatrix(glm::vec2(ScreenWidth(), ScreenHeight()), 90, 0.1f, 1000.0f, camera, e)
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

    void CollisionMovement(float speed, float elapsed)
    {
        const float Offset = 0.25f;

        glm::vec3 rotation = camera.getRotation();

        glm::vec3 movedPos = camera.getPosition();
        if (m_keys[SDL_SCANCODE_W])
        {
            // Calculate the directional vector and add it to camera position
            movedPos.z -= speed * elapsed * cosf(glm::radians(-rotation.y));
            movedPos.x -= speed * elapsed * sinf(glm::radians(-rotation.y));
        }
        if (m_keys[SDL_SCANCODE_S])
        {
            movedPos.z += speed * elapsed * cosf(glm::radians(-rotation.y));
            movedPos.x += speed * elapsed * sinf(glm::radians(-rotation.y));
        }
        if (m_keys[SDL_SCANCODE_D])
        {
            movedPos.z -= speed * elapsed * sinf(glm::radians(-rotation.y));
            movedPos.x += speed * elapsed * cosf(glm::radians(-rotation.y));
        }
        if (m_keys[SDL_SCANCODE_A])
        {
            movedPos.z += speed * elapsed * sinf(glm::radians(-rotation.y));
            movedPos.x -= speed * elapsed * cosf(glm::radians(-rotation.y));
        }

        if (m_keys[SDL_SCANCODE_LCTRL])
        {
            movedPos.y -= speed * elapsed;
        }
        if (m_keys[SDL_SCANCODE_SPACE])
        {
            movedPos.y += speed * elapsed;
        }

        // Add extra offset to movedPos because the camera clips the faces otherwise
        // check if we need to add + or - offset depending on the direction we are moving to so (newPos - curPos)
        glm::vec3 offset_movePos = movedPos;
        if (movedPos.x - camera.getPosition().x < 0)
            offset_movePos.x -= Offset;
        else
            offset_movePos.x += Offset;

        if (movedPos.y - camera.getPosition().y < 0)
            offset_movePos.y -= Offset;
        else
            offset_movePos.y += Offset;

        if (movedPos.z - camera.getPosition().z < 0)
            offset_movePos.z -= Offset;
        else
            offset_movePos.z += Offset;

        if (chunk_manager.getBlockGlobal(offset_movePos.x, offset_movePos.y, offset_movePos.z) > 0 ||
            chunk_manager.getBlockGlobal(offset_movePos.x, offset_movePos.y - (PLAYER_HEIGHT - 1), offset_movePos.z) > 0)
        {
            
        }
        // Keep the player inside of chunk borders by making sure he can only travel through air blocks
        else if (chunk_manager.getBlockGlobal(offset_movePos.x, offset_movePos.y, offset_movePos.z) == 0)
        {
            camera.updatePosition(movedPos);
        }
        
    }

private:
    gl::Shader shader;
    Camera camera;
    ChunkManager chunk_manager;
    glm::vec3 lastRayPos;
    glm::vec3 lastUnitRay;

    Entity voxelOutline;
    gl::Shader outline;
};

int main(int argc, char* argv[])
{
    Woxel game("Woxel", 1280, 720);
    game.Run();
    return 0;
}