#pragma once

#include "../ECS/System.h"
#include "../ECS/Registry.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <glm/vec2.hpp>

#include <string>
#include <functional>

namespace Core
{
    /**
     * The window is responsible for creating and managing the window.
     *
     * The window must be initialized before it can be used.
     */
    class Window : public ECS::System
    {
    public:
        /**
         * Creates a new window with the given title and dimensions.
         *
         * Initializes glfw.
         *
         * @param windowTitle The title of the window.
         * @param windowWidth The width of the window.
         * @param windowHeight The height of the window.
         * @param fullscreen Whether to create the window in fullscreen mode.
         */
        Window(std::string windowTitle, unsigned int windowWidth, unsigned int windowHeight, bool fullscreen = false);

        /**
         * Destroys the window.
         */
        virtual ~Window();

        /**
         * Initializes the window.
         *
         * This will crash if the opengl version is not supported.
         *
         * @throws std::runtime_error if the window failed to initialize.
         * @throws std::invalid_argument if the opengl version is not supported.
         */
        void init();

        /**
         * Destroys the window, freeing all resources and closing the window.
         */
        void destroy();

        /**
         * Updates the window
         *
         * @param registry The registry to update with.
         * @param timestep The timestep since the last update.
         */
        void update(const ECS::Registry &registry, const Core::Timestep &timestep) override;

        /**
         * Shows the window.
         */
        void show();

        /**
         * Hides the window.
         */
        void hide();

        /**
         * Returns the width and height of the window.
         */
        glm::uvec2 getSize() const;

        /**
         * Sets the width and height of the window.
         *
         * @param width The width of the window.
         * @param height The height of the window.
         */
        void setSize(unsigned int width, unsigned int height);

        /**
         * Gets the width of the window.
         *
         * @returns The width of the window.
         */
        unsigned int getWidth() const;

        /**
         * Sets the width of the window.
         *
         * @param width The width of the window.
         */
        void setWidth(unsigned int width);

        /**
         * Gets the height of the window.
         *
         * @returns The height of the window.
         */
        unsigned int getHeight() const;

        /**
         * Sets the height of the window.
         *
         * @param height The height of the window.
         */
        void setHeight(unsigned int height);

        /**
         * Returns the position of the window.
         *
         * The position is relative to the top left corner of the screen.
         *
         * @returns The position of the window.
         */
        glm::ivec2 getPosition() const;

        /**
         * Sets the position of the window.
         *
         * The position is relative to the top left corner of the screen.
         *
         * @param x The x position of the window.
         * @param y The y position of the window.
         */
        void setPosition(int x, int y);

        /**
         * Enables or disables vsync.
         *
         * By default vsync is disabled.
         *
         * This may not work on all platforms.
         *
         * @param enable Whether to enable or disable vsync.
         */
        void toggleVsync(bool enable);

        /**
         * Returns the GLFW window.
         *
         * @returns The GLFW window.
         */
        GLFWwindow *getGLFWWindow() const;

    private:
        std::string windowTitle;
        unsigned int initialWindowWidth;
        unsigned int initialWindowHeight;
        bool isFullscreen;

        /**
         * OpenGL ES 3.0 for webgl 2.0 matching.
         */

        const unsigned int openglMajorVersion = 3;
        const unsigned int openglMinorVersion = 0;

        bool showWindow = true;

        GLFWwindow *glfwWindow;

        /**
         * Creates a GLFW window.
         *
         * @param openglMajorVersion The major version of OpenGL to use.
         * @param openglMinorVersion The minor version of OpenGL to use.
         * @param debugContext Whether to create a debug context on opengl versions >= 4.3.
         * @param monitor The monitor to create the window on, or null for windowed mode.
         */
        GLFWwindow *createGLFWWindow(int openglMajorVersion, int openglMinorVersion, bool debugContext, GLFWmonitor *monitor = nullptr);

        struct OpenGLContext
        {
            int majorVersion;
            int minorVersion;
            std::string versionString;
            std::string vendor;

            bool debugContext;
        };

        /**
         * Creates an OpenGL context.
         *
         * @param window The window to create the context for.
         *
         * @returns The OpenGL context or nullptr if failed to create context.
         */
        OpenGLContext *createOpenGLContext(GLFWwindow *window);

        /**
         * Returns all OpenGL contexts that support the given parameters.
         *
         *
         * It does this by creating a window for each monitor and then destroying it.
         *
         * @param openglMajorVersion The major version of OpenGL to use.
         * @param openglMinorVersion The minor version of OpenGL to use.
         * @param debugContext Whether to create a debug context on opengl versions >= 4.3.
         *
         * @returns A vector of all supported OpenGL contexts and the monitor they were created on.
         *
         * @warning Doesn't really work, it will return the same context for each monitor.
         */
        std::vector<std::pair<OpenGLContext *, GLFWmonitor *>> getAllSupportedOpenGLContexts(int openglMajorVersion, int openglMinorVersion, bool debugContext);
    };
}