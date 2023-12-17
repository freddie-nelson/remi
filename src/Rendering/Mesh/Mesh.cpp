#include "../../../include/Rendering/Mesh/Mesh.h"
#include "../../../include/Rendering/Mesh/Polygons.h"
#include "../../../include/Config.h"

#include <glm/gtc/matrix_transform.hpp>
#include <stdexcept>

Rendering::Mesh2D::Mesh2D()
{
}

Rendering::Mesh2D::Mesh2D(std::vector<glm::vec2> vertices)
{
    createPolygon(vertices);
}

Rendering::Mesh2D::Mesh2D(unsigned int sides, float radius)
{
    createRegularPolygon(sides, radius);
}

Rendering::Mesh2D::Mesh2D(float width, float height)
{
    createRect(width, height);
}

void Rendering::Mesh2D::createPolygon(std::vector<glm::vec2> vertices)
{
    auto iv = Rendering::createPolygon(vertices);

    this->vertices = iv.vertices;
    this->indices = iv.indices;
}

void Rendering::Mesh2D::createRegularPolygon(unsigned int sides, float radius)
{
    auto iv = Rendering::createRegularPolygon(sides, radius);

    this->vertices = iv.vertices;
    this->indices = iv.indices;
}

void Rendering::Mesh2D::createRect(float width, float height)
{
    auto iv = Rendering::createRect(width, height);

    this->vertices = iv.vertices;
    this->indices = iv.indices;
}

void Rendering::Mesh2D::setVertices(std::vector<glm::vec2> vertices)
{
    if (vertices.size() < 3)
    {
        throw std::runtime_error("vertices must have at least 3 vertices.");
    }

    this->vertices = vertices;
}

const std::vector<glm::vec2> &Rendering::Mesh2D::getVertices() const
{
    return vertices;
}

void Rendering::Mesh2D::setIndices(std::vector<unsigned int> indices)
{
    if (indices.size() < 3)
    {
        throw std::runtime_error("indices must have at least 3 indices.");
    }

    this->indices = indices;
}

const std::vector<unsigned int> &Rendering::Mesh2D::getIndices() const
{
    return indices;
}
