#include "../../../include/Rendering/Mesh/Polygons.h"
#include "../../../include/Rendering/Mesh/Triangulate.h"

#include <math.h>

Rendering::Mesh2D Rendering::createPolygon(const std::vector<glm::vec2> &vertices)
{
    return triangulate(vertices);
}

Rendering::Mesh2D Rendering::createRegularPolygon(float radius, int sides)
{
    std::vector<glm::vec2> vertices;

    float angle = -std::numbers::pi / 2; // start at top
    float angleIncrement = 2 * std::numbers::pi / sides;

    for (int i = 0; i < sides; i++)
    {
        vertices.push_back(glm::vec2{
            radius * std::cos(angle),
            radius * std::sin(angle),
        });

        angle += angleIncrement;
    }

    return createPolygon(vertices);
}

Rendering::Mesh2D Rendering::createRect(float w, float h)
{
    std::vector<glm::vec2> vertices{
        glm::vec2{0, 0},
        glm::vec2{w, 0},
        glm::vec2{w, -h},
        glm::vec2{0, -h},
    };

    return createPolygon(vertices);
}