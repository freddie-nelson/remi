#include "../include/Utility/TypeHelpers.h"

#include <iostream>
#include <string>
#include <unordered_map>


remi::TypeId remi::generateTypeId(const char *typeName)
{
    // NOTE: had this outside function but was causing memory leak and heap overflow on wasm build
    // wasm might not like variables declared outside of functions and being used but who knows
    // works as static variable
    static std::unordered_map<std::string, remi::TypeId> memoizedTypeIds;

    std::string name(typeName);

    if (memoizedTypeIds.contains(name))
    {
        return memoizedTypeIds[name];
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

    // std::cout << "generateTypeId " << name << std::endl;
    // std::cout << "memoizedTypeIds.size(): " << memoizedTypeIds.size() << std::endl;
    // std::cout << "memoizedTypeIds[name]: " << memoizedTypeIds[name] << std::endl;

    return h; // or return h % C;
}