#include "../../include/Rendering/Mesh.h"

#include <stdexcept>
#include <list>
#include <glm/glm.hpp>
#include <numbers>
#include <unordered_map>

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/hash.hpp"

struct TriangulationVertex
{
    glm::vec2 vertex;
    bool isConvex;
    bool isReflex;
    bool isEar;
};

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
    std::list<TriangulationVertex> verticesList;
    int numVertices = vertices.size();

    for (auto &vertex : vertices)
    {
        verticesList.push_back(TriangulationVertex{
            vertex : vertex,
            isConvex : false,
            isReflex : false,
            isEar : false
        });
    }

    while (numVertices > 3)
    {
        // find convex and reflex vertices
        findConvexAndReflexVertices(verticesList);

        // find ears
        findEars(verticesList);

        // find the first ear
        auto ear = verticesList.begin();
        while (!ear->isEar)
        {
            ear++;
        }

        // add ear and neighbours to mesh to create triangle
        mesh.vertices.push_back(ear->vertex);

        // prev vertex
        --ear;
        mesh.vertices.push_back(ear->vertex);
        ++ear;

        // next vertex
        ++ear;
        mesh.vertices.push_back(ear->vertex);
        --ear;

        // remove ear from list
        verticesList.erase(ear);

        // update number of vertices
        numVertices--;
    }

    // add last triangle
    for (auto &vertex : verticesList)
    {
        mesh.vertices.push_back(vertex.vertex);
    }

    // index vertices
    createIndexedVertices(mesh.vertices, mesh.indices);

    return mesh;
}

void Rendering::createIndexedVertices(std::vector<glm::vec2> &vertices, std::vector<unsigned int> &indices)
{
    // clear indices
    indices.clear();

    // create map of vertices
    std::unordered_map<glm::vec2, unsigned int> verticesMap;

    for (auto &v : vertices)
    {
        if (!verticesMap.contains(v))
        {
            verticesMap[v] = verticesMap.size();
        }
    }

    // create indexed vertices
    std::vector<glm::vec2> oldVertices;
    for (auto &v : vertices)
    {
        oldVertices.push_back(v);
    }

    // remove duplicate vertices
    vertices.clear();

    for (auto &v : verticesMap)
    {
        vertices.push_back(v.first);
    }

    // create indices
    for (auto &v : oldVertices)
    {
        indices.push_back(verticesMap[v]);
    }
}

/**
 * Finds the convex and reflex vertices of the given vertices.
 *
 * A vertex is convex if the it creates a convex angle with its neighbours.
 *
 * A convex angle is an angle that is less than 180 degrees.
 *
 * A vertex is reflex if it is not convex.
 *
 * A reflex angle is an angle that is greater than or equal to 180 degrees.
 *
 * @param vertices The vertices to find the convex vertices of.
 */
void findConvexAndReflexVertices(const std::list<TriangulationVertex> &vertices)
{
    for (auto it = vertices.begin(); it != vertices.end(); it++)
    {
        // get vertex and its neighbours
        auto v = *it;

        --it;
        auto prev = *it;
        ++it;

        ++it;
        auto next = *it;
        --it;

        // get angle between previous and next vertices
        // formula explained here: https://byjus.com/maths/angle-between-two-lines/
        float m1 = (v.vertex.y - prev.vertex.y) / (v.vertex.x - prev.vertex.x);
        float m2 = (next.vertex.y - v.vertex.y) / (next.vertex.x - v.vertex.x);

        float angle = std::atan((m2 - m1) / (1 + m1 * m2));

        // if angle is less than 180 degrees, then vertex is convex
        if (angle < std::numbers::pi)
        {
            v.isConvex = true;
            break;
        }
        else
        {
            v.isReflex = true;
            break;
        }

        v.isConvex = false;
        v.isReflex = false;
    }
}

/**
 * Finds the ears of the given vertices.
 *
 * @param vertices The vertices to find the ears of.
 */
void findEars(const std::list<TriangulationVertex> &vertices)
{
    for (auto it = vertices.begin(); it != vertices.end(); it++)
    {
        // get vertex and its neighbours
        auto v = *it;

        --it;
        auto prev = *it;
        ++it;

        ++it;
        auto next = *it;
        --it;

        // check if vertex is convex
        if (!v.isConvex)
        {
            continue;
        }

        // check if vertex is an ear tip
        // an ear is a vertex that creates a triangle with its neighbours that does not contain any other vertices on the polygon
        bool isEar = true;

        for (auto &check : vertices)
        {
            // skip if vertex is part of the ear
            if (&check == &v || &check == &prev || &check == &next)
            {
                continue;
            }

            if (inTriangle(check.vertex, prev.vertex, v.vertex, next.vertex))
            {
                isEar = false;
                break;
            }
        }

        v.isEar = isEar;
    }
}

/**
 * Checks if the given point is in the given triangle.
 *
 * The vertices of the triangle must be in counter-clockwise order.
 *
 * Adapted from [here](https://stackoverflow.com/questions/2049582/how-to-determine-if-a-point-is-in-a-2d-triangle)
 *
 * @param v The point to check.
 * @param a The first vertex of the triangle.
 * @param b The second vertex of the triangle.
 * @param c The third vertex of the triangle.
 *
 * @returns True if the point is in the triangle, false otherwise.
 */
bool inTriangle(glm::vec2 v, glm::vec2 a, glm::vec2 b, glm::vec2 c)
{
    float d1 = sign(v, a, b);
    float d2 = sign(v, b, c);
    float d3 = sign(v, c, a);

    bool hasNeg = (d1 < 0) || (d2 < 0) || (d3 < 0);
    bool hasPos = (d1 > 0) || (d2 > 0) || (d3 > 0);

    return !(hasNeg && hasPos);
}

float sign(glm::vec2 p1, glm::vec2 p2, glm::vec2 p3)
{
    return (p1.x - p3.x) * (p2.y - p3.y) - (p2.x - p3.x) * (p1.y - p3.y);
}