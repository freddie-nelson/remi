#pragma once

#include "../Core/Timestep.h"
#include "Registry.h"

namespace ECS
{
    class System
    {
    public:
        virtual ~System() = default;

        virtual void update(const Registry &registry, const Core::Timestep &timestep){};

        virtual void fixedUpdate(const Registry &registry, const Core::Timestep &timestep){};
    };
}