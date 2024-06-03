#pragma once

#include "Timestep.h"
#include "Subject.h"

#include <SDL2/SDL.h>

#include <glm/vec2.hpp>

#include <string>
#include <functional>

namespace Core
{
    /**
     * The type of the window.
     *
     * This determines wether the window is in windowed, fullscreen or windowed fullscreen mode.
     */
    enum WindowType
    {
        WINDOWED,
        FULLSCREEN,
        WINDOWED_FULLSCREEN
    };

    const std::string WindowPollEventName = "poll";

    /**
     * The window is responsible for creating and managing the window.
     *
     * The window must be initialized before it can be used.
     *
     * @note By default the window is shown.
     *
     * @note By default the window is resizable.
     */
    class Window : public Subject<const std::vector<SDL_Event> &>
    {
    public:
        /**
         * Creates a new window with the given title and dimensions.
         *
         * Initializes SDL2.
         *
         * @param windowTitle The title of the window.
         * @param windowWidth The width of the window.
         * @param windowHeight The height of the window.
         * @param type The type of window to create.
         */
        Window(std::string windowTitle, unsigned int windowWidth, unsigned int windowHeight, WindowType type = WINDOWED);

        /**
         * Destroys the window.
         */
        virtual ~Window();

        /**
         * Updates the window
         *
         * @param timestep The timestep since the last update.
         */
        void update(const Core::Timestep &timestep);

        /**
         * Destroys the window, freeing all resources and closing the window.
         */
        void destroy();

        /**
         * Swaps the front and back buffers of the window.
         *
         * This will display the rendered image to the window.
         *
         * This should be called after rendering to the window.
         */
        void swapBuffers();

        /**
         * Polls for window events.
         *
         * @returns The events that were polled.
         */
        const std::vector<SDL_Event> &pollEvents();

        /**
         * Returns the events that were last polled.
         *
         * @returns The events that were last polled.
         */
        const std::vector<SDL_Event> &getEvents() const;

        /**
         * Shows the window.
         */
        void show();

        /**
         * Hides the window.
         */
        void hide();

        /**
         * Gets whether the window is shown.
         *
         * @returns Whether the window is shown.
         */
        bool isShown() const;

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
         * Sets the width and height of the window.
         *
         * @param size The width and height of the window.
         */
        void setSize(const glm::uvec2 &size);

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
         * Gets the type of the window.
         *
         * @returns The type of the window.
         */
        WindowType getType() const;

        /**
         * Sets the type of the window.
         *
         * @param type The type of the window.
         */
        void setType(WindowType type);

        /**
         * Toggles whether the window is resizable.
         *
         * @param resizeable Whether to enable or disable window resizing.
         */
        void toggleResizeable(bool resizeable);

        /**
         * Returns whether the window is resizable.
         *
         * @returns Whether the window is resizable.
         */
        bool isResizeable() const;

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
         * Returns the internal window.
         *
         * @warning Only use this if you know what you are doing.
         *
         * @returns The internal window.
         */
        SDL_Window *getInternalWindow();

        /**
         * Gets whether or not the window should close.
         *
         * @returns Whether or not the window should close.
         */
        bool getWindowShouldClose() const;

        /**
         * Returns whether the window is minimized.
         *
         * @returns Whether the window is minimized.
         */
        bool isMinimized() const;

        /**
         * Returns whether the window is maximized.
         *
         * @returns Whether the window is maximized.
         */
        bool isMaximized() const;

    private:
        std::string windowTitle;
        unsigned int initialWindowWidth;
        unsigned int initialWindowHeight;
        WindowType type;

        std::vector<SDL_Event> events;

        /**
         * OpenGL ES 3.0 for webgl 2.0 matching.
         */

        const unsigned int openglMajorVersion = 3;
        const unsigned int openglMinorVersion = 0;

        bool showWindow = true;
        bool resizeable = true;

        SDL_Window *internalWindow = nullptr;

        /**
         * Creates a window.
         *
         * @param openglMajorVersion The major version of OpenGL to use.
         * @param openglMinorVersion The minor version of OpenGL to use.
         * @param debugContext Whether to create a debug context on opengl versions >= 4.3.
         * @param type The type of window to create.
         */
        SDL_Window *createWindow(int openglMajorVersion, int openglMinorVersion, bool debugContext, WindowType type);

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
        OpenGLContext *createOpenGLContext(SDL_Window *window);
    };
}