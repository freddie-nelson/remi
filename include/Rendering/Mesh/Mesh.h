#pragma once

#include <glm/vec2.hpp>
#include <glm/mat2x2.hpp>
#include <vector>

namespace Rendering
{
    /**
     * A 2D mesh.
     *
     * This mesh must be ready to be rendered by OpenGL.
     *
     * A Mesh2D should be rendered using the GL_TRIANGLES primitive.
     *
     * A Mesh2D must represent a simple polygon.
     *
     * A Mesh2D's vertices must be in counter-clockwise order.
     *
     * The mesh is transformed by multiplying each vertex by the transform matrix and then adding the translation vector.
     *
     * The transform matrix is the combined rotation and scale matrix. See [here](https://en.wikipedia.org/wiki/Transformation_matrix#Examples_in_2_dimensions) for an explanation.
     */
    struct Mesh2D
    {
        std::vector<glm::vec2> vertices;
        std::vector<unsigned int> indices;

        glm::vec2 translation = glm::vec2{0, 0};
        glm::mat2x2 transform = glm::mat2x2{1, 0, 0, 1};
    };
}
