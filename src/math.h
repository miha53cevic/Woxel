#ifndef MATH_H
#define MATH_H

#pragma once
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "glm/gtc/noise.hpp"

#include <random>

#ifdef _WIN32
#include <SDL.h>
#elif __linux__
#include <SDL2/SDL.h>
#endif

struct Entity;

class Camera
{
public:
    Camera();

    void Update(SDL_Window* win, bool hasFocus, float sens = 0.25f);
    void Movement(const Uint8* keys, float elapsed, float speed = 10);

    glm::vec3 getPosition();
    glm::vec3 getRotation();

    void updatePosition(glm::vec3 position);
    void updateRotation(glm::vec3 rotation);

private:
    glm::vec3 m_position;
    // Rotations are: Pitch, Yaw, Roll
    glm::vec3 m_rotation;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace Math
{
    glm::mat4x4 createTransformationMatrix(glm::vec3 translation, glm::vec3 rotation, glm::vec3 scale);
    glm::mat4x4 createProjectionMatrix(glm::vec2 screenSize, float FOV, float NEAR_PLANE, float FAR_PLANE);
    glm::mat4x4 createViewMatrix(Camera& camera);
    
    // Calculate the Model View Projection Matrix by multiplying in the reverse order and then multiplying by the vertex
    // - MVP = Projection * View * Model
    // - Shaders: gl_Position = MVP * vec4(vertexPosition, 1.0);
    glm::mat4x4 createMVPMatrix(glm::vec2 screenSize, float FOV, float NEAR_PLANE, float FAR_PLANE, Camera& camera, Entity& entity);

    // Random number generator in given range
    float fRandom(float first, float second);
    int   iRandom(int first, int second);

    // Maps values from one range to another
    float map(float s, float a1, float a2, float b1, float b2);

    // Get 2D simplex noise
    // - algorithm from: https://github.com/fogleman/Craft/blob/master/deps/noise/noise.c
    // - octaves: how many times should you combine the noise
    // - persistence: changes the height values (lower values add more height, higher values make the height deeper)
    // - lacunarity: frequency of sample points (lower values adds noise closer, whilst higher values make the noise more distant)
    float simplex2(float x, float y, int octaves, float persistence, float lacunarity);

    // Get 3D simplex noise
    float simplex3(float x, float y, float z, int octaves, float persistence, float lacunarity);

    // Ray class
    class Ray
    {
    public:
        Ray(const glm::vec3& position, const glm::vec3& direction);

        void step(float scale);

        const glm::vec3& getEnd() const;

        float getLength() const;

    private:
        glm::vec3 m_rayStart;
        glm::vec3 m_rayEnd;
        glm::vec3 m_direction;
    };
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif