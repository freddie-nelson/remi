#include "../../include/Core/AABB.h"

#include <stdexcept>

Core::AABB::AABB()
{
    setMin(glm::vec2(0.0f, 0.0f));
    setMax(glm::vec2(0.0f, 0.0f));
}

Core::AABB::AABB(glm::vec2 min, glm::vec2 max)
{
    setMin(min);
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

    setMin(glm::vec2(minX, minY));
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

void Core::AABB::updateCentre()
{
    centre = (min + max) / 2.0f;
}