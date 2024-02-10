#pragma once

#include "Core/Timestep.h"
#include "Core/Window.h"
#include "Rendering/Renderer.h"
#include "Rendering/RenderPipeline.h"
#include "Rendering/RenderManager.h"
#include "Rendering/Texture/AnimationSystem.h"
#include "ECS/Registry.h"
#include "Input/Mouse.h"
#include "Input/Keyboard.h"
#include "Core/SpaceTransformer.h"
#include "Physics/PhysicsWorld.h"

namespace blz
{

    /**
     * The configuration of the engine.
     *
     * This is used to configure the engine when it is created.
     */
    struct EngineConfig
    {

        /**
         * The title of the window.
         *
         * Changing this after engine creation will not change the title of the window.
         */
        std::string windowTitle = "Blaze";

        /**
         * The width of the window.
         *
         * Changing this after engine creation will not change the size of the window.
         */
        unsigned int windowWidth = 800;

        /**
         * The height of the window.
         *
         * Changing this after engine creation will not change the size of the window.
         */
        unsigned int windowHeight = 600;

        /**
         * Whether to create the window in fullscreen mode.
         *
         * Changing this after engine creation will not change the fullscreen mode of the window.
         */
        bool windowFullscreen = false;

        /**
         * The major version of OpenGL to use.
         *
         * This must be greater than or equal to 3.
         *
         * Changing this after engine creation will not change the OpenGL version of the window.
         */
        unsigned int openglMajorVersion = 3;

        /**
         * The minor version of OpenGL to use.
         *
         * This must be greater than or equal to 3.3 and less than or equal to 4.6.
         *
         * Changing this after engine creation will not change the OpenGL version of the window.
         */
        unsigned int openglMinorVersion = 0;

        /**
         * The maximum number of non fixed system updates per second.
         */
        unsigned int updatesPerSecond = 10000;

        /**
         * The maximum number of fixed system updates per second.
         */
        unsigned int fixedUpdatesPerSecond = 60;

        /**
         * The maximum z index an entity can have.
         *
         * This is the number of layers that can be used.
         *
         * A higher number may result in worse performance.
         */
        unsigned int maxZIndex = 128;

        /**
         * The number of pixels per meter.
         *
         * This is used to convert between pixels and meters.
         *
         * A lower number will make objects appear smaller on screen.
         *
         * Changing this can affect the physics system performance.
         */
        unsigned int pixelsPerMeter = 100;

        /**
         * Whether to show debug info.
         */
        bool showDebugInfo = true;

        /**
         * The configuration of the physics world.
         */
        Physics::PhysicsWorldConfig physicsWorldConfig;
    };

    /**
     * The engine.
     *
     * The engine is responsible for managing the window, renderer and ecs.
     *
     * The engine cannot be copied.
     *
     * The engine is also responsible for updating the systems. There are two types of updates in systems:
     *
     * - fixedUpdate: These are updated as close to the fixed update rate as possible, and will receive a fixed timestep.
     * - update: These are updated as close to the update rate as possible, and will receive a variable timestep.
     *
     * NOTE:
     * The renderer is cleared before each system update, and presented after all systems have been updated.
     * This means that anything rendered in a fixed update will be cleared before it can ever be shown. i.e. Rendering should not be done in fixedUpdate.
     *
     * Fixed updates will be performed before updates, if they are both due to be performed in the same tick.
     *
     * NOTE:
     * The physics world is updated in the fixed update, and will receive a fixed timestep. It is updated after all other systems have been updated.
     */
    class Engine
    {
    public:
        /**
         * Creates a new engine.
         *
         * This will create a new window, renderer and registry.
         *
         * @param config The configuration of the engine.
         */
        Engine(EngineConfig config);

        Engine(const Engine &other) = delete;

        Engine &operator=(const Engine &other) = delete;

        /**
         * Destroys the engine.
         */
        ~Engine();

        /**
         * Runs the engine.
         *
         * This will update each system and fixed system in the ecs, until the program exits.
         */
        void run();

        /**
         * Adds a system to the engine.
         *
         * @param system The system to add.
         */
        void addSystem(ECS::System *system);

        /**
         * Removes a system from the engine.
         *
         * @param system The system to remove.
         *
         * @returns Whether the system was removed.
         */
        bool removeSystem(ECS::System *system);

        /**
         * Gets the configuration of the engine.
         *
         * @returns The configuration of the engine.
         */
        EngineConfig &getConfig();

        /**
         * Gets the window of the engine.
         *
         * This is the window that is used to render the game.
         *
         * @returns The window of the engine.
         */
        Core::Window *const getWindow();

        /**
         * Gets the renderer of the engine.
         *
         * This is the renderer that is used to render the game.
         *
         * @returns The renderer of the engine.
         */
        Rendering::Renderer *const getRenderer();

        /**
         * Gets the render pipeline of the engine.
         *
         * This is the render pipeline that is used to render the game.
         *
         * @returns The render pipeline of the engine.
         */
        Rendering::RenderPipeline *const getPipeline();

        /**
         * Gets the render manager of the engine.
         *
         * This is the render manager that is used to render the game.
         *
         * @returns The render manager of the engine.
         */
        Rendering::RenderManager *const getRenderManager();

        /**
         * Gets the animation system of the engine.
         *
         * This is the animation system that is used to animate textures.
         *
         * @returns The animation system of the engine.
         */
        Rendering::AnimationSystem *const getAnimationSystem();

        /**
         * Gets the physics world of the engine.
         *
         * This is the physics world that is used to simulate physics.
         *
         * @returns The physics world of the engine.
         */
        Physics::PhysicsWorld *const getPhysicsWorld();

        /**
         * Gets the registry of the engine.
         *
         * This is the registry that is used to store all the entities and their components.
         *
         * @returns The registry of the engine.
         */
        ECS::Registry *const getRegistry();

        /**
         * Gets the mouse of the engine.
         *
         * This is the mouse that is used to get the mouse position and button states.
         *
         * @returns The mouse of the engine.
         */
        Input::Mouse *const getMouse();

        /**
         * Gets the keyboard of the engine.
         *
         * This is the keyboard that is used to get the keyboard key states.
         *
         * @returns The keyboard of the engine.
         */
        Input::Keyboard *const getKeyboard();

        /**
         * Gets the space transformer of the engine.
         *
         * This is used to convert between different coordinate systems.
         *
         * @returns The space transformer of the engine.
         */
        Core::SpaceTransformer *const getSpaceTransformer();

    private:
        EngineConfig config;

        Core::Window *window;
        Rendering::Renderer *renderer;
        Rendering::RenderPipeline *pipeline;
        Rendering::RenderManager *renderManager;

        Rendering::AnimationSystem *animationSystem;

        Physics::PhysicsWorld *physicsWorld;

        ECS::Registry *registry;

        Input::Mouse *mouse;
        Input::Keyboard *keyboard;

        Core::SpaceTransformer *spaceTransformer;

        /**
         * Systems that are updated every frame.
         */
        std::vector<ECS::System *> systems;

        /**
         * The data that is passed to the main loop.
         */
        struct MainLoopArgs
        {
            double timeBetweenFixedUpdatesSeconds;
            Core::Time timeBetweenFixedUpdates;
            Core::Time timeSinceLastFixedUpdate;

            double timeBetweenUpdatesSeconds;
            Core::Time timeBetweenUpdates;
            Core::Time timeSinceLastUpdate;

            Core::Timestep tick;
        };

        /**
         * The main loop for the run function.
         *
         * @param args The arguments for the main loop.
         */
        void mainLoop(MainLoopArgs *args);
    };
}