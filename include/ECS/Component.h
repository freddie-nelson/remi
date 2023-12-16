#pragma once

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

    class ComponentIdGenerator
    {
    private:
        inline static ComponentId nextId = 0;

    public:
        template <typename T>
        inline static const ComponentId id = nextId++;
    };
}