#include "../include/Application.h"
#include "../include/Rendering/Renderer.h"
#include "../include/Globals.h"
#include "../include/Utility/Timestep.h"

#include <math.h>
#include <random>
#include <time.h>
#include <iostream>
#include <chrono>
#include <thread>

Application::Application(std::string windowTitle, int windowWidth, int windowHeight) : windowTitle(windowTitle), windowWidth(windowWidth), windowHeight(windowHeight)
{
}

Application::~Application()
{
    destroy();
}

int Application::run()
{
    const int initCode = init();
    if (initCode != 0)
    {
        return initCode;
    }

    const int desiredFps = 60;
    const int desiredFrameTime = 1000 / desiredFps;

    auto lastUpdateTime = timeSinceEpochMillisec() - desiredFrameTime;

    while (state == ApplicationState::RUNNING)
    {
        // update timestep
        const auto now = timeSinceEpochMillisec();
        const auto diff = now - lastUpdateTime;
        lastUpdateTime = now;

        // dt in seconds
        const float dt = (float)diff / 1000.0f;

        // wait for renderer events to be processed
        const bool shouldExit = renderer->pollEvents();
        if (shouldExit)
        {
            state = ApplicationState::EXIT;
            break;
        }

        // clear renderer first so that update functions can draw to the screen
        // renderer->clear();

        update(dt);
        render();

        // wait until frame time is reached
        const auto frameEndTime = now + desiredFrameTime;
        while (timeSinceEpochMillisec() < frameEndTime)
        {
            std::this_thread::sleep_for(std::chrono::microseconds(500));
        }
    }

    return 0;
}

int Application::init()
{
    // init renderer
    renderer = new Rendering::Renderer(windowTitle, windowWidth, windowHeight);
    if (renderer->init() != 0)
    {
        std::cout << "Failed to initialize renderer." << std::endl;
        return 1;
    }

    return 0;
}

void Application::destroy()
{
    delete renderer;
    delete this;
}

void Application::update(float dt)
{
    // print timestep info
    std::cout << "\rdt: " << dt << "          ";
}

void Application::render(bool clear)
{
    // clear last frame
    if (clear)
        renderer->clear();

    // draw a circle
    renderer->circle(Rendering::Circle{glm::vec2(windowWidth / 2, windowHeight / 2), 100}, Rendering::Color{
                                                                                               255, 255, 255, 255});

    // render
    renderer->present();
}
