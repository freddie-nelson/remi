#pragma once

#include "Joint.h"

#include <box2d/b2_distance_joint.h>

namespace Physics
{
    /**
     * A distance joint.
     *
     * This joint connects the bodies at the anchor points and tries to maintain the distance between the two bodies.
     *
     * You may also set a stiffness and damping to the joint to make it act like a spring.
     */
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
         * Sets the rest length of the distance joint.
         *
         * This is the distance the joint will try to maintain between the two bodies.
         *
         * @param length The length of the distance joint.
         *
         * @returns The actual rest length of the distance joint.
         */
        float setLength(float length);

        /**
         * Gets the rest length of the distance joint.
         *
         * If -1 then the length hasn't been set yet.
         *
         * @returns The rest length of the distance joint.
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
         *
         * @returns The actual minimum length of the distance joint.
         */
        float setMinLength(float minLength);

        /**
         * Gets the minimum length of the distance joint.
         *
         * If -1 then the min length hasn't been set yet.
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
         *
         * @returns The actual maximum length of the distance joint.
         */
        float setMaxLength(float maxLength);

        /**
         * Gets the maximum length of the distance joint.
         *
         * If -1 then the max length hasn't been set yet.
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
         * Usually between 0 and 1.
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
         * Usually between 0 and 1.
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
         * Creates the box2d distance joint.
         *
         * This does not set the joint component's joint variable.
         *
         * @param world The world the entity is in.
         * @param entity The entity that owns the joint.
         * @param box2dWorld The box2d world to create the joint in.
         * @param owner The box2d body of the owning entity.
         * @param connected The box2d body of the connected entity.
         *
         * @returns The box2d distance joint.
         */
        b2DistanceJoint *createBox2DJoint(World::World &world, ECS::Entity entity, b2World *box2dWorld, b2Body *owner, b2Body *connected) override;

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
        b2DistanceJoint *joint = nullptr;

        float length = -1.0f;
        float minLength = -1.0f;
        float maxLength = -1.0f;
        float stiffness = 1.0f;
        float damping = 0.0f;
    };
}