#include "../../include/ECS/Component.h"
#include <iostream>
#include <string>
#include <unordered_map>

ECS::ComponentId ECS::generate(const char *typeName)
{
    const unsigned int A = 54059; /* a prime */
    const unsigned int B = 76963; /* another prime */

    std::string name(typeName);

    unsigned int h = 37;

    for (auto &c : name)
    {
        h = (h * A) ^ (c * B);
    }

    std::cout << "Generating component id" << std::endl;
    std::cout << "name: " << name << std::endl;
    std::cout << "hash: " << h << std::endl;

    return h; // or return h % C;
}