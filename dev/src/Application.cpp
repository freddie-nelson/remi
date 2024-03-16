#include "./include/Application.h"

#include <remi/Core/SpaceTransformer.h>
#include <remi/ECS/SparseSet.h>
#include <remi/Rendering/Mesh/Mesh.h>
#include <remi/Rendering/Mesh/Polygons.h>
#include <remi/Rendering/Material/Material.h>
#include <remi/Core/Timestep.h>
#include <remi/Core/Transform.h>
#include <remi/Core/BoundingCircle.h>
#include <remi/Rendering/Renderable.h>
#include <remi/Rendering/Camera/Camera.h>
#include <remi/Rendering/Camera/ActiveCamera.h>
#include <remi/Rendering/Font/Font.h>
#include <remi/Rendering/Font/Text.h>
#include <remi/Rendering/Font/MemoizedText.h>
#include <remi/Rendering/Material/ShaderMaterial.h>
#include <remi/Rendering/Shader/Uniform.h>
#include <remi/Core/Timestep.h>
#include <remi/Rendering/Passes/ColorBlendPass.h>
#include <remi/Rendering/Passes/GaussianBlurPass.h>
#include <remi/Rendering/Passes/BrightnessPass.h>
#include <remi/Rendering/Passes/PosterizePass.h>
#include <remi/Rendering/Texture/AnimatedTexture.h>
#include <remi/Physics/RigidBody2D.h>
#include <remi/Physics/Collider2D.h>
#include <remi/Physics/Ray.h>
#include <remi/Physics/Joints/DistanceJoint.h>

#include <glm/gtx/string_cast.hpp>
#include <math.h>
#include <iostream>

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

// ! Implement some kind of texture/asset loading system at engine level
// would avoid user having to manage memory etc for textures and maybe other things

ECS::Entity player;
ECS::Entity character;
ECS::Entity camera;
ECS::Entity fpsEntity;
ECS::Entity deletable;

