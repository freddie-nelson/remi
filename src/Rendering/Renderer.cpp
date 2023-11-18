#include "../../include/Rendering/Renderer.h"

#include <iostream>
#include <math.h>

Rendering::Renderer::Renderer(GLFWwindow *glfwWindow, int windowWidth, int windowHeight) : glfwWindow(glfwWindow), windowWidth(windowWidth), windowHeight(windowHeight)
{
}

Rendering::Renderer::~Renderer()
{
    delete this;
}

void Rendering::Renderer::init()
{
}

void Rendering::Renderer::clear()
{
    // SDL_SetRenderDrawColor(sdlRenderer, 0, 0, 0, 255);
    // SDL_RenderClear(sdlRenderer);
}

void Rendering::Renderer::present()
{
    // SDL_RenderPresent(sdlRenderer);
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