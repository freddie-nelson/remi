#pragma once

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
         * @param registry The registry to use.
         * @param timestep The timestep since the last update.
         * @param events The events that have occurred since the last update.
         */
        virtual void update(const ECS::Registry &registry, const Core::Timestep &timestep);
    };
}