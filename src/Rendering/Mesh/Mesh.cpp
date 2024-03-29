#include "../../../include/Rendering/Mesh/Mesh.h"
#include "../../../include/Rendering/Mesh/Polygons.h"
#include "../../../include/Config.h"
#include "include/Core/Transform.h"

#include <glm/gtc/matrix_transform.hpp>
#include <stdexcept>
#include <iostream>

Rendering::Mesh2D::Mesh2D()
{
    vertices = {
        glm::vec2{0.0f, 0.5f},
        glm::vec2{0.5f, -0.5f},
        glm::vec2{-0.5f, -0.5f},
    };

    indices = {0, 1, 2};

    uvs = {
        glm::vec2{0.5f, 1.0f},
        glm::vec2{1.0f, 0.0f},
        glm::vec2{0.0f, 0.0f},
    };

    aabb.setMin(glm::vec2{-0.5f, -0.5f});
    aabb.setMax(glm::vec2{0.5f, 0.5f});
}

Rendering::Mesh2D::Mesh2D(std::vector<glm::vec2> vertices, bool preserveCentre)
{
    createPolygon(vertices, preserveCentre);
}

Rendering::Mesh2D::Mesh2D(std::vector<glm::vec2> vertices, std::vector<unsigned int> indices, bool preserveCentre)
{
    setVertices(std::move(vertices));
    setIndices(std::move(indices));

    if (!preserveCentre)
        setCentre(glm::vec2(0.0f));
}

Rendering::Mesh2D::Mesh2D(float radius, unsigned int sides)
{
    createRegularPolygon(radius, sides);
}

Rendering::Mesh2D::Mesh2D(float width, float height)
{
    createRect(width, height);
}

Rendering::Mesh2D::Mesh2D(const glm::vec2 &start, const glm::vec2 &end, float thickness, bool centre)
{
    createLine(start, end, thickness, centre);
}

void Rendering::Mesh2D::createPolygon(const std::vector<glm::vec2> &vertices, bool preserveCentre)
{
    auto iv = Rendering::createPolygon(vertices, !preserveCentre);

    setVertices(std::move(iv.vertices));
    setIndices(std::move(iv.indices));
}

void Rendering::Mesh2D::createRegularPolygon(float radius, unsigned int sides)
{
    auto iv = Rendering::createRegularPolygon(radius, sides);

    vertices = std::move(iv.vertices);
    indices = std::move(iv.indices);

    aabb.setFromPoints(vertices);
    setUvsFromAABB();
}

void Rendering::Mesh2D::createRect(float width, float height)
{
    auto iv = Rendering::createRect(width, height);

    vertices = std::move(iv.vertices);
    indices = std::move(iv.indices);

    aabb.setFromPoints(vertices);
    setUvsFromAABB();
}

void Rendering::Mesh2D::createLine(const glm::vec2 &start, const glm::vec2 &end, float thickness, bool centre)
{
    if (thickness <= 0)
    {
        throw std::invalid_argument("Mesh (createLine): thickness must be greater than 0.");
    }

    if (start == end)
    {
        throw std::invalid_argument("Mesh (createLine): start and end cannot be the same.");
    }

    auto width = glm::length(end - start);
    auto iv = Rendering::createRect(width, thickness);

    // rotate the line to the correct angle
    auto angle = atan2(end.y - start.y, end.x - start.x);
    Core::Transform rotTransform;
    rotTransform.setRotation(angle);

    for (auto &v : iv.vertices)
    {
        v = glm::vec2(rotTransform.getTransformationMatrix() * glm::vec4(v, 0.0f, 1.0f));
    }

    vertices = std::move(iv.vertices);
    indices = std::move(iv.indices);

    aabb.setFromPoints(vertices);
    setUvsFromAABB();

    if (centre)
    {
        setCentre(glm::vec2(0));
    }
    else
    {
        setCentre(glm::vec2(0));
        setCentre(glm::vec2((start + end) / 2.0f));
    }
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
    // auto max = aabb.getMax();

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

const glm::vec2 &Rendering::Mesh2D::getCentre() const
{
    return aabb.getCentre();
}

void Rendering::Mesh2D::setCentre(const glm::vec2 &centre)
{
    auto oldCentre = aabb.getCentre();
    auto oldToNew = centre - oldCentre;

    // move each vertex by the difference between the old and new centre
    for (auto &v : vertices)
    {
        v += oldToNew;
    }

    aabb.setFromPoints(vertices);
}

Rendering::Mesh2D Rendering::Mesh2D::transform(const Core::Transform &transform) const
{
    auto newMesh = *this;

    auto &mat = transform.getTransformationMatrix();

    for (auto &v : newMesh.vertices)
    {
        v = glm::vec2(mat * glm::vec4(v, 0.0f, 1.0f));
    }

    newMesh.aabb.setFromPoints(newMesh.vertices);
    newMesh.setUvsFromAABB();

    return newMesh;
}