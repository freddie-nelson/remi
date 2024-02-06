#include "../../include/Physics/PhysicsWorld.h"
#include "../../include/Physics/RigidBody2D.h"
#include "../../include/Physics/Collider2D.h"
#include "../../include/Core/Transform.h"

#include <box2d/b2_body.h>
#include <box2d/b2_polygon_shape.h>
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
    updateBodies(registry);
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
    auto entities = registry.view<Core::Transform, Physics::RigidBody2D>();
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
    b2BodyDef bodyDef;

    for (auto &e : entitySet)
    {
        // add body
        auto &transform = registry.get<Core::Transform>(e);
        auto &body = registry.get<Physics::RigidBody2D>(e);

        // set body def
        bodyDef.position.Set(transform.getTranslation().x, transform.getTranslation().y);
        bodyDef.angle = transform.getRotation();

        bodyDef.type = static_cast<b2BodyType>(body.type);

        bodyDef.linearVelocity.x = body.velocity.x;
        bodyDef.linearVelocity.y = body.velocity.y;

        bodyDef.angularVelocity = body.angularVelocity;

        bodyDef.linearDamping = body.linearDamping;
        bodyDef.angularDamping = body.angularDamping;

        bodyDef.allowSleep = body.allowSleep;
        bodyDef.awake = body.isInitiallyAwake;

        bodyDef.fixedRotation = body.fixedRotation;

        bodyDef.bullet = body.isBullet;

        bodyDef.enabled = body.startsEnabled;

        bodyDef.gravityScale = body.gravityScale;

        // create body
        bodies[e] = world.CreateBody(&bodyDef);

        // create collider
        if (registry.has<Physics::Collider2D>(e))
        {
            auto &collider = registry.get<Physics::Collider2D>(e);

            switch (collider.shape->type)
            {
            }

            // create b2 vertices
            auto b2Vertices = std::vector<b2Vec2>();
            b2Vertices.reserve(vertices.size());

            for (auto &v : vertices)
            {
                b2Vertices.push_back(b2Vec2(v.x, v.y));
            }

            shape.Set(b2Vertices.data(), b2Vertices.size())
        }
    }
}
