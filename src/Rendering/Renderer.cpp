#include "../../include/Rendering/Renderer.h"

#include "../../include/externals/glad/glad.h"
#include <iostream>
#include <math.h>

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

void Rendering::Renderer::mesh(const Mesh2D &mesh, const Color &color)
{
    // SDL_SetRenderDrawColor(sdlRenderer, color.r, color.g, color.b, color.a);

    const std::vector<unsigned int> &indices = mesh.indices;
    std::vector<glm::vec2> transformedVertices;

    for (auto &v : mesh.vertices)
    {
        glm::vec2 transformedVertex = v;
        transformedVertex += mesh.translation;
        // transformedVertex *= mesh.transform;

        transformedVertices.push_back(transformedVertex);
    }

    for (int i = 0; i < indices.size(); i += 3)
    {
        int j = (i + 1) % indices.size();
        int k = (i + 2) % indices.size();

        glm::vec2 v1 = transformedVertices[indices[i]];
        glm::vec2 v2 = transformedVertices[indices[j]];
        glm::vec2 v3 = transformedVertices[indices[k]];

        // SDL_RenderDrawLine(sdlRenderer, v1.x, v1.y, v2.x, v2.y);
        // SDL_RenderDrawLine(sdlRenderer, v2.x, v2.y, v3.x, v3.y);
        // SDL_RenderDrawLine(sdlRenderer, v3.x, v3.y, v1.x, v1.y);
    }
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