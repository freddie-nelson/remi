#pragma once

#include "../../World/World.h"

namespace Rendering
{
    /**
     * The animation system.
     *
     * Steps all AnimatedTextures in the registry every frame.
     */
    class AnimationSystem : public World::System
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
         * @param world The world to use.
         * @param timestep The timestep since the last update.
         */
        virtual void update(World::World &world, const Core::Timestep &timestep);
    };
}