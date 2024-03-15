
#pragma once

#include "RenderPass.h"
#include "CullingPass.h"
#include "../../physics/PhysicsWorld.h"

namespace Rendering
{
    /**
     * Represents a debug render tree pass.
     *
     * This pass renders the aabb trees from the culling pass.
     */
    class DebugRenderTreePass : public RenderPass
    {
    public:
        /**
         * Creates a DebugRenderTreePass instance.
         *
         * @param cullingPass The culling pass draw the trees with.
         */
        DebugRenderTreePass(const CullingPass *cullingPass);

        /**
         * Destroys the DebugRenderTreePass instance.
         */
        ~DebugRenderTreePass() = default;

        /**
         * Executes the render pass.
         *
         * @param input The input to the render pass.
         */
        RenderPassInput *execute(RenderPassInput *input) override;

        /**
         * Gets the name of the render pass.
         *
         * @returns The name of the render pass.
         */
        virtual constexpr std::string getName() override
        {
            return "DebugRenderTreePass";
        };

    private:
        const CullingPass *cullingPass;
    };
}