#include "../../../include/Rendering/Passes/DebugRenderTreePass.h"

#include <stdexcept>

Rendering::DebugRenderTreePass::DebugRenderTreePass(const CullingPass *cullingPass) : cullingPass(cullingPass)
{
    if (cullingPass == nullptr)
    {
        throw std::invalid_argument("DebugRenderTreePass (constructor): cullingPass cannot be null.");
    }
}

Rendering::RenderPassInput *Rendering::DebugRenderTreePass::execute(Rendering::RenderPassInput *input)
{
    checkInput<int>(input);

    auto &world = *input->world;
    auto &registry = world.getRegistry();
    auto &renderer = *input->renderer;

    auto &renderTarget = *input->renderTarget;
    auto &textureManager = *input->textureManager;

    cullingPass->drawAABBTree(true, world, renderer, renderTarget, textureManager);

    return input;
}