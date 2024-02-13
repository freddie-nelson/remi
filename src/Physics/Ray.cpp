#include "../../include/Physics/Ray.h"

#include <stdexcept>

Physics::Ray::Ray(glm::vec2 origin, glm::vec2 direction, float length)
{
    start = origin;
    end = origin + (direction * length);
    this->length = length;
}

Physics::Ray::Ray(glm::vec2 start, glm::vec2 end)
{
    this->start = start;
    this->end = end;
    length = glm::length(end - start);
}

Physics::RaycastCallback::RaycastCallback(const Ray &ray, RaycastType type, const std::unordered_map<b2Body *, ECS::Entity> &bodyToEntity) : ray(ray), type(type), bodyToEntity(bodyToEntity)
{
}

float Physics::RaycastCallback::ReportFixture(b2Fixture *fixture, const b2Vec2 &point, const b2Vec2 &normal, float fraction)
{
    auto body = fixture->GetBody();

    if (!bodyToEntity.contains(body))
    {
        throw std::runtime_error("RaycastCallback (ReportFixture): Fixture does not have a body attached.");
    }
    auto entity = bodyToEntity.at(body);

    RaycastHit hit{
        entity,
        glm::vec2(point.x, point.y),
        glm::vec2(normal.x, normal.y),
        ray.length * fraction,
        fraction,
    };

    switch (type)
    {
    case RaycastType::CLOSEST:
        if (hits.empty() || fraction < hits[0].fraction)
        {
            hits.clear();
            hits.emplace_back(std::move(hit));
        }

        return fraction;
    case RaycastType::ANY:
        hits.emplace_back(std::move(hit));
        return 0.0f;
    case RaycastType::ALL:
        hits.emplace_back(std::move(hit));
        return 1.0f;
    default:
        throw std::runtime_error("RaycastCallback (ReportFixture): Unknown raycast type.");
    }

    // Should never reach
    return 1.0f;
}

const std::vector<Physics::RaycastHit> &Physics::RaycastCallback::getHits() const
{
    return hits;
}