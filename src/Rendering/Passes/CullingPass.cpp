#include "../../../include/Rendering/Passes/CullingPass.h"
#include "../../../include/Rendering/Camera/Camera.h"
#include "../../../include/Core/BoundingCircle.h"
#include "../../../include/Rendering/Renderable.h"
#include "../../../include/Rendering/Mesh/Mesh.h"
#include "../../../include/Rendering/Passes/RenderablesPass.h"

Rendering::RenderPassInput *Rendering::CullingPass::execute(RenderPassInput *input)
{
    checkInput<RenderablesPassData>(input);

    auto inputTyped = static_cast<RenderPassInputTyped<RenderablesPassData> *>(input);

    auto &registry = *inputTyped->registry;
    auto camera = inputTyped->camera;
    auto &entities = *inputTyped->data;

    // get camera aabb
    auto viewAabb = getCullingAABB(registry, camera);

    // get renderables
    std::vector<ECS::Entity> *renderables = new std::vector<ECS::Entity>;

    // get static renderables
    getRenderables(registry, entities, viewAabb, true, *renderables);

    // get dynamic renderables
    getRenderables(registry, entities, viewAabb, false, *renderables);

    // create output
    RenderPassInputTyped<CullingPassData> *output = new RenderPassInputTyped<CullingPassData>(input, renderables);

    delete inputTyped;

    return output;
}

Core::AABB Rendering::CullingPass::getCullingAABB(const ECS::Registry &registry, const ECS::Entity camera) const
{
    auto &cameraComponent = registry.get<Camera>(camera);
    auto &cameraTransform = registry.get<Core::Transform>(camera);

    auto cameraAabb = cameraComponent.getAABB();

    Core::AABB fatCameraAabb;

    // if camera is rotated use bounding circle aabb
    if (cameraTransform.getRotation() != 0)
    {
        auto cameraBoundingCircle = Core::BoundingCircle(cameraAabb, cameraTransform);
        fatCameraAabb = Core::AABB(cameraBoundingCircle.getCentre(), cameraBoundingCircle.getRadius());
    }
    else
    {
        // otherwise scaled and translated aabb is sufficient
        fatCameraAabb = cameraComponent.getScaledAndTranslatedAabb(cameraTransform);
    }

    return fatCameraAabb;
}

void Rendering::CullingPass::pruneTrees(const ECS::Registry &registry)
{
    auto staticTreeEntities = staticRenderablesTree.getIds();
    for (auto &e : staticTreeEntities)
    {
        if (!registry.has<Renderable>(e))
        {
            staticRenderablesTree.remove(e);
            staticRenderables.erase(e);
        }
    }

    auto dynamicTreeEntities = dynamicRenderablesTree.getIds();
    for (auto &e : dynamicTreeEntities)
    {
        if (!registry.has<Renderable>(e))
        {
            dynamicRenderablesTree.remove(e);
            dynamicRenderables.erase(e);
        }
    }
}

size_t Rendering::CullingPass::getRenderables(const ECS::Registry &registry, const std::vector<ECS::Entity> &entities, const Core::AABB &viewAabb, bool isStatic, std::vector<ECS::Entity> &renderables)
{
    // prune trees
    callsSinceLastTreePrune++;
    if (callsSinceLastTreePrune == treePruneFrequency)
    {
        pruneTrees(registry);
        callsSinceLastTreePrune = 0;
    }

    size_t otherCount = 0;

    for (auto &e : entities)
    {
        auto &renderable = registry.get<Renderable>(e);
        if (!renderable.isVisible)
        {
            continue;
        }

        if (isStatic && !renderable.isStatic)
        {
            // check if entity was previously static
            if (staticRenderables.contains(e))
            {
                staticRenderablesTree.remove(e);
                staticRenderables.erase(e);
            }

            otherCount++;

            continue;
        }

        if (!isStatic && renderable.isStatic)
        {
            // check if entity was previously dynamic
            if (dynamicRenderables.contains(e))
            {
                dynamicRenderablesTree.remove(e);
                dynamicRenderables.erase(e);
            }

            otherCount++;

            continue;
        }

        // skip if already in tree and is static
        if (isStatic && staticRenderables.contains(e))
        {
            continue;
        }

        auto &transform = registry.get<Core::Transform>(e);
        auto &mesh = registry.get<Mesh2D>(e);

        auto boundingCircle = Core::BoundingCircle(mesh.getAABB(), transform);
        auto aabb = Core::AABB(boundingCircle.getCentre(), boundingCircle.getRadius());

        if (dynamicRenderablesTree.has(e))
        {
            bool updated = dynamicRenderablesTree.update(e, aabb);
            if (updated)
            {
                dynamicRenderables.emplace(e, std::move(aabb));
            }

            continue;
        }

        if (isStatic)
        {
            staticRenderables.emplace(e, std::move(aabb));
            staticRenderablesTree.insert(e, staticRenderables[e]);
        }
        else
        {
            dynamicRenderables.emplace(e, std::move(aabb));
            dynamicRenderablesTree.insert(e, dynamicRenderables[e]);
        }
    }

    // get entities in view
    if (isStatic)
    {
        staticRenderablesTree.query(viewAabb, renderables);
    }
    else
    {
        dynamicRenderablesTree.query(viewAabb, renderables);
    }

    return otherCount;
}