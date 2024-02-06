#include "../../include/Physics/Collider2D.h"

Physics::PolygonCollider2D::PolygonCollider2D(std::vector<glm::vec2> vertices)
{
    type = ColliderShapeType::POLYGON;
    this->vertices = vertices;
}

Physics::CircleCollider2D::CircleCollider2D(float radius, glm::vec2 centre)
{
    type = ColliderShapeType::CIRCLE;
    this->radius = radius;
    this->centre = centre;
}

Physics::EdgeCollider2D::EdgeCollider2D(glm::vec2 start, glm::vec2 end)
{
    type = ColliderShapeType::EDGE;
    this->start = start;
    this->end = end;

    oneSided = false;
}

Physics::EdgeCollider2D::EdgeCollider2D(glm::vec2 adjacentStart, glm::vec2 start, glm::vec2 end, glm::vec2 adjacentEnd)
{
    type = ColliderShapeType::EDGE;
    this->adjacentStart = adjacentStart;
    this->start = start;
    this->end = end;
    this->adjacentEnd = adjacentEnd;

    oneSided = false;
}

Physics::ChainCollider2D::ChainCollider2D(std::vector<glm::vec2> vertices, bool isLoop)
{
    type = ColliderShapeType::CHAIN;
    this->vertices = vertices;

    this->isLoop = isLoop;
    hasAdjacentVertices = false;
}

Physics::ChainCollider2D::ChainCollider2D(glm::vec2 adjacentStart, std::vector<glm::vec2> vertices, glm::vec2 adjacentEnd)
{
    type = ColliderShapeType::CHAIN;
    this->adjacentStart = adjacentStart;
    this->vertices = vertices;
    this->adjacentEnd = adjacentEnd;

    isLoop = false;
    hasAdjacentVertices = true;
}

Physics::Collider2D::Collider2D()
{
    shape = nullptr;
    density = 1.0f;
    friction = 0.0f;
    restitution = 0.0f;
    restitutionThreshold = 0.0f;
    isSensor = false;
}
