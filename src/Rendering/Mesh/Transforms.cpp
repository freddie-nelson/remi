#include "../../../include/Rendering/Mesh/Transforms.h"

Rendering::Mesh2D Rendering::translateMesh(Mesh2D &mesh, const glm::vec2 &translation)
{
    mesh.translation += translation;
    return mesh;
}

Rendering::Mesh2D Rendering::rotateMesh(Mesh2D &mesh, float angle)
{
    glm::mat2x2 rotationMatrix{
        std::cos(angle),
        -std::sin(angle),
        std::sin(angle),
        std::cos(angle),
    };

    mesh.transform *= rotationMatrix;
    return mesh;
}

Rendering::Mesh2D Rendering::scaleMesh(Mesh2D &mesh, const glm::vec2 &scale)
{
    glm::mat2x2 scaleMatrix{
        scale.x,
        0,
        0,
        scale.y,
    };

    mesh.transform *= scaleMatrix;
    return mesh;
}