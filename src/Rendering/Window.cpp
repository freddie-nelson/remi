#include "../../include/Rendering/Window.h"
#include "../../include/Rendering/Utility/Timestep.h"

#include "../../include/externals/glad/glad.h"
#include <iostream>
#include <stdexcept>
#include <thread>

Rendering::Window::Window(std::string windowTitle, int windowWidth, int windowHeight)
    : windowTitle(windowTitle), windowWidth(windowWidth), windowHeight(windowHeight)
{
}

Rendering::Window::~Window()
{
    destroy();
}

int Rendering::Window::init()
{
    // init sdl
    if (!glfwInit())
    {
        std::cout << "Failed to initialize GLFW." << std::endl;
        return 1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);

    glfwWindow = glfwCreateWindow(windowWidth, windowHeight, windowTitle.c_str(), NULL, NULL);
    if (!glfwWindow)
    {
        glfwTerminate();
        std::cout << "Failed to create glfw window." << std::endl;
        return 1;
    }

    glfwMakeContextCurrent(glfwWindow);
    gladLoadGL();
    glfwSwapInterval(1);

    renderer = new Rendering::Renderer(glfwWindow, windowWidth, windowHeight);
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
    const int desiredFrameTime = getFrameTime();
    auto lastUpdateTime = timeSinceEpochMillisec() - desiredFrameTime;

    running = true;

    while (running)
    {
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

        // render
        renderer->present();

        // wait for renderer events to be processed
        pollEvents();

        // wait until frame time is reached
        const auto frameEndTime = now + desiredFrameTime;
        while (timeSinceEpochMillisec() < frameEndTime)
        {
            std::this_thread::sleep_for(std::chrono::microseconds(500));
        }
    }

    return 0;
}

void Rendering::Window::stop()
{
    running = false;
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

void Rendering::Window::pollEvents()
{
    glfwPollEvents();

    if (glfwWindowShouldClose(glfwWindow))
    {
        destroy();
    }
}
