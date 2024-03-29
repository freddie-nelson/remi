#include "../../include/Core/Window.h"
#include "../../include/Rendering/Utility/OpenGLHelpers.h"
#include "../../include/gl.h"

#include <iostream>
#include <stdexcept>
#include <thread>

Core::Window::Window(std::string windowTitle, unsigned int windowWidth, unsigned int windowHeight, bool fullscreen)
    : windowTitle(windowTitle), initialWindowWidth(windowWidth), initialWindowHeight(windowHeight)
{
    if (!glfwInit())
    {
        throw std::runtime_error("Failed to initialize GLFW.");
    }

    GLFWmonitor *monitor = nullptr;
    if (fullscreen)
    {
        monitor = glfwGetPrimaryMonitor();
    }

    glfwWindow = createGLFWWindow(openglMajorVersion, openglMinorVersion, true, monitor);
    if (!glfwWindow)
    {
        throw std::runtime_error("Failed to create GLFW window.");
    }

    // std::cout << "Window created." << std::endl;

    // create opengl context
    auto context = createOpenGLContext(glfwWindow);

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
    glfwDestroyWindow(glfwWindow);
    glfwTerminate();
}

void Core::Window::swapBuffers()
{
    glfwSwapBuffers(glfwWindow);
}

void Core::Window::pollEvents()
{
    glfwPollEvents();
}

void Core::Window::show()
{
    showWindow = true;

    if (glfwWindow)
        glfwShowWindow(glfwWindow);
}

void Core::Window::hide()
{
    showWindow = false;

    if (glfwWindow)
        glfwHideWindow(glfwWindow);
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
    glfwGetFramebufferSize(glfwWindow, &width, &height);

    return width;
}

void Core::Window::setWidth(unsigned int width)
{
    if (width <= 0)
    {
        throw std::invalid_argument("width must be greater than 0.");
    }

    glfwSetWindowSize(glfwWindow, width, getHeight());
}

unsigned int Core::Window::getHeight() const
{
    int width, height;
    glfwGetFramebufferSize(glfwWindow, &width, &height);

    return height;
}

void Core::Window::setHeight(unsigned int height)
{
    if (height <= 0)
    {
        throw std::invalid_argument("height must be greater than 0.");
    }

    glfwSetWindowSize(glfwWindow, getWidth(), height);
}

glm::ivec2 Core::Window::getPosition() const
{
    int x, y;
    glfwGetWindowPos(glfwWindow, &x, &y);

    return glm::ivec2(x, y);
}

void Core::Window::setPosition(int x, int y)
{
    if (x < 0 || y < 0)
    {
        throw std::invalid_argument("x and y must be greater than or equal to 0.");
    }

    glfwSetWindowPos(glfwWindow, x, y);
}

void Core::Window::toggleFullscreen(bool fullscreen)
{
    if (fullscreen)
    {
        glfwSetWindowMonitor(glfwWindow, glfwGetPrimaryMonitor(), 0, 0, initialWindowWidth, initialWindowHeight, GLFW_DONT_CARE);
    }
    else
    {
        glfwSetWindowMonitor(glfwWindow, nullptr, 0, 0, initialWindowWidth, initialWindowHeight, GLFW_DONT_CARE);
    }
}

bool Core::Window::isFullscreen() const
{
    return glfwGetWindowMonitor(glfwWindow) != nullptr;
}

void Core::Window::toggleResizeable(bool resizeable)
{
    glfwSetWindowAttrib(glfwWindow, GLFW_RESIZABLE, resizeable);
}

bool Core::Window::isResizeable() const
{
    return glfwGetWindowAttrib(glfwWindow, GLFW_RESIZABLE);
}

void Core::Window::toggleVsync(bool enable)
{
    glfwSwapInterval(enable ? 1 : 0);
}

GLFWwindow *Core::Window::getGLFWWindow() const
{
    return glfwWindow;
}

bool Core::Window::getWindowShouldClose() const
{
    return glfwWindowShouldClose(glfwWindow);
}

bool Core::Window::isMinimized() const
{
    return glfwGetWindowAttrib(glfwWindow, GLFW_ICONIFIED);
}

bool Core::Window::isMaximized() const
{
    return glfwGetWindowAttrib(glfwWindow, GLFW_MAXIMIZED);
}

GLFWwindow *Core::Window::createGLFWWindow(int openglMajorVersion, int openglMinorVersion, bool debugContext, GLFWmonitor *monitor)
{
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, openglMajorVersion);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, openglMinorVersion);
    glfwWindowHint(GLFW_CONTEXT_NO_ERROR, GLFW_TRUE);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_ANY_PROFILE);

    // enable debug context if opengl version is greater than or equal to 4.3
    if (openglMajorVersion >= 4 && openglMinorVersion >= 3)
    {
        glfwWindowHint(GLFW_CONTEXT_NO_ERROR, GLFW_FALSE);
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, debugContext);
    }

    glfwWindow = glfwCreateWindow(initialWindowWidth, initialWindowHeight, windowTitle.c_str(), monitor, NULL);
    if (!glfwWindow)
    {
#ifdef __EMSCRIPTEN__
        const char *msg = "Cannot get exact error on wasm.";
        const char **error = &msg;
#else
        const char **error = nullptr;
        glfwGetError(error);
#endif

        std::cout << "Failed to create glfw window. Error: " << *error << std::endl;

        glfwTerminate();
        return nullptr;
    }

    return glfwWindow;
}

Core::Window::OpenGLContext *Core::Window::createOpenGLContext(GLFWwindow *window)
{
    glfwMakeContextCurrent(window);

#ifndef __EMSCRIPTEN__
    int version = gladLoadGLES2Loader((GLADloadproc)glfwGetProcAddress);
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

std::vector<std::pair<Core::Window::OpenGLContext *, GLFWmonitor *>> Core::Window::getAllSupportedOpenGLContexts(int openglMajorVersion, int openglMinorVersion, bool debugContext)
{
    std::vector<std::pair<OpenGLContext *, GLFWmonitor *>> contexts;

    int monitorCount;
    auto monitors = glfwGetMonitors(&monitorCount);

    for (int i = 0; i < monitorCount; i++)
    {
        auto m = monitors[i];

        auto window = createGLFWWindow(openglMajorVersion, openglMinorVersion, debugContext, m);
        if (!window)
        {
            continue;
        }

        auto context = createOpenGLContext(window);
        if (!context)
        {
            continue;
        }

        glfwDestroyWindow(window);

        contexts.push_back(std::make_pair(context, m));
    }

    return contexts;
}
