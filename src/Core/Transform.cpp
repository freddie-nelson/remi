#include "../../include/Core/Transform.h"
#include "../../include/Config.h"

#include <glm/gtx/matrix_decompose.hpp>
#include <string>
#include <stdexcept>

Core::Transform::Transform(glm::vec2 translation, glm::vec2 scale, glm::vec2 shear, float rotation, unsigned int zIndex)
{
    setTranslation(translation);
    setScale(scale);
    setShear(shear);
    setRotation(rotation);
    setZIndex(zIndex);
}

Core::Transform::Transform(const glm::mat4 &mat)
{
    setTransformationMatrix(mat);
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
    propertyChanges.zIndexChanges++;
}

unsigned int Core::Transform::getZIndex() const
{
    return zIndex;
}

void Core::Transform::move(glm::vec2 move)
{
    translation += move;
    isTransformDirty = true;
    propertyChanges.translationChanges++;
}

void Core::Transform::translate(glm::vec2 translation)
{
    this->translation += translation;
    isTransformDirty = true;
    propertyChanges.translationChanges++;
}

void Core::Transform::setTranslation(glm::vec2 translation)
{
    this->translation = translation;
    isTransformDirty = true;
    propertyChanges.translationChanges++;
}

const glm::vec2 &Core::Transform::getTranslation() const
{
    return translation;
}

void Core::Transform::scale(glm::vec2 scale)
{
    this->scaleVec *= scale;
    isTransformDirty = true;
    propertyChanges.scaleChanges++;
}

void Core::Transform::scale(float scale)
{
    this->scaleVec *= scale;
    isTransformDirty = true;
    propertyChanges.scaleChanges++;
}

void Core::Transform::setScale(glm::vec2 scale)
{
    this->scaleVec = scale;
    isTransformDirty = true;
    propertyChanges.scaleChanges++;
}

const glm::vec2 &Core::Transform::getScale() const
{
    return scaleVec;
}

void Core::Transform::setShear(glm::vec2 shear)
{
    this->shear = shear;
    isTransformDirty = true;
    propertyChanges.shearChanges++;
}

const glm::vec2 &Core::Transform::getShear() const
{
    return shear;
}

void Core::Transform::rotate(float rotation)
{
    this->rotation += rotation;
    isTransformDirty = true;
    propertyChanges.rotationChanges++;
}

void Core::Transform::setRotation(float rotation)
{
    this->rotation = rotation;
    isTransformDirty = true;
    propertyChanges.rotationChanges++;
}

float Core::Transform::getRotation() const
{
    return rotation;
}

const Core::Transform::PropertyChanges &Core::Transform::getPropertyChanges() const
{
    return propertyChanges;
}

const glm::mat4 &Core::Transform::getTransformationMatrix() const
{
    if (!isTransformDirty)
        return transformationMatrix;

    float r1 = glm::cos(rotation);
    float r3 = glm::sin(rotation);
    float r2 = -r3;
    float r4 = r1;

    // only need to update these rows and columns as the rest of the matrix is the identity matrix
    // and shouldn't have changed

    // column major ordering

    // rotation, scale and shear
    transformationMatrix[0][0] = scaleVec.x * r1 + shear.y * r3;
    transformationMatrix[0][1] = shear.x * r1 + scaleVec.y * r3;

    transformationMatrix[1][0] = scaleVec.x * r2 + shear.y * r4;
    transformationMatrix[1][1] = shear.x * r2 + scaleVec.y * r4;

    // translation
    transformationMatrix[3][0] = translation.x;
    transformationMatrix[3][1] = translation.y;
    transformationMatrix[3][2] = static_cast<int>(zIndex) - static_cast<int>(Config::MAX_Z_INDEX);

    isTransformDirty = false;

    return transformationMatrix;
}

void Core::Transform::setTransformationMatrix(const glm::mat4 &mat)
{
    transformationMatrix = mat;
    isTransformDirty = false;

    // @see https://math.stackexchange.com/questions/237369/given-this-transformation-matrix-how-do-i-decompose-it-into-translation-rotati
    // for explanation of the following code

    glm::vec3 scale{};
    glm::quat rotation{};
    glm::vec3 translation{};
    glm::vec3 shear{};
    glm::vec4 perspective{};
    glm::decompose(mat, scale, rotation, translation, shear, perspective);

    unsigned int zIndex = static_cast<unsigned int>(translation.z + Config::MAX_Z_INDEX);
    if (zIndex > Config::MAX_Z_INDEX)
        throw std::invalid_argument("zIndex must be less than or equal to " + std::to_string(Config::MAX_Z_INDEX) + ".");

    setTranslation(glm::vec2(translation));
    setZIndex(zIndex);
    setScale(glm::vec2(scale));
    setShear(glm::vec2(shear));
    setRotation(glm::eulerAngles(rotation).z);
}