#pragma once

#include "Joint.h"

#include <box2d/b2_distance_joint.h>

namespace Physics
{
    class DistanceJoint : public Joint
    {
    public:
        /**
         * Creates a new distance joint.
         *
         * Anchor points are in local space of the respective body.
         *
         * @param connected The entity the joint is connected to.
         * @param anchorA The anchor point on the first body (the one owning the joint component).
         * @param anchorB The anchor point on the second body (the connected entity).
         */
        DistanceJoint(ECS::Entity connected, glm::vec2 anchorA = glm::vec2(0), glm::vec2 anchorB = glm::vec2(0));

        /**
         * Sets the length of the distance joint.
         *
         * This is the distance the joint will try to maintain between the two bodies.
         *
         * @param length The length of the distance joint.
         */
        void setLength(float length);

        /**
         * Gets the length of the distance joint.
         *
         * @returns The length of the distance joint.
         */
        float getLength() const;

        /**
         * Sets the minimum length of the distance joint.
         *
         * This is the minimum distance the joint will try to maintain between the two bodies.
         *
         * Must be less than or equal to the maximum length.
         *
         * @param minLength The minimum length of the distance joint.
         */
        void setMinLength(float minLength);

        /**
         * Gets the minimum length of the distance joint.
         *
         * @returns The minimum length of the distance joint.
         */
        float getMinLength() const;

        /**
         * Sets the maximum length of the distance joint.
         *
         * This is the maximum distance the joint will try to maintain between the two bodies.
         *
         * Must be greater than or equal to the minimum length.
         *
         * @param maxLength The maximum length of the distance joint.
         */
        void setMaxLength(float maxLength);

        /**
         * Gets the maximum length of the distance joint.
         *
         * @returns The maximum length of the distance joint.
         */
        float getMaxLength() const;

        /**
         * Sets the stiffness of the distance joint.
         *
         * This is the stiffness of the spring that the distance joint uses to maintain the distance between the two bodies.
         *
         * This is in N/m.
         *
         * @param stiffness The stiffness of the distance joint.
         */
        void setStiffness(float stiffness);

        /**
         * Gets the stiffness of the distance joint.
         *
         * @returns The stiffness of the distance joint.
         */
        float getStiffness() const;

        /**
         * Sets the damping of the distance joint.
         *
         * This is the damping of the spring that the distance joint uses to maintain the distance between the two bodies.
         *
         * This is in (N * s)/m.
         *
         * @param damping The damping of the distance joint.
         */
        void setDamping(float damping);

        /**
         * Gets the damping of the distance joint.
         *
         * @returns The damping of the distance joint.
         */
        float getDamping() const;

        /**
         * Sets the underlying Box2D joint.
         *
         * Must be a b2DistanceJoint.
         *
         * @warning Do not use this unless you know what you are doing.
         *
         * @param joint The Box2D joint.
         */
        void setJoint(b2Joint *joint) override;

        /**
         * Gets the underlying Box2D joint.
         *
         * May be nullptr if the joint has not been created yet.
         *
         * @warning Do not use this unless you know what you are doing.
         *
         * @returns The underlying Box2D joint.
         */
        b2DistanceJoint *getJoint() override;

        /**
         * Gets the underlying Box2D joint.
         *
         * May be nullptr if the joint has not been created yet.
         *
         * @warning Do not use this unless you know what you are doing.
         *
         * @returns The underlying Box2D joint.
         */
        const b2DistanceJoint *getJoint() const override;

    private:
        b2DistanceJoint *joint;

        float length;
        float minLength;
        float maxLength;
        float stiffness;
        float damping;
    };
}