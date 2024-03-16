#include "../../../include/Rendering/Passes/RenderablesPass.h"
#include "../../../include/Rendering/Mesh/Mesh.h"
#include "../../../include/Rendering/Renderable.h"
#include "../../../include/Rendering/Material/Material.h"
#include "../../../include/Rendering/Material/ShaderMaterial.h"
#include "../../../include/Core/Transform.h"

Rendering::RenderPassInput *Rendering::RenderablesPass::execute(RenderPassInput *input)
{
    checkInput<int>(input);

    auto *inputTyped = static_cast<RenderPassInputTyped<int> *>(input);

    auto &world = *input->world;
    auto &registry = world.getRegistry();

    // get entities
    auto &entities = registry.view<Mesh2D, Core::Transform, Renderable>();
    auto cacheTime = registry.viewCachedTime<Mesh2D, Core::Transform, Renderable>();

    if (cacheTime == lastViewCacheTime)
    {
        auto data = new RenderablesPassData();
        data->staticRenderables = oldData.staticRenderables;
        data->dynamicRenderables = oldData.dynamicRenderables;

        return new RenderPassInputTyped(input, data);
    }

    // update cache time
    lastViewCacheTime = cacheTime;

    RenderablesPassData *data = new RenderablesPassData();
    std::vector<ECS::Entity> &staticRenderables = data->staticRenderables;
    std::vector<ECS::Entity> &newStaticRenderables = data->newStaticRenderables;

    std::vector<ECS::Entity> &dynamicRenderables = data->dynamicRenderables;
    std::vector<ECS::Entity> &newDynamicRenderables = data->newDynamicRenderables;

    staticRenderables.reserve(entities.size());

    bool sameEntities = entities.size() == oldEntities.size();

    // check if entities have materials
    // auto start = Core::timeSinceEpochMicrosec();

    for (size_t i = 0; i < entities.size(); i++)
    {
        auto e = entities[i];

        if (sameEntities && e != oldEntities[i])
        {
            sameEntities = false;
        }

        auto &renderable = registry.get<Renderable>(e);
        if (renderable.isStatic)
        {
            staticRenderables.push_back(e);
        }
        else
        {
            dynamicRenderables.push_back(e);
        }
    }

    if (!sameEntities)
    {
        newStaticRenderables = staticRenderables;
        newDynamicRenderables = dynamicRenderables;
    }

    // auto end = Core::timeSinceEpochMicrosec();
    // std::cout << "find renderables: " << (end - start) << " microseconds" << std::endl;

    // create output
    auto *output = new RenderPassInputTyped(input, data);

    // update old entities
    oldEntities = entities;

    // update old data
    oldData = *data;

    delete inputTyped;

    return output;
}