#include "../include/Application.h"
#include "../include/Globals.h"
#include "../include/Rendering/Mesh/Polygons.h"
#include "../include/Rendering/Mesh/Transforms.h"

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

    return window->run(std::bind(&Application::update, this, std::placeholders::_1, std::placeholders::_2));
}

int Application::init()
{
    // init window
    window = new Rendering::Window(windowTitle, windowWidth, windowHeight);
    if (window->init() != 0)
    {
        std::cout << "Failed to initialize window." << std::endl;
        return 1;
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

    render(renderer);
}

void Application::render(Rendering::Renderer *renderer)
{
    Rendering::Mesh2D shape = Rendering::createRegularPolygon(100, 50);
    Rendering::translateMesh(shape, glm::vec2{windowWidth / 2, windowHeight / 2});

    renderer->mesh(shape, Rendering::Color(255, 255, 255, 255));
}
