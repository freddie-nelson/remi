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
#include <blaze++/Rendering/Font/Font.h>
#include <blaze++/Rendering/Font/Text.h>

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
    config.updatesPerSecond = 60;

    engine = new blz::Engine(config);

    // add this system to the engine
    engine->addSystem(this);

    auto window = engine->getWindow();
    auto renderer = engine->getRenderer();
    auto registry = engine->getRegistry();

    // Rendering::Color clearColor(0.0f);
    // clearColor.fromHSLA(0.82f, 0.6f, 0.45f, 1.0f);

    registry->view<Rendering::ActiveCamera>();

    renderer->enableAlphaBlending(true);
    // renderer->setClearColor(clearColor);

    // window->syncRendererSize(false);
    // renderer->syncActiveCameraSize(true);

    // create texture
    Rendering::Texture *texture = new Rendering::Texture("assets/liv piggy.jpg");
    Rendering::Texture *gradient = new Rendering::Texture("assets/gradient.png");

    // create font
    Rendering::Font *font = new Rendering::Font("assets/Roboto-Regular.ttf");

    // create camera
    auto camera = registry->create();
    registry->add(camera, Rendering::Camera(window->getWidth(), window->getHeight()));
    registry->add(camera, Core::Transform());
    registry->add(camera, Rendering::ActiveCamera());

    // create entities
    int entityCount = 100;
    int xRange = (config.windowWidth * std::sqrt(entityCount) / 10);
    int yRange = (config.windowHeight * std::sqrt(entityCount) / 10);
    int zRange = 10;

    for (int i = 0; i < entityCount; i++)
    {
        auto e = registry->create();

        // auto &m = registry->add(e, Rendering::Mesh2D(static_cast<float>(rand() % 100 + 50), static_cast<float>(rand() % 100 + 50)));
        auto &m = registry->add(e, Rendering::Mesh2D(static_cast<float>(rand() % 50 + 25), static_cast<unsigned int>(rand() % 13 + 3)));
        auto &t = registry->add(e, Core::Transform());
        auto &material = registry->add(e, Rendering::Material());
        auto &renderable = registry->add(e, Rendering::Renderable{true, true});

        t.setZIndex(rand() % zRange);
        t.translate(glm::vec2{rand() % xRange - xRange / 2, rand() % yRange - yRange / 2});

        auto r = (rand() % 255) / 255.0f;
        auto g = (rand() % 255) / 255.0f;
        auto b = (rand() % 255) / 255.0f;
        // material.setColor(Rendering::Color((t.getZIndex() + 1) / 10.0f, 0.0f, 0.0f, (t.getZIndex() + 1) / 10.0f));
        material.setColor(Rendering::Color(r, g, b, (t.getZIndex() + 1) / static_cast<float>(zRange)));
        // material.setColor(Rendering::Color((t.getZIndex() + 1) / 10.0f, 0.0f, 0.0f, 1.0f));

        material.setTexture(texture);
    }

    // create text
    auto text = Rendering::Text("Hello World!", *font);

    ECS::Entity textEntity = registry->create();
    registry->add(textEntity, text.mesh(Rendering::Text::TextAlignment::CENTRE));
    registry->add(textEntity, Core::Transform());
    registry->add(textEntity, Rendering::Material(Rendering::Color(1.0f, 1.0f, 1.0f, 1.0f), gradient));
    registry->add(textEntity, Rendering::Renderable{true, true});

    auto &t = registry->get<Core::Transform>(textEntity);
    t.scale(75);
    t.setZIndex(zRange - 1);
}

void Application::destroy()
{
    delete engine;
}

unsigned long long totalFps = 0;
unsigned long long frames = 0;

void Application::update(const ECS::Registry &registry, const Core::Timestep &timestep)
{
    // print timestep info
    auto fps = 1.0f / timestep.getSeconds();
    totalFps += fps;
    frames++;

    auto averageFps = totalFps / frames;

    std::cout << "\rdt: " << timestep.getSeconds() << ", fps: " << 1.0f / timestep.getSeconds() << ", average fps: " << averageFps << "        " << std::endl;

    auto renderer = engine->getRenderer();

    // move camera
    auto keyboard = engine->getKeyboard();

    auto camera = renderer->getActiveCamera(registry);
    auto &t = registry.get<Core::Transform>(camera);

    float camSpeed = 150.0f * (keyboard->isPressed(Input::Key::LEFT_SHIFT) ? 2.0f : 1.0f);
    glm::vec2 camMove(0.0f);

    if (keyboard->isPressed(Input::Key::W))
        camMove.y += camSpeed;
    if (keyboard->isPressed(Input::Key::S))
        camMove.y -= camSpeed;
    if (keyboard->isPressed(Input::Key::A))
        camMove.x -= camSpeed;
    if (keyboard->isPressed(Input::Key::D))
        camMove.x += camSpeed;

    t.translate(camMove * static_cast<float>(timestep.getSeconds()));

    // rotate camera
    float camRotSpeed = 1.0f * (keyboard->isPressed(Input::Key::LEFT_SHIFT) ? 2.0f : 1.0f);

    if (keyboard->isPressed(Input::Key::ARROW_LEFT))
        t.rotate(camRotSpeed * timestep.getSeconds());
    if (keyboard->isPressed(Input::Key::ARROW_RIGHT))
        t.rotate(-camRotSpeed * timestep.getSeconds());

    // zoom camera
    float camZoomSpeed = 1.0f * (keyboard->isPressed(Input::Key::LEFT_SHIFT) ? 2.0f : 1.0f);

    if (keyboard->isPressed(Input::Key::ARROW_UP))
        t.scale(1.0f - camZoomSpeed * timestep.getSeconds());
    if (keyboard->isPressed(Input::Key::ARROW_DOWN))
        t.scale(1.0f + camZoomSpeed * timestep.getSeconds());

    // rotate all entities with a transform component except camera
    // auto entities = registry.view<Core::Transform>();

    // for (auto &e : entities)
    // {
    //     if (e == camera)
    //         continue;

    //     auto &t = registry.get<Core::Transform>(e);
    //     t.rotate(-std::numbers::pi * 0.5f * timestep.getSeconds());
    // }
}