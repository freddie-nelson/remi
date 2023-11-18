#pragma once

#include "Renderer.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <string>
#include <functional>

namespace Rendering
{
    using WindowFrameCallback = std::function<void(float, Renderer *)>;

    class Window
    {
    public:
        /**
         * Creates a new window with the given title and dimensions.
         */
        Window(std::string windowTitle, int windowWidth, int windowHeight);
        ~Window();

        /**
         * Initializes the window.
         *
         * @return The error code, 0 if no error.
         */
        int init();

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
         */
        void stop();

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

    private:
        std::string windowTitle;
        int windowWidth;
        int windowHeight;

        int fps = 60;

        GLFWwindow *glfwWindow;
        Rendering::Renderer *renderer;

        bool running = false;

        /**
         * Polls events from the window.
         */
        void pollEvents();
    };
}