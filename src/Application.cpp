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
    if (window->init(4, 3) != 0)
    {
        std::cout << "Failed to initialize window." << std::endl;
        return 1;
    }

    // Rendering::Color clearColor(0.0f);
    // clearColor.fromHSLA(0.82f, 0.6f, 0.45f, 1.0f);

    // auto renderer = window->getRenderer();
    // renderer->setClearColor(clearColor);

    // window->syncRendererSize(false);

    // meshs
    int meshCount = 0;
    for (int i = 0; i < meshCount; i++)
    {
        meshs.push_back(Rendering::createRect(rand() % 100 + 50, rand() % 100 + 50));

        meshs[i].zIndex = rand() % 10;

        auto r = (rand() % 255) / 255.0f;
        auto g = (rand() % 255) / 255.0f;
        auto b = (rand() % 255) / 255.0f;
        instancedMeshs[i].color.setColor((meshs[i].zIndex + 1) / 10.0f, 0.0f, 0.0f, 1.0f);

        Rendering::translate(meshs[i], glm::vec2{(rand() % initialWindowWidth) - initialWindowWidth / 2,
                                                 (rand() % initialWindowHeight) - initialWindowHeight / 2});
    }

    // instanced meshs
    int instancedMeshCount = 100;
    for (int i = 0; i < instancedMeshCount; i++)
    {
        instancedMeshs.push_back(Rendering::createRect(1, 1));

        instancedMeshs[i].zIndex = rand() % 10;

        auto r = (rand() % 255) / 255.0f;
        auto g = (rand() % 255) / 255.0f;
        auto b = (rand() % 255) / 255.0f;
        instancedMeshs[i].color.setColor((instancedMeshs[i].zIndex + 1) / 10.0f, 0.0f, 0.0f, 1.0f);

        Rendering::translate(instancedMeshs[i], glm::vec2((rand() % initialWindowWidth) - initialWindowWidth / 2,
                                                          (rand() % initialWindowHeight) - initialWindowHeight / 2));

        Rendering::scale(instancedMeshs[i], glm::vec2(rand() % 100 + 50, rand() % 100 + 50));
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

    // move camera
    auto &camera = renderer->getCamera();

    double n = Rendering::timeSinceEpochMillisec() / 1000.0;
    float camX = sin(n) * 200.0f;
    float camY = cos(n) * 200.0f;

    camera.setCentre(glm::vec2{camX, camY});

    // rotate meshs
    for (auto &m : meshs)
    {
        Rendering::rotate(m, std::numbers::pi * (rand() % 100) * 0.01f * dt);
    }

    for (int i = 0; i < instancedMeshs.size(); i++)
    {
        Rendering::rotate(instancedMeshs[i], std::numbers::pi * (rand() % 100) * 0.01f * dt);
    }

    render(renderer);
}

void Application::render(Rendering::Renderer *renderer)
{
    // auto resolution = window->getSize();

    // Rendering::Mesh2D shape = Rendering::createPolygon({glm::vec2(0, 0), glm::vec2(100, 0), glm::vec2(20, 20), glm::vec2(0, 100)});

    // Rendering::Mesh2D shape = Rendering::createPolygon({glm::vec2(0, 0.5), glm::vec2(0.5, -0.5), glm::vec2(-0.5, -0.5)});

    // Rendering::Mesh2D shape = Rendering::createRect(200, 100);
    // renderer->mesh(shape);

    for (auto &m : meshs)
    {
        renderer->mesh(m);
    }

    std::vector<unsigned int> zIndices(instancedMeshs.size());
    std::vector<glm::vec2> translations(instancedMeshs.size());
    std::vector<glm::mat2> transforms(instancedMeshs.size());
    std::vector<glm::vec4> colors(instancedMeshs.size());

    int i = 0;
    for (auto &m : instancedMeshs)
    {
        zIndices[i] = m.zIndex;
        translations[i] = m.translation;
        transforms[i] = getTransformationMatrix(m);
        colors[i] = m.color.getColor();

        i++;
    }

    if (instancedMeshs.size() > 0)
        renderer->instancedMesh(instancedMeshs[0], zIndices, translations, transforms, colors);
}
