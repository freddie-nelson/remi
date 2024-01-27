#include "../../include/Rendering/RenderManager.h"

Rendering::RenderManager::RenderManager(Rendering::Renderer *renderer, Rendering::RenderPipeline *pipeline)
{
    this->renderer = renderer;
    this->pipeline = pipeline;
}

void Rendering::RenderManager::render(const ECS::Registry &registry, ECS::Entity *camera, const RenderTarget *renderTarget)
{
    ECS::Entity cameraEntity = 0;
    if (camera == nullptr)
    {
        cameraEntity = this->renderer->getActiveCamera(registry);
    }
    else
    {
        cameraEntity = *camera;
    }

    if (renderTarget == nullptr)
    {
        renderTarget = this->renderer->getRenderTarget();
    }

    RenderPassInputTyped<int> input;
    input.renderer = renderer;
    input.registry = &registry;
    input.camera = cameraEntity;
    input.renderTarget = renderTarget;
    input.textureManager = &this->renderer->getTextureManager();
    input.data = 0;

    pipeline->execute(&input);
}