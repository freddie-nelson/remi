#include "../../include/ECS/Entity.h"

ECS::Entity ECS::createEntity()
{
    static Entity nextId = 0;
    return nextId++;
}