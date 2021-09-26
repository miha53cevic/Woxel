#ifndef UI_H
#define UI_H
#include <glm/glm.hpp>
#include "../gl/glObjects.h"
#include <string>
#include <map>
#include <memory>

struct UI
{
    glm::vec2 position, size;
    float rotation;
    
    gl::Texture texture;
};

class UIRenderer
{
public:
    UIRenderer(glm::ivec2 screenSize);

    void addUI(std::string ui_name, std::string texturePath);
    void setUI(std::string ui_name, glm::vec2 position, glm::vec2 size, float rotation);
    UI&  getUI(std::string ui_name);

    void DrawUI(std::string ui_name);
    void DrawAllUI();

    void HideUI(std::string ui_name);

private:
    void prepRenderData();

    gl::VertexArray m_VAO;
    gl::Shader      m_shader;
    glm::ivec2      m_screenSize;

    void setVBO(const std::vector<GLfloat>& data, int attributeID, int size, GLsizei stride = 0, const void * offset = nullptr, int DrawMode = GL_STATIC_DRAW);
    void updateVBO(int index, const std::vector<GLfloat>& data, int attributeID, int size, GLsizei stride = 0, const void * offset = nullptr, int DrawMode = GL_STATIC_DRAW);
    void freeVBOs();

    std::vector<std::unique_ptr<gl::VertexBufferObject>> VBOs;

    std::map<std::string, UI> m_uiElements;
};

#endif