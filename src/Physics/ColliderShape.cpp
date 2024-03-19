#include "../../include/Physics/ColliderShape.h"
#include "include/Rendering/Mesh/Triangulate.h"

#include <box2d/b2_polygon_shape.h>
#include <box2d/b2_circle_shape.h>
#include <box2d/b2_edge_shape.h>
#include <box2d/b2_chain_shape.h>

//
// ColliderShape2D
//

Physics::ColliderShapeType Physics::ColliderShape2D::getType() const
{
    return type;
}

//
// PolygonColliderShape2D
//

Physics::PolygonColliderShape2D::PolygonColliderShape2D(std::vector<glm::vec2> vertices)
{
    if (vertices.size() < 3)
        throw std::invalid_argument("PolygonColliderShape2D: A polygon must have at least 3 vertices.");
    if (vertices.size() > 8)
        throw std::invalid_argument("PolygonColliderShape2D: A polygon must have at most 8 vertices.");

    type = ColliderShapeType::POLYGON;
    this->vertices = vertices;
}

Physics::PolygonColliderShape2D::PolygonColliderShape2D(const Rendering::Mesh2D &mesh)
{
    type = ColliderShapeType::POLYGON;
    vertices = mesh.getVertices();

    if (vertices.size() < 3)
        throw std::invalid_argument("PolygonColliderShape2D: A polygon must have at least 3 vertices.");
    if (vertices.size() > 8)
        throw std::invalid_argument("PolygonColliderShape2D: A polygon must have at most 8 vertices.");
}

Physics::PolygonColliderShape2D::PolygonColliderShape2D(const Rendering::Mesh2D &mesh, const Core::Transform &transform)
{
    type = ColliderShapeType::POLYGON;
    vertices = mesh.transform(transform).getVertices();

    if (vertices.size() < 3)
        throw std::invalid_argument("PolygonColliderShape2D: A polygon must have at least 3 vertices.");
    if (vertices.size() > 8)
        throw std::invalid_argument("PolygonColliderShape2D: A polygon must have at most 8 vertices.");
}

Physics::PolygonColliderShape2D::PolygonColliderShape2D(const PolygonColliderShape2D &other)
{
    type = other.type;
    vertices = other.vertices;
}

b2PolygonShape *Physics::PolygonColliderShape2D::createBox2DShape() const
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

//
// ConcavePolygonColliderShape2D
//

Physics::ConcavePolygonColliderShape2D::ConcavePolygonColliderShape2D(std::vector<glm::vec2> vertices)
{
    type = ColliderShapeType::CONCAVE_POLYGON;

    auto iv = Rendering::triangulate(vertices);

    vertices = std::move(iv.vertices);
    indices = std::move(iv.indices);
}

Physics::ConcavePolygonColliderShape2D::ConcavePolygonColliderShape2D(const Rendering::Mesh2D &mesh)
{
    type = ColliderShapeType::CONCAVE_POLYGON;

    vertices = mesh.getVertices();
    indices = mesh.getIndices();
}

Physics::ConcavePolygonColliderShape2D::ConcavePolygonColliderShape2D(const Rendering::Mesh2D &mesh, const Core::Transform &transform)
{
    type = ColliderShapeType::CONCAVE_POLYGON;

    auto transformedMesh = mesh.transform(transform);

    vertices = transformedMesh.getVertices();
    indices = transformedMesh.getIndices();
}

Physics::ConcavePolygonColliderShape2D::ConcavePolygonColliderShape2D(const ConcavePolygonColliderShape2D &other)
{
    type = other.type;
    vertices = other.vertices;
    indices = other.indices;
}

b2PolygonShape *Physics::ConcavePolygonColliderShape2D::createBox2DShape() const
{
    auto triCount = getShapeCount();
    b2PolygonShape *shapes = new b2PolygonShape[triCount];

    for (size_t i = 0; i < triCount; i++)
    {
        auto &shape = shapes[i];

        b2Vec2 b2Vertices[3];
        for (size_t j = 0; j < 3; j++)
        {
            auto index = indices[i * 3 + j];
            auto vertex = vertices[index];
            b2Vertices[j].Set(vertex.x, vertex.y);
        }

        shape.Set(b2Vertices, 3);
    }

    return shapes;
}

Physics::ColliderShape2D *Physics::ConcavePolygonColliderShape2D::clone() const
{
    return new ConcavePolygonColliderShape2D(*this);
}

size_t Physics::ConcavePolygonColliderShape2D::getShapeCount() const
{
    return indices.size() / 3;
}

//
// CircleColliderShape2D
//

Physics::CircleColliderShape2D::CircleColliderShape2D(float radius, glm::vec2 centre)
{
    type = ColliderShapeType::CIRCLE;
    this->radius = radius;
    this->centre = centre;
}

Physics::CircleColliderShape2D::CircleColliderShape2D(const CircleColliderShape2D &other)
{
    type = other.type;
    radius = other.radius;
    centre = other.centre;
}

b2CircleShape *Physics::CircleColliderShape2D::createBox2DShape() const
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

//
// EdgeColliderShape2D
//

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

Physics::EdgeColliderShape2D::EdgeColliderShape2D(const EdgeColliderShape2D &other)
{
    type = other.type;
    start = other.start;
    end = other.end;
    adjacentStart = other.adjacentStart;
    adjacentEnd = other.adjacentEnd;
    oneSided = other.oneSided;
}

b2EdgeShape *Physics::EdgeColliderShape2D::createBox2DShape() const
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

//
// ChainColliderShape2D
//

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

Physics::ChainColliderShape2D::ChainColliderShape2D(const ChainColliderShape2D &other)
{
    type = other.type;
    vertices = other.vertices;
    adjacentStart = other.adjacentStart;
    adjacentEnd = other.adjacentEnd;
    isLoop = other.isLoop;
}

b2ChainShape *Physics::ChainColliderShape2D::createBox2DShape() const
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