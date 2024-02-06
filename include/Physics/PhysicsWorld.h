#pragma once

#include "../ECS/System.h"

#include <unordered_map>
#include <box2d/b2_world.h>
#include <glm/vec2.hpp>

namespace Physics
{
    /**
     * The physics world config.
     *
     * Configures how the physics world will behave.
     */
    struct PhysicsWorldConfig
    {
        /**
         * The gravity of the world.
         */
        glm::vec2 gravity = glm::vec2(0.0f, -10.0f);
    };

    /**
     * Represents the physics world.
     *
     * Stores and updates the physics world.
     */
    class PhysicsWorld : public ECS::System
    {
    public:
        /**
         * Creates a new physics world.
         */
        PhysicsWorld(PhysicsWorldConfig config);

        /**
         * Destroys the physics world.
         */
        ~PhysicsWorld();

        /**
         * Updates the physics world.
         *
         * @param registry The registry to use.
         * @param timestep The timestep since the last update.
         */
        void fixedUpdate(const ECS::Registry &registry, const Core::Timestep &timestep) override;

        /**
         * Sets the physics world's config.
         *
         * @param config The config object.
         */
        void setConfig(PhysicsWorldConfig config);

        /**
         * Gets the physics world's config.
         *
         * @returns The physics world's config.
         */
        const PhysicsWorldConfig &getConfig();

        /**
         * Sets the world's gravity vector.
         *
         * @param gravity The world's gravity.
         */
        void setGravity(glm::vec2 gravity);

        /**
         * Gets the world's gravity vector.
         *
         * @returns The world's gravity vector.
         */
        glm::vec2 getGravity();

    private:
        PhysicsWorldConfig config;

        /**
         * The box2d world.
         */
        b2World world;

        /**
         * The entity to body map.
         */
        std::unordered_map<ECS::Entity, b2Body *> bodies;

        /**
         * Updates the bodies map.
         *
         * Will add or remove bodies from the map if they are added or removed from the registry.
         *
         * @param registry The registry to use.
         */
        void updateBodies(const ECS::Registry &registry);
    };
}