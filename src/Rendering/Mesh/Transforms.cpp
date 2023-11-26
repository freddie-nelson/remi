#include "../../../include/Rendering/Mesh/Transforms.h"

glm::vec2 Rendering::translate(glm::vec2 &v, const glm::vec2 &translation)
{
    v += translation;
    return v;
}

Rendering::Mesh2D Rendering::translate(Mesh2D &mesh, const glm::vec2 &translation)
{
    translate(mesh.translation, translation);
    return mesh;
}

glm::mat2 Rendering::rotate(glm::mat2 &m, float angle)
{
    glm::mat2 rotationMatrix{
        std::cos(angle),
        -std::sin(angle),
        std::sin(angle),
        std::cos(angle),
    };

    m *= rotationMatrix;
    return m;
}

Rendering::Mesh2D Rendering::rotate(Mesh2D &mesh, float angle)
{
    rotate(mesh.rotation, angle);
    return mesh;
}

glm::mat2 Rendering::scale(glm::mat2 &m, const glm::vec2 &s)
{
    glm::mat2 scaleMatrix{
        s.x,
        0,
        0,
        s.y,
    };

    m *= scaleMatrix;
    return m;
}

Rendering::Mesh2D Rendering::scale(Mesh2D &mesh, const glm::vec2 &s)
{
    scale(mesh.scale, s);
    return mesh;
}

glm::mat2 Rendering::sheer(glm::mat2 &m, const glm::vec2 &s)
{
    glm::mat2 sheerMatrix{
        1,
        s.x,
        s.y,
        1,
    };

    m *= sheerMatrix;
    return m;
}

Rendering::Mesh2D Rendering::sheer(Mesh2D &mesh, const glm::vec2 &s)
{
    sheer(mesh.sheer, s);
    return mesh;
}