#include "../../include/Physics/QueryCallback.h"

#include "../../include/Physics/RigidBody2D.h"
#include "../../include/Physics/Collider2D.h"

#include <stdexcept>

Physics::QueryCallback::QueryCallback(const World::World &world, const Core::AABB &aabb, const std::unordered_map<b2Body *, ECS::Entity> &bodyToEntity) : world(world), aabb(aabb), bodyToEntity(bodyToEntity)
{
}

bool Physics::QueryCallback::ReportFixture(b2Fixture *fixture)
{
    auto body = fixture->GetBody();

    if (!bodyToEntity.contains(body))
    {
        throw std::runtime_error("QueryCallback (ReportFixture): Fixture does not have a body attached.");
    }

    auto &registry = world.getRegistry();

    auto entity = bodyToEntity.at(body);

    // entity is invalid but not removed from physics world yet
    if (registry.has<Physics::RigidBody2D>(entity) && registry.has<Physics::Collider2D>(entity))
    {
        results.push_back(entity);
    }

    return true;
}

const std::vector<ECS::Entity> &Physics::QueryCallback::getResults() const
{
    return results;
}