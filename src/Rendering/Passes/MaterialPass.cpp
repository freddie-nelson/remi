#include "../../../include/Rendering/Passes/MaterialPass.h"
#include "../../../include/Rendering/Material/Material.h"
#include "../../../include/Rendering/Material/ShaderMaterial.h"
#include "../../../include/Rendering/Material/MaterialHelpers.h"
#include "../../../include/Rendering/Passes/CullingPass.h"
#include "../../../include/Rendering/Renderer.h"

#include <map>

Rendering::RenderPassInput *Rendering::MaterialPass::execute(RenderPassInput *input)
{
    checkInput<CullingPassData>(input);

    auto inputTyped = static_cast<RenderPassInputTyped<CullingPassData> *>(input);

    auto &renderer = *inputTyped->renderer;
    auto &registry = *inputTyped->registry;
    auto &renderables = *inputTyped->data;

    std::vector<ShaderMaterial::FragShaderKey> keys;

    // split renderables into opaque and transparent
    std::unordered_map<ShaderMaterial::FragShaderKey, std::vector<ECS::Entity>> opaqueRenderables;
    std::unordered_map<ShaderMaterial::FragShaderKey, std::vector<ECS::Entity>> transparentRenderables;

    for (auto &e : renderables)
    {
        ShaderMaterial::FragShaderKey key = DEFAULT_SHADER_KEY;
        if (registry.has<ShaderMaterial>(e))
        {
            auto &material = registry.get<ShaderMaterial>(e);
            key = material.getFragmentShaderKey();
        }

        auto material = getMaterial(registry, e);

        auto &map = material->isTransparent() && renderer.isAlphaBlendingEnabled() ? transparentRenderables : opaqueRenderables;

        if (!map.contains(key))
        {
            // if key is not in any map yet, add it to keys
            if (!opaqueRenderables.contains(key) && !transparentRenderables.contains(key))
            {
                keys.push_back(key);
            }

            map.emplace(key, std::vector<ECS::Entity>());
        }

        map[key].push_back(e);
    }

    // sort transparent renderables by z index (back to front)
    // for correct alpha blending
    if (renderer.isAlphaBlendingEnabled())
    {
        for (auto &map : transparentRenderables)
        {
            sortRenderables(registry, map.second);
        }
    }

    // create output
    auto data = new MaterialPassData{std::move(opaqueRenderables), std::move(transparentRenderables), std::move(keys)};
    auto output = new RenderPassInputTyped<MaterialPassData>(input, data);

    delete inputTyped;

    return output;
}

void Rendering::MaterialPass::sortRenderables(const ECS::Registry &registry, std::vector<ECS::Entity> &renderables)
{
    std::map<unsigned int, std::vector<ECS::Entity>> layers;

    for (auto &e : renderables)
    {
        auto &transform = registry.get<Core::Transform>(e);
        auto zIndex = transform.getZIndex();

        if (!layers.contains(zIndex))
        {
            layers.emplace(zIndex, std::vector<ECS::Entity>());
        }

        layers[zIndex].push_back(e);
    }

    renderables.clear();

    for (auto &layer : layers)
    {
        for (auto &e : layer.second)
        {
            renderables.push_back(e);
        }
    }

    // first renderable should be at lowest index and last renderable at highest index

    // auto first = renderables.front();
    // auto &firstTransform = registry.get<Core::Transform>(first);

    // auto last = renderables.back();
    // auto &lastTransform = registry.get<Core::Transform>(last);

    // std::cout << "first: " << firstTransform.getZIndex() << ", last: " << lastTransform.getZIndex() << std::endl;
}
