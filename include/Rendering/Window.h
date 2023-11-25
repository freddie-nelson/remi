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
     */
    class Window
    {
    public:
        /**
         * Creates a new window with the given title and dimensions.
         */
        Window(std::string windowTitle, int windowWidth, int windowHeight);

        /**
         * Destroys the window.
         */
        ~Window();

        /**
         * Initializes the window.
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
         */
        int getFps() const;

        /**
         * Sets the max fps that the window should update at.
         */
        void setFps(int fps);

        /**
         * Returns the time in milliseconds that a frame should take.
         */
        int getFrameTime() const;

        /**
         * Returns the width and height of the window.
         */
        std::pair<int, int> getSize() const;

        /**
         * Sets the width and height of the window.
         *
         * @param width The width of the window.
         * @param height The height of the window.
         */
        void setSize(int width, int height);

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
         */
        void syncRendererSize(bool sync);

        /**
         * Returns whether the renderer is resized when the window is resized.
         */
        bool getSyncRendererSize() const;

        Renderer *getRenderer() const;

    private:
        std::string windowTitle;
        int initialWindowWidth;
        int initialWindowHeight;

        int fps = 60;
        bool showWindow = true;
        bool syncRendererSizeWithWindow = true;

        GLFWwindow *glfwWindow;
        Rendering::Renderer *renderer;

        bool running = false;

        /**
         * Polls events from the window.
         */
        void pollEvents();
    };
}