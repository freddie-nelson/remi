#pragma once

#include <stddef.h>

namespace ECS
{
#if (SIZE_MAX == UINT16_MAX)
    using Entity = unsigned short;
#elif (SIZE_MAX == UINT32_MAX)
    using Entity = unsigned int;
#elif (SIZE_MAX == UINT64_MAX)
    using Entity = long long unsigned int;
#else
#error "Could not determine size_t size"
#endif

    Entity createEntity();
}