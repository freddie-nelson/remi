#include "../../include/Physics/QueryCallback.h"

#include <stdexcept>

Physics::QueryCallback::QueryCallback(const Core::AABB &aabb, const std::unordered_map<b2Body *, ECS::Entity> &bodyToEntity) : aabb(aabb), bodyToEntity(bodyToEntity)
{
}

bool Physics::QueryCallback::ReportFixture(b2Fixture *fixture)
{
    auto body = fixture->GetBody();

    if (!bodyToEntity.contains(body))
    {
        throw std::runtime_error("QueryCallback (ReportFixture): Fixture does not have a body attached.");
    }

    auto entity = bodyToEntity.at(body);
    results.push_back(entity);

    return true;
}

const std::vector<ECS::Entity> &Physics::QueryCallback::getResults() const
{
    return results;
}