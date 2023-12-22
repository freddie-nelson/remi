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

Rendering::Mesh2D::Mesh2D(float radius, unsigned int sides)
{
    createRegularPolygon(radius, sides);
}

Rendering::Mesh2D::Mesh2D(float width, float height)
{
    createRect(width, height);
}

void Rendering::Mesh2D::createPolygon(std::vector<glm::vec2> vertices)
{
    auto iv = Rendering::createPolygon(vertices);

    vertices = iv.vertices;
    indices = iv.indices;

    aabb.setFromPoints(vertices);
    setUvsFromAABB();
}

void Rendering::Mesh2D::createRegularPolygon(float radius, unsigned int sides)
{
    auto iv = Rendering::createRegularPolygon(radius, sides);

    vertices = iv.vertices;
    indices = iv.indices;

    aabb.setFromPoints(vertices);
    setUvsFromAABB();
}

void Rendering::Mesh2D::createRect(float width, float height)
{
    auto iv = Rendering::createRect(width, height);

    vertices = iv.vertices;
    indices = iv.indices;

    aabb.setFromPoints(vertices);
    setUvsFromAABB();
}

void Rendering::Mesh2D::setVertices(std::vector<glm::vec2> vertices)
{
    if (vertices.size() < 3)
    {
        throw std::invalid_argument("Mesh (setVertices): vertices must have at least 3 vertices.");
    }

    this->vertices = vertices;

    aabb.setFromPoints(vertices);
    if (!hasCustomUvs)
        setUvsFromAABB();
}

const std::vector<glm::vec2> &Rendering::Mesh2D::getVertices() const
{
    return vertices;
}

void Rendering::Mesh2D::setIndices(std::vector<unsigned int> indices)
{
    if (indices.size() < 3)
    {
        throw std::invalid_argument("Mesh (setIndices): indices must have at least 3 indices.");
    }

    this->indices = indices;
}

const std::vector<unsigned int> &Rendering::Mesh2D::getIndices() const
{
    return indices;
}

void Rendering::Mesh2D::setUvs(std::vector<glm::vec2> uvs)
{
    if (uvs.size() != vertices.size())
    {
        throw std::invalid_argument("Mesh (setUvs): uvs must have the same size as vertices.");
    }

    hasCustomUvs = true;
    this->uvs = uvs;
}

void Rendering::Mesh2D::setUvsFromAABB()
{
    auto width = aabb.getWidth();
    auto height = aabb.getHeight();
    auto min = aabb.getMin();
    auto max = aabb.getMax();

    uvs.clear();

    for (auto &v : vertices)
    {
        auto x = width != 0 ? (v.x - min.x) / width : 0;
        auto y = height != 0 ? (v.y - min.y) / height : 0;

        uvs.push_back(glm::vec2(x, y));
    }
}

const std::vector<glm::vec2> &Rendering::Mesh2D::getUvs() const
{
    return uvs;
}

const Core::AABB &Rendering::Mesh2D::getAABB() const
{
    return aabb;
}
