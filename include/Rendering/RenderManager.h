#pragma once

#include "./Renderer.h"
#include "./RenderPipeline.h"

namespace Rendering
{
    /**
     * Represents the render manager.
     *
     * The render manager orchestrates the rendering of the scene.
     */
    class RenderManager
    {
    public:
        /**
         * Creates the render manager.
         */
        RenderManager(Renderer *renderer, RenderPipeline *pipeline);

        /**
         * Destroys the render manager.
         *
         * Destroys the renderer and render pipeline.
         */
        ~RenderManager() = default;

        /**
         * Renders the scene.
         *
         * If no camera is provided, the active camera will be used.
         *
         * If no render target is provided, the default render target will be used.
         *
         * @param registry The registry to read data from.
         * @param camera The camera to render with.
         * @param renderTarget The render target to render to.
         */
        void render(const ECS::Registry &registry, ECS::Entity *camera = nullptr, const RenderTarget *renderTarget = nullptr);

    private:
        Renderer *renderer;
        RenderPipeline *pipeline;
    };
}