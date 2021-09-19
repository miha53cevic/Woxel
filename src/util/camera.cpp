#include "camera.h"

Camera::Camera()
    : m_position(glm::vec3(0, 0, 0))
    , m_rotation(glm::vec3(0, 0, 0))
{
}

void Camera::Update(SDL_Window * win, bool hasFocus, float sens)
{
    // Get the offset between mouse movements then add them to rotation
    int mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);

    int w, h;
    SDL_GetWindowSize(win, &w, &h);
    int screenCenterW = w / 2;
    int screenCenterH = h / 2;

    if (hasFocus)
    {
        float xOffset = mouseX - screenCenterW;
        float yOffset = mouseY - screenCenterH;

        xOffset *= sens;
        yOffset *= sens;

        m_rotation.x += yOffset;
        m_rotation.y += xOffset;

        // Set max up/down rotation
        if (m_rotation.x > 90.0f)
            m_rotation.x = 90.0f;

        if (m_rotation.x < -90.0f)
            m_rotation.x = -90.0f;

        // Set mouse position to the center of the screen
        if (xOffset != 0 || yOffset != 0)
            SDL_WarpMouseInWindow(win, screenCenterW, screenCenterH);
    }
}

void Camera::Movement(const Uint8 * keys, float elapsed, float speed)
{
    if (keys[SDL_SCANCODE_W])
    {
        // Calculate the directional vector and add it to camera position
        m_position.z -= speed * elapsed * cosf(glm::radians(-m_rotation.y));
        m_position.x -= speed * elapsed * sinf(glm::radians(-m_rotation.y));
    }
    if (keys[SDL_SCANCODE_S])
    {
        m_position.z += speed * elapsed * cosf(glm::radians(-m_rotation.y));
        m_position.x += speed * elapsed * sinf(glm::radians(-m_rotation.y));
    }
    if (keys[SDL_SCANCODE_D])
    {
        m_position.z -= speed * elapsed * sinf(glm::radians(-m_rotation.y));
        m_position.x += speed * elapsed * cosf(glm::radians(-m_rotation.y));
    }
    if (keys[SDL_SCANCODE_A])
    {
        m_position.z += speed * elapsed * sinf(glm::radians(-m_rotation.y));
        m_position.x -= speed * elapsed * cosf(glm::radians(-m_rotation.y));
    }

    if (keys[SDL_SCANCODE_LCTRL])
    {
        m_position.y -= speed * elapsed;
    }
    if (keys[SDL_SCANCODE_SPACE])
    {
        m_position.y += speed * elapsed;
    }
}

glm::vec3 Camera::getPosition()
{
    return m_position;
}

glm::vec3 Camera::getRotation()
{
    return m_rotation;
}

void Camera::setPosition(glm::vec3 position)
{
    m_position = position;
}

void Camera::setRotation(glm::vec3 rotation)
{
    m_rotation = rotation;
}