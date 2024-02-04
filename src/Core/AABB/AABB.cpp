#include "../../../include/Core/AABB/AABB.h"

#include <stdexcept>

Core::AABB::AABB()
{
    min = glm::vec2(0.0f, 0.0f);
    setMax(glm::vec2(0.0f, 0.0f));
}

Core::AABB::AABB(glm::vec2 min, glm::vec2 max)
{
    this->min = min;
    setMax(max);
}

Core::AABB::AABB(const std::vector<glm::vec2> &points)
{
    setFromPoints(points);
}

Core::AABB::AABB(const glm::vec2 &centre, float radius)
{
    setFromCircle(centre, radius);
}

const glm::vec2 &Core::AABB::getMin() const
{
    return min;
}

void Core::AABB::setMin(glm::vec2 min)
{
    if (min.x > max.x || min.y > max.y)
    {
        throw std::invalid_argument("AABB (setMin): min must be less than max.");
    }

    this->min = min;
    updateCentre();
    updateSurfaceArea();
}

const glm::vec2 &Core::AABB::getMax() const
{
    return max;
}

void Core::AABB::setMax(glm::vec2 max)
{
    if (min.x > max.x || min.y > max.y)
    {
        throw std::invalid_argument("AABB (setMax): min must be less than max.");
    }

    this->max = max;
    updateCentre();
    updateSurfaceArea();
}

void Core::AABB::setFromPoints(const std::vector<glm::vec2> &points)
{
    if (points.size() == 0)
    {
        throw std::invalid_argument("AABB (setFromPoints): points must not be empty.");
    }

    float minX = points[0].x;
    float minY = points[0].y;
    float maxX = points[0].x;
    float maxY = points[0].y;

    for (auto &p : points)
    {
        if (p.x < minX)
        {
            minX = p.x;
        }

        if (p.y < minY)
        {
            minY = p.y;
        }

        if (p.x > maxX)
        {
            maxX = p.x;
        }

        if (p.y > maxY)
        {
            maxY = p.y;
        }
    }

    min = glm::vec2(minX, minY);
    setMax(glm::vec2(maxX, maxY));
}

void Core::AABB::setFromCircle(const glm::vec2 &centre, float radius)
{
    min = glm::vec2(centre.x - radius, centre.y - radius);
    setMax(glm::vec2(centre.x + radius, centre.y + radius));
}

float Core::AABB::getWidth() const
{
    return max.x - min.x;
}

float Core::AABB::getHeight() const
{
    return max.y - min.y;
}

const glm::vec2 &Core::AABB::getCentre() const
{
    return centre;
}

float Core::AABB::getSurfaceArea() const
{
    return surfaceArea;
}

Core::AABB Core::AABB::merge(const AABB &other)
{
    auto &otherMin = other.getMin();
    auto &otherMax = other.getMax();

    float minX = std::min(min.x, otherMin.x);
    float minY = std::min(min.y, otherMin.y);
    float maxX = std::max(max.x, otherMax.x);
    float maxY = std::max(max.y, otherMax.y);

    auto min = glm::vec2(minX, minY);
    auto max = glm::vec2(maxX, maxY);

    return AABB(min, max);
}

bool Core::AABB::contains(const AABB &other) const
{
    auto &otherMin = other.getMin();
    auto &otherMax = other.getMax();

    return otherMin.x >= min.x && otherMax.x <= max.x && otherMin.y >= min.y && otherMax.y <= max.y;
}

bool Core::AABB::overlaps(const AABB &other) const
{
    auto &otherMin = other.getMin();
    auto &otherMax = other.getMax();

    return max.x >= otherMin.x && min.x <= otherMax.x && max.y >= otherMin.y && min.y <= otherMax.y;
}

Core::AABB Core::AABB::transform(const Core::Transform &t) const
{
    auto m = t.getTransformationMatrix();
    auto tMin = m * glm::vec4(min, 0, 1);
    auto tMax = m * glm::vec4(max, 0, 1);

    return AABB(glm::vec2(tMin), glm::vec2(tMax));
}

void Core::AABB::scale(const glm::vec2 &scale)
{
    min *= scale;
    max *= scale;

    updateCentre();
    updateSurfaceArea();
}

void Core::AABB::scale(float scale)
{
    min *= scale;
    max *= scale;

    updateCentre();
    updateSurfaceArea();
}

void Core::AABB::updateCentre()
{
    centre = (min + max) / 2.0f;
}

void Core::AABB::updateSurfaceArea()
{
    surfaceArea = getWidth() * getHeight();
}