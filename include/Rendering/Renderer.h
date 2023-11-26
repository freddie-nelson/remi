#pragma once

#include "Color.h"
#include "./Mesh/Mesh.h"
#include "./Shader/Shader.h"

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
        /**
         * Creates a new renderer attached to the given GLFWwindow.
         *
         * @param glfwWindow The GLFWwindow to attach to.
         * @param width The width of the renderer viewport.
         * @param height The height of the renderer viewport.
         */
        Renderer(GLFWwindow *glfwWindow, int width, int height);

        /**
         * Destroys the renderer.
         */
        ~Renderer();

        /**
         * Initializes the renderer.
         */
        void init();

        /**
         * Clears the render buffers.
         */
        void clear(bool clearColorBuffer = true, bool clearDepthBuffer = true, bool clearStencilBuffer = true);

        /**
         * Swaps the front and back buffers, presenting the rendered image to the screen.
         */
        void present();

        /**
         * Renders the given mesh with the `mesh.color`.
         *
         * @param m The mesh to render.
         */
        void mesh(const Mesh2D &m);

        /**
         * Renders the given mesh with the given color.
         *
         * @param m The mesh to render.
         * @param color The color to render the mesh, overwrites the color set on the mesh.
         */
        void mesh(const Mesh2D &m, const Color &color);

        /**
         * Renders the given mesh instances.
         *
         * Uses the vertices and indices from `m`. `translations[0], transformations[0], colors[0]` will be used for the first instance (`m`).
         *
         * Params are not const because they are passed to OpenGL, they most likely won't be modified.
         *
         * @param m The mesh to render.
         * @param translations The translations of the instances.
         * @param transformations The transformations of the instances.
         * @param colors The colors of the instances, as vec4s.
         */
        void instancedMesh(const Mesh2D &m, std::vector<glm::vec2> &translations, std::vector<glm::mat2> &transformations, std::vector<glm::vec4> &colors);

        /**
         * Sets the clear color.
         *
         * @param color The new clear color.
         */
        void setClearColor(const Color &color);

        /**
         * Returns the current clear color.
         *
         * @returns The clear color.
         */
        Color getClearColor() const;

        /**
         * Sets the width and height of the renderer.
         *
         * This will also set the glViewport.
         *
         * @param w The width of the renderer
         * @param h The height of the renderer
         */
        void resize(int w, int h);

        /**
         * Returns the width and height of the renderer.
         *
         * @returns The width and height of the renderer.
         */
        std::pair<int, int> getSize() const;

        /**
         * Returns the width and height of the attached GLFWwindow.
         *
         * @returns The width and height of the attached GLFWwindow.
         */
        std::pair<int, int> getWindowSize() const;

    private:
        int width;
        int height;

        Shader meshShader;
        Shader instancedMeshShader;

        Color clearColor = Color(0.0f);

        GLFWwindow *glfwWindow;
    };
}