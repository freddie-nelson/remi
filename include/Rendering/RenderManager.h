#pragma once

#include "./Renderer.h"
#include "./RenderPipeline.h"
#include "../Core/SpaceTransformer.h"

namespace Rendering
{
    /**
     * Represents the render manager.
     *
     * The render manager orchestrates the rendering of the scene.
     *
     * ! TODO: Implement multiple render pipelines, that allow selection of pipeline per entity through the renderable component.
     */
    class RenderManager
    {
    public:
        /**
         * Creates the render manager.
         *
         * @param renderer The renderer to use for rendering.
         * @param pipeline The render pipeline to execute on render.
         * @param spaceTransformer The space transformer to use for converting between coordinate spaces. This is passed to each render pass.
         */
        RenderManager(Renderer *renderer, RenderPipeline *pipeline, Core::SpaceTransformer *spaceTransformer);

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
        Core::SpaceTransformer *spaceTransformer;
    };
}