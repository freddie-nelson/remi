#include "../../include/Core/SpaceTransformer.h"
#include "../../include/Rendering/Camera/Camera.h"

#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/quaternion.hpp>
#include <stdexcept>

Core::SpaceTransformer::SpaceTransformer(const Rendering::Renderer *renderer, World::World *const world, unsigned int pixelsPerMeter) : renderer(renderer), world(world), pixelsPerMeter(pixelsPerMeter), pixelsPerMeterFloat(pixelsPerMeter) {}

glm::vec2 Core::SpaceTransformer::transform(const glm::vec2 &v, Space from, Space to) const
{
    return transform(v, nullptr, from, to);
}

glm::vec2 Core::SpaceTransformer::transform(const glm::vec2 &v, ECS::Entity entity, Space from, Space to) const
{
    return transform(v, &entity, from, to);
}

glm::vec2 Core::SpaceTransformer::transform(const glm::vec2 &v, const ECS::Entity *entity, Space from, Space to) const
{
    if ((from == Space::LOCAL || to == Space::LOCAL) && entity == nullptr)
    {
        throw std::invalid_argument("SpaceTransformer (transform): Cannot convert to/from local space without entity.");
    }

    // no need to convert
    if (from == to)
    {
        return v;
    }

    Space curr = from;
    glm::vec2 transformed = v;

    while (curr != to)
    {
        curr = getNextSpace(curr, to, transformed, entity);
    }

    return transformed;
}

float Core::SpaceTransformer::transformLocalRotationToWorld(float rotation, const ECS::Entity entity) const
{
    auto &sceneGraph = world->getSceneGraph();
    auto &registry = world->getRegistry();

    if (!sceneGraph.hasParent(entity))
    {
        return rotation;
    }

    auto &worldTransform = sceneGraph.getModelMatrix(entity);
    auto rotationMat = glm::eulerAngleZ(rotation);

    auto worldRotation = worldTransform * rotationMat;
    auto quat = glm::toQuat(worldRotation);

    return glm::eulerAngles(quat).z;
}

float Core::SpaceTransformer::transformWorldRotationToLocal(float rotation, const ECS::Entity entity) const
{
    auto &sceneGraph = world->getSceneGraph();
    auto &registry = world->getRegistry();

    if (!sceneGraph.hasParent(entity))
    {
        return rotation;
    }

    auto &worldTransform = sceneGraph.getModelMatrix(entity);
    auto rotationMat = glm::eulerAngleZ(rotation);

    auto inverseWorldTransform = glm::inverse(worldTransform);
    auto localRotation = inverseWorldTransform * rotationMat;
    auto quat = glm::toQuat(localRotation);

    return glm::eulerAngles(quat).z;
}

float Core::SpaceTransformer::pixelsToMeters(float pixels) const
{
    return pixels / pixelsPerMeterFloat;
}

glm::vec2 Core::SpaceTransformer::pixelsToMeters(const glm::vec2 &pixels) const
{
    return pixels / pixelsPerMeterFloat;
}

float Core::SpaceTransformer::metersToPixels(float meters) const
{
    return meters * pixelsPerMeterFloat;
}

glm::vec2 Core::SpaceTransformer::metersToPixels(const glm::vec2 &meters) const
{
    return meters * pixelsPerMeterFloat;
}

float Core::SpaceTransformer::getPixelsPerMeter() const
{
    return pixelsPerMeterFloat;
}

Core::SpaceTransformer::Space Core::SpaceTransformer::getNextSpace(Space s, Space goal, glm::vec2 &v, const ECS::Entity *entity) const
{
    if (s == goal)
    {
        return s;
    }
    else if (s > goal)
    {
        // we are going towards local space
        switch (s)
        {
        case Space::SCREEN:
            screenToClip(v);
            return Space::CLIP;
        case Space::CLIP:
            clipToView(v);
            return Space::VIEW;
        case Space::VIEW:
            viewToWorld(v);
            return Space::WORLD;
        case Space::WORLD:
            if (entity == nullptr)
            {
                throw std::invalid_argument("SpaceTransform (getNextSpace): can't convert from world to local without entity.");
            }

            worldToLocal(v, world->getSceneGraph().getParentModelMatrix(*entity));
            return Space::LOCAL;
        default:
            throw std::invalid_argument("SpaceTransformer (getNextSpace): can't convert space any further.");
        }
    }
    else
    {
        // we are going towards screen space
        switch (s)
        {
        case Space::LOCAL:
            if (entity == nullptr)
            {
                throw std::invalid_argument("SpaceTransform (getNextSpace): can't convert from local to world without entity.");
            }

            localToWorld(v, world->getSceneGraph().getParentModelMatrix(*entity));
            return Space::WORLD;
        case Space::WORLD:
            worldToView(v);
            return Space::VIEW;
        case Space::VIEW:
            viewToClip(v);
            return Space::CLIP;
        case Space::CLIP:
            clipToScreen(v);
            return Space::SCREEN;
        default:
            throw std::invalid_argument("SpaceTransformer (getNextSpace): can't convert space any further.");
        }
    }
}

