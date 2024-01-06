#include "../../include/Core/Window.h"
#include "../../include/Rendering/Utility/OpenGLHelpers.h"

#include <glad/gl.h>
#include <iostream>
#include <stdexcept>
#include <thread>

Core::Window::Window(std::string windowTitle, unsigned int windowWidth, unsigned int windowHeight, bool fullscreen)
    : windowTitle(windowTitle), initialWindowWidth(windowWidth), initialWindowHeight(windowHeight), isFullscreen(fullscreen)
{
    if (!glfwInit())
    {
        throw std::runtime_error("Failed to initialize GLFW.");
    }
}

Core::Window::~Window()
{
    destroy();
}

void Core::Window::init(unsigned int openglMajorVersion, unsigned int openglMinorVersion)
{
    if (openglMajorVersion < 3)
    {
        throw std::invalid_argument("openglMajorVersion must be greater than or equal to 3.");
    }
    else if (openglMajorVersion == 3 && openglMinorVersion < 3)
    {
        throw std::invalid_argument("openglMinorVersion must be greater than or equal to 3.3.");
    }
    else if (openglMajorVersion == 4 && openglMinorVersion > 6)
    {
        throw std::invalid_argument("openglMinorVersion must be less than or equal to 4.6.");
    }

    GLFWmonitor *monitor = nullptr;
    if (isFullscreen)
    {
        monitor = glfwGetPrimaryMonitor();
    }

    glfwWindow = createGLFWWindow(openglMajorVersion, openglMinorVersion, true, monitor);
    if (!glfwWindow)
    {
        throw std::runtime_error("Failed to create GLFW window.");
    }

    std::cout
        << "Window created." << std::endl;

    // create opengl context
    auto context = createOpenGLContext(glfwWindow);

    // Output the opengl version
    std::cout << "OpenGL version: " << context->versionString << std::endl;
    std::cout << "OpenGL vendor: " << context->vendor << std::endl;
}

void Core::Window::destroy()
{
    glfwDestroyWindow(glfwWindow);
    glfwTerminate();
}

void Core::Window::update(const ECS::Registry &registry, const Core::Timestep &timestep)
{
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

GLFWwindow *Core::Window::getGLFWWindow() const
{
    return glfwWindow;
}

GLFWwindow *Core::Window::createGLFWWindow(int openglMajorVersion, int openglMinorVersion, bool debugContext, GLFWmonitor *monitor)
{
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, openglMajorVersion);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, openglMinorVersion);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_CONTEXT_NO_ERROR, GLFW_TRUE);

    // enable debug context if opengl version is greater than or equal to 4.3
    if (openglMajorVersion >= 4 && openglMinorVersion >= 3)
    {
        glfwWindowHint(GLFW_CONTEXT_NO_ERROR, GLFW_FALSE);
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, debugContext);
    }

    glfwWindow = glfwCreateWindow(initialWindowWidth, initialWindowHeight, windowTitle.c_str(), monitor, NULL);
    if (!glfwWindow)
    {
        const char **error = nullptr;
        glfwGetError(error);
        std::cout << "Failed to create glfw window. Error: " << *error << std::endl;

        glfwTerminate();
        return nullptr;
    }

    return glfwWindow;
}

Core::Window::OpenGLContext *Core::Window::createOpenGLContext(GLFWwindow *window)
{
    glfwMakeContextCurrent(window);

    int version = gladLoadGL(glfwGetProcAddress);
    if (version == 0)
    {
        std::cout << "Failed to initialize OpenGL context." << std::endl;
        return nullptr;
    }

    std::cout << "OpenGL version loaded: " << GLAD_VERSION_MAJOR(version) << "." << GLAD_VERSION_MINOR(version) << std::endl;

    glfwSwapInterval(1);

    int flags;
    glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
    if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
    {
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(Rendering::glDebugOutput, nullptr);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
    }
    else
    {
        std::cout << "No debug context." << std::endl;
    }

    return new OpenGLContext{GLAD_VERSION_MAJOR(version), GLAD_VERSION_MINOR(version), std::string((const char *)glGetString(GL_VERSION)), std::string((const char *)glGetString(GL_VENDOR)), (flags & GL_CONTEXT_FLAG_DEBUG_BIT) != 0};
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
