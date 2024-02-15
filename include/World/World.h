#pragma once

#include "../ECS/Registry.h"
#include "../Scene/SceneGraph.h"
#include "../Core/Timestep.h"

namespace World
{
    class System;

    /**
     * Represents the world.
     *
     * This contains the registry and scene graph.
     *
     * The registry is used to store entities and their components.
     *
     * The scene graph is used to organize entities into a parent-child hierarchy.
     */
    class World
    {
    public:
        /**
         * Creates a new world.
         *
         * The world will create a new registry and scene graph.
         */
        World();

        /**
         * Destroys the world.
         *
         * The world will destroy the registry and scene graph.
         */
        ~World();

        /**
         * Runs all updates.
         *
         * Does not run fixed updates.
         *
         * @param timestep The timestep since the last update.
         */
        void update(const Core::Timestep &timestep);

        /**
         * Runs all fixed updates.
         *
         * @param timestep The timestep since the last update.
         */
        void fixedUpdate(const Core::Timestep &timestep);

        /**
         * Gets the world's systems.
         *
         * @returns The world's systems.
         */
        const std::vector<System *> &getSystems();

        /**
         * Adds a system to the world.
         *
         * @param system The system to add.
         *
         * @returns True if the system was added, false otherwise.
         *
         * @throws std::invalid_argument If the system is nullptr.
         */
        bool addSystem(System *system);

        /**
         * Removes a system from the world.
         *
         * @param system The system to remove.
         *
         * @returns True if the system was removed, false otherwise.
         */
        bool removeSystem(System *system);

        /**
         * Checks if the world has the given system.
         *
         * @param system The system to check for.
         *
         * @returns True if the world has the system, false otherwise.
         */
        bool hasSystem(System *system);

        /**
         * Gets the registry.
         *
         * @returns The registry.
         */
        ECS::Registry &getRegistry();

        /**
         * Gets the registry.
         *
         * @returns The registry.
         */
        const ECS::Registry &getRegistry() const;

        /**
         * Gets the scene graph.
         *
         * @returns The scene graph.
         */
        Scene::SceneGraph &getSceneGraph();

        /**
         * Gets the scene graph.
         *
         * @returns The scene graph.
         */
        const Scene::SceneGraph &getSceneGraph() const;

    private:
        ECS::Registry registry;
        Scene::SceneGraph sceneGraph;

        std::vector<System *> systems;
    };

    /**
     * Represents a system that can be updated.
     *
     * A system is used to update the world.
     */
    class System
    {
    public:
        virtual ~System() = default;

        /**
         * Updates the system.
         *
         * @param world The world to use.
         * @param timestep The timestep since the last update.
         */
        virtual void update(World &world, const Core::Timestep &timestep){};

        /**
         * Updates the system at a fixed rate.
         *
         * @param world The world to use.
         * @param timestep The timestep since the last update.
         */
        virtual void fixedUpdate(World &world, const Core::Timestep &timestep){};
    };
}