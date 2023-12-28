#include "../include/Application.h"
#include "../include/Globals.h"
#include "../include/Rendering/Mesh/Polygons.h"
#include "../include/Rendering/Material/Material.h"
#include "../include/Rendering/Utility/Timestep.h"
#include "../include/Core/Transform.h"
#include "../include/Core/BoundingCircle.h"
#include "../include/Rendering/Renderable.h"

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

    auto renderer = window->getRenderer();
    renderer->enableAlphaBlending(true);
    // renderer->setClearColor(clearColor);

    // window->syncRendererSize(false);

    // create texture
    Rendering::Texture *texture = new Rendering::Texture("assets/liv piggy.jpg");

    // create entities
    int entityCount = 1000;
    int xRange = (initialWindowWidth * entityCount / 500);
    int yRange = (initialWindowHeight * entityCount / 500);

    for (int i = 0; i < entityCount; i++)
    {
        auto e = registry.create();
        // auto &m = registry.add(e, Rendering::Mesh2D(static_cast<float>(rand() % 100 + 50), static_cast<float>(rand() % 100 + 50)));
        auto &m = registry.add(e, Rendering::Mesh2D(static_cast<float>(rand() % 50 + 25), static_cast<unsigned int>(rand() % 13 + 3)));
        auto &t = registry.add(e, Core::Transform());
        auto &material = registry.add(e, Rendering::Material());
        auto &renderable = registry.add(e, Rendering::Renderable{true, true});

        t.setZIndex(rand() % 10);
        t.translate(glm::vec2{rand() % xRange - xRange / 2, rand() % yRange - yRange / 2});

        auto r = (rand() % 255) / 255.0f;
        auto g = (rand() % 255) / 255.0f;
        auto b = (rand() % 255) / 255.0f;
        // material.setColor(Rendering::Color((t.getZIndex() + 1) / 10.0f, 0.0f, 0.0f, 1.0f));

        material.setTexture(texture);
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

    // camera.setCentre(glm::vec2{camX, camY});
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
    // auto now = Rendering::timeSinceEpochMillisec();
    // auto &renderables = registry.view<Rendering::Mesh2D, Core::Transform, Rendering::Material>();
    // std::cout << "registry search time: " << Rendering::timeSinceEpochMillisec() - now << std::endl;

    // auto &cameraAABB = renderer->getCamera().getAABB();
    // auto cameraBoundingCircle = Core::BoundingCircle(cameraAABB);
    // Core::BoundingCircle entityBoundingCircle(glm::vec2(0), 0);

    // std::vector<Rendering::Mesh2D *> meshs(renderables.size());
    // std::vector<Core::Transform *> transforms(renderables.size());
    // std::vector<Rendering::Material *> materials(renderables.size());

    // now = Rendering::timeSinceEpochMillisec();
    // size_t j = 0;

    // for (size_t i = 0; i < renderables.size(); i++)
    // {
    //     auto &m = registry.get<Rendering::Mesh2D>(renderables[i]);
    //     auto &t = registry.get<Core::Transform>(renderables[i]);

    //     entityBoundingCircle.set(m.getAABB(), t);
    //     if (!cameraBoundingCircle.intersects(entityBoundingCircle))
    //     {
    //         continue;
    //     }

    //     meshs[j] = &m;
    //     transforms[j] = &t;

    //     auto &mat = registry.get<Rendering::Material>(renderables[i]);
    //     materials[j] = &mat;

    //     j++;
    // }

    // meshs.resize(j);
    // transforms.resize(j);
    // materials.resize(j);

    // std::cout << "registry get time: " << Rendering::timeSinceEpochMillisec() - now << std::endl;

    // // single mesh
    // // for (size_t i = 0; i < renderables.size(); i++)
    // // {
    // //     renderer->mesh(meshs[i], transforms[i], materials[i]);
    // // }

    // // instancing
    // // renderer->instancedMesh(meshs[0], transforms, materials);

    // // batching
    // renderer->batchedMesh(meshs, transforms, materials);

    // std::cout << "rendered " << j << " of " << renderables.size() << " entities." << std::endl;

    renderer->update(registry, 0);
}
