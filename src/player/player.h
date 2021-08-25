#pragma once
#include "camera.h"

class World;

class Player
{
public:
    Player(World* world);

    void Update(float elapsed);

private:
    void CollisionMovement(const Uint8* keys, float elapsed, float speed = 10);
    void Movement(const Uint8* keys, float elapsed, float speed = 10);
    void CreateSelectOutline();
    void BreakBlock();

private:
    World* m_world;

    Camera m_camera;
};