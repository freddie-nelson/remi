#include "../../include/Physics/ColliderShape.h"

#include <box2d/b2_polygon_shape.h>
#include <box2d/b2_circle_shape.h>
#include <box2d/b2_edge_shape.h>
#include <box2d/b2_chain_shape.h>

Physics::PolygonColliderShape2D::PolygonColliderShape2D(std::vector<glm::vec2> vertices)
{
    type = ColliderShapeType::POLYGON;
    this->vertices = vertices;
}

Physics::PolygonColliderShape2D::PolygonColliderShape2D(const Rendering::Mesh2D &mesh)
{
    type = ColliderShapeType::POLYGON;
    vertices = mesh.getVertices();
}

Physics::PolygonColliderShape2D::PolygonColliderShape2D(const Rendering::Mesh2D &mesh, const Core::Transform &transform)
{
    type = ColliderShapeType::POLYGON;
    vertices = mesh.transform(transform).getVertices();
}

b2Shape *Physics::PolygonColliderShape2D::createBox2DShape() const
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

Physics::ColliderShape2D *Physics::PolygonColliderShape2D::clone() const
{
    return new PolygonColliderShape2D(*this);
}

Physics::CircleColliderShape2D::CircleColliderShape2D(float radius, glm::vec2 centre)
{
    type = ColliderShapeType::CIRCLE;
    this->radius = radius;
    this->centre = centre;
}

b2Shape *Physics::CircleColliderShape2D::createBox2DShape() const
{
    b2CircleShape *shape = new b2CircleShape();
    shape->m_p.Set(centre.x, centre.y);
    shape->m_radius = radius;

    return shape;
}

Physics::ColliderShape2D *Physics::CircleColliderShape2D::clone() const
{
    return new CircleColliderShape2D(*this);
}

Physics::EdgeColliderShape2D::EdgeColliderShape2D(glm::vec2 start, glm::vec2 end)
{
    type = ColliderShapeType::EDGE;
    this->start = start;
    this->end = end;

    oneSided = false;
}

Physics::EdgeColliderShape2D::EdgeColliderShape2D(glm::vec2 adjacentStart, glm::vec2 start, glm::vec2 end, glm::vec2 adjacentEnd)
{
    type = ColliderShapeType::EDGE;
    this->adjacentStart = adjacentStart;
    this->start = start;
    this->end = end;
    this->adjacentEnd = adjacentEnd;

    oneSided = false;
}

b2Shape *Physics::EdgeColliderShape2D::createBox2DShape() const
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

Physics::ColliderShape2D *Physics::EdgeColliderShape2D::clone() const
{
    return new EdgeColliderShape2D(*this);
}

Physics::ChainColliderShape2D::ChainColliderShape2D(std::vector<glm::vec2> vertices)
{
    type = ColliderShapeType::CHAIN;
    this->vertices = vertices;
    isLoop = true;
}

Physics::ChainColliderShape2D::ChainColliderShape2D(glm::vec2 adjacentStart, std::vector<glm::vec2> vertices, glm::vec2 adjacentEnd)
{
    type = ColliderShapeType::CHAIN;
    this->adjacentStart = adjacentStart;
    this->vertices = vertices;
    this->adjacentEnd = adjacentEnd;
    isLoop = false;
}

b2Shape *Physics::ChainColliderShape2D::createBox2DShape() const
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

Physics::ColliderShape2D *Physics::ChainColliderShape2D::clone() const
{
    return new ChainColliderShape2D(*this);
}