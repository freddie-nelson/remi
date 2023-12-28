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

Core::AABB::AABB(std::vector<glm::vec2> points)
{
    setFromPoints(points);
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

void Core::AABB::setFromPoints(std::vector<glm::vec2> points)
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

unsigned int Core::AABB::getWidth() const
{
    return max.x - min.x;
}

unsigned int Core::AABB::getHeight() const
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
    float minX = std::min(min.x, other.min.x);
    float minY = std::min(min.y, other.min.y);
    float maxX = std::max(max.x, other.max.x);
    float maxY = std::max(max.y, other.max.y);

    min = glm::vec2(minX, minY);
    max = glm::vec2(maxX, maxY);

    return AABB(min, max);
}

bool Core::AABB::contains(const AABB &other) const
{
    auto otherMin = other.getMin();
    auto otherMax = other.getMax();

    return otherMin.x >= min.x && otherMax.x <= max.x && otherMin.y >= min.y && otherMax.y <= max.y;
}

bool Core::AABB::overlaps(const AABB &other) const
{
    auto otherMin = other.getMin();
    auto otherMax = other.getMax();

    return max.x > otherMin.x && min.x < otherMax.x && max.y > otherMin.y && min.y < otherMax.y;
}

void Core::AABB::updateCentre()
{
    centre = (min + max) / 2.0f;
}

void Core::AABB::updateSurfaceArea()
{
    surfaceArea = getWidth() * getHeight();
}