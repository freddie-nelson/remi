#include "../../include/Core/Transform.h"
#include "../../include/Config.h"

#include <stdexcept>

Core::Transform::Transform(glm::vec2 translation, glm::vec2 scale, glm::vec2 shear, float rotation, unsigned int zIndex)
{
    setTranslation(translation);
    setScale(scale);
    setShear(shear);
    setRotation(rotation);
    setZIndex(zIndex);
}

void Core::Transform::moveForward(unsigned int amount)
{
    setZIndex(zIndex + amount);
}

void Core::Transform::moveBackward(unsigned int amount)
{
    setZIndex(zIndex - amount);
}

void Core::Transform::setZIndex(unsigned int zIndex)
{
    if (zIndex < 0)
        throw std::invalid_argument("zIndex must be greater than or equal to 0.");

    if (zIndex > Config::MAX_Z_INDEX)
        throw std::invalid_argument("zIndex must be less than or equal to " + std::to_string(Config::MAX_Z_INDEX) + ".");

    this->zIndex = zIndex;
    isTransformDirty = true;
}

unsigned int Core::Transform::getZIndex() const
{
    return zIndex;
}

void Core::Transform::move(glm::vec2 move)
{
    translation += move;
    isTransformDirty = true;
}

void Core::Transform::translate(glm::vec2 translation)
{
    this->translation += translation;
    isTransformDirty = true;
}

void Core::Transform::setTranslation(glm::vec2 translation)
{
    this->translation = translation;
    isTransformDirty = true;
}

glm::vec2 Core::Transform::getTranslation() const
{
    return translation;
}

void Core::Transform::scale(glm::vec2 scale)
{
    this->scaleVec *= scale;
    isTransformDirty = true;
}

void Core::Transform::scale(float scale)
{
    this->scaleVec *= scale;
    isTransformDirty = true;
}

void Core::Transform::setScale(glm::vec2 scale)
{
    this->scaleVec = scale;
    isTransformDirty = true;
}

glm::vec2 Core::Transform::getScale() const
{
    return scaleVec;
}

void Core::Transform::setShear(glm::vec2 shear)
{
    this->shear = shear;
    isTransformDirty = true;
}

glm::vec2 Core::Transform::getShear() const
{
    return shear;
}

void Core::Transform::rotate(float rotation)
{
    this->rotation += rotation;
    isTransformDirty = true;
}

void Core::Transform::setRotation(float rotation)
{
    this->rotation = rotation;
    isTransformDirty = true;
}

float Core::Transform::getRotation() const
{
    return rotation;
}

glm::mat4 Core::Transform::getTransformationMatrix() const
{
    if (!isTransformDirty)
        return transformationMatrix;

    // translation
    glm::mat4 t(1.0f);
    t[3][0] = translation.x;
    t[3][1] = translation.y;
    t[3][2] = Config::MAX_Z_INDEX - zIndex;

    // scale
    glm::mat4 s(1.0f);
    s[0][0] = scaleVec.x;
    s[1][1] = scaleVec.y;

    // shear
    s[1][0] = shear.x;
    s[0][1] = shear.y;

    // rotation
    glm::mat4 r(1.0f);
    r[0][0] = glm::cos(rotation);
    r[0][1] = glm::sin(rotation);
    r[1][0] = -glm::sin(rotation);
    r[1][1] = glm::cos(rotation);

    transformationMatrix = t * r * s;
    isTransformDirty = false;

    return transformationMatrix;
}
