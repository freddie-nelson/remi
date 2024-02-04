#include "../../include/Core/BoundingCircle.h"

#include <stdexcept>

Core::BoundingCircle::BoundingCircle()
{
    centre = glm::vec2(0, 0);
    radius = 0.0f;
    sqrRadius = 0.0f;
}

Core::BoundingCircle::BoundingCircle(glm::vec2 centre, float radius)
{
    set(centre, radius);
}

Core::BoundingCircle::BoundingCircle(const AABB &aabb)
{
    set(aabb);
}

Core::BoundingCircle::BoundingCircle(const AABB &aabb, const Transform &transform)
{
    set(aabb, transform);
}

bool Core::BoundingCircle::intersects(const BoundingCircle &b) const
{
    auto toB = b.centre - centre;
    return glm::dot(toB, toB) < sqrRadius + b.getSqrRadius();
}

void Core::BoundingCircle::set(glm::vec2 centre, float radius)
{
    setCentre(centre);
    setRadius(radius);
}

void Core::BoundingCircle::set(const AABB &aabb)
{
    setCentre(aabb.getCentre());
    setRadius(glm::distance(aabb.getMin(), aabb.getMax()) / 2.0f);
}

void Core::BoundingCircle::set(const AABB &aabb, const Transform &transform)
{
    auto &t = transform.getTransformationMatrix();
    auto transformedMin = t * glm::vec4(aabb.getMin().x, aabb.getMin().y, 0, 1);
    auto transformedMax = t * glm::vec4(aabb.getMax().x, aabb.getMax().y, 0, 1);

    setCentre((transformedMin + transformedMax) / 2.0f);
    setRadius(glm::distance(transformedMin, transformedMax) / 2.0f);
}

void Core::BoundingCircle::setCentre(glm::vec2 centre)
{
    this->centre = centre;
}

glm::vec2 Core::BoundingCircle::getCentre() const
{
    return centre;
}

void Core::BoundingCircle::setRadius(float radius)
{
    if (radius < 0.0f)
        throw std::invalid_argument("Radius cannot be negative.");

    this->radius = radius;
    this->sqrRadius = radius * radius;
}

float Core::BoundingCircle::getRadius() const
{
    return radius;
}

float Core::BoundingCircle::getSqrRadius() const
{
    return sqrRadius;
}