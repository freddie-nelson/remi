#pragma once

#include "../ECS/System.h"

#include <box2d/b2_world.h>

namespace Physics
{
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
        PhysicsWorld();

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
        void update(const ECS::Registry &registry, const Core::Timestep &timestep) override;

    private:
        b2World world;
    };
}