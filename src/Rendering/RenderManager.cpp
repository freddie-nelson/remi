#include "../../include/Rendering/RenderManager.h"

Rendering::RenderManager::RenderManager(Rendering::Renderer *renderer, Rendering::RenderPipeline *pipeline, Core::SpaceTransformer *spaceTransformer) : renderer(renderer), pipeline(pipeline), spaceTransformer(spaceTransformer)
{
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

    // this will be deleted by the pipeline
    auto input = new RenderPassInputTyped<int>();
    input->renderer = renderer;
    input->registry = &registry;
    input->camera = cameraEntity;
    input->renderTarget = renderTarget;
    input->textureManager = &this->renderer->getTextureManager();
    input->spaceTransformer = this->spaceTransformer;
    input->data = new int(0);

    pipeline->execute(input);
}