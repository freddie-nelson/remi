#pragma once

#include "../Utility/TypeHelpers.h"

namespace ECS
{
    using ComponentId = unsigned long long;

    /**
     * Generates unique ids for components.
     */
    class ComponentIdGenerator
    {
    public:
        /**
         * Generates a unique id using the hash code of the type.
         *
         * NOTE: Switched to using the hash code of the type as the id instead of incrementing a static variable.
         *       This is because different dlls and exes will generate their own version of the template and
         *       therefore have different ids for the same component.
         */
        template <typename T>
        inline static const ComponentId id = blz::generateTypeId(typeid(T).name());
    };
}