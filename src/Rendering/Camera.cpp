#include "../../include/Rendering/Camera.h"

#include <glm/gtc/matrix_transform.hpp>
#include "glm/gtx/string_cast.hpp"
#include <stdexcept>
#include <iostream>

Rendering::Camera::Camera(glm::vec2 centre, float width, float height, float near, float far)
    : centre(centre), width(width), height(height), near(near), far(far)
{
    setCentre(centre);
    setWidth(width);
    setHeight(height);
    setNear(near);
    setFar(far);
}

glm::vec2 Rendering::Camera::move(glm::vec2 move)
{
    centre += move;
    updateViewProjectionMatrix();
    return centre;
}

void Rendering::Camera::setCentre(glm::vec2 centre)
{
    this->centre = centre;
    updateViewProjectionMatrix();
}

glm::vec2 Rendering::Camera::getCentre()
{
    return centre;
}

void Rendering::Camera::setWidth(float width)
{
    if (width <= 0)
        throw std::invalid_argument("Width cannot be less than or equal to 0.");

    this->width = width;
    updateViewProjectionMatrix();
}

float Rendering::Camera::getWidth()
{
    return width;
}

void Rendering::Camera::setHeight(float height)
{
    if (height <= 0)
        throw std::invalid_argument("Height cannot be less than or equal to 0.");

    this->height = height;
    updateViewProjectionMatrix();
}

float Rendering::Camera::getHeight()
{
    return height;
}

void Rendering::Camera::setNear(float near)
{
    if (near < 0)
        throw std::invalid_argument("Near cannot be less than 0.");
    if (near > far)
        throw std::invalid_argument("Near cannot be greater than far.");

    this->near = near;
    updateViewProjectionMatrix();
}

float Rendering::Camera::getNear()
{
    return near;
}

void Rendering::Camera::setFar(float far)
{
    if (far < 0)
        throw std::invalid_argument("Far cannot be less than 0.");
    if (far < near)
        throw std::invalid_argument("Far cannot be less than near.");

    this->far = far;
    updateViewProjectionMatrix();
}

float Rendering::Camera::getFar()
{
    return far;
}

void Rendering::Camera::setSize(float width, float height)
{
    setWidth(width);
    setHeight(height);
}

std::pair<float, float> Rendering::Camera::getSize()
{
    return std::make_pair(width, height);
}

glm::mat4 Rendering::Camera::getViewProjectionMatrix()
{
    return viewProjectionMatrix;
}

void Rendering::Camera::updateViewProjectionMatrix()
{
    // camera is at 0.0f on the z axis looking towards z = far (looks towards positive z)
    glm::mat4 viewMatrix = glm::lookAt(glm::vec3(centre, 0.0f), glm::vec3(centre, far), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 projectionMatrix = glm::ortho(-width / 2.0f, width / 2.0f, -height / 2.0f, height / 2.0f, near, far);

    viewProjectionMatrix = projectionMatrix * viewMatrix;
}
