#pragma once

#include "../../World/World.h"
#include "../../ECS/System.h"

namespace Rendering
{
    /**
     * The animation system.
     *
     * Steps all AnimatedTextures in the registry every frame.
     */
    class AnimationSystem : public ECS::System
    {
    public:
        /**
         * Creates an animation system.
         */
        AnimationSystem();

        /**
         * Destroys the animation system.
         */
        ~AnimationSystem();

        /**
         * Updates the system.
         *
         * @param data The system update data, holds references to the world, physics world, space transformer, and timestep.
         */
        virtual void update(const ECS::System::SystemUpdateData &data) override;
    };
}