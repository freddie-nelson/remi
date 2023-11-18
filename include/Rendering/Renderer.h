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

    /**
     * The renderer is responsible for rendering meshes to the screen.
     *
     * It is also responsible for clearing the screen and swapping the front and back buffers.
     *
     * The renderer does not handle the window, it only handles the rendering through OpenGL.
     */
    class Renderer
    {
    public:
        Renderer(GLFWwindow *glfwWindow, int width, int height);
        ~Renderer();

        /**
         * Initializes the renderer.
         */
        void init();

        /**
         * Clears the render back buffer.
         */
        void clear();

        /**
         * Swaps the front and back buffers, presenting the rendered image to the screen.
         */
        void present();

        /**
         * Renders the given mesh with the given color.
         */
        void mesh(const Mesh2D &mesh, const Color &color);

        /**
         * Sets the clear color.
         */
        void setClearColor(const glm::vec4 &color);

        /**
         * Returns the current clear color.
         */
        glm::vec4 getClearColor() const;

        /**
         * Sets the width and height of the renderer.
         *
         * This will also set the glViewport.
         */
        void resize(int w, int h);

        /**
         * Returns the width and height of the renderer.
         */
        std::pair<int, int> getSize() const;

        /**
         * Returns the width and height of the attached GLFWwindow.
         */
        std::pair<int, int> getWindowSize() const;

    private:
        int width;
        int height;

        glm::vec4 clearColor = {0, 0, 0, 1};

        GLFWwindow *glfwWindow;
    };
}