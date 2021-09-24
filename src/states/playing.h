#pragma once
#include "state.h"

#include "../world/blocks.h"
#include "../world/chunkmanager.h"
#include "../util/math.h"
#include "../util/cube.h"
#include "../util/camera.h"
#include "../util/entity.h"
#include "../ui/ui.h"


class Playing : public State
{
public:
    Playing();

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

    gl::Material shader_material;
    gl::Material outline_material;

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
    void createCubeOutline(float x, float y, float z, int width);
    void createBreakingAnimation(glm::ivec2 breakAnimTexCoords);
    void breakBlockAction(float elapsed);
    void CollisionMovement(float speed, float elapsed);
};