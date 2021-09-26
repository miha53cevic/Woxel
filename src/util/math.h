#pragma once
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "glm/gtc/noise.hpp"

#ifdef _WIN32
#include <SDL.h>
#elif __linux__
#include <SDL2/SDL.h>
#endif

struct Entity;
class Camera;

namespace Math
{
    glm::mat4x4 createTransformationMatrix(glm::vec3 translation, glm::vec3 rotation, glm::vec3 scale);
    glm::mat4x4 createProjectionMatrix(glm::vec2 screenSize, float FOV = 90.0f, float NEAR_PLANE = 0.1f, float FAR_PLANE = 1000.0f);
    glm::mat4x4 createViewMatrix(Camera& camera);
    
    // Calculate the Model View Projection Matrix by multiplying in the reverse order and then multiplying by the vertex
    // - MVP = Projection * View * Model
    // - Shaders: gl_Position = MVP * vec4(vertexPosition, 1.0);
    glm::mat4x4 createMVPMatrix(Entity& entity, Camera& camera, glm::vec2 screenSize, float FOV = 90.0f, float NEAR_PLANE = 0.1f, float FAR_PLANE = 1000.0f);

    // Random number generator in given range
    float fRandom(float first, float second);
    int   iRandom(int first, int second);

    // Maps values from one range to another
    float map(float s, float a1, float a2, float b1, float b2);

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

namespace Noise
{
    struct NoiseOptions
    {
        int octaves;
        float frequency;
        float roughness;
        float redistribution;
    };

    float simplex2(float x, float y, NoiseOptions& options);
    float simplex3(float x, float y, float z, NoiseOptions& options);
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////