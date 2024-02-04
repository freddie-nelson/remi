#include "../../include/Core/SpaceTransformer.h"
#include "../../include/Rendering/Camera/Camera.h"

#include <stdexcept>

Core::SpaceTransformer::SpaceTransformer(const Rendering::Renderer *const renderer, const ECS::Registry *const registry, unsigned int pixelsPerMeter) : renderer(renderer), registry(registry), pixelsPerMeter(pixelsPerMeter), pixelsPerMeterFloat(pixelsPerMeter) {}

glm::vec2 Core::SpaceTransformer::transform(const glm::vec2 &v, Spaces from, Spaces to) const
{
    return transform(v, nullptr, from, to);
}

glm::vec2 Core::SpaceTransformer::transform(const glm::vec2 &v, const Core::Transform &localTransform, Spaces from, Spaces to) const
{
    return transform(v, &localTransform, from, to);
}

glm::vec2 Core::SpaceTransformer::transform(const glm::vec2 &v, const Core::Transform *const localTransform, Spaces from, Spaces to) const
{
    if (from == Spaces::LOCAL || to == Spaces::LOCAL)
    {
        throw std::invalid_argument("SpaceTransformer (transform): Cannot convert to/from local space without local transform.");
    }

    // no need to convert
    if (from == to)
    {
        return v;
    }

    Spaces curr = from;
    glm::vec2 transformed = v;

    while (curr != to)
    {
        curr = getNextSpace(curr, to, transformed, localTransform);
    }

    return transformed;
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

Core::SpaceTransformer::Spaces Core::SpaceTransformer::getNextSpace(Spaces s, Spaces goal, glm::vec2 &v, const Core::Transform *const localTransform) const
{
    if (s > goal)
    {
        // we are going towards local space
        switch (s)
        {
        case Spaces::SCREEN:
            screenToClip(v);
            return Spaces::CLIP;
        case Spaces::CLIP:
            clipToView(v);
            return Spaces::VIEW;
        case Spaces::VIEW:
            viewToWorld(v);
            return Spaces::WORLD;
        case Spaces::WORLD:
            if (localTransform == nullptr)
            {
                throw std::invalid_argument("SpaceTransform (getNextSpace): can't convert from world to local without local transform.");
            }

            worldToLocal(v, *localTransform);
            return Spaces::LOCAL;
        default:
            throw std::invalid_argument("SpaceTransformer (getNextSpace): can't convert space any further.");
        }
    }
    else
    {
        // we are going towards screen space
        switch (s)
        {
        case Spaces::LOCAL:
            if (localTransform == nullptr)
            {
                throw std::invalid_argument("SpaceTransform (getNextSpace): can't convert from local to world without local transform.");
            }

            localToWorld(v, *localTransform);
            return Spaces::WORLD;
        case Spaces::WORLD:
            worldToView(v);
            return Spaces::VIEW;
        case Spaces::VIEW:
            viewToClip(v);
            return Spaces::CLIP;
        case Spaces::CLIP:
            clipToScreen(v);
            return Spaces::SCREEN;
        default:
            throw std::invalid_argument("SpaceTransformer (getNextSpace): can't convert space any further.");
        }
    }
}

Core::SpaceTransformer::Spaces Core::SpaceTransformer::screenToClip(glm::vec2 &v) const
{
    // since we are in 2d and orthographic there is no perspective divide (w component is always 1)
    // so our calculation can be a lot simpler

    auto viewportSize = renderer->getSize();

    v /= viewportSize;
    v *= 2.0f;
    v -= 1.0f;

    return Spaces::CLIP;
}

Core::SpaceTransformer::Spaces Core::SpaceTransformer::clipToScreen(glm::vec2 &v) const
{
    auto viewportSize = renderer->getSize();

    v += 1.0f;
    v /= 2.0f;
    v *= viewportSize;

    return Spaces::SCREEN;
}

Core::SpaceTransformer::Spaces Core::SpaceTransformer::clipToView(glm::vec2 &v) const
{
    auto camera = renderer->getActiveCamera(*registry);
    auto &cameraComponent = registry->get<Rendering::Camera>(camera);
    auto &t = registry->get<Core::Transform>(camera);

    auto projection = cameraComponent.getProjectionMatrix(t);
    auto inverseProjection = glm::inverse(projection);

    glm::vec4 transformed = inverseProjection * glm::vec4(v, 0, 1);

    v.x = pixelsToMeters(transformed.x);
    v.y = pixelsToMeters(transformed.y);

    return Spaces::VIEW;
}

Core::SpaceTransformer::Spaces Core::SpaceTransformer::viewToClip(glm::vec2 &v) const
{
    auto camera = renderer->getActiveCamera(*registry);
    auto &cameraComponent = registry->get<Rendering::Camera>(camera);
    auto &t = registry->get<Core::Transform>(camera);

    auto projection = cameraComponent.getProjectionMatrix(t);

    glm::vec4 transformed = projection * glm::vec4(metersToPixels(v.x), metersToPixels(v.y), 0, 1);

    v.x = transformed.x;
    v.y = transformed.y;

    return Spaces::CLIP;
}

Core::SpaceTransformer::Spaces Core::SpaceTransformer::viewToWorld(glm::vec2 &v) const
{
    auto camera = renderer->getActiveCamera(*registry);
    auto &cameraComponent = registry->get<Rendering::Camera>(camera);
    auto &t = registry->get<Core::Transform>(camera);

    auto view = cameraComponent.getViewMatrix(t, pixelsPerMeterFloat);
    auto inverseView = glm::inverse(view);

    glm::vec4 transformed = inverseView * glm::vec4(v, 0, 1);

    v.x = transformed.x;
    v.y = transformed.y;

    return Spaces::WORLD;
}

Core::SpaceTransformer::Spaces Core::SpaceTransformer::worldToView(glm::vec2 &v) const
{
    auto camera = renderer->getActiveCamera(*registry);
    auto &cameraComponent = registry->get<Rendering::Camera>(camera);
    auto &t = registry->get<Core::Transform>(camera);

    auto view = cameraComponent.getViewMatrix(t, pixelsPerMeterFloat);

    glm::vec4 transformed = view * glm::vec4(v, 0, 1);

    v.x = transformed.x;
    v.y = transformed.y;

    return Spaces::VIEW;
}

Core::SpaceTransformer::Spaces Core::SpaceTransformer::worldToLocal(glm::vec2 &v, const Core::Transform &localTransform) const
{
    auto &transform = localTransform.getTransformationMatrix();
    auto inverse = glm::inverse(transform);

    glm::vec4 transformed = inverse * glm::vec4(v, 0, 1);

    v.x = transformed.x;
    v.y = transformed.y;

    return Spaces::LOCAL;
}

Core::SpaceTransformer::Spaces Core::SpaceTransformer::localToWorld(glm::vec2 &v, const Core::Transform &localTransform) const
{
    auto &transform = localTransform.getTransformationMatrix();
    glm::vec4 transformed = transform * glm::vec4(v, 0, 1);

    v.x = transformed.x;
    v.y = transformed.y;

    return Spaces::WORLD;
}