Core::SpaceTransformer::Space Core::SpaceTransformer::screenToClip(glm::vec2 &v) const
{
    // since we are in 2d and orthographic there is no perspective divide (w component is always 1)
    // so our calculation can be a lot simpler

    auto viewportSize = renderer->getSize();

    v /= viewportSize;
    v *= 2.0f;
    v -= 1.0f;

    // std::cout << "[clip] x: " << v.x << " y: " << v.y << std::endl;

    return Space::CLIP;
}

Core::SpaceTransformer::Space Core::SpaceTransformer::clipToScreen(glm::vec2 &v) const
{
    auto viewportSize = renderer->getSize();

    v += 1.0f;
    v /= 2.0f;
    v *= viewportSize;

    return Space::SCREEN;
}

Core::SpaceTransformer::Space Core::SpaceTransformer::clipToView(glm::vec2 &v) const
{

    auto &registry = world->getRegistry();

    auto camera = renderer->getActiveCamera(registry);
    auto &cameraComponent = registry.get<Rendering::Camera>(camera);
    auto &t = registry.get<Core::Transform>(camera);

    auto projection = cameraComponent.getProjectionMatrix(t);
    auto inverseProjection = glm::inverse(projection);

    glm::vec4 transformed = inverseProjection * glm::vec4(v, 0, 1);

    v.x = transformed.x;
    v.y = transformed.y;

    // std::cout << "[view] x: " << v.x << " y: " << v.y << std::endl;

    return Space::VIEW;
}

Core::SpaceTransformer::Space Core::SpaceTransformer::viewToClip(glm::vec2 &v) const
{
    auto &registry = world->getRegistry();

    auto camera = renderer->getActiveCamera(registry);
    auto &cameraComponent = registry.get<Rendering::Camera>(camera);
    auto &t = registry.get<Core::Transform>(camera);

    auto projection = cameraComponent.getProjectionMatrix(t);

    glm::vec4 transformed = projection * glm::vec4(v, 0, 1);

    v.x = transformed.x;
    v.y = transformed.y;

    return Space::CLIP;
}

Core::SpaceTransformer::Space Core::SpaceTransformer::viewToWorld(glm::vec2 &v) const
{
    auto &registry = world->getRegistry();

    auto camera = renderer->getActiveCamera(registry);
    auto &cameraComponent = registry.get<Rendering::Camera>(camera);
    auto &t = registry.get<Core::Transform>(camera);

    auto view = cameraComponent.getViewMatrix(t, pixelsPerMeterFloat);
    auto inverseView = glm::inverse(view);

    glm::vec4 transformed = inverseView * glm::vec4(v, 0, 1);

    v.x = pixelsToMeters(transformed.x);
    v.y = pixelsToMeters(transformed.y);

    // std::cout << "[world] x: " << v.x << " y: " << v.y << std::endl;

    return Space::WORLD;
}

Core::SpaceTransformer::Space Core::SpaceTransformer::worldToView(glm::vec2 &v) const
{
    auto &registry = world->getRegistry();

    auto camera = renderer->getActiveCamera(registry);
    auto &cameraComponent = registry.get<Rendering::Camera>(camera);
    auto &t = registry.get<Core::Transform>(camera);

    auto view = cameraComponent.getViewMatrix(t, pixelsPerMeterFloat);

    glm::vec4 transformed = view * glm::vec4(metersToPixels(v), 0, 1);

    v.x = transformed.x;
    v.y = transformed.y;

    return Space::VIEW;
}

Core::SpaceTransformer::Space Core::SpaceTransformer::worldToLocal(glm::vec2 &v, const glm::mat4 &worldTransform) const
{
    auto inverse = glm::inverse(worldTransform);

    glm::vec4 transformed = inverse * glm::vec4(v, 0, 1);

    v.x = transformed.x;
    v.y = transformed.y;

    return Space::LOCAL;
}

Core::SpaceTransformer::Space Core::SpaceTransformer::localToWorld(glm::vec2 &v, const glm::mat4 &worldTransform) const
{
    glm::vec4 transformed = worldTransform * glm::vec4(v, 0, 1);

    v.x = transformed.x;
    v.y = transformed.y;

    return Space::WORLD;
}