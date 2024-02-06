#include "../../include/Physics/PhysicsWorld.h"
#include "../../include/Physics/RigidBody2d.h"
#include "../../include/Core/Transform.h"

#include <unordered_set>

Physics::PhysicsWorld::PhysicsWorld(PhysicsWorldConfig config) : world(b2Vec2(config.gravity.x, config.gravity.y))
{
    setConfig(config);
}

Physics::PhysicsWorld::~PhysicsWorld()
{
    // TODO: implement
}

void Physics::PhysicsWorld::fixedUpdate(const ECS::Registry &registry, const Core::Timestep &timestep)
{
    updateBodies();
}

void Physics::PhysicsWorld::setConfig(PhysicsWorldConfig config)
{
    this->config = config;
    setGravity(config.gravity);
}

const Physics::PhysicsWorldConfig &Physics::PhysicsWorld::getConfig()
{
    return config;
}

void Physics::PhysicsWorld::setGravity(glm::vec2 gravity)
{
    config.gravity = gravity;

    b2Vec2 g(gravity.x, gravity.y);
    world.SetGravity(g);
}

glm::vec2 Physics::PhysicsWorld::getGravity()
{
    return config.gravity;
}

void Physics::PhysicsWorld::updateBodies(const ECS::Registry &registry)
{
    auto entities = registry.view<Core::Transform, Physics::RigidBody2d>();
    std::unordered_set<ECS::Entity> entitySet(entities.begin(), entities.end());

    // check if any bodies need to be removed or added
    for (auto it = bodies.begin(); it != bodies.end();)
    {
        auto &entity = it->first;

        // entity is not in the registry or no longer a valid body
        if (!entitySet.contains(entity))
        {
            it = bodies.erase(it);
        }
        // remove the entity from the set if it is still a valid body
        else
        {
            entitySet.erase(entity);
            ++it;
        }
    }

    // now all bodies in the set are new bodies
    // so add them to world
    for (auto &e : entitySet)
    {
        // add body
    }
}
