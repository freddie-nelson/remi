#pragma once

#include "../ECS/Entity.h"

namespace Physics {
    /**
     * The user data that is attached to a Box2D body created by the physics world.
    */
    struct BodyUserData {
        /**
         * Creates a new body user data.
         *
         * @param entity The entity that the body is attached to.
         * @param ignoreCollisionCallbacks Whether the body should ignore collision callbacks.
        */
        BodyUserData(ECS::Entity entity, bool ignoreCollisionCallbacks = false) : entity(entity), ignoreCollisionCallbacks(ignoreCollisionCallbacks) {}

        /**
         * The entity that the body is attached to.
        */
        ECS::Entity entity;

        /**
         * Whether the body should ignore collision callbacks.
        */
        bool ignoreCollisionCallbacks;
    };
}