#pragma once

#include "Color.h"
#include "./Mesh/Mesh.h"
#include "./Shader/Shader.h"
#include "./Camera.h"

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
     *
     * The renderer also handles the camera, by default it will create a camera with the same size as the renderer.
     * The camera's size can be synced to the renderer's size, or it can be set manually. By default, the camera's size is not synced.
     *
     * Depth testing is enabled by default.
     *
     * Alpha blending is disabled by default.
     *
     * The default alpha blending function is `GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA`.
     */
    class Renderer
    {
    public:
        /**
         * Creates a new renderer attached to the given GLFWwindow.
         *
         * Will also create a default camera with the given width and height, and centre at (0, 0).
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
        void clear(bool clearColorBuffer = true, bool clearDepthBuffer = true, bool clearStencilBuffer = true) const;

        /**
         * Swaps the front and back buffers, presenting the rendered image to the screen.
         */
        void present() const;

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
         * Uses the vertices and indices from `m`. `transformations[0], colors[0]` will be used for the first instance (`m`).
         *
         * Params are not const because they are passed to OpenGL, they most likely won't be modified.
         *
         * @param m The mesh to render.
         * @param transformations The transformations matrices of the instances.
         * @param colors The colors of the instances, as vec4s.
         */
        void instancedMesh(const Mesh2D &m, std::vector<glm::mat4> &transformations, std::vector<glm::vec4> &colors);

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
         * Enables or disables depth testing.
         *
         * Depth testing is enabled by default.
         *
         * When depth testing is enabled, the renderer will only render the closest fragment.
         *
         * @param enable Whether to enable or disable depth testing.
         */
        void enableDepthTest(bool enable);

        /**
         * Returns whether depth testing is enabled.
         *
         * @returns whether depth testing is enabled.
         */
        bool isDepthTestEnabled() const;

        /**
         * Enables or disables alpha blending.
         *
         * Alpha blending is disabled by default.
         *
         * When alpha blending is enabled, the renderer will blend the fragment color with the color already in the framebuffer.
         *
         * The correct sorting of objects is not handled by the renderer, it is up to the user to sort objects correctly.
         *
         * @param enable Whether to enable or disable alpha blending.
         */
        void enableAlphaBlending(bool enable);

        /**
         * Sets the alpha blending function.
         *
         * This is the sfactor and dfactor used in `glBlendFunc(sfactor, dfactor)`.
         *
         * By default, this is `GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA`.
         *
         * This does not enable alpha blending, use `enableAlphaBlending` to enable alpha blending.
         *
         * @param sfactor The sfactor to use in `glBlendFunc(sfactor, dfactor)`.
         * @param dfactor The dfactor to use in `glBlendFunc(sfactor, dfactor)`.
         *
         * @throws std::invalid_argument if sfactor or dfactor is not a valid value.
         */
        void setAlphaBlendingFunction(GLenum sfactor, GLenum dfactor);

        /**
         * Returns whether alpha blending is enabled.
         *
         * @returns whether alpha blending is enabled.
         */
        bool isAlphaBlendingEnabled() const;

        /**
         * Sets the width and height of the renderer.
         *
         * This will also set the glViewport.
         *
         * @param w The width of the renderer
         * @param h The height of the renderer
         */
        void setSize(int w, int h);

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

        /**
         * Sets the camera to use for rendering.
         *
         * @param camera The camera to use for rendering.
         */
        void setCamera(const Camera &camera);

        /**
         * Returns a reference to the camera used for rendering.
         *
         * @returns The camera used for rendering.
         */
        Camera &getCamera();

        /**
         * Sets whether the renderer should update the camera size to match the renderer size, always.
         *
         * By default, this is false.
         *
         * @param sync Whether to sync the camera size to the renderer size.
         */
        void syncCameraSize(bool sync);

        /**
         * Returns whether the renderer is syncing the camera size to the renderer size.
         *
         * @returns Whether the renderer is syncing the camera size to the renderer size.
         */
        bool getSyncCameraSize() const;

        /**
         * Updates the view projection matrix from the camera.
         *
         * Should be called at the start of each frame.
         */
        void updateViewProjectionMatrix();

    private:
        int width;
        int height;

        /**
         * The view projection matrix.
         *
         * Should be updated at the start of each frame from the camera.
         */
        glm::mat4 viewProjectionMatrix;

        bool syncCameraSizeWithRenderer = false;
        Camera camera;

        Shader meshShader;
        Shader instancedMeshShader;

        GLenum alphaBlendingSFactor = GL_SRC_ALPHA;
        GLenum alphaBlendingDFactor = GL_ONE_MINUS_SRC_ALPHA;

        Color clearColor = Color(0.0f);

        GLFWwindow *glfwWindow;
    };
}