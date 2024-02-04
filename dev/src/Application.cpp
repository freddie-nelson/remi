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
#include <blaze++/Rendering/Font/MemoizedText.h>
#include <blaze++/Rendering/Material/ShaderMaterial.h>
#include <blaze++/Rendering/Shader/Uniform.h>
#include <blaze++/Core/Timestep.h>
#include <blaze++/Rendering/Passes/ColorBlendPass.h>
#include <blaze++/Rendering/Passes/GaussianBlurPass.h>
#include <blaze++/Rendering/Passes/BrightnessPass.h>
#include <blaze++/Rendering/Passes/PosterizePass.h>
#include <blaze++/Rendering/Texture/AnimatedTexture.h>

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

Rendering::ColorBlendPass *colorBlendPass;
Rendering::GaussianBlurPass *blurPass;
Rendering::BrightnessPass *brightnessPass;
Rendering::PosterizePass *posterizePass;

Rendering::Font *font;

struct FPS
{
    unsigned int fps;
};

// ! Implement some kind of texture/asset loading system at engine level
// would avoid user having to manage memory etc for textures and maybe other things

void Application::init()
{
    // init engine
    blz::EngineConfig config;
    config.updatesPerSecond = 100000;

    engine = new blz::Engine(config);

    // add this system to the engine
    engine->addSystem(this);

    auto window = engine->getWindow();
    auto renderer = engine->getRenderer();
    auto registry = engine->getRegistry();
    auto pipeline = engine->getPipeline();

    colorBlendPass = new Rendering::ColorBlendPass(Rendering::Color(1.0f, 0.0f, 1.0f, 1.0f));
    blurPass = new Rendering::GaussianBlurPass();
    brightnessPass = new Rendering::BrightnessPass(0.5f);
    posterizePass = new Rendering::PosterizePass(8.0f);
    // pipeline->add(colorBlendPass, 4500);

    // renderer->setProjectionMode(Rendering::RendererProjectionMode::MATCH);

    // Rendering::Color clearColor(0.0f);
    // clearColor.fromHSLA(0.82f, 0.6f, 0.45f, 1.0f);

    registry->view<Rendering::ActiveCamera>();

    renderer->enableAlphaBlending(true);

    // create texture
    Rendering::Texture *texture = new Rendering::Texture("assets/liv.jpg");
    Rendering::Texture *gradient = new Rendering::Texture("assets/gradient.png");

    // create font
    font = new Rendering::Font("assets/Roboto-Regular.ttf");

    // create camera
    auto camera = registry->create();
    registry->add(camera, Rendering::Camera(window->getWidth(), window->getHeight()));
    registry->add(camera, Core::Transform());
    registry->add(camera, Rendering::ActiveCamera());

    // create entities
    float pixelsPerMeter = config.pixelsPerMeter;

    int entityCount = 1000;
    int xRange = (config.windowWidth * std::sqrt(entityCount) / 10);
    int yRange = (config.windowHeight * std::sqrt(entityCount) / 10);
    int zRange = 10;

    for (int i = 0; i < entityCount; i++)
    {
        auto e = registry->create();

        auto &m = registry->add(e, Rendering::Mesh2D(static_cast<float>((rand() % 50) / 100.0f + 0.2f), static_cast<unsigned int>(rand() % 13 + 3)));
        auto &t = registry->add(e, Core::Transform());
        auto &material = registry->add(e, Rendering::Material());
        auto &renderable = registry->add(e, Rendering::Renderable(true, true));

        t.setZIndex(rand() % zRange);
        t.translate(glm::vec2{rand() % xRange - xRange / 2, rand() % yRange - yRange / 2} / pixelsPerMeter);

        auto r = (rand() % 255) / 255.0f;
        auto g = (rand() % 255) / 255.0f;
        auto b = (rand() % 255) / 255.0f;
        // material.setColor(Rendering::Color((t.getZIndex() + 1) / 10.0f, 0.0f, 0.0f, (t.getZIndex() + 1) / 10.0f));
        material.setColor(Rendering::Color(r, g, b, (rand() % zRange) / static_cast<float>(zRange)));
        // material.setColor(Rendering::Color((t.getZIndex() + 1) / 10.0f, 0.0f, 0.0f, 1.0f));

        material.setTexture(texture);
    }

    // create text
    auto text = Rendering::Text("LIV SMELLS", *font);

    ECS::Entity textEntity = registry->create();
    registry->add(textEntity, text.mesh(Rendering::Text::TextAlignment::CENTRE));
    registry->add(textEntity, Core::Transform());
    // registry->add(textEntity, Rendering::Material(Rendering::Color(1.0f, 1.0f, 1.0f, 1.0f), gradient));
    registry->add(textEntity, Rendering::Renderable(true, true));

    const std::string textShader =
        "#version 300 es\n"
        "\n"
        "precision mediump float;\n"
        "\n"
        // "uniform float uTime;\n"
        "uniform float uColorAlpha;\n"
        "\n"
        "uniform sampler2D uTextures[__MAX_TEXTURE_UNITS__];\n"
        "\n"
        "flat in uint vTextureUnit;\n"
        "in vec2 vTexCoord;\n"
        "in vec4 vColor;\n"
        "in vec2 vUv;\n"
        "\n"
        "out vec4 FragColor;\n"
        "\n"
        "__getTexture__"
        "\n"
        "void main()\n"
        "{\n"
        "   FragColor = vec4(1.0f, 1.0f, 1.0f, uColorAlpha) * vColor * getTexture(vTextureUnit, vTexCoord);\n"
        // "   FragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f / (uTime / 30.0f)) * vColor;\n"
        "}\n";

    Rendering::ShaderMaterial m(textShader);
    m.setTexture(gradient);
    m.setTransparency(true);
    registry->add(textEntity, m);

    auto &t = registry->get<Core::Transform>(textEntity);
    t.scale(1);
    t.setZIndex(zRange - 2);

    // animation
    auto character = registry->create();

    auto &cm = registry->add(character, Rendering::Mesh2D(1.0f, 1.0f));
    auto &ct = registry->add(character, Core::Transform());
    auto &cMat = registry->add(character, Rendering::Material());
    registry->add(character, Rendering::Renderable(true, false));

    ct.scale(3);
    ct.setZIndex(zRange + 1);

    std::vector<std::string> frames = {
        "assets/character/run0.png",
        "assets/character/run1.png",
        "assets/character/run2.png",
        "assets/character/run3.png",
        "assets/character/run4.png",
        "assets/character/run5.png",
        "assets/character/run6.png",
        "assets/character/run7.png",
    };

    auto *animTexture = new Rendering::AnimatedTexture(frames, 1000.0f, Rendering::AnimatedTexture::AnimationMode::LOOP);
    cMat.setTexture(animTexture);

    std::cout << "character: " << character << std::endl;

    // fps text
    auto fpsText = Rendering::MemoizedText::text("FPS: 0", font);
    auto fps = registry->create();
    registry->add(fps, FPS{});
    auto &fpsMesh = registry->add(fps, fpsText.mesh(Rendering::Text::TextAlignment::LEFT));
    auto &fpsTransform = registry->add(fps, Core::Transform());
    auto &fpsMaterial = registry->add(fps, Rendering::Material());
    auto &fpsRenderable = registry->add(fps, Rendering::Renderable(true, false));

    fpsTransform.scale(0.25f);
    fpsTransform.setZIndex(Config::MAX_Z_INDEX);
}

