#pragma once
#include "state.h"

#include "../world/entity.h"
#include "../util/math.h"
#include "../world/world.h"
#include "../util/cube.h"
#include "../world/blocks.h"
#include "../ui/ui.h"
#include "../player/camera.h"
#include "../world/chunkmanager.h"

class App;

class Playing : public State
{
public:
    Playing(App* app);

    virtual void Event(SDL_Event& e) override;
    virtual void Setup() override;
    virtual void Loop(float elapsed) override;
    
    virtual void Pause();
    virtual void Resume();

private:
    gl::Shader shader;
    Camera camera;
    ChunkManager chunk_manager;
    glm::vec3 lastRayPos;
    glm::vec3 lastUnitRay;

    Entity voxelOutline;
    gl::Shader outline;

    glm::vec3 velocity;

    float totalTime;
    Entity breakingCube;
    glm::ivec3 breakingBlockPos;

    UIRenderer uirenderer;
    int hotbar_selection = 0;

    int hotbar[7];

    bool bWireframe = false;
    bool bCreativeMode = false;

private:
    void RenderEntity(Entity& e, gl::Shader& s, GLenum mode = GL_TRIANGLES);
    void RenderChunk(Entity& e, gl::Shader& s);
    void createCubeOutline(float x, float y, float z, int width);
    void createBreakingAnimation(glm::ivec2 breakAnimTexCoords);
    void breakBlockAction(float elapsed);
    void CollisionMovement(float speed, float elapsed);
};