#include "../../include/Core/Window.h"
#include "../../include/Rendering/Utility/OpenGLHelpers.h"
#include "../../include/gl.h"

#include <SDL2/SDL_mixer.h>

#include <iostream>
#include <stdexcept>
#include <string>
#include <thread>

Core::Window::Window(std::string windowTitle, unsigned int windowWidth, unsigned int windowHeight, WindowType type)
    : windowTitle(windowTitle), initialWindowWidth(windowWidth), initialWindowHeight(windowHeight)
{
    internalWindow = createWindow(openglMajorVersion, openglMinorVersion, true, type);
    if (!internalWindow)
    {
        throw std::runtime_error("Failed to create SDL2 window.");
    }

    // std::cout << "Window created." << std::endl;

    // create opengl context
    auto context = createOpenGLContext(internalWindow);

    // Output the opengl version
    // std::cout << "OpenGL version: " << context->versionString << std::endl;
    // std::cout << "OpenGL vendor: " << context->vendor << std::endl;
}

Core::Window::~Window()
{
    destroy();
}

void Core::Window::update(const Core::Timestep &timestep)
{
}

void Core::Window::destroy()
{
    SDL_DestroyWindow(internalWindow);
    SDL_Quit();
}

void Core::Window::swapBuffers()
{
    SDL_GL_SwapWindow(internalWindow);
}

const std::vector<SDL_Event> &Core::Window::pollEvents()
{
    events.clear();

    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        events.push_back(std::move(event));
        event = SDL_Event();
    }

    notifyObservers(Core::WindowPollEventName, events);

    return events;
}

const std::vector<SDL_Event> &Core::Window::getEvents() const
{
    return events;
}

void Core::Window::show()
{
    showWindow = true;

    if (internalWindow != nullptr)
    {
        SDL_ShowWindow(internalWindow);
    }
}

void Core::Window::hide()
{
    showWindow = false;

    if (internalWindow != nullptr)
    {
        SDL_HideWindow(internalWindow);
    }
}

bool Core::Window::isShown() const
{
    return showWindow;
}

glm::uvec2 Core::Window::getSize() const
{
    auto width = getWidth();
    auto height = getHeight();

    return glm::uvec2(width, height);
}

void Core::Window::setSize(unsigned int width, unsigned int height)
{
    setWidth(width);
    setHeight(height);
}

void Core::Window::setSize(const glm::uvec2 &size)
{
    setSize(size.x, size.y);
}

unsigned int Core::Window::getWidth() const
{
    int width, height;
    SDL_GetWindowSize(internalWindow, &width, &height);

    return width;
}

void Core::Window::setWidth(unsigned int width)
{
    if (width <= 0)
    {
        throw std::invalid_argument("width must be greater than 0.");
    }

    SDL_SetWindowSize(internalWindow, width, getHeight());
}

unsigned int Core::Window::getHeight() const
{
    int width, height;
    SDL_GetWindowSize(internalWindow, &width, &height);

    return height;
}

void Core::Window::setHeight(unsigned int height)
{
    if (height <= 0)
    {
        throw std::invalid_argument("height must be greater than 0.");
    }

    SDL_SetWindowSize(internalWindow, getWidth(), height);
}

glm::ivec2 Core::Window::getPosition() const
{
    int x, y;
    SDL_GetWindowPosition(internalWindow, &x, &y);

    return glm::ivec2(x, y);
}

void Core::Window::setPosition(int x, int y)
{
    if (x < 0 || y < 0)
    {
        throw std::invalid_argument("x and y must be greater than or equal to 0.");
    }

    SDL_SetWindowPosition(internalWindow, x, y);
}

Core::WindowType Core::Window::getType() const
{
    return type;
}

void Core::Window::setType(WindowType type)
{
    int code = 0;

    if (type == WindowType::FULLSCREEN)
    {
        code = SDL_SetWindowFullscreen(internalWindow, SDL_WINDOW_FULLSCREEN);
    }
    else if (type == WindowType::WINDOWED_FULLSCREEN)
    {
        code = SDL_SetWindowFullscreen(internalWindow, SDL_WINDOW_FULLSCREEN_DESKTOP);
    }
    else if (type == WindowType::WINDOWED)
    {
        code = SDL_SetWindowFullscreen(internalWindow, 0);
    }
    else
    {
        throw std::invalid_argument("Window (setType): WindowType not implemented.");
    }

    if (code < 0)
    {
        std::cout << "Failed to set window type. Error: " << SDL_GetError() << std::endl;
    }
}

