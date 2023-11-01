#include "../../include/Rendering/Mesh.h"

Rendering::Mesh2D Rendering::triangulate(const std::vector<glm::vec2> &vertices)
{
    Mesh2D mesh;

    if (vertices.size() < 3)
    {
        for (int i = 0; i < vertices.size(); i++)
        {
            mesh.vertices.push_back(vertices[i]);
            mesh.indices.push_back(i);
        }

        return mesh;
    }

    // triangulate using ear clipping
}