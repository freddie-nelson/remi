#include "../../include/Physics/Collider2D.h"

#include <box2d/b2_polygon_shape.h>
#include <box2d/b2_circle_shape.h>
#include <box2d/b2_edge_shape.h>
#include <box2d/b2_chain_shape.h>

Physics::PolygonCollider2D::PolygonCollider2D(std::vector<glm::vec2> vertices)
{
    type = ColliderShapeType::POLYGON;
    this->vertices = vertices;
}

b2Shape *Physics::PolygonCollider2D::createBox2DShape() const
{
    b2PolygonShape *shape = new b2PolygonShape();
    b2Vec2 b2Vertices[vertices.size()];

    for (size_t i = 0; i < vertices.size(); i++)
    {
        b2Vertices[i].Set(vertices[i].x, vertices[i].y);
    }

    shape->Set(b2Vertices, vertices.size());

    return shape;
}

Physics::ColliderShape2D *Physics::PolygonCollider2D::clone() const
{
    return new PolygonCollider2D(*this);
}

Physics::CircleCollider2D::CircleCollider2D(float radius, glm::vec2 centre)
{
    type = ColliderShapeType::CIRCLE;
    this->radius = radius;
    this->centre = centre;
}

b2Shape *Physics::CircleCollider2D::createBox2DShape() const
{
    b2CircleShape *shape = new b2CircleShape();
    shape->m_p.Set(centre.x, centre.y);
    shape->m_radius = radius;

    return shape;
}

Physics::ColliderShape2D *Physics::CircleCollider2D::clone() const
{
    return new CircleCollider2D(*this);
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

b2Shape *Physics::EdgeCollider2D::createBox2DShape() const
{
    b2EdgeShape *shape = new b2EdgeShape();
    if (oneSided)
    {
        shape->SetOneSided(b2Vec2(adjacentStart.x, adjacentStart.y), b2Vec2(start.x, start.y), b2Vec2(end.x, end.y), b2Vec2(adjacentEnd.x, adjacentEnd.y));
    }
    else
    {
        shape->SetTwoSided(b2Vec2(start.x, start.y), b2Vec2(end.x, end.y));
    }

    return shape;
}

Physics::ColliderShape2D *Physics::EdgeCollider2D::clone() const
{
    return new EdgeCollider2D(*this);
}

Physics::ChainCollider2D::ChainCollider2D(std::vector<glm::vec2> vertices)
{
    type = ColliderShapeType::CHAIN;
    this->vertices = vertices;
    isLoop = true;
}

Physics::ChainCollider2D::ChainCollider2D(glm::vec2 adjacentStart, std::vector<glm::vec2> vertices, glm::vec2 adjacentEnd)
{
    type = ColliderShapeType::CHAIN;
    this->adjacentStart = adjacentStart;
    this->vertices = vertices;
    this->adjacentEnd = adjacentEnd;
    isLoop = false;
}

b2Shape *Physics::ChainCollider2D::createBox2DShape() const
{
    b2ChainShape *shape = new b2ChainShape();
    b2Vec2 b2Vertices[vertices.size()];

    for (size_t i = 0; i < vertices.size(); i++)
    {
        b2Vertices[i].Set(vertices[i].x, vertices[i].y);
    }

    if (isLoop)
    {
        shape->CreateLoop(b2Vertices, vertices.size());
    }
    else
    {
        shape->CreateChain(b2Vertices, vertices.size(), b2Vec2(adjacentStart.x, adjacentStart.y), b2Vec2(adjacentEnd.x, adjacentEnd.y));
    }

    return shape;
}

Physics::ColliderShape2D *Physics::ChainCollider2D::clone() const
{
    return new ChainCollider2D(*this);
}

Physics::Collider2D::Collider2D(ColliderShape2D *shape)
{
    this->shape = shape->clone();
    density = 1.0f;
    friction = 0.0f;
    restitution = 0.0f;
    restitutionThreshold = 0.0f;
    isSensor = false;
}

const Physics::ColliderShape2D *Physics::Collider2D::getShape() const
{
    return shape;
}

void Physics::Collider2D::setShape(ColliderShape2D *shape)
{
    this->shape = shape->clone();
    shapeDirty = true;
}

bool Physics::Collider2D::isShapeDirty() const
{
    return shapeDirty;
}

void Physics::Collider2D::informOfShapeUpdate()
{
    shapeDirty = false;
}