void Core::Window::toggleResizeable(bool resizeable)
{
    this->resizeable = resizeable;
    SDL_SetWindowResizable(internalWindow, resizeable ? SDL_TRUE : SDL_FALSE);
}

bool Core::Window::isResizeable() const
{
    return SDL_GetWindowFlags(internalWindow) & SDL_WINDOW_RESIZABLE;
}

void Core::Window::toggleVsync(bool enable)
{
    // SDL_SetHint(SDL_HINT_RENDER_VSYNC, enable ? "1" : "0");

    if (SDL_GL_SetSwapInterval(enable ? 1 : 0) < 0)
    {
        std::cout << "Failed to set vsync. Error: " << SDL_GetError() << std::endl;
    }
}

SDL_Window *Core::Window::getInternalWindow()
{
    return internalWindow;
}

bool Core::Window::getWindowShouldClose() const
{
    for (const auto &event : events)
    {
        if (event.type == SDL_QUIT)
        {
            return true;
        }
    }

    return false;
}

bool Core::Window::isMinimized() const
{
    return SDL_GetWindowFlags(internalWindow) & SDL_WINDOW_MINIMIZED;
}

bool Core::Window::isMaximized() const
{
    return SDL_GetWindowFlags(internalWindow) & SDL_WINDOW_MAXIMIZED;
}

SDL_Window *Core::Window::createWindow(int openglMajorVersion, int openglMinorVersion, bool debugContext, WindowType type)
{
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, openglMajorVersion);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, openglMinorVersion);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_NO_ERROR, SDL_TRUE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);

    // enable debug context if opengl version is greater than or equal to 4.3
    if (openglMajorVersion >= 4 && openglMinorVersion >= 3)
    {
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_NO_ERROR, SDL_FALSE);

        if (debugContext)
        {
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
        }
    }

    internalWindow = SDL_CreateWindow(windowTitle.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, initialWindowWidth, initialWindowHeight, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    if (internalWindow == nullptr)
    {
#ifdef __EMSCRIPTEN__
        const char *msg = "Cannot get exact error on wasm.";
        const char **error = &msg;
#else
        const char *error = SDL_GetError();
#endif

        std::cout << "Failed to create sdl2 window. Error: " << error << std::endl;

        return nullptr;
    }

    setType(type);

    if (!showWindow)
    {
        hide();
    }

    if (isResizeable())
    {
        toggleResizeable(false);
    }

    return internalWindow;
}

Core::Window::OpenGLContext *Core::Window::createOpenGLContext(SDL_Window *window)
{
    SDL_GLContext context = SDL_GL_CreateContext(window);
    if (context == nullptr)
    {
        std::cout << "Failed to create OpenGL context. Error: " << SDL_GetError() << std::endl;
        return nullptr;
    }

#ifndef __EMSCRIPTEN__
    int version = gladLoadGLES2Loader((GLADloadproc)SDL_GL_GetProcAddress);
    if (version == 0)
    {
        std::cout << "Failed to initialize OpenGL context." << std::endl;
        return nullptr;
    }

    // std::cout << "OpenGL version loaded: " << GLAD_VERSION_MAJOR(version) << "." << GLAD_VERSION_MINOR(version) << std::endl;
#endif

    toggleVsync(false);

    // int flags;
    // glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
    // if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
    // {
    //     glEnable(GL_DEBUG_OUTPUT);
    //     glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    //     glDebugMessageCallback(Rendering::glDebugOutput, nullptr);
    //     glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
    // }
    // else
    // {
    //     std::cout << "No debug context." << std::endl;
    // }

    int major;
    glGetIntegerv(GL_MAJOR_VERSION, &major);

    int minor;
    glGetIntegerv(GL_MINOR_VERSION, &minor);

    return new OpenGLContext{major, minor, std::string((const char *)glGetString(GL_VERSION)), std::string((const char *)glGetString(GL_VENDOR)), false};
}