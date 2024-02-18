#include "../../include/World/World.h"

#include <stdexcept>

World::World::World(size_t maxEntities) : registry(ECS::Registry(maxEntities)), sceneGraph(Scene::SceneGraph(&registry))
{
}

World::World::~World()
{
}

void World::World::update(const Core::Timestep &timestep)
{
    for (auto system : systems)
    {
        system->update(*this, timestep);
    }

    sceneGraph.updateModelMatrices();
}

void World::World::fixedUpdate(const Core::Timestep &timestep)
{
    for (auto system : systems)
    {
        system->fixedUpdate(*this, timestep);
    }

    sceneGraph.updateModelMatrices();
}

const std::vector<World::System *> &World::World::getSystems()
{
    return systems;
}

bool World::World::addSystem(System *system)
{
    if (hasSystem(system))
    {
        return false;
    }

    if (system == nullptr)
    {
        throw std::invalid_argument("World (addSystem): system cannot be nullptr.");
    }

    systems.push_back(system);
    return true;
}

bool World::World::removeSystem(System *system)
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
            return true;
        }
    }

    return false;
}

bool World::World::hasSystem(System *system)
{
    for (auto s : systems)
    {
        if (s == system)
        {
            return true;
        }
    }

    return false;
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
