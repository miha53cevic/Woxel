#include "ui.h"
#include <glm/gtc/matrix_transform.hpp>

UIRenderer::UIRenderer(glm::ivec2 screenSize, std::string texturePath)
    : m_screenSize(screenSize)
{
    prepRenderData();

    // Load shader
    m_shader.setAttribute(0, "position");
    m_shader.setAttribute(1, "textureCoords");
    m_shader.createProgram("resources/shaders/ui");
    
    m_shader.setUniformLocation("model");
    m_shader.setUniformLocation("projection");

    // Create projection matrix (2D so we use ortho)
    glm::mat4 projection = glm::ortho(0.0f, (float)screenSize.x, (float)screenSize.y, 0.0f, -1.0f, 1.0f);

    m_shader.Bind();
    m_shader.loadMatrix(m_shader.getUniformLocation("projection"), projection);
    m_shader.Unbind();
}

void UIRenderer::addUI(std::string ui_name, std::string texturePath)
{
    if (m_uiElements.find(ui_name) == m_uiElements.end())
    {
        m_uiElements[ui_name].texture.loadTexture(texturePath);

        m_uiElements[ui_name].position  = { 0, 0 };
        m_uiElements[ui_name].size      = { 0, 0 };
        m_uiElements[ui_name].rotation  = 0.0f;
    }
    else printf("Could not add UI element %s because it already exists!\n", ui_name.c_str());
}

void UIRenderer::setUI(std::string ui_name, glm::vec2 position, glm::vec2 size, float rotation)
{
    if (m_uiElements.find(ui_name) != m_uiElements.end())
    {
        m_uiElements[ui_name].position  = position;
        m_uiElements[ui_name].size      = size;
        m_uiElements[ui_name].rotation  = rotation;
    }
    else printf("UI element %s does not exist!\n", ui_name.c_str());
}

UI & UIRenderer::getUI(std::string ui_name)
{
    if (m_uiElements.find(ui_name) != m_uiElements.end())
    {
        return m_uiElements[ui_name];
    }
    else
    {
        printf("UI element %s does not exist!\n", ui_name.c_str());
        UI temp;
        return temp;
    }
}

// Reference
// https://learnopengl.com/In-Practice/2D-Game/Rendering-Sprites
void UIRenderer::DrawUI(std::string ui_name)
{
    if (m_uiElements.find(ui_name) != m_uiElements.end())
    {
        auto& ui = m_uiElements[ui_name];
        m_shader.Bind();

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(ui.position, 0.0f));

        model = glm::translate(model, glm::vec3(0.5f * ui.size.x, 0.5f * ui.size.y, 0.0f));
        model = glm::rotate(model, glm::radians(ui.rotation), glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::translate(model, glm::vec3(-0.5f * ui.size.x, -0.5f * ui.size.y, 0.0f));

        model = glm::scale(model, glm::vec3(ui.size, 1.0f));

        m_shader.loadMatrix(m_shader.getUniformLocation("model"), model);

        ui.texture.activateAndBind();
        m_VAO.Bind();
        glDrawArrays(GL_TRIANGLES, 0, 6);
        m_VAO.Unbind();

        m_shader.Unbind();
    }
    else printf("UI element %s does not exist!\n", ui_name.c_str());
}

void UIRenderer::prepRenderData()
{
    std::vector<float> verticies = {
        // pos      
        0.0f, 1.0f, 
        1.0f, 0.0f, 
        0.0f, 0.0f, 

        0.0f, 1.0f, 
        1.0f, 1.0f, 
        1.0f, 0.0f, 
    };

    std::vector<float> textureCoords = {
        // tex
        0.0f, 1.0f,
        1.0f, 0.0f,
        0.0f, 0.0f,

        0.0f, 1.0f,
        1.0f, 1.0f,
        1.0f, 0.0f
    };

    setVBO(verticies, 0, 2);
    setVBO(textureCoords, 1, 2);
}

void UIRenderer::setVBO(const std::vector<GLfloat>& data, int attributeID, int size, int DrawMode)
{
    m_VAO.Bind();
    auto VBO = std::make_unique<gl::VertexBufferObject>();
    VBO->setData(data, attributeID, size, DrawMode);
    VBOs.push_back(std::move(VBO));
    m_VAO.Unbind();
}

void UIRenderer::updateVBO(int index, const std::vector<GLfloat>& data, int attributeID, int size, int DrawMode)
{
    VBOs[index]->setData(data, attributeID, size, DrawMode);
}

void UIRenderer::freeVBOs()
{
    for (auto& vbo : VBOs)
        vbo.reset(); // delete the object, leaving vbo empty
}
