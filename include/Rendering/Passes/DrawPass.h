
#pragma once

#include "RenderPass.h"

namespace Rendering
{
    /**
     * Represents a draw pass.
     *
     * This pass batch renders all the renderable entities in the scene.
     *
     * They will be rendered to the render target.
     */
    class DrawPass : public RenderPass
    {
    public:
        /**
         * Creates a DrawPass instance.
         */
        DrawPass() = default;

        /**
         * Destroys the DrawPass instance.
         */
        virtual ~DrawPass() = default;

        /**
         * Executes the draw pass.
         *
         * Requires the output of the batch pass.
         *
         * @param input The input to the draw pass.
         */
        RenderPassInput *execute(RenderPassInput *input) override;

        /**
         * Gets the name of the render pass.
         *
         * @returns The name of the render pass.
         */
        virtual constexpr std::string getName() override
        {
            return "BatchPass";
        };
    };
}