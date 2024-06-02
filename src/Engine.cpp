#include "../include/Engine.h"
#include "../include/Config.h"
#include "../include/Rendering/Passes/RenderablesPass.h"
#include "../include/Rendering/Passes/CullingPass.h"
#include "../include/Rendering/Passes/BatchPass.h"
#include "../include/Rendering/Passes/DrawPass.h"
#include "../include/Rendering/Passes/OutputPass.h"
#include "../include/Rendering/Passes/PhysicsDebugPass.h"
#include "../include/Rendering/Passes/DebugRenderTreePass.h"
#include "../include/Utility/SDLHelpers.h"

#include <iostream>

#ifdef __EMSCRIPTEN__
#include "../include/emscriptenHelpers.h"
#endif

remi::Engine::Engine(EngineConfig config)
{
    initSDL();

    this->config = config;
    Config::MAX_Z_INDEX = config.maxZIndex;

    window = new Core::Window(config.windowTitle, config.windowWidth, config.windowHeight, config.windowType);
    renderer = new Rendering::Renderer(window, config.windowWidth, config.windowHeight, config.pixelsPerMeter);

    auto cullingPass = new Rendering::CullingPass();

    pipeline = new Rendering::RenderPipeline();
    pipeline->add(new Rendering::RenderablesPass(), 1000);
    pipeline->add(cullingPass, 2000);
    pipeline->add(new Rendering::BatchPass(), 3000);
    pipeline->add(new Rendering::DrawPass(), 4000);
    pipeline->add(new Rendering::OutputPass(), 5000);

    std::cout << "Default render pipeline:" << std::endl;
    std::cout << pipeline->toString() << std::endl;

    world = new World::World(config.maxEntities);

    spaceTransformer = new Core::SpaceTransformer(renderer, world, config.pixelsPerMeter);

    physicsWorld = new Physics::PhysicsWorld(config.physicsWorldConfig, world, spaceTransformer);

    if (config.drawDebugPhysics)
    {
        pipeline->add(new Rendering::PhysicsDebugPass(physicsWorld), 4050);
    }

    if (config.drawDebugRenderTree)
    {
        pipeline->add(new Rendering::DebugRenderTreePass(cullingPass), 4100);
    }

    renderManager = new Rendering::RenderManager(renderer, pipeline, spaceTransformer);

    animationSystem = new Rendering::AnimationSystem();
    world->addSystem(animationSystem);

    mouse = new Input::Mouse(*window);
    keyboard = new Input::Keyboard(*window);

    mouseJointUpdateSystem = new Physics::MouseJointUpdateSystem(mouse, spaceTransformer);
    world->addSystem(mouseJointUpdateSystem);

    audioManager = new Audio::AudioManager();
}

remi::Engine::~Engine()
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

void remi::Engine::run()
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
    window->pollEvents();

    // pre update scene graph
    auto &sceneGraph = world->getSceneGraph();
    sceneGraph.updateModelMatrices();

    // pre update physics world
    // this will make sure that the physics world is in sync with the world before the first update
    auto timestep = Core::Timestep(0);
    physicsWorld->fixedUpdate(createSystemUpdateData(timestep));

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

remi::EngineConfig &remi::Engine::getConfig()
{
    return config;
}

Core::Window *const remi::Engine::getWindow()
{
    return window;
}

Rendering::Renderer *const remi::Engine::getRenderer()
{
    return renderer;
}

Rendering::RenderPipeline *const remi::Engine::getPipeline()
{
    return pipeline;
}

Rendering::RenderManager *const remi::Engine::getRenderManager()
{
    return renderManager;
}

Rendering::AnimationSystem *const remi::Engine::getAnimationSystem()
{
    return animationSystem;
}

Physics::PhysicsWorld *const remi::Engine::getPhysicsWorld()
{
    return physicsWorld;
}

World::World *const remi::Engine::getWorld()
{
    return world;
}

Input::Mouse *const remi::Engine::getMouse()
{
    return mouse;
}

Input::Keyboard *const remi::Engine::getKeyboard()
{
    return keyboard;
}

Core::SpaceTransformer *const remi::Engine::getSpaceTransformer()
{
    return spaceTransformer;
}

Physics::MouseJointUpdateSystem *const remi::Engine::getMouseJointUpdateSystem()
{
    return mouseJointUpdateSystem;
}

Audio::AudioManager *const remi::Engine::getAudioManager()
{
    return audioManager;
}

ECS::System::SystemUpdateData remi::Engine::createSystemUpdateData(const Core::Timestep &timestep)
{
    return ECS::System::SystemUpdateData{*world,
                                         *physicsWorld,
                                         *spaceTransformer,
                                         timestep,
                                         *mouse,
                                         *keyboard,
                                         *audioManager};
}

void remi::Engine::mainLoop(MainLoopArgs *args)
{
    auto &[timeBetweenFixedUpdatesSeconds, timeBetweenFixedUpdates, timeSinceLastFixedUpdate, timeBetweenUpdatesSeconds, timeBetweenUpdates, timeSinceLastUpdate, tick] = *args;

    timeSinceLastFixedUpdate += tick.getMicroseconds();
    timeSinceLastUpdate += tick.getMicroseconds();

    // run fixed updates
    if (timeSinceLastFixedUpdate >= timeBetweenFixedUpdates)
    {
        // auto start = Core::timeSinceEpochMicrosec();

        Core::Timestep fixedTimestep(0);
        fixedTimestep.update(timeBetweenFixedUpdates);

        auto data = createSystemUpdateData(fixedTimestep);

        world->fixedUpdate(data);
        physicsWorld->fixedUpdate(data);

        timeSinceLastFixedUpdate = 0;

        // auto end = Core::timeSinceEpochMicrosec();
        // std::cout << "Fixed update time: " << (end - start) << std::endl;
    }

    // run updates
    if (timeSinceLastUpdate >= timeBetweenUpdates)
    {
        // auto start = Core::timeSinceEpochMicrosec();

        Core::Timestep timestep(0);
        timestep.update(timeSinceLastUpdate);

        auto data = createSystemUpdateData(timestep);

        // clear renderer
        renderer->clear();

        // update window
        window->update(timestep);

        bool isMinimized = window->isMinimized();

        // only update renderer if window is not minimized
        // otherwise resizing render target will cause a crash
        if (!isMinimized)
        {
            renderer->update(data);
        }

        world->update(data);

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
            quitSDL();

#ifdef __EMSCRIPTEN__
            emscripten_cancel_main_loop();
            return;
#else
            exit(0);
#endif
        }

        timeSinceLastUpdate = 0;

        // auto end = Core::timeSinceEpochMicrosec();
        // std::cout << "Update time: " << (end - start) << std::endl;
    }

    // update ticker
    tick.update();
}
