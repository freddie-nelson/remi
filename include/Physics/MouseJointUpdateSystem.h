#pragma once

#include "../World/World.h"
#include "Joints/MouseJoint.h"
#include "../Input/Mouse.h"
#include "../Core/SpaceTransformer.h"
#include "../ECS/System.h"

namespace Physics
{
    /**
     * A system that updates mouse joints.
     */
    class MouseJointUpdateSystem : public ECS::System
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
         * @param data The system update data.
         */
        void update(const ECS::System::SystemUpdateData &data) override;

    private:
        Input::Mouse *mouse;
        Core::SpaceTransformer *spaceTransformer;
    };
}
