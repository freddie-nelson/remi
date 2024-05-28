#pragma once

#include "../ECS/Registry.h"
#include "../Scene/SceneGraph.h"
#include "../Core/Timestep.h"
#include "../ECS/System.h"

namespace World
{

    /**
     * Represents the world.
     *
     * This contains the registry and scene graph.
     *
     * The registry is used to store entities and their components.
     *
     * The scene graph is used to organize entities into a parent-child hierarchy.
     *
     * The world cannot be copied or moved.
     */
    class World
    {
    public:
        /**
         * Creates a new world.
         *
         * The world will create a new registry and scene graph.
         *
         * @param maxEntities The maximum number of entities the world can have.
         */
        World(size_t maxEntities);

        /**
         * Destroys the world.
         *
         * The world will destroy the registry and scene graph.
         */
        ~World();

        World(const World &) = delete;

        World &operator=(const World &) = delete;

        /**
         * Runs all updates.
         *
         * Does not run fixed updates.
         *
         * @param timestep The timestep since the last update.
         */
        void update(const ECS::System::SystemUpdateData &data);

        /**
         * Runs all fixed updates.
         *
         * @param timestep The timestep since the last update.
         */
        void fixedUpdate(const ECS::System::SystemUpdateData &data);

        /**
         * Gets the world's systems.
         *
         * @returns The world's systems.
         */
        const std::vector<ECS::System *> &getSystems();

        /**
         * Adds a system to the world.
         *
         * @param system The system to add.
         *
         * @returns True if the system was added, false otherwise.
         *
         * @throws std::invalid_argument If the system is nullptr.
         */
        bool addSystem(ECS::System *system);

        /**
         * Removes a system from the world.
         *
         * @param system The system to remove.
         *
         * @returns True if the system was removed, false otherwise.
         */
        bool removeSystem(ECS::System *system);

        /**
         * Checks if the world has the given system.
         *
         * @param system The system to check for.
         *
         * @returns True if the world has the system, false otherwise.
         */
        bool hasSystem(ECS::System *system);

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

        std::unordered_set<ECS::System *> systemsSet;
        std::vector<ECS::System *> systems;
    };
}