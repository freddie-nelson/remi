#include "../../include/Physics/ContactFilter.h"

Physics::ContactFilter::ContactFilter(const World::World &world, const std::unordered_map<b2Body *, ECS::Entity> &bodyToEntity)
    : world(world), bodyToEntity(bodyToEntity)
{
}

bool Physics::ContactFilter::ShouldCollide(b2Fixture *fixtureA, b2Fixture *fixtureB)
{
    if (callback == nullptr)
    {
        return true;
    }

    if (!bodyToEntity.contains(fixtureA->GetBody()) || !bodyToEntity.contains(fixtureB->GetBody()))
    {
        return true;
    }

    auto a = bodyToEntity.at(fixtureA->GetBody());
    auto b = bodyToEntity.at(fixtureB->GetBody());

    return callback(world, a, b);
}

void Physics::ContactFilter::setCallback(ContactFilterCallback callback)
{
    this->callback = callback;
}