#pragma once

#include <typeinfo>

namespace ECS
{
    using ComponentId = unsigned long long;

    /**
     * Generates a unique id for a component.
     *
     * @param typeName The name of the type of the component
     *
     * @returns The component id
     */
    ComponentId generate(const char *typeName);

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
        inline static const ComponentId id = generate(typeid(T).name());
    };
}