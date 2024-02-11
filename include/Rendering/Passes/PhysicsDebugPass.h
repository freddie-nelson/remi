
#pragma once

#include "RenderPass.h"
#include "../../physics/PhysicsWorld.h"

namespace Rendering
{
    /**
     * Represents a physics debug pass.
     *
     * This pass renders boxes around the colliders in the scene.
     */
    class PhysicsDebugPass : public RenderPass
    {
    public:
        /**
         * Creates a PhysicsDebugPass instance.
         *
         * @param physicsWorld The physics world to render.
         */
        PhysicsDebugPass(const Physics::PhysicsWorld *physicsWorld);

        /**
         * Destroys the PhysicsDebugPass instance.
         */
        ~PhysicsDebugPass();

        /**
         * Executes the post processor.
         *
         * @param input The input to the post processor.
         */
        RenderPassInput *execute(RenderPassInput *input) override;

        /**
         * Gets the name of the render pass.
         *
         * @returns The name of the render pass.
         */
        virtual constexpr std::string getName() override
        {
            return "PhysicsDebugPass";
        };

    private:
        const Physics::PhysicsWorld *physicsWorld;
    };
}