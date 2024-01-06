#pragma once

#include "../Core/Timestep.h"

#include "Registry.h"

namespace ECS
{
    class System
    {
    public:
        virtual ~System() = default;

        /**
         * Updates the system.
         *
         * @param registry The registry to use.
         * @param timestep The timestep since the last update.
         * @param events The events that have occurred since the last update.
         */
        virtual void update(const Registry &registry, const Core::Timestep &timestep){};

        /**
         * Updates the system at a fixed rate.
         *
         * @param registry The registry to use.
         * @param timestep The timestep since the last update.
         * @param events The events that have occurred since the last update.
         */
        virtual void fixedUpdate(const Registry &registry, const Core::Timestep &timestep){};
    };
}