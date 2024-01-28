
#pragma once

#include "RenderPass.h"

namespace Rendering
{
    /**
     * Represents a batch pass.
     *
     * This pass batch renders all the renderable entities in the scene.
     *
     * They will be rendered to the render target.
     */
    class BatchPass : public RenderPass
    {
    public:
        /**
         * Creates a BatchPass instance.
         */
        BatchPass() = default;

        /**
         * Destroys the BatchPass instance.
         */
        virtual ~BatchPass() = default;

        /**
         * Executes the batch pass.
         *
         * Requires the output of the material pass.
         *
         * @param input The input to the batch pass.
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