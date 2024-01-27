#include "../../../include/Rendering/Passes/BatchPass.h"
#include "../../../include/Rendering/Passes/MaterialPass.h"

Rendering::RenderPassInput *Rendering::BatchPass::execute(Rendering::RenderPassInput *input)
{
    checkInput<MaterialPassData>(input);

    auto inputTyped = static_cast<RenderPassInputTyped<MaterialPassData> *>(input);

    auto &renderer = *inputTyped->renderer;
    auto &registry = *inputTyped->registry;
    auto camera = inputTyped->camera;
    auto &renderTarget = *inputTyped->renderTarget;
    auto &textureManager = *inputTyped->textureManager;

    // bind render target
    renderTarget.bind(textureManager);

    // batch render
    auto &[opaqueRenderables, transparentRenderables, keys] = *inputTyped->data;

    for (auto &key : keys)
    {
        if (opaqueRenderables.contains(key))
            renderer.batch(registry, camera, opaqueRenderables[key]);

        // if alpha blending is not enabled, then this will be empty
        if (transparentRenderables.contains(key))
            renderer.batch(registry, camera, transparentRenderables[key]);
    }

    // unbind render target
    renderTarget.unbind();

    // create output
    auto output = new RenderPassInputTyped<int>;
    output->renderer = inputTyped->renderer;
    output->registry = inputTyped->registry;
    output->camera = inputTyped->camera;
    output->renderTarget = inputTyped->renderTarget;
    output->textureManager = inputTyped->textureManager;
    output->data = 0;

    delete inputTyped;

    return output;
}
