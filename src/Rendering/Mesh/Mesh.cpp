#include "../../../include/Rendering/Mesh/Mesh.h"
#include "../../../include/Rendering/Mesh/Polygons.h"
#include "../../../include/Rendering/Config.h"

#include <glm/gtc/matrix_transform.hpp>
#include <stdexcept>

Rendering::Mesh2D::Mesh2D()
{
}

Rendering::Mesh2D::Mesh2D(std::vector<glm::vec2> vertices)
{
    createPolygon(vertices);
}

Rendering::Mesh2D::Mesh2D(unsigned int sides, float radius)
{
    createRegularPolygon(sides, radius);
}

Rendering::Mesh2D::Mesh2D(float width, float height)
{
    createRect(width, height);
}

void Rendering::Mesh2D::createPolygon(std::vector<glm::vec2> vertices)
{
    auto iv = Rendering::createPolygon(vertices);

    this->vertices = iv.vertices;
    this->indices = iv.indices;
}

void Rendering::Mesh2D::createRegularPolygon(unsigned int sides, float radius)
{
    auto iv = Rendering::createRegularPolygon(sides, radius);

    this->vertices = iv.vertices;
    this->indices = iv.indices;
}

void Rendering::Mesh2D::createRect(float width, float height)
{
    auto iv = Rendering::createRect(width, height);

    this->vertices = iv.vertices;
    this->indices = iv.indices;
}

void Rendering::Mesh2D::setVertices(std::vector<glm::vec2> vertices)
{
    if (vertices.size() < 3)
    {
        throw std::runtime_error("vertices must have at least 3 vertices.");
    }

    this->vertices = vertices;
}

const std::vector<glm::vec2> &Rendering::Mesh2D::getVertices() const
{
    return vertices;
}

void Rendering::Mesh2D::setIndices(std::vector<unsigned int> indices)
{
    if (indices.size() < 3)
    {
        throw std::runtime_error("indices must have at least 3 indices.");
    }

    this->indices = indices;
}

const std::vector<unsigned int> &Rendering::Mesh2D::getIndices() const
{
    return indices;
}

void Rendering::Mesh2D::moveForward(unsigned int amount)
{
    setZIndex(zIndex + amount);
}

void Rendering::Mesh2D::moveBackward(unsigned int amount)
{
    setZIndex(zIndex - amount);
}

void Rendering::Mesh2D::setZIndex(unsigned int zIndex)
{
    if (zIndex < 0)
        throw std::invalid_argument("zIndex must be greater than or equal to 0.");

    if (zIndex > Config::MAX_Z_INDEX)
        throw std::invalid_argument("zIndex must be less than or equal to " + std::to_string(Config::MAX_Z_INDEX) + ".");

    this->zIndex = zIndex;
}

unsigned int Rendering::Mesh2D::getZIndex() const
{
    return zIndex;
}

void Rendering::Mesh2D::move(glm::vec2 move)
{
    translation += move;
}

void Rendering::Mesh2D::translate(glm::vec2 translation)
{
    this->translation += translation;
}

void Rendering::Mesh2D::setTranslation(glm::vec2 translation)
{
    this->translation = translation;
}

glm::vec2 Rendering::Mesh2D::getTranslation() const
{
    return translation;
}

void Rendering::Mesh2D::scale(glm::vec2 scale)
{
    this->scaleVec *= scale;
}

void Rendering::Mesh2D::scale(float scale)
{
    this->scaleVec *= scale;
}

void Rendering::Mesh2D::setScale(glm::vec2 scale)
{
    this->scaleVec = scale;
}

glm::vec2 Rendering::Mesh2D::getScale() const
{
    return scaleVec;
}

void Rendering::Mesh2D::setShear(glm::vec2 shear)
{
    this->shear = shear;
}

glm::vec2 Rendering::Mesh2D::getShear() const
{
    return shear;
}

void Rendering::Mesh2D::rotate(float rotation)
{
    this->rotation += rotation;
}

void Rendering::Mesh2D::setRotation(float rotation)
{
    this->rotation = rotation;
}

float Rendering::Mesh2D::getRotation() const
{
    return rotation;
}

void Rendering::Mesh2D::setColor(Rendering::Color color)
{
    this->color = color;
}

const Rendering::Color &Rendering::Mesh2D::getColor() const
{
    return color;
}

glm::mat4 Rendering::Mesh2D::getTransformationMatrix() const
{
    // translation
    glm::mat4 t(1.0f);
    t[3][0] = translation.x;
    t[3][1] = translation.y;
    t[3][2] = Config::MAX_Z_INDEX - zIndex;

    // scale
    glm::mat4 s(1.0f);
    s[0][0] = scaleVec.x;
    s[1][1] = scaleVec.y;

    // // shear
    s[1][0] = shear.x;
    s[0][1] = shear.y;

    // rotation
    glm::mat4 r(1.0f);
    r[0][0] = glm::cos(rotation);
    r[0][1] = glm::sin(rotation);
    r[1][0] = -glm::sin(rotation);
    r[1][1] = glm::cos(rotation);

    return t * r * s;
}
