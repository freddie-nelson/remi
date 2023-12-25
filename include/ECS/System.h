#pragma once

#include "Registry.h"

namespace ECS
{
    class System
    {
    public:
        virtual void update(const Registry &registry, float dt) = 0;
    };
}