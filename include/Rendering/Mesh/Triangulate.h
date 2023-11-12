#pragma once

#include "Mesh.h"

#include <vector>
#include <glm/vec2.hpp>

namespace Rendering
{
    /**
     * Triangulates the given vertices into a mesh.
     *
     * The vertices must be in counter-clockwise order.
     *
     * The vertices must represent a simple polygon.
     *
     * @param vertices The vertices to triangulate, must be in counter-clockwise order.
     *
     * @returns A Mesh2D representing the triangulated polygon.
     */
    Mesh2D triangulate(const std::vector<glm::vec2> &vertices);

    /**
     * Transforms the given vertices into indexed vertices.
     *
     * Vertices should be in counter-clockwise order.
     *
     * Vertices should be triangluated, i.e. v[0], v[1], v[2] should be a triangle, v[3], v[4], v[5] should be a triangle, etc.
     *
     * @param vertices The vertices to create indices for, must be in counter-clockwise order.
     * @param indices The indices to add to.
     */
    void createIndexedVertices(std::vector<glm::vec2> &vertices, std::vector<unsigned int> &indices);
}