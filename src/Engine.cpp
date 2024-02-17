#include "../include/Engine.h"
#include "../include/Config.h"
#include "../include/Rendering/Passes/RenderablesPass.h"
#include "../include/Rendering/Passes/CullingPass.h"
#include "../include/Rendering/Passes/BatchPass.h"
#include "../include/Rendering/Passes/DrawPass.h"
#include "../include/Rendering/Passes/OutputPass.h"
#include "../include/Rendering/Passes/PhysicsDebugPass.h"

#include <iostream>

#ifdef __EMSCRIPTEN__
#include "../include/emscriptenHelpers.h"
#endif

blz::Engine::Engine(EngineConfig config)
{
    this->config = config;
    Config::MAX_Z_INDEX = config.maxZIndex;

    window = new Core::Window(config.windowTitle, config.windowWidth, config.windowHeight, config.windowFullscreen);
    renderer = new Rendering::Renderer(window, config.windowWidth, config.windowHeight, config.pixelsPerMeter);

    pipeline = new Rendering::RenderPipeline();
    pipeline->add(new Rendering::RenderablesPass(), 1000);
    pipeline->add(new Rendering::CullingPass(), 2000);
    pipeline->add(new Rendering::BatchPass(), 3000);
    pipeline->add(new Rendering::DrawPass(), 4000);
    pipeline->add(new Rendering::OutputPass(), 5000);

    std::cout << "Default render pipeline:" << std::endl;
    std::cout << pipeline->toString() << std::endl;

    world = new World::World();

    spaceTransformer = new Core::SpaceTransformer(renderer, world, config.pixelsPerMeter);

    physicsWorld = new Physics::PhysicsWorld(config.physicsWorldConfig, spaceTransformer);

    if (config.drawDebugPhysics)
    {
        pipeline->add(new Rendering::PhysicsDebugPass(physicsWorld), 4050);
    }

    renderManager = new Rendering::RenderManager(renderer, pipeline, spaceTransformer);

    animationSystem = new Rendering::AnimationSystem();
    world->addSystem(animationSystem);

    mouse = new Input::Mouse(window->getGLFWWindow());
    keyboard = new Input::Keyboard(window->getGLFWWindow());
}

blz::Engine::~Engine()
{
    delete keyboard;
    delete mouse;
    delete renderManager;
    delete spaceTransformer;
    delete world;
    delete pipeline;
    delete renderer;
    delete window;
}

void blz::Engine::run()
{
    MainLoopArgs *args = new MainLoopArgs();

    args->timeBetweenFixedUpdatesSeconds = 1.0 / config.fixedUpdatesPerSecond;
    args->timeBetweenFixedUpdates = args->timeBetweenFixedUpdatesSeconds * 1000.0 * 1000.0;

    args->timeSinceLastFixedUpdate = 0;

    args->timeBetweenUpdatesSeconds = 1.0 / config.updatesPerSecond;
    args->timeBetweenUpdates = args->timeBetweenUpdatesSeconds * 1000.0 * 1000.0;

    args->timeSinceLastUpdate = 0;

    // std::cout << "Time between fixed updates: " << args->timeBetweenFixedUpdates << std::endl;
    // std::cout << "Time between updates: " << args->timeBetweenUpdates << std::endl;

    // create timestep at current time
    args->tick = Core::Timestep();

    // poll for events before first update
    glfwPollEvents();

    // pre update physics world
    // this will make sure that the physics world is in sync with the world before the first update
    physicsWorld->fixedUpdate(*world, Core::Timestep(0));

#ifdef __EMSCRIPTEN__
    std::function<void()> mainLoopWrapper = std::bind(&Engine::mainLoop, this, args);
    emscriptenSetMainLoop(mainLoopWrapper, 0, true);
#else
    while (true)
    {
        mainLoop(args);
    }
#endif
}

blz::EngineConfig &blz::Engine::getConfig()
{
    return config;
}

Core::Window *const blz::Engine::getWindow()
{
    return window;
}

Rendering::Renderer *const blz::Engine::getRenderer()
{
    return renderer;
}

Rendering::RenderPipeline *const blz::Engine::getPipeline()
{
    return pipeline;
}

Rendering::RenderManager *const blz::Engine::getRenderManager()
{
    return renderManager;
}

Rendering::AnimationSystem *const blz::Engine::getAnimationSystem()
{
    return animationSystem;
}

Physics::PhysicsWorld *const blz::Engine::getPhysicsWorld()
{
    return physicsWorld;
}

World::World *const blz::Engine::getWorld()
{
    return world;
}

Input::Mouse *const blz::Engine::getMouse()
{
    return mouse;
}

Input::Keyboard *const blz::Engine::getKeyboard()
{
    return keyboard;
}

Core::SpaceTransformer *const blz::Engine::getSpaceTransformer()
{
    return spaceTransformer;
}

void blz::Engine::mainLoop(MainLoopArgs *args)
{
    auto &[timeBetweenFixedUpdatesSeconds, timeBetweenFixedUpdates, timeSinceLastFixedUpdate, timeBetweenUpdatesSeconds, timeBetweenUpdates, timeSinceLastUpdate, tick] = *args;

    timeSinceLastFixedUpdate += tick.getMicroseconds();
    timeSinceLastUpdate += tick.getMicroseconds();

    // run fixed updates
    if (timeSinceLastFixedUpdate >= timeBetweenFixedUpdates)
    {
        Core::Timestep fixedTimestep(0);
        fixedTimestep.update(timeBetweenFixedUpdates);

        world->fixedUpdate(fixedTimestep);

        physicsWorld->fixedUpdate(*world, fixedTimestep);

        timeSinceLastFixedUpdate = 0;
    }

    // run updates
    if (timeSinceLastUpdate >= timeBetweenUpdates)
    {
        Core::Timestep timestep(0);
        timestep.update(timeSinceLastUpdate);

        // clear renderer
        renderer->clear();

        // update window
        window->update(timestep);

        bool isMinimized = window->isMinimized();

        // only update renderer if window is not minimized
        // otherwise resizing render target will cause a crash
        if (!isMinimized)
        {
            renderer->update(*world, timestep);
        }

        world->update(timestep);

        // no need to render if window is minimized
        if (!isMinimized)
        {
            // render
            renderManager->render(*world);
        }

        // present renderer
        renderer->present();

        // poll for new events after frame has been rendered
        window->pollEvents();

        // check if window should close
        if (window->getWindowShouldClose())
        {
#ifdef __EMSCRIPTEN__
            emscripten_cancel_main_loop();
            return;
#else
            exit(0);
#endif
        }

        timeSinceLastUpdate = 0;
    }

    // update ticker
    tick.update();
}