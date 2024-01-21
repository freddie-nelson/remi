#include "../../../include/Rendering/Mesh/Polygons.h"
#include "../../../include/Rendering/Mesh/Triangulate.h"

#include <numbers>
#include <math.h>

Rendering::IndexedVertices Rendering::createPolygon(const std::vector<glm::vec2> &vertices, bool centered)
{
    auto triangulated = triangulate(vertices);

    if (!centered)
        return triangulated;

    // calculate centre
    glm::vec2 centre(0.0f);
    for (auto &v : triangulated.vertices)
    {
        centre += v;
    }
    centre /= triangulated.vertices.size();

    // translate vertices by centre to make new centre (0, 0)
    for (auto &v : triangulated.vertices)
    {
        v -= centre;
    }

    return triangulated;
}

Rendering::IndexedVertices Rendering::createRegularPolygon(float radius, unsigned int sides)
{
    std::vector<glm::vec2> vertices;

    float angle = -std::numbers::pi / 2; // start at top
    float angleIncrement = 2 * std::numbers::pi / sides;

    for (size_t i = 0; i < sides; i++)
    {
        vertices.push_back(glm::vec2{
            radius * std::cos(angle),
            radius * std::sin(angle),
        });

        angle += angleIncrement;
    }

    return createPolygon(vertices);
}

Rendering::IndexedVertices Rendering::createRect(float w, float h, bool centered)
{
    std::vector<glm::vec2> vertices;

    if (centered)
    {
        vertices = {
            glm::vec2{-w / 2, h / 2},
            glm::vec2{w / 2, h / 2},
            glm::vec2{w / 2, -h / 2},
            glm::vec2{-w / 2, -h / 2},
        };
    }
    else
    {
        vertices = {
            glm::vec2{0, 0},
            glm::vec2{w, 0},
            glm::vec2{w, -h},
            glm::vec2{0, -h},
        };
    }

    return createPolygon(vertices);
}