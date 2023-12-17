#include "../include/Application.h"
#include "../include/Globals.h"
#include "../include/Rendering/Mesh/Polygons.h"
#include "../include/Rendering/Color.h"
#include "../include/Rendering/Utility/Timestep.h"
#include "../include/Core/Transform.h"

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

struct Position
{
    float x;
    float y;
};

struct Velocity
{
    float x;
    float y;
};

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

    // entity count
    int entityCount = 2000;
    for (int i = 0; i < entityCount; i++)
    {
        auto e = registry.create();
        auto &m = registry.add(e, Rendering::Mesh2D(static_cast<float>(rand() % 100 + 50), static_cast<float>(rand() % 100 + 50)));
        auto &t = registry.add(e, Core::Transform());
        auto &c = registry.add(e, Rendering::Color());

        t.setZIndex(rand() % 10);
        t.translate(glm::vec2{(rand() % initialWindowWidth) - initialWindowWidth / 2,
                              (rand() % initialWindowHeight) - initialWindowHeight / 2});

        auto r = (rand() % 255) / 255.0f;
        auto g = (rand() % 255) / 255.0f;
        auto b = (rand() % 255) / 255.0f;
        c.setColor((t.getZIndex() + 1) / 10.0f, 0.0f, 0.0f, 1.0f);
    }

    // ECS test
    // auto e0 = registry.create();
    // registry.add<Position>(e0, {0.0f, 0.0f});
    // registry.add<Velocity>(e0, {1.0f, 1.0f});

    // auto e1 = registry.create();
    // registry.add<Position>(e1, {1.0f, 1.0f});

    // auto e2 = registry.create();
    // registry.add<Position>(e2, {2.0f, 2.0f});
    // registry.add<Velocity>(e2, {2.0f, 2.0f});

    // std::cout << "Entities with position: " << std::endl;
    // auto entities = registry.view<Position>();
    // for (auto &e : entities)
    // {
    //     auto &p = registry.get<Position>(e);
    //     std::cout << e << ": p: " << p.x << ", " << p.y << std::endl;
    // }

    // std::cout << "Entities with velocity: " << std::endl;
    // entities = registry.view<Velocity>();
    // for (auto &e : entities)
    // {
    //     auto &v = registry.get<Velocity>(e);
    //     std::cout << e << ": v: " << v.x << ", " << v.y << std::endl;
    // }

    // std::cout << "Entities with position and velocity: " << std::endl;
    // entities = registry.view<Position, Velocity>();
    // for (auto &e : entities)
    // {
    //     std::cout << e << std::endl;
    //     auto &p = registry.get<Position>(e);
    //     auto &v = registry.get<Velocity>(e);
    //     std::cout << e << ": p: " << p.x << ", " << p.y << ", v: " << v.x << ", " << v.y << std::endl;
    // }

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
    // camera.rotate(std::numbers::pi * 0.5f * dt);

    // rotate all entities with a transform component
    auto entities = registry.view<Core::Transform>();

    for (auto &e : entities)
    {
        auto &t = registry.get<Core::Transform>(e);
        t.rotate(-std::numbers::pi * 0.5f * dt);
    }

    render(renderer);
}

void Application::render(Rendering::Renderer *renderer)
{
    // auto resolution = window->getSize();

    // render all entities with a mesh, transform and color component
    auto renderables = registry.view<Rendering::Mesh2D, Core::Transform, Rendering::Color>();
    std::vector<Rendering::Mesh2D> meshs(renderables.size());
    std::vector<Core::Transform> transforms(renderables.size());
    std::vector<Rendering::Color> colors(renderables.size());

    for (size_t i = 0; i < renderables.size(); i++)
    {
        auto &m = registry.get<Rendering::Mesh2D>(renderables[i]);
        auto &t = registry.get<Core::Transform>(renderables[i]);
        auto &c = registry.get<Rendering::Color>(renderables[i]);

        meshs[i] = m;
        transforms[i] = t;
        colors[i] = c;
    }

    // single mesh
    // for (size_t i = 0; i < renderables.size(); i++)
    // {
    //     renderer->mesh(meshs[i], transforms[i], colors[i]);
    // }

    // instancing
    // renderer->instancedMesh(meshs[0], transforms, colors);

    // batching
    renderer->batchedMesh(meshs, transforms, colors);
}
