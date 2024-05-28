#pragma once

#include "./Joint.h"

#include <box2d/b2_weld_joint.h>

namespace Physics
{
    /**
     * A weld joint.
     *
     * This join connects two bodies at a point in world coordinates.
     *
     * A stiffness and damping can be set to change the reaction torque of the joint.
     */
    class WeldJoint : public Joint
    {
    public:
        /**
         * Creates a new weld joint.
         *
         * @param connected The entity the joint is connected to.
         * @param worldAnchor The point of connection in world coordinates.
         */
        WeldJoint(ECS::Entity connected, glm::vec2 worldAnchor = glm::vec2(0));

        /**
         * Sets the anchor point on the first body (the one owning the joint component).
         *
         * This is in local space of the first body.
         *
         * @param anchor The anchor point.
         */
        void setAnchorA(glm::vec2 anchorA) override;

        /**
         * Sets the anchor point on the second body (the connected entity).
         *
         * This is in local space of the second body.
         *
         * @param anchor The anchor point.
         */
        void setAnchorB(glm::vec2 anchorB) override;

        /**
         * Sets the world anchor point of the weld joint.
         *
         * @param worldAnchor The world anchor point of the weld joint.
         */
        void setWorldAnchor(glm::vec2 worldAnchor);

        /**
         * Gets the world anchor point of the weld joint.
         *
         * @returns The world anchor point of the weld joint.
         */
        const glm::vec2 &getWorldAnchor() const;

        /**
         * Sets the stiffness of the weld joint.
         *
         * @param stiffness The stiffness of the weld joint.
         */
        void setStiffness(float stiffness);

        /**
         * Gets the stiffness of the weld joint.
         *
         * @returns The stiffness of the weld joint.
         */
        float getStiffness() const;

        /**
         * Sets the damping of the weld joint.
         *
         * @param damping The damping of the weld joint.
         */
        void setDamping(float damping);

        /**
         * Gets the damping of the weld joint.
         *
         * @returns The damping of the weld joint.
         */
        float getDamping() const;

        /**
         * Sets the reference angle of the weld joint.
         *
         * This is the angle between the two bodies in the reference state.
         *
         * @param referenceAngle The reference angle of the weld joint.
         */
        void setReferenceAngle(float referenceAngle);

        /**
         * Gets the reference angle of the weld joint.
         *
         * This is the angle between the two bodies in the reference state.
         *
         * @returns The reference angle of the weld joint.
         */
        float getReferenceAngle() const;

        /**
         * Sets the underlying Box2D joint.
         *
         * Must be a b2WeldJoint.
         *
         * @warning Do not use this unless you know what you are doing.
         *
         * @param joint The Box2D joint.
         */
        void setJoint(b2Joint *joint) override;

        /**
         * Creates the box2d weld joint.
         *
         * This does not set the joint component's joint variable.
         *
         * @param world The world the entity is in.
         * @param entity The entity that owns the joint.
         * @param box2dWorld The box2d world to create the joint in.
         * @param owner The box2d body of the owning entity.
         * @param connected The box2d body of the connected entity.
         *
         * @returns The box2d weld joint.
         */
        b2WeldJoint *createBox2DJoint(World::World &world, ECS::Entity entity, b2World *box2dWorld, b2Body *owner, b2Body *connected) override;

        /**
         * Gets the underlying Box2D joint.
         *
         * May be nullptr if the joint has not been created yet.
         *
         * @warning Do not use this unless you know what you are doing.
         *
         * @returns The underlying Box2D joint.
         */
        b2WeldJoint *getJoint() override;

        /**
         * Gets the underlying Box2D joint.
         *
         * May be nullptr if the joint has not been created yet.
         *
         * @warning Do not use this unless you know what you are doing.
         *
         * @returns The underlying Box2D joint.
         */
        const b2WeldJoint *getJoint() const override;

    private:
        b2WeldJoint *joint = nullptr;

        glm::vec2 worldAnchor = glm::vec2(0);
        bool useWorldAnchor = false;

        float stiffness = 0.0f;
        float damping = 0.0f;

        float referenceAngle = 0.0f;
        bool referenceAngleSet = false;
    };
}