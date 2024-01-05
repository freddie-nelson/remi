#pragma once

#include <typeinfo>

namespace ECS
{
#if (SIZE_MAX == UINT16_MAX)
    using ComponentId = unsigned short;
#elif (SIZE_MAX == UINT32_MAX)
    using ComponentId = unsigned int;
#elif (SIZE_MAX == UINT64_MAX)
    using ComponentId = long long unsigned int;
#else
#error "Could not determine size_t size"
#endif

    /**
     * Generates unique ids for components.
     */
    class ComponentIdGenerator
    {
    private:
        // inline static ComponentId nextId = 0;

    public:
        /**
         * Generates a unique id using the hash code of the type.
         *
         * NOTE: Switched to using the hash code of the type as the id instead of incrementing a static variable.
         *       This is because different dlls and exes will generate their own version of the template and
         *       therefore have different ids for the same component.
         */
        template <typename T>
        inline static const ComponentId id = typeid(T).hash_code();
    };
}