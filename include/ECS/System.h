#pragma once

#include "../Input/Mouse.h"
#include "../Input/Keyboard.h"
#include "../Core/Timestep.h"
#include "../Audio/SoundEffectManager.h"

namespace World
{
    class World;
}

namespace Physics
{
    class PhysicsWorld;
}

namespace Core
{
    class SpaceTransformer;
}

namespace ECS
{
    /**
     * Represents a system that can be updated.
     *
     * A system is used to update the world.
     */
    class System
    {
    public:
        struct SystemUpdateData
        {
            World::World &world;
            Physics::PhysicsWorld &physicsWorld;
            const Core::SpaceTransformer &spaceTransformer;
            const Core::Timestep &timestep;

            const Input::Mouse &mouse;
            const Input::Keyboard &keyboard;

            const Audio::SoundEffectManager &soundEffectManager;
        };

        virtual ~System() = default;

        /**
         * Updates the system.
         *
         * @param data The system update data, holds references to the world, physics world, space transformer, and timestep.
         */
        virtual void update(const SystemUpdateData &data) {};

        /**
         * Updates the system at a fixed rate.
         *
         * @param data The system update data, holds references to the world, physics world, space transformer, and timestep.
         */
        virtual void fixedUpdate(const SystemUpdateData &data) {};
    };
}