#include "../../include/World/World.h"

#include <stdexcept>

World::World::World(size_t maxEntities) : registry(ECS::Registry(maxEntities)), sceneGraph(Scene::SceneGraph(&registry))
{
}

World::World::~World()
{
}

void World::World::update(const ECS::System::SystemUpdateData &data)
{
    // create a copy of the systems vector to avoid issues with systems being added or removed during the loop
    auto systemsCopy = systems;

    for (auto system : systemsCopy)
    {
        // system may have been removed during the loop
        if (!hasSystem(system))
        {
            continue;
        }

        system->update(data);
    }

    sceneGraph.updateModelMatrices();
}

void World::World::fixedUpdate(const ECS::System::SystemUpdateData &data)
{
    // create a copy of the systems vector to avoid issues with systems being added or removed during the loop
    auto systemsCopy = systems;

    for (auto system : systemsCopy)
    {
        // system may have been removed during the loop
        if (!hasSystem(system))
        {
            continue;
        }

        system->fixedUpdate(data);
    }

    sceneGraph.updateModelMatrices();
}

const std::vector<ECS::System *> &World::World::getSystems()
{
    return systems;
}

bool World::World::addSystem(ECS::System *system)
{
    if (hasSystem(system))
    {
        return false;
    }

    if (system == nullptr)
    {
        throw std::invalid_argument("World (addSystem): system cannot be nullptr.");
    }

    systemsSet.emplace(system);
    systems.push_back(system);
    return true;
}

bool World::World::removeSystem(ECS::System *system)
{
    if (!hasSystem(system))
    {
        return false;
    }

    for (auto it = systems.begin(); it != systems.end(); it++)
    {
        if (*it == system)
        {
            systems.erase(it);
            systemsSet.erase(system);
            return true;
        }
    }

    return false;
}

bool World::World::hasSystem(ECS::System *system)
{
    return systemsSet.contains(system);
}

ECS::Registry &World::World::getRegistry()
{
    return registry;
}

const ECS::Registry &World::World::getRegistry() const
{
    return registry;
}

Scene::SceneGraph &World::World::getSceneGraph()
{
    return sceneGraph;
}

const Scene::SceneGraph &World::World::getSceneGraph() const
{
    return sceneGraph;
}
