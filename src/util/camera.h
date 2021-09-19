#pragma once
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/matrix_transform.hpp>

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

    void setPosition(glm::vec3 position);
    void setRotation(glm::vec3 rotation);

private:
    glm::vec3 m_position;
    // Rotations are: Pitch, Yaw, Roll
    glm::vec3 m_rotation;
};