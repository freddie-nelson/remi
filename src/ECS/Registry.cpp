#include "../../include/ECS/Registry.h"

ECS::Registry::Registry()
{
}

ECS::Registry::~Registry()
{
    for (auto &pair : componentPools)
    {
        delete pair.second;
    }
}

ECS::Entity ECS::Registry::create()
{
    auto entity = createEntity();
    entities.push_back(entity);

    return entity;
}

void ECS::Registry::destroy(Entity &entity)
{
    // remove entity from entities vector
    for (auto it = entities.begin(); it != entities.end(); it++)
    {
        if (*it == entity)
        {
            entities.erase(it);
            break;
        }
    }

    // remove entity and its components from component pools
    for (auto &pair : componentPools)
    {
        pair.second->remove(entity);
    }
}

void ECS::Registry::destroyAll()
{
    entities.clear();

    for (auto &pair : componentPools)
    {
        delete pair.second;
    }

    componentPools.clear();
}
