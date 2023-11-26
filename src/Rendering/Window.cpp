#include "../../include/Rendering/Window.h"
#include "../../include/Rendering/Utility/Timestep.h"
#include "../../include/Rendering/Utility/OpenGLHelpers.h"

#include "../../include/externals/glad/gl.h"
#include <iostream>
#include <stdexcept>
#include <thread>

Rendering::Window::Window(std::string windowTitle, int windowWidth, int windowHeight)
    : windowTitle(windowTitle), initialWindowWidth(windowWidth), initialWindowHeight(windowHeight)
{
    if (!glfwInit())
    {
        throw std::runtime_error("Failed to initialize GLFW.");
    }
}

Rendering::Window::~Window()
{
    destroy();
}

int Rendering::Window::init(unsigned int openglMajorVersion, unsigned int openglMinorVersion)
{
    if (openglMajorVersion < 3)
    {
        std::cout << "OpenGL major version must be greater than or equal to 3." << std::endl;
        return 1;
    }
    else if (openglMajorVersion == 3 && openglMinorVersion < 3)
    {
        std::cout << "OpenGL minor version must be greater than or equal to 3.3." << std::endl;
        return 1;
    }
    else if (openglMajorVersion == 4 && openglMinorVersion > 6)
    {
        std::cout << "OpenGL minor version must be less than or equal to 4.6." << std::endl;
        return 1;
    }

    glfwWindow = createGLFWWindow(openglMajorVersion, openglMinorVersion, true);
    if (!glfwWindow)
    {
        return 1;
    }

    std::cout
        << "Window created." << std::endl;

    // create opengl context
    auto context = createOpenGLContext(glfwWindow);

    // Output the opengl version
    std::cout << "OpenGL version: " << context->versionString << std::endl;
    std::cout << "OpenGL vendor: " << context->vendor << std::endl;

    renderer = new Rendering::Renderer(glfwWindow, initialWindowWidth, initialWindowHeight);
    renderer->init();

    return 0;
}

void Rendering::Window::destroy()
{
    running = false;

    delete renderer;

    glfwDestroyWindow(glfwWindow);
    glfwTerminate();
}

int Rendering::Window::run(WindowFrameCallback frameCallback)
{
    // TODO change dt to timestep struct or class
    const int desiredFrameTime = getFrameTime();
    auto lastUpdateTime = timeSinceEpochMillisec() - desiredFrameTime;

    running = true;

    while (true)
    {
        if (syncRendererSizeWithWindow)
        {
            auto size = getSize();
            renderer->resize(size.first, size.second);
        }

        // when not running, just poll events
        // this is to prevent the window from freezing
        if (!running)
        {
            pollEvents();
            continue;
        }

        // update timestep
        const auto now = timeSinceEpochMillisec();
        const auto diff = now - lastUpdateTime;
        lastUpdateTime = now;

        // calculate dt in seconds
        const float dt = (float)diff / 1000.0f;

        // clear renderer
        renderer->clear();

        // call user frame callback
        frameCallback(dt, renderer);

        // wait for renderer events to be processed
        pollEvents();

        // present rendered image
        renderer->present();

        // wait until frame time is reached
        const auto frameEndTime = now + desiredFrameTime;
        while (timeSinceEpochMillisec() < frameEndTime)
        {
            std::this_thread::sleep_for(std::chrono::microseconds(100));
        }
    }

    return 0;
}

void Rendering::Window::stop()
{
    running = false;
}

void Rendering::Window::show()
{
    showWindow = true;

    if (glfwWindow)
        glfwShowWindow(glfwWindow);
}

void Rendering::Window::hide()
{
    showWindow = false;

    if (glfwWindow)
        glfwHideWindow(glfwWindow);
}

int Rendering::Window::getFps() const
{
    return fps;
}

void Rendering::Window::setFps(int fps)
{
    if (fps <= 0)
    {
        throw std::invalid_argument("fps must be greater than 0");
    }

    this->fps = fps;
}

int Rendering::Window::getFrameTime() const
{
    return 1000 / fps;
}

std::pair<int, int> Rendering::Window::getSize() const
{
    int width, height;
    glfwGetFramebufferSize(glfwWindow, &width, &height);

    return std::make_pair(width, height);
}

void Rendering::Window::setSize(int width, int height)
{
    if (width <= 0 || height <= 0)
    {
        throw std::invalid_argument("width and height must be greater than 0.");
    }

    glfwSetWindowSize(glfwWindow, width, height);
}

std::pair<int, int> Rendering::Window::getPosition() const
{
    int x, y;
    glfwGetWindowPos(glfwWindow, &x, &y);

    return std::make_pair(x, y);
}

void Rendering::Window::setPosition(int x, int y)
{
    if (x < 0 || y < 0)
    {
        throw std::invalid_argument("x and y must be greater than or equal to 0.");
    }

    glfwSetWindowPos(glfwWindow, x, y);
}

void Rendering::Window::syncRendererSize(bool sync)
{
    syncRendererSizeWithWindow = sync;
}

bool Rendering::Window::getSyncRendererSize() const
{
    return syncRendererSizeWithWindow;
}

Rendering::Renderer *Rendering::Window::getRenderer() const
{
    return renderer;
}

GLFWwindow *Rendering::Window::createGLFWWindow(int openglMajorVersion, int openglMinorVersion, bool debugContext, GLFWmonitor *monitor)
{
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, openglMajorVersion);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, openglMinorVersion);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_CONTEXT_NO_ERROR, GLFW_FALSE);

    // enable debug context if opengl version is greater than or equal to 4.3
    if (openglMajorVersion >= 4 && openglMinorVersion >= 3)
    {
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, debugContext);
    }

    glfwWindow = glfwCreateWindow(initialWindowWidth, initialWindowHeight, windowTitle.c_str(), monitor, NULL);
    if (!glfwWindow)
    {
        const char **error;
        glfwGetError(error);
        std::cout << "Failed to create glfw window. Error: " << *error << std::endl;

        glfwTerminate();
        return nullptr;
    }

    return glfwWindow;
}

Rendering::Window::OpenGLContext *Rendering::Window::createOpenGLContext(GLFWwindow *window)
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
        glDebugMessageCallback(glDebugOutput, nullptr);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
    }
    else
    {
        std::cout << "No debug context." << std::endl;
    }

    return new OpenGLContext{GLAD_VERSION_MAJOR(version), GLAD_VERSION_MINOR(version), std::string((const char *)glGetString(GL_VERSION)), std::string((const char *)glGetString(GL_VENDOR)), (flags & GL_CONTEXT_FLAG_DEBUG_BIT) != 0};
}

std::vector<std::pair<Rendering::Window::OpenGLContext *, GLFWmonitor *>> Rendering::Window::getAllSupportedOpenGLContexts(int openglMajorVersion, int openglMinorVersion, bool debugContext)
{
    std::vector<std::pair<OpenGLContext *, GLFWmonitor *>> contexts;

    int monitorCount;
    auto monitors = glfwGetMonitors(&monitorCount);

    for (size_t i = 0; i < monitorCount; i++)
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

void Rendering::Window::pollEvents()
{
    glfwPollEvents();

    if (glfwWindowShouldClose(glfwWindow))
    {
        destroy();
    }
}
