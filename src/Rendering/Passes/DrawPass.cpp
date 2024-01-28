#include "../../../include/Rendering/Passes/DrawPass.h"
#include "../../../include/Rendering/Passes/BatchPass.h"

Rendering::RenderPassInput *Rendering::DrawPass::execute(Rendering::RenderPassInput *input)
{
    checkInput<BatchPassData>(input);

    auto inputTyped = static_cast<RenderPassInputTyped<BatchPassData> *>(input);

    auto &renderer = *inputTyped->renderer;
    auto &registry = *inputTyped->registry;
    auto camera = inputTyped->camera;
    auto &renderTarget = *inputTyped->renderTarget;
    auto &textureManager = *inputTyped->textureManager;

    // bind render target
    renderTarget.bind(textureManager);

    // save depth write
    bool isDepthWriteEnabled = renderer.isDepthWriteEnabled();

    // batch render
    for (auto &batch : *inputTyped->data)
    {
        renderer.enableDepthWrite(!batch.transparent);
        renderer.batch(registry, camera, batch.renderables);
    }

    // restore depth write
    renderer.enableDepthWrite(isDepthWriteEnabled);

    // unbind render target
    renderTarget.unbind(textureManager);

    // create output
    auto output = new RenderPassInputTyped<int>(input, new int(0));

    delete inputTyped;

    return output;
}