void Application::destroy()
{
    delete engine;
}

unsigned long long totalFps = 0;
unsigned long long frames = 0;
unsigned long long averageFps = 0;
float timeSinceStart = 0;
float textAlpha = 1.0f;

void Application::update(const ECS::Registry &registry, const Core::Timestep &timestep)
{
    auto spaceTransformer = engine->getSpaceTransformer();
    auto renderer = engine->getRenderer();
    auto pipeline = engine->getPipeline();

    // print timestep info
    auto fps = 1.0f / timestep.getSeconds();
    totalFps += fps;
    frames++;

    averageFps = totalFps / frames;
    timeSinceStart = static_cast<float>(frames) / static_cast<float>(averageFps);

    // std::cout << "\rdt: " << timestep.getSeconds() << ", fps: " << fps << ", average fps: " << averageFps << "        " << std::endl;

    // std::cout << "fps pos: " << fpsTransform.getTranslation().x << ", " << fpsTransform.getTranslation().y << std::endl;

    // move camera
    auto keyboard = engine->getKeyboard();

    auto camera = renderer->getActiveCamera(registry);
    auto &t = registry.get<Core::Transform>(camera);

    float camSpeed = 1.5f * (keyboard->isPressed(Input::Key::LEFT_SHIFT) ? 2.0f : 1.0f);
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

    // update fps text
    auto fpsEntity = registry.view<FPS>()[0];
    auto &fpsComponent = registry.get<FPS>(fpsEntity);
    auto &fpsMesh = registry.get<Rendering::Mesh2D>(fpsEntity);

    if (fpsComponent.fps != static_cast<unsigned int>(fps))
    {
        fpsComponent.fps = fps;

        auto fpsText = Rendering::MemoizedText::text("FPS: " + std::to_string(fpsComponent.fps), font);
        fpsMesh = fpsText.mesh(Rendering::Text::TextAlignment::LEFT);
    }

    auto &fpsTransform = registry.get<Core::Transform>(fpsEntity);
    auto transformedAABB = fpsMesh.getAABB().transform(fpsTransform);
    float fpsPadding = 10.0f;
    float fpsScale = 0.25f;

    fpsTransform.setTranslation(spaceTransformer->transform(glm::vec2(spaceTransformer->metersToPixels(transformedAABB.getWidth()) / 2.0f + fpsPadding, renderer->getHeight() - spaceTransformer->metersToPixels(transformedAABB.getHeight()) / 2.0f - fpsPadding), Core::SpaceTransformer::Space::SCREEN, Core::SpaceTransformer::Space::WORLD));
    fpsTransform.setScale(fpsScale * t.getScale());
    fpsTransform.setRotation(t.getRotation());

    // set text entity uniform
    auto textEntity = registry.view<Rendering::ShaderMaterial>()[0];
    auto &m = registry.get<Rendering::ShaderMaterial>(textEntity);

    // m.uniform(new Rendering::Uniform("uTime", timeSinceStart));
    textAlpha = sin(timeSinceStart) * 0.5f + 0.5f;
    m.uniform(new Rendering::Uniform("uColorAlpha", textAlpha));

    // toggle color blend pass
    if (keyboard->isPressed(Input::Key::C))
    {
        if (pipeline->has(4100))
        {
            pipeline->remove(colorBlendPass);
            std::cout << "removed color blend pass" << std::endl;
        }
        else
        {
            pipeline->add(colorBlendPass, 4100);
            std::cout << "added color blend pass" << std::endl;
        }
    }

    // toggle blur pass
    if (keyboard->isPressed(Input::Key::B))
    {
        if (pipeline->has(4200))
        {
            pipeline->remove(blurPass);
            std::cout << "removed blur pass" << std::endl;
        }
        else
        {
            pipeline->add(blurPass, 4200);
            std::cout << "added blur pass" << std::endl;
        }
    }

    // toggle brightness pass
    if (keyboard->isPressed(Input::Key::V))
    {
        if (pipeline->has(4300))
        {
            pipeline->remove(brightnessPass);
            std::cout << "removed brightness pass" << std::endl;
        }
        else
        {
            pipeline->add(brightnessPass, 4300);
            std::cout << "added brightness pass" << std::endl;
        }
    }

    // toggle posterize pass
    if (keyboard->isPressed(Input::Key::P))
    {
        if (pipeline->has(4400))
        {
            pipeline->remove(posterizePass);
            std::cout << "removed posterize pass" << std::endl;
        }
        else
        {
            pipeline->add(posterizePass, 4400);
            std::cout << "added posterize pass" << std::endl;
        }
    }

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