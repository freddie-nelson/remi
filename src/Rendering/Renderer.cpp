#include "../../include/Rendering/Renderer.h"
#include "../../include/Rendering/Shader/MeshShader.h"
#include "../../include/Rendering/Utility/OpenGLHelpers.h"

#include "../../include/externals/glad/glad.h"
#include <iostream>
#include <math.h>
#include <glm/gtc/type_ptr.hpp>

Rendering::Renderer::Renderer(GLFWwindow *glfwWindow, int width, int height) : glfwWindow(glfwWindow), width(width), height(height)
{
    resize(width, height);
}

Rendering::Renderer::~Renderer()
{
    delete this;
}

void Rendering::Renderer::init()
{
    meshShader = Shader();
    if (!meshShader.loadFromSource(meshVertexShader, meshFragShader))
    {
        throw std::runtime_error("Failed to load mesh shader.");
    }
}

void Rendering::Renderer::clear(bool clearColorBuffer, bool clearDepthBuffer, bool clearStencilBuffer)
{
    glClearColor(clearColor.r(), clearColor.g(), clearColor.b(), clearColor.a());

    int clearBits = 0;
    if (clearColorBuffer)
    {
        clearBits |= GL_COLOR_BUFFER_BIT;
    }
    if (clearDepthBuffer)
    {
        clearBits |= GL_DEPTH_BUFFER_BIT;
    }
    if (clearStencilBuffer)
    {
        clearBits |= GL_STENCIL_BUFFER_BIT;
    }

    glClear(clearBits);
}

void Rendering::Renderer::present()
{
    glfwSwapBuffers(glfwWindow);
}

void Rendering::Renderer::mesh(const Mesh2D &m)
{
    mesh(m, m.color);
}

void Rendering::Renderer::mesh(const Mesh2D &m, const Color &color)
{
    float *vertices = (float *)glm::value_ptr(m.vertices[0]);
    const std::vector<unsigned int> &indices = m.indices;

    // for (int i = 0; i < mesh.vertices.size(); i++)
    // {
    //     std::cout << vertices[i * 2] << ", " << vertices[i * 2 + 1] << std::endl;
    //     std::cout << mesh.vertices[i].x << ", " << mesh.vertices[i].y << std::endl;
    // }

    meshShader.use();

    auto colorVec = color.getColor();
    meshShader.setUniform("uColor", &colorVec);

    glm::vec2 resolution(width, height);
    meshShader.setUniform("uResolution", &resolution);

    glm::vec2 translation = m.translation;
    glm::mat2 transform = m.transform;

    meshShader.setUniform("uMeshTranslation", &translation);
    meshShader.setUniform("uMeshTransform", &transform);

    meshShader.setAttrib("aPos", vertices, m.vertices.size() * 2, 2, GL_FLOAT, false, 0, GL_DYNAMIC_DRAW);
    meshShader.setIndices("aPos", &m.indices[0], m.indices.size(), GL_DYNAMIC_DRAW);

    meshShader.draw(GL_TRIANGLES, m.indices.size());
    meshShader.unbind();
};

void Rendering::Renderer::setClearColor(const Color &color)
{
    clearColor = color;
}

Rendering::Color Rendering::Renderer::getClearColor() const
{
    return clearColor;
}

void Rendering::Renderer::resize(int w, int h)
{
    width = w;
    height = h;

    glViewport(0, 0, width, height);
}

std::pair<int, int> Rendering::Renderer::getSize() const
{
    return std::make_pair(width, height);
}

std::pair<int, int> Rendering::Renderer::getWindowSize() const
{
    int width, height;
    glfwGetFramebufferSize(glfwWindow, &width, &height);

    return std::make_pair(width, height);
}