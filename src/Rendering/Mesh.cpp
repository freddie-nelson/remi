#include "../../include/Rendering/Mesh.h"

#include <stdexcept>
#include <list>
#include <glm/glm.hpp>
#include <numbers>

Rendering::Mesh2D Rendering::triangulate(const std::vector<glm::vec2> &vertices)
{
    if (vertices.size() < 3)
    {
        throw std::invalid_argument("Cannot triangulate a polygon with less than 3 vertices.");
    }

    Mesh2D mesh;

    // if the polygon is a triangle, just return it
    if (vertices.size() == 3)
    {
        for (int i = 0; i < vertices.size(); i++)
        {
            mesh.vertices.push_back(vertices[i]);
            mesh.indices.push_back(i);
        }

        return mesh;
    }

    // triangulate using ear clipping

    // copy vertices into doubly linked list
    std::list<glm::vec2> verticesList;
    int numVertices = vertices.size();

    for (auto &vertex : vertices)
    {
        verticesList.push_back(vertex);
    }
}

/**
 * Finds the convex vertices of the given vertices.
 *
 * A vertex is convex if the it creates a convex angle with its neighbours.
 *
 * A convex angle is an angle that is less than 180 degrees.
 *
 * @param vertices The vertices to find the convex vertices of.
 * @return The convex vertices of the given vertices.
 */
std::vector<glm::vec2> findConvexVertices(const std::vector<glm::vec2> &vertices)
{
    std::vector<glm::vec2> convexVertices;

    for (int i = 0; i < vertices.size(); i++)
    {
        auto v = vertices[i];
        auto prev = vertices[(i - 1) % vertices.size()];
        auto next = vertices[(i + 1) % vertices.size()];

        // get angle between previous and next vertices
        // formula explained here: https://byjus.com/maths/angle-between-two-lines/
        float m1 = (v.y - prev.y) / (v.x - prev.x);
        float m2 = (next.y - v.y) / (next.x - v.x);

        float angle = std::atan((m2 - m1) / (1 + m1 * m2));

        // if angle is less than 180 degrees, then vertex is convex
        if (angle < std::numbers::pi)
        {
            convexVertices.push_back(v);
        }
    }

    return convexVertices;
}