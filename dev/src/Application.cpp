#include "./include/Application.h"
#include "./include/Globals.h"

#include <blaze++/Rendering/Mesh/Polygons.h>
#include <blaze++/Rendering/Material/Material.h>
#include <blaze++/Core/Timestep.h>
#include <blaze++/Core/Transform.h>
#include <blaze++/Core/BoundingCircle.h>
#include <blaze++/Rendering/Renderable.h>
#include <blaze++/Rendering/Camera/Camera.h>
#include <blaze++/Rendering/Camera/ActiveCamera.h>

#include <math.h>
#include <random>
#include <time.h>
#include <iostream>
#include <chrono>
#include <thread>

Application::Application()
{
}

Application::~Application()
{
    destroy();
}

void Application::run()
{
    // init application
    init();

    // run engine
    engine->run();
}

void Application::init()
{
    // init engine
    blz::EngineConfig config;
    config.openglMajorVersion = 4;
    config.openglMinorVersion = 3;
    config.updatesPerSecond = 60;

    engine = new blz::Engine(config);

    // add this system to the engine
    engine->addSystem(this);

    auto window = engine->getWindow();
    auto renderer = engine->getRenderer();
    auto registry = engine->getRegistry();

    // Rendering::Color clearColor(0.0f);
    // clearColor.fromHSLA(0.82f, 0.6f, 0.45f, 1.0f);

    renderer->enableAlphaBlending(true);
    // renderer->setClearColor(clearColor);

    // window->syncRendererSize(false);
    // renderer->syncActiveCameraSize(true);

    // create camera
    auto camera = registry->create();
    registry->add(camera, Rendering::Camera(window->getWidth(), window->getHeight()));
    registry->add(camera, Core::Transform());
    registry->add(camera, Rendering::ActiveCamera());

    // create texture
    Rendering::Texture *texture = new Rendering::Texture("assets/liv piggy.jpg");

    // create entities
    int entityCount = 1000;
    int xRange = (config.windowWidth * std::sqrt(entityCount) / 10);
    int yRange = (config.windowHeight * std::sqrt(entityCount) / 10);

    for (int i = 0; i < entityCount; i++)
    {
        auto e = registry->create();

        // auto &m = registry->add(e, Rendering::Mesh2D(static_cast<float>(rand() % 100 + 50), static_cast<float>(rand() % 100 + 50)));
        auto &m = registry->add(e, Rendering::Mesh2D(static_cast<float>(rand() % 50 + 25), static_cast<unsigned int>(rand() % 13 + 3)));
        auto &t = registry->add(e, Core::Transform());
        auto &material = registry->add(e, Rendering::Material());
        auto &renderable = registry->add(e, Rendering::Renderable{true, true});

        t.setZIndex(rand() % 10);
        t.translate(glm::vec2{rand() % xRange - xRange / 2, rand() % yRange - yRange / 2});

        // auto r = (rand() % 255) / 255.0f;
        // auto g = (rand() % 255) / 255.0f;
        // auto b = (rand() % 255) / 255.0f;
        material.setColor(Rendering::Color((t.getZIndex() + 1) / 10.0f, 0.0f, 0.0f, (t.getZIndex() + 1) / 10.0f));
        // material.setColor(Rendering::Color((t.getZIndex() + 1) / 10.0f, 0.0f, 0.0f, 1.0f));

        material.setTexture(texture);
    }
}

void Application::destroy()
{
    delete engine;
}

void Application::update(const ECS::Registry &registry, const Core::Timestep &timestep)
{
    // print timestep info
    std::cout << "\rdt: " << timestep.getSeconds() << "          ";

    auto renderer = engine->getRenderer();

    // move camera
    auto camera = renderer->getActiveCamera(registry);

    // double n = Rendering::timeSinceEpochMillisec() / 1000.0;
    // float camX = sin(n) * 200.0f;
    // float camY = cos(n) * 200.0f;

    // camera.setCentre(glm::vec2{camX, camY});
    // camera.rotate(std::numbers::pi * 0.5f * dt);

    // rotate all entities with a transform component except camera
    auto entities = registry.view<Core::Transform>();

    for (auto &e : entities)
    {
        if (e == camera)
            continue;

        auto &t = registry.get<Core::Transform>(e);
        t.rotate(-std::numbers::pi * 0.5f * timestep.getSeconds());
    }
}