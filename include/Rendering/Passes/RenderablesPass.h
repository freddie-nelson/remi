#pragma once

#include "RenderPass.h"

#include <vector>
#include <unordered_set>

namespace Rendering
{
    struct RenderablesPassData
    {
        std::vector<ECS::Entity> staticRenderables;
        std::vector<ECS::Entity> newStaticRenderables;

        std::vector<ECS::Entity> dynamicRenderables;
        std::vector<ECS::Entity> newDynamicRenderables;
    };

    /**
     * Represents a renderables pass.
     *
     * This pass gets all the renderable entities in the scene.
     *
     * This pass does not require any data in the input.
     *
     * The output of this pass is a list of renderable entities.
     *
     * The lists of renderable entities is not culled for entities that are not in the camera's view.
     *
     * This pass is the first pass in the default render pipeline.
     */
    class RenderablesPass : public RenderPass
    {
    public:
        /**
         * Creates a RenderablesPass instance.
         */
        RenderablesPass() = default;

        /**
         * Destroys the RenderablesPass instance.
         */
        virtual ~RenderablesPass() = default;

        /**
         * Executes the renderables pass.
         *
         * Does not require any data in the input.
         *
         * @param input The input to the renderables pass.
         */
        RenderPassInput *execute(RenderPassInput *input) override;

        /**
         * Gets the name of the render pass.
         *
         * @returns The name of the render pass.
         */
        virtual constexpr std::string getName() override
        {
            return "RenderablesPass";
        };

    private:
        std::vector<ECS::Entity> oldEntities;
    };
}