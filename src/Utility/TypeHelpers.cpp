#include "../include/Utility/TypeHelpers.h"

#include <iostream>
#include <string>
#include <unordered_map>

std::unordered_map<std::string, blz::TypeId> memoizedTypeIds;

blz::TypeId blz::generateTypeId(const char *typeName)
{
    std::string name(typeName);

    if (memoizedTypeIds.contains(name))
    {
        return memoizedTypeIds[typeName];
    }

    const unsigned int A = 54059; /* a prime */
    const unsigned int B = 76963; /* another prime */

    unsigned int h = 37;

    for (auto &c : name)
    {
        h = (h * A) ^ (c * B);
    }

    // std::cout << "Generating type id" << std::endl;
    // std::cout << "name: " << name << std::endl;
    // std::cout << "hash: " << h << std::endl;

    memoizedTypeIds[name] = h;

    return h; // or return h % C;
}