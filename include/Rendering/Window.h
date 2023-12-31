#pragma once

#include "Renderer.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <string>
#include <functional>

namespace Rendering
{
    using WindowFrameCallback = std::function<void(float, Renderer *)>;

    /**
     * The window is responsible for creating and managing the window.
     *
     * It is also responsible for polling events and running the main loop.
     *
     * The window will also create and manage the Renderer.
     *
     * The window must be initialized before it can be used.
     *
     * By default the window runs at 60 fps.
     */
    class Window
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
         */
        Window(std::string windowTitle, unsigned int windowWidth, unsigned int windowHeight);

        /**
         * Destroys the window.
         */
        ~Window();

        /**
         * Initializes the window.
         *
         * This will crash if the opengl version is not supported.
         *
         * @param openglMajorVersion The major version of OpenGL to use.
         * @param openglMinorVersion The minor version of OpenGL to use.
         *
         * @returns The error code, 0 if no error.
         */
        int init(unsigned int openglMajorVersion = 4, unsigned int openglMinorVersion = 6);

        /**
         * Destroys the window, freeing all resources and closing the window.
         */
        void destroy();

        /**
         * Runs the window.
         *
         * This will run the main loop until the window is closed.
         *
         * @param frameCallback The callback to call each frame.
         *
         * @returns The error code, 0 if no error.
         */
        int run(WindowFrameCallback frameCallback);

        /**
         * Stops the `run` loop if it is running.
         *
         * This will not close the window.
         *
         * When stopped the window will only poll events, to prevent the window from freezing. This **will** call event listeners.
         */
        void stop();

        /**
         * Shows the window.
         */
        void show();

        /**
         * Hides the window.
         */
        void hide();

        /**
         * Returns the max fps that the window should update at.
         *
         * @returns The max fps that the window should update at.
         */
        unsigned int getFps() const;

        /**
         * Sets the max fps that the window should update at.
         *
         * @param fps The max fps that the window should update at.
         */
        void setFps(unsigned int fps);

        /**
         * Returns the time in milliseconds that a frame should take.
         *
         * @returns The time in milliseconds that a frame should take.
         */
        unsigned int getFrameTime() const;

        /**
         * Returns the width and height of the window.
         */
        std::pair<unsigned int, unsigned int> getSize() const;

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
        std::pair<int, int> getPosition() const;

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
         * Sets whether the renderer should be resized when the window is resized.
         *
         * By default, this is true.
         *
         * The renderer will be resized at the start of each frame.
         *
         * @param sync Whether to sync the renderer size with the window size.
         */
        void syncRendererSize(bool sync);

        /**
         * Returns whether the renderer is resized when the window is resized.
         *
         * @returns Whether the renderer is resized when the window is resized.
         */
        bool getSyncRendererSize() const;

        Renderer *getRenderer() const;

    private:
        std::string windowTitle;
        unsigned int initialWindowWidth;
        unsigned int initialWindowHeight;

        unsigned int fps = 60;
        bool showWindow = true;
        bool syncRendererSizeWithWindow = true;

        GLFWwindow *glfwWindow;
        Rendering::Renderer *renderer;

        bool running = false;

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

        /**
         * Polls events from the window.
         */
        void pollEvents();
    };
}