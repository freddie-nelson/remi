#pragma once

#include "../World/World.h"
#include "Joints/MouseJoint.h"
#include "../Input/Mouse.h"
#include "../Core/SpaceTransformer.h"

namespace Physics
{
    /**
     * A system that updates mouse joints.
     */
    class MouseJointUpdateSystem : public World::System
    {
    public:
        /**
         * Creates a new mouse joint update system.
         *
         * @param mouse The mouse input.
         * @param spaceTransformer The space transformer.
         */
        MouseJointUpdateSystem(Input::Mouse *mouse, Core::SpaceTransformer *spaceTransformer);

        /**
         * Updates the mouse joints.
         *
         * @param world The world.
         * @param timestep The timestep.
         */
        void update(World::World &world, const Core::Timestep &timestep) override;

    private:
        Input::Mouse *mouse;
        Core::SpaceTransformer *spaceTransformer;
    };
}