void Application::init()
{
    // init engine
    remi::EngineConfig config;
    // config.maxEntities = 1000;
    // config.windowFullscreen = true;
    config.updatesPerSecond = 10000;
    config.drawDebugPhysics = true;
    config.maxEntities = std::pow(2, 17);
    // config.drawDebugRenderTree = true;

    engine = new remi::Engine(config);

    auto window = engine->getWindow();
    auto renderer = engine->getRenderer();
    auto pipeline = engine->getPipeline();

    auto world = engine->getWorld();
    auto &registry = world->getRegistry();
    auto &sceneGraph = world->getSceneGraph();

    // add this system to the world
    world->addSystem(this);

    colorBlendPass = new Rendering::ColorBlendPass(Rendering::Color(1.0f, 0.0f, 1.0f, 1.0f));
    blurPass = new Rendering::GaussianBlurPass();
    brightnessPass = new Rendering::BrightnessPass(0.5f);
    posterizePass = new Rendering::PosterizePass(8.0f);
    // pipeline->add(colorBlendPass, 4500);

    // renderer->setProjectionMode(Rendering::RendererProjectionMode::MATCH);

    // Rendering::Color clearColor(0.0f);
    // clearColor.fromHSLA(0.82f, 0.6f, 0.45f, 1.0f);

    registry.view<Rendering::ActiveCamera>();

    renderer->enableAlphaBlending(true);

    // create texture
    Rendering::Texture *texture = new Rendering::Texture("assets/liv.jpg");
    Rendering::Texture *gradient = new Rendering::Texture("assets/gradient.png");

    // create font
    font = new Rendering::Font("assets/Roboto-Regular.ttf");

    // create camera
    camera = registry.create();
    registry.add(camera, Rendering::Camera(window->getWidth(), window->getHeight()));
    registry.add(camera, Core::Transform());
    registry.add(camera, Rendering::ActiveCamera());

    // create entities
    float pixelsPerMeter = config.pixelsPerMeter;

    int entityCount = 100000;
    int xRange = (config.windowWidth * std::sqrt(entityCount) / 10);
    int yRange = (config.windowHeight * std::sqrt(entityCount) / 10);
    int zRange = 10;

    srand(10);

    for (int i = 0; i < entityCount; i++)
    {
        auto e = registry.create();

        auto &m = registry.add(e, Rendering::Mesh2D(static_cast<float>((rand() % 50) / 100.0f + 0.2f), static_cast<unsigned int>(rand() % 13 + 3)));
        auto &t = registry.add(e, Core::Transform());
        auto &material = registry.add(e, Rendering::Material());
        auto &renderable = registry.add(e, Rendering::Renderable(true, true));

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

    ECS::Entity textEntity = registry.create();
    registry.add(textEntity, text.mesh(Rendering::Text::TextAlignment::CENTRE));
    registry.add(textEntity, Core::Transform());
    registry.add(textEntity, Rendering::Renderable(true, true));

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
    registry.add(textEntity, m);

    auto &t = registry.get<Core::Transform>(textEntity);
    t.scale(1);
    t.setZIndex(zRange - 2);

    // animation
    character = registry.create();
    std::cout << "character: " << character << std::endl;

    auto &cm = registry.add(character, Rendering::Mesh2D(1.0f, 1.0f));
    auto &ct = registry.add(character, Core::Transform());
    auto &cMat = registry.add(character, Rendering::Material());
    registry.add(character, Rendering::Renderable(true, false));

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

    // create player
    player = registry.create();
    registry.add(player, Core::Transform());

    // auto &body = registry.add(player, Physics::RigidBody2D());
    // body.setFixedRotation(true);

    // body.setBeginContactCallback([&](Physics::ContactInfo contactInfo)
    //                              { std::cout << "player contact: " << contactInfo.entityA << ", " << contactInfo.entityB << std::endl; });

    // auto shape = Physics::PolygonColliderShape2D(Rendering::Mesh2D(0.8f, 1.45f));
    // auto &collider = registry.add(player, Physics::Collider2D(&shape));

    // collider.setFriction(0.5f);

    sceneGraph.relate(player, camera);
    sceneGraph.relate(player, character);

    std::cout << "player: " << player << std::endl;

    // floor
    auto floor = registry.create();
    std::cout << "floor: " << floor << std::endl;

    auto &fm = registry.add(floor, Rendering::Mesh2D(100.0f, 1.0f));
    auto &ft = registry.add(floor, Core::Transform());
    auto &fMat = registry.add(floor, Rendering::Material());
    // auto &fRenderable = registry.add(floor, Rendering::Renderable(true, true));

    ft.translate(glm::vec2(0.0f, -5.0f));

    // auto &fBody = registry.add(floor, Physics::RigidBody2D());
    // auto &fCollider = registry.add(floor, Physics::Collider2D(new Physics::PolygonColliderShape2D(fm)));

    // fBody.setType(Physics::RigidBodyType::STATIC);
    // fCollider.setDensity(0.0f);
    // fCollider.setFriction(1.0f);

    // fps text
    auto fpsText = Rendering::MemoizedText::text("FPS: 0", font);
    fpsEntity = registry.create();
    auto &fpsMesh = registry.add(fpsEntity, fpsText.mesh(Rendering::Text::TextAlignment::LEFT));
    auto &fpsTransform = registry.add(fpsEntity, Core::Transform());
    auto &fpsMaterial = registry.add(fpsEntity, Rendering::Material());
    auto &fpsRenderable = registry.add(fpsEntity, Rendering::Renderable(true, false));

    fpsTransform.scale(0.25f);
    fpsTransform.setZIndex(Config::MAX_Z_INDEX);

    sceneGraph.relate(camera, fpsEntity);

    float paddingX = 55.0f;
    float paddingY = 15.0f;
    auto spaceTransformer = engine->getSpaceTransformer();
    auto screenPos = glm::vec2(paddingX, window->getHeight() - paddingY);
    auto localPos = spaceTransformer->transform(screenPos, fpsEntity, Core::SpaceTransformer::Space::SCREEN, Core::SpaceTransformer::Space::LOCAL);

    fpsTransform.setTranslation(localPos);

    // create physics bodies
    size_t count = 0;
    float width = 0.5f;
    float height = 0.5f;
    int areaX = std::sqrt(count);
    int areaY = std::sqrt(count);

    for (size_t i = 0; i < count; i++)
    {
        auto e = registry.create();
        registry.add(e, Rendering::Mesh2D(width, height));
        registry.add(e, Rendering::Material(Rendering::Color(1.0f, 1.0f, 1.0f, 1.0f)));
        registry.add(e, Rendering::Renderable(true, false));
        registry.add(e, Physics::RigidBody2D());

        auto &t = registry.add(e, Core::Transform());
        t.translate(glm::vec2(rand() % areaX - areaX / 2, rand() % areaY - areaY / 2));

        auto &collider = registry.add(e, Physics::Collider2D(new Physics::PolygonColliderShape2D(registry.get<Rendering::Mesh2D>(e))));
        collider.setFriction(1.0f);
    }

    // create concave collider
    // auto concave = registry.create();
    // registry.add(concave, Core::Transform());
    // auto &concaveMesh = registry.add(concave, Rendering::Mesh2D({{0.0f, 0.0f}, {0.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 0.0f}, {0.5f, 0.5f}}));
    // registry.add(concave, Rendering::Material(Rendering::Color(1.0f, 1.0f, 1.0f, 1.0f)));
    // registry.add(concave, Rendering::Renderable(true, false));
    // registry.add(concave, Physics::RigidBody2D());
    // registry.add(concave, Physics::Collider2D(new Physics::ConcavePolygonColliderShape2D(concaveMesh)));

    // // create bodies joined by distance joint
    // auto b1 = registry.create();
    // registry.add(b1, Core::Transform());
    // registry.add(b1, Rendering::Mesh2D(0.5f, 32u));
    // registry.add(b1, Rendering::Material());
    // registry.add(b1, Rendering::Renderable(true, false));
    // registry.add(b1, Physics::RigidBody2D());
    // registry.add(b1, Physics::Collider2D(new Physics::CircleColliderShape2D(0.5f)));

    // auto b2 = registry.create();
    // registry.add(b2, Core::Transform(glm::vec2(1.5f, 0.0f)));
    // registry.add(b2, Rendering::Mesh2D(0.5f, 32u));
    // registry.add(b2, Rendering::Material());
    // registry.add(b2, Rendering::Renderable(true, false));
    // registry.add(b2, Physics::RigidBody2D());
    // registry.add(b2, Physics::Collider2D(new Physics::CircleColliderShape2D(0.5f)));

    // // add joint
    // registry.add(b1, Physics::DistanceJoint(b2));

    // deletable
    // deletable = registry.create();
    // registry.add(deletable, Core::Transform());
    // registry.add(deletable, Rendering::Mesh2D(2.0f, 32u));
    // registry.add(deletable, Rendering::Material(Rendering::Color(1.0f, 1.0f, 0.0f, 1.0f)));
    // registry.add(deletable, Rendering::Renderable(true, false));
    // registry.add(deletable, Physics::RigidBody2D());
    // registry.add(deletable, Physics::Collider2D(new Physics::CircleColliderShape2D(2.0f)));

    // auto test = registry.create();
    // registry.add(test, Core::Transform());
    // registry.add(test, Rendering::Mesh2D(2.0f, 32u));
    // registry.add(test, Rendering::Material(Rendering::Color(1.0f, 1.0f, 0.0f, 1.0f)));
    // registry.add(test, Rendering::Renderable(true, false));
    // registry.add(test, Physics::RigidBody2D());
    // registry.add(test, Physics::Collider2D(new Physics::CircleColliderShape2D(2.0f)));
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

void Application::update(World::World &world, const Core::Timestep &timestep)
{
    auto &registry = world.getRegistry();

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

    // camera controls
    auto keyboard = engine->getKeyboard();

    auto camera = renderer->getActiveCamera(registry);
    auto &t = registry.get<Core::Transform>(camera);

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
    auto &fpsMesh = registry.get<Rendering::Mesh2D>(fpsEntity);

    auto fpsText = Rendering::MemoizedText::text("FPS: " + std::to_string(static_cast<unsigned int>(fps)), font);
    fpsMesh = fpsText.mesh(Rendering::Text::TextAlignment::LEFT);

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

bool createdRaycastEntity = false;
ECS::Entity raycastEntity;

bool createdQueryEntity = false;
ECS::Entity queryEntity;

bool deleted = false;

void Application::fixedUpdate(World::World &world, const Core::Timestep &timestep)
{
    return;

    auto &registry = world.getRegistry();
    auto &sceneGraph = world.getSceneGraph();
    auto &physicsWorld = *engine->getPhysicsWorld();
    auto &spaceTransformer = *engine->getSpaceTransformer();

    // control character
    auto keyboard = engine->getKeyboard();

    auto &playerBody = registry.get<Physics::RigidBody2D>(player);
    auto &playerTransform = registry.get<Core::Transform>(player);
    auto &characterTransform = registry.get<Core::Transform>(character);

    std::cout << "player mass: " << playerBody.getMass() << std::endl;

    playerBody.applyLinearImpulse(glm::vec2(1.0f, 0.0f));

    float speed = 2.5f * (keyboard->isPressed(Input::Key::LEFT_SHIFT) ? 2.0f : 1.0f);
    float jumpSpeed = 5.0f;

    glm::vec2 charVel(0.0f);

    if (keyboard->isPressed(Input::Key::SPACE))
    {
        charVel.y = jumpSpeed;
    }

    if (keyboard->isPressed(Input::Key::A))
    {
        charVel.x -= speed;
        characterTransform.setScale(glm::vec2(-3, 3));
    }
    if (keyboard->isPressed(Input::Key::D))
    {
        charVel.x += speed;
        characterTransform.setScale(glm::vec2(3, 3));
    }

    if (charVel.x != 0.0f)
    {
        playerBody.setVelocityX(charVel.x);
    }
    if (charVel.y != 0.0f)
    {
        playerBody.setVelocityY(charVel.y);
    }

    // raycast test
    auto playerWorldTransform = Core::Transform(sceneGraph.getModelMatrix(player));

    glm::vec2 origin = playerWorldTransform.getTranslation();
    glm::vec2 mouse = spaceTransformer.transform(engine->getMouse()->getPosition(true), Core::SpaceTransformer::Space::SCREEN, Core::SpaceTransformer::Space::WORLD);
    Physics::Ray ray(origin, mouse);

    // std::cout << "start: " << ray.start.x << ", " << ray.start.y << std::endl;
    // std::cout << "end: " << ray.end.x << ", " << ray.end.y << std::endl;

    auto hits = physicsWorld.raycast(ray, Physics::RaycastType::CLOSEST);

    if (!createdRaycastEntity)
    {
        raycastEntity = registry.create();
        registry.add(raycastEntity, Rendering::Mesh2D());
        registry.add(raycastEntity, Core::Transform());
        registry.add(raycastEntity, Rendering::Material(Rendering::Color(1.0f, 1.0f, 1.0f, 1.0f)));
        registry.add(raycastEntity, Rendering::Renderable(true, false));
        createdRaycastEntity = true;

        auto &rayTransform = registry.get<Core::Transform>(raycastEntity);
        rayTransform.setZIndex(Config::MAX_Z_INDEX);
    }

    auto &raycastMesh = registry.get<Rendering::Mesh2D>(raycastEntity);
    raycastMesh.createLine(ray.start, ray.end, 0.025f);

    auto &raycastMaterial = registry.get<Rendering::Material>(raycastEntity);
    if (hits.empty())
    {
        raycastMaterial.setColor(Rendering::Color(1.0f, 1.0f, 1.0f, 1.0f));
    }
    else
    {
        raycastMaterial.setColor(Rendering::Color(1.0f, 0.0f, 0.0f, 1.0f));
    }

    // query test
    if (!createdQueryEntity)
    {
        queryEntity = registry.create();
        registry.add(queryEntity, Rendering::Mesh2D(0.5f, 32u));
        registry.add(queryEntity, Core::Transform());
        registry.add(queryEntity, Rendering::Material(Rendering::Color(1.0f, 1.0f, 1.0f, 0.1f)));
        registry.add(queryEntity, Rendering::Renderable(true, false));
        createdQueryEntity = true;

        auto &queryTransform = registry.get<Core::Transform>(queryEntity);
        queryTransform.setZIndex(Config::MAX_Z_INDEX);
    }

    auto &queryTransform = registry.get<Core::Transform>(queryEntity);
    queryTransform.setTranslation(mouse);

    Core::BoundingCircle circle(mouse, 0.5f);
    auto queryResults = physicsWorld.query(circle);

    auto colliderEntities = registry.view<Physics::Collider2D>();
    for (auto &e : colliderEntities)
    {
        if (!registry.has<Rendering::Material>(e))
            continue;

        auto &m = registry.get<Rendering::Material>(e);
        m.setColor(Rendering::Color(1.0f, 1.0f, 1.0f, 1.0f));
    }

    for (auto &e : queryResults)
    {
        if (!registry.has<Rendering::Material>(e))
            continue;

        auto &m = registry.get<Rendering::Material>(e);
        m.setColor(Rendering::Color(1.0f, 0.0f, 0.0f, 1.0f));
    }

    // test deleting
    if (frames >= 200 && !deleted)
    {
        std::cout << "deleting entity: " << deletable << std::endl;
        deleted = true;
        // registry.destroy(deletable);
        std::cout << "deleted entity: " << deletable << std::endl;
    }
}