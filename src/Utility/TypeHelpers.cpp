#include "../include/Utility/TypeHelpers.h"

#include <string>

blz::TypeId blz::generateTypeId(const char *typeName)
{
    const unsigned int A = 54059; /* a prime */
    const unsigned int B = 76963; /* another prime */

    std::string name(typeName);

    unsigned int h = 37;

    for (auto &c : name)
    {
        h = (h * A) ^ (c * B);
    }

    // std::cout << "Generating type id" << std::endl;
    // std::cout << "name: " << name << std::endl;
    // std::cout << "hash: " << h << std::endl;

    return h; // or return h % C;
}