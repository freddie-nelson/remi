#include "../../include/ECS/Registry.h"

ECS::Registry::Registry(size_t maxEntities) : maxEntities(maxEntities)
{
    // create entity ids
    for (Entity entity = 0; entity < maxEntities; entity++)
    {
        freeEntityIds.push(entity);
    }
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
    if (freeEntityIds.empty())
    {
        throw std::runtime_error("Registry (create): no more entity ids available.");
    }

    auto entity = freeEntityIds.front();
    freeEntityIds.pop();

    entities.push_back(entity);
    entitiesSet.insert(entity);

    return entity;
}

void ECS::Registry::destroy(Entity entity)
{
    if (!has(entity))
    {
        throw std::runtime_error("Registry (destroy): entity does not exist.");
    }

    // remove entity from entities vector
    for (auto it = entities.begin(); it != entities.end(); it++)
    {
        if (*it == entity)
        {
            entities.erase(it);
            break;
        }
    }

    // remove entity from entities set
    entitiesSet.erase(entity);

    // remove entity and its components from component pools
    for (auto &pair : componentPools)
    {
        if (pair.second->has(entity))
        {
            pair.second->remove(entity);
            invalidateCachedViews(pair.first);
        }
    }

    freeEntityIds.push(entity);
}

void ECS::Registry::destroy(const std::vector<Entity> &entities)
{
    for (auto &entity : entities)
    {
        destroy(entity);
    }
}

void ECS::Registry::destroyAll()
{
    entities.clear();
    entitiesSet.clear();

    for (auto &pair : componentPools)
    {
        delete pair.second;
    }

    componentPools.clear();
}

bool ECS::Registry::has(Entity entity) const
{
    return entitiesSet.contains(entity);
}

const std::vector<ECS::Entity> &ECS::Registry::getEntities() const
{
    return entities;
}

size_t ECS::Registry::size() const
{
    return entities.size();
}