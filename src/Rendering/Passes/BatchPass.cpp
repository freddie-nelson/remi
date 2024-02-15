#include "../../../include/Rendering/Passes/BatchPass.h"
#include "../../../include/Rendering/Material/Material.h"
#include "../../../include/Rendering/Material/ShaderMaterial.h"
#include "../../../include/Rendering/Material/MaterialHelpers.h"
#include "../../../include/Rendering/Passes/CullingPass.h"
#include "../../../include/Rendering/Renderer.h"

#include <map>

Rendering::RenderPassInput *Rendering::BatchPass::execute(RenderPassInput *input)
{
    checkInput<CullingPassData>(input);

    auto inputTyped = static_cast<RenderPassInputTyped<CullingPassData> *>(input);

    auto &renderer = *inputTyped->renderer;

    auto &world = *inputTyped->world;
    auto &registry = world.getRegistry();
    auto &sceneGraph = world.getSceneGraph();

    auto &renderables = *inputTyped->data;

    std::vector<ShaderMaterial::FragShaderKey> keys;

    // split renderables into opaque and transparent
    std::vector<ECS::Entity> opaqueRenderables;
    std::vector<ShaderMaterial::FragShaderKey> opaqueKeys;

    std::vector<ECS::Entity> transparentRenderables;
    std::vector<ShaderMaterial::FragShaderKey> transparentKeys;
    std::vector<unsigned int> transparentZIndices;

    // preallocate vectors (small optimization)
    opaqueRenderables.reserve(renderables.size());
    opaqueKeys.reserve(renderables.size());

    transparentRenderables.reserve(renderables.size());
    transparentKeys.reserve(renderables.size());
    transparentKeys.reserve(renderables.size());

    for (auto &e : renderables)
    {
        ShaderMaterial::FragShaderKey key = DEFAULT_SHADER_KEY;
        if (registry.has<ShaderMaterial>(e))
        {
            auto &material = registry.get<ShaderMaterial>(e);
            key = material.getFragmentShaderKey();
        }

        auto material = getMaterial(registry, e);

        if (material->isTransparent() && renderer.isAlphaBlendingEnabled())
        {
            transparentRenderables.push_back(e);
            transparentKeys.push_back(key);
            transparentZIndices.push_back(registry.get<Core::Transform>(e).getZIndex());
        }
        else
        {
            opaqueRenderables.push_back(e);
            opaqueKeys.push_back(key);
        }
    }

    // sort transparent renderables by z index (back to front)
    // for correct alpha blending
    // sort all transparent renderables at once to avoid multiple sorts (small optimization)
    // if (renderer.isAlphaBlendingEnabled())
    // {
    //     sortRenderables(registry, transparentRenderables);
    // }

    // TODO: cleanup this mess at some point
    // create batches
    BatchPassData *batches = new BatchPassData();

    // create opaque batches

    std::unordered_map<ShaderMaterial::FragShaderKey, Batch> opaqueBatches;

    for (size_t i = 0; i < opaqueRenderables.size(); i++)
    {
        auto &e = opaqueRenderables[i];
        auto &key = opaqueKeys[i];

        if (!opaqueBatches.contains(key))
        {
            Batch batch;
            batch.transparent = false;
            batch.key = key;

            opaqueBatches.emplace(key, batch);
        }

        opaqueBatches[key].renderables.push_back(e);
    }

    // add opaque batches to batches
    for (auto &batch : opaqueBatches)
    {
        batches->emplace_back(std::move(batch.second));
    }

    // split transparent renderables into layers and their shader materials
    std::map<unsigned int, std::unordered_map<ShaderMaterial::FragShaderKey, std::vector<ECS::Entity>>> layeredTransparentRenderables;

    for (size_t i = 0; i < transparentRenderables.size(); i++)
    {
        auto &e = transparentRenderables[i];
        auto &key = transparentKeys[i];
        auto &zIndex = transparentZIndices[i];

        if (!layeredTransparentRenderables.contains(zIndex))
        {
            layeredTransparentRenderables.emplace(zIndex, std::unordered_map<ShaderMaterial::FragShaderKey, std::vector<ECS::Entity>>());
        }

        auto &layer = layeredTransparentRenderables[zIndex];

        if (!layer.contains(key))
        {
            layer.emplace(key, std::vector<ECS::Entity>());
        }

        layer[key].push_back(e);
    }

    // create transparent batches
    size_t batchedEntities = 0;

    while (batchedEntities < transparentRenderables.size())
    {
        Batch batch;
        batch.transparent = true;

        bool looped = false;

        auto it = layeredTransparentRenderables.begin();

        // while there is only entities with the same shader on this layer
        // we keep adding them to the batch
        while (it != layeredTransparentRenderables.end() && it->second.size() == 1)
        {

            auto &layer = it->second;
            auto &key = layer.begin()->first;
            auto &entities = layer.begin()->second;

            if (looped && key != batch.key)
            {
                break;
            }

            batch.key = key;
            looped = true;

            batch.renderables.reserve(batch.renderables.size() + entities.size());
            batch.renderables.insert(batch.renderables.end(), entities.begin(), entities.end());

            it = layeredTransparentRenderables.erase(it);

            // std::cout << "added " << entities.size() << " entities to batch" << std::endl;
        }

        // std::cout << "batched " << batch.renderables.size() << " entities" << std::endl;

        // batch has been filled with entities with the same shader
        // we are now on a layer with entities with different shaders
        // so we must create new batches for each shader

        // try to add to current batch before creating new batches
        if (it != layeredTransparentRenderables.end())
        {
            auto &layer = it->second;

            if (looped && layer.contains(batch.key))
            {
                auto &entities = layer[batch.key];

                batch.renderables.reserve(batch.renderables.size() + entities.size());
                batch.renderables.insert(batch.renderables.end(), entities.begin(), entities.end());

                // remove from layer so we don't add them again
                layer.erase(batch.key);
            }

            // std::cout << "batched extra from same layer: " << batch.renderables.size() << std::endl;
        }

        // add batch to batches, we can move it because we don't need it anymore
        if (looped)
        {
            batchedEntities += batch.renderables.size();
            batches->emplace_back(std::move(batch));

            // std::cout << "added batch to batches" << std::endl;
        }

        // no more layers to check
        if (it == layeredTransparentRenderables.end())
        {
            break;
        }

        // create batches for each shader
        for (auto &key : it->second)
        {
            batchedEntities += key.second.size();
            batches->emplace_back(Batch{true, key.first, std::move(key.second)});

            // std::cout << "batched from different layer: " << batches->back().renderables.size() << std::endl;
            // std::cout << "added batch to batches" << std::endl;
        }

        // remove layer as we don't need it anymore
        layeredTransparentRenderables.erase(it);

        // std::cout << "removed layer" << std::endl;
    }

    // create output
    auto output = new RenderPassInputTyped<BatchPassData>(input, batches);

    delete inputTyped;

    return output;
}

void Rendering::BatchPass::sortRenderables(const ECS::Registry &registry, std::vector<ECS::Entity> &renderables)
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
    renderables.reserve(renderables.size());

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
