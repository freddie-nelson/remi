#include "../include/Application.h"
#include "../include/Globals.h"
#include "../include/Rendering/Mesh/Polygons.h"
#include "../include/Rendering/Mesh/Transforms.h"
#include "../include/Rendering/Utility/Timestep.h"

#include <math.h>
#include <random>
#include <time.h>
#include <iostream>
#include <chrono>
#include <thread>

Application::Application(std::string windowTitle, int windowWidth, int windowHeight) : windowTitle(windowTitle), initialWindowWidth(windowWidth), initialWindowHeight(windowHeight)
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

    return window->run(std::bind(&Application::update, this, std::placeholders::_1, std::placeholders::_2));
}

int Application::init()
{
    // init window
    window = new Rendering::Window(windowTitle, initialWindowWidth, initialWindowHeight);
    if (window->init() != 0)
    {
        std::cout << "Failed to initialize window." << std::endl;
        return 1;
    }

    // Rendering::Color clearColor(0.0f);
    // clearColor.fromHSLA(0.82f, 0.6f, 0.45f, 1.0f);

    // auto renderer = window->getRenderer();
    // renderer->setClearColor(clearColor);

    // window->syncRendererSize(false);

    for (int i = 0; i < 100; i++)
    {
        meshs.push_back(Rendering::createRect(rand() % 100 + 50, rand() % 100 + 50));

        Rendering::translateMesh(meshs[i], glm::vec2{(rand() % initialWindowWidth) - initialWindowWidth / 2,
                                                     (rand() % initialWindowHeight) - initialWindowHeight / 2});
    }

    return 0;
}

void Application::destroy()
{
    delete window;
    delete this;
}

void Application::update(float dt, Rendering::Renderer *renderer)
{
    // print timestep info
    std::cout << "\rdt: " << dt << "          ";

    for (auto &m : meshs)
    {
        Rendering::rotateMesh(m, std::numbers::pi * (rand() % 100) * 0.01f * dt);
    }

    render(renderer);
}

void Application::render(Rendering::Renderer *renderer)
{
    auto resolution = window->getSize();

    // Rendering::Mesh2D shape = Rendering::createPolygon({glm::vec2(0, 0), glm::vec2(100, 0), glm::vec2(20, 20), glm::vec2(0, 100)});

    // Rendering::Mesh2D shape = Rendering::createPolygon({glm::vec2(0, 0.5), glm::vec2(0.5, -0.5), glm::vec2(-0.5, -0.5)});

    // Rendering::Mesh2D shape = Rendering::createRect(1, 1);
    // Rendering::translateMesh(shape, glm::vec2(-0.5, -0.5));

    auto now = Rendering::timeSinceEpochMillisec() / 200;

    for (auto &m : meshs)
    {
        const auto r = std::sin(now) / 2.0f + 0.5f;
        const auto g = std::sin(now * 1023 - 897) / 2.0f + 0.5f;
        const auto b = std::sin(now % 3958) / 2.0f + 0.5f;

        renderer->mesh(m, Rendering::Color(r, g, b, 1.0f));
    }
}
