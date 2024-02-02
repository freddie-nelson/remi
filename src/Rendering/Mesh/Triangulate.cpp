#include "../../../include/Rendering/Mesh/Triangulate.h"
#include "../../../include/Rendering/Utility/GlmHelpers.h"

#include <stdexcept>
#include <list>
#include <glm/glm.hpp>
#include <numbers>
#include <unordered_map>
#include <iostream>

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/hash.hpp"

struct TriangulationVertex
{
    glm::vec2 vertex;
    bool isConvex;
    bool isReflex;
    bool isEar;
};

/**
 * Gets the neighbours of the given vertex.
 *
 * @param vertices The vertices to get the neighbours from.
 * @param it The iterator of the vertex to get the neighbours of.
 * @param prev The iterator for the previous neighbour of the vertex.
 * @param next The iterator for the next neighbour of the vertex.
 */
void getVertexNeighbours(std::list<TriangulationVertex> &vertices, std::list<TriangulationVertex>::iterator it, std::list<TriangulationVertex>::iterator &prev, std::list<TriangulationVertex>::iterator &next)
{
    --it;
    prev = it;
    ++it;

    ++it;
    next = it;
    --it;

    if (it == vertices.begin())
    {
        prev = --vertices.end();
    }

    if (it == --vertices.end())
    {
        next = vertices.begin();
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
void findConvexAndReflexVertices(std::list<TriangulationVertex> &vertices)
{
    for (auto it = vertices.begin(); it != vertices.end(); it++)
    {
        // get vertex and its neighbours
        auto v = it;
        auto prev = it;
        auto next = it;

        getVertexNeighbours(vertices, it, prev, next);

        glm::vec2 vToPrev = prev->vertex - v->vertex;
        glm::vec2 vToNext = next->vertex - v->vertex;

        float angle = std::acos(glm::dot(vToPrev, vToNext) / (glm::length(vToPrev) * glm::length(vToNext)));

        // if angle is less than 180 degrees, then vertex is convex
        if (angle < std::numbers::pi)
        {
            v->isConvex = true;
            v->isReflex = false;
        }
        else
        {
            v->isReflex = true;
            v->isConvex = false;
        }
    }
}

/**
 * Calculates the sign of the given points.
 *
 * This is essentially a cross product if the points had a z component of 0.
 */
float sign(glm::vec2 p1, glm::vec2 p2, glm::vec2 p3)
{
    return (p1.x - p3.x) * (p2.y - p3.y) - (p2.x - p3.x) * (p1.y - p3.y);
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

/**
 * Finds the ears of the given vertices.
 *
 * @param vertices The vertices to find the ears of.
 */
void findEars(std::list<TriangulationVertex> &vertices)
{
    for (auto it = vertices.begin(); it != vertices.end(); it++)
    {
        // get vertex and its neighbours
        auto v = it;
        auto prev = it;
        auto next = it;

        getVertexNeighbours(vertices, it, prev, next);

        // check if vertex is convex
        if (!v->isConvex)
        {
            continue;
        }

        // check if vertex is an ear tip
        // an ear is a vertex that creates a triangle with its neighbours that does not contain any other vertices on the polygon
        bool isEar = true;

        for (auto it = vertices.begin(); it != vertices.end(); it++)
        {
            // skip if vertex is part of the ear
            if (it == v || it == prev || it == next)
            {
                continue;
            }

            if (inTriangle(it->vertex, prev->vertex, v->vertex, next->vertex))
            {
                isEar = false;
                break;
            }
        }

        v->isEar = isEar;
    }
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

    // remove duplicate vertices insuring that the order of the vertices is preserved
    vertices.clear();
    std::unordered_map<glm::vec2, bool> addedVertices;

    for (auto &v : oldVertices)
    {
        // add vertex if it has not been added yet
        if (!addedVertices.contains(v))
        {
            vertices.push_back(v);
            addedVertices[v] = true;
        }
    }

    // create indices
    for (auto &v : oldVertices)
    {
        indices.push_back(verticesMap[v]);
    }

    // std::cout << "old vertices: " << std::endl;
    // for (auto &v : oldVertices)
    // {
    //     std::cout << "(" << v.x << ", " << v.y << ")" << std::endl;
    // }

    // std::cout << "new vertices: " << std::endl;
    // for (auto &i : indices)
    // {
    //     auto v = vertices[i];
    //     std::cout << "(" << v.x << ", " << v.y << ")" << std::endl;
    // }
}

Rendering::IndexedVertices Rendering::triangulate(const std::vector<glm::vec2> &vertices)
{
    if (vertices.size() < 3)
    {
        throw std::invalid_argument("Cannot triangulate a polygon with less than 3 vertices.");
    }

    std::vector<glm::vec2> meshVertices;
    std::vector<unsigned int> meshIndices;

    // if the polygon is a triangle, just return it
    if (vertices.size() == 3)
    {
        for (size_t i = 0; i < vertices.size(); i++)
        {
            meshVertices.push_back(vertices[i]);
            meshIndices.push_back(i);
        }

        return {
            .vertices = meshVertices,
            .indices = meshIndices,
        };
    }

    // triangulate using ear clipping

    // copy vertices into doubly linked list
    std::list<TriangulationVertex> verticesList;
    int numVertices = vertices.size();

    for (auto &vertex : vertices)
    {
        verticesList.push_back(TriangulationVertex{
            .vertex = vertex,
            .isConvex = false,
            .isReflex = false,
            .isEar = false,
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

            if (ear == verticesList.end())
            {
                throw std::runtime_error("Could not find ear.");
            }
        }

        // add ear and neighbours to mesh to create triangle
        auto prev = ear;
        auto next = ear;
        getVertexNeighbours(verticesList, ear, prev, next);

        // std::cout << "Adding triangle: " << std::endl;
        // std::cout << "  (" << prev->vertex.x << ", " << prev->vertex.y << "), " << std::endl;
        // std::cout << "  (" << ear->vertex.x << ", " << ear->vertex.y << "), " << std::endl;
        // std::cout << "  (" << next->vertex.x << ", " << next->vertex.y << "), " << std::endl;

        meshVertices.push_back(prev->vertex);
        meshVertices.push_back(ear->vertex);
        meshVertices.push_back(next->vertex);

        // remove ear from list
        verticesList.erase(ear);

        // update number of vertices
        numVertices--;
    }

    // add last triangle
    for (auto &vertex : verticesList)
    {
        meshVertices.push_back(vertex.vertex);
    }

    // index vertices
    createIndexedVertices(meshVertices, meshIndices);

    return {
        .vertices = meshVertices,
        .indices = meshIndices,
    };
}
