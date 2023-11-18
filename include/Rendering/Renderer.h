#pragma once

#include "Color.h"
#include "./Mesh/Mesh.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <glm/vec2.hpp>
#include <string>
#include <vector>

namespace Rendering
{

    class Renderer
    {
    public:
        Renderer(GLFWwindow *glfwWindow, int windowWidth, int windowHeight);
        ~Renderer();

        void init();

        void clear();
        void present();

        void mesh(const Mesh2D &mesh, const Color &color);

    private:
        int windowWidth;
        int windowHeight;

        GLFWwindow *glfwWindow;
    };
}