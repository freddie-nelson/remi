#pragma once

#include "../ECS/Entity.h"

#include <glm/vec2.hpp>

namespace Physics {
    /**
     * Contains information about a contact between two colliders.
    */
    struct ContactInfo {
        /**
         * The first entity involved in the contact.
         *
         * In a collision callback, this is the entity that the contact listener is attached to.
        */
        ECS::Entity entityA;

        /**
         * The second entity involved in the contact.
         *
         * In a collision callback, this is the other entity involved in the contact.
        */
        ECS::Entity entityB;

        /**
         * The friction of the contact.
        */
        float friction;

        /**
         * The restitution of the contact.
        */
        float restitution;

        /**
         * The restitution threshold of the contact.
        */
        float restitutionThreshold;

        /**
         * The tangent speed of the contact.
        */
        float tangentSpeed;

        /**
         * The normal of the collision.
        */
        glm::vec2 normal;

        /**
         * The point of the collision.
        */
        glm::vec2 point;
    };
}