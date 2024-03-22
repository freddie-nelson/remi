#pragma once

#include "Joint.h"
#include "../../Input/Mouse.h"

#include <box2d/b2_mouse_joint.h>

namespace Physics
{
    /**
     * A mouse joint.
     *
     * The joint tries to drive a point on the owning body towards the current position of the mouse.
     *
     * You can set the target position of the mouse joint, which is the position the joint will try to move the body towards.
     *
     * If autoUpdate is enabled, the target position will be updated every update to the current position of the mouse. By default this is enabled.
     */
    class MouseJoint : public Joint
    {
    public:
        /**
         * Creates a new mouse joint.
         *
         * @param target The target position of the mouse joint, should initially correspond to the anchor point on the body. In world space.
         * @param maxForce The maximum force the mouse joint can apply to move the body towards the mouse.
         * @param stiffness The stiffness of the spring that connects the body to the mouse.
         * @param damping The damping of the spring that connects the body to the mouse.
         */
        MouseJoint(glm::vec2 target, float maxForce = FLT_MAX, float stiffness = FLT_MAX, float damping = 0.0f);

        void setAnchorA(glm::vec2 anchorA) override;

        const glm::vec2 &getAnchorA() const override;

        void setAnchorB(glm::vec2 anchorB) override;

        const glm::vec2 &getAnchorB() const override;

        void setConnected(ECS::Entity connected) override;

        ECS::Entity getConnected() const override;

        void setCollideConnected(bool collideConnected) override;

        bool getCollideConnected() const override;

        /**
         * Sets the target position of the mouse joint.
         *
         * This is the position the joint will try to move the body towards.
         *
         * In world space.
         *
         * @param target The target position.
         */
        void setTarget(glm::vec2 target);

        /**
         * Gets the target position of the mouse joint.
         *
         * @returns The target position.
         */
        const glm::vec2 &getTarget() const;

        /**
         * Sets the maximum force the mouse joint can apply.
         *
         * This is the maximum force the joint can apply to move the body towards the mouse.
         *
         * @param maxForce The maximum force of the mouse joint.
         */
        void setMaxForce(float maxForce);

        /**
         * Gets the maximum force the mouse joint can apply.
         *
         * @returns The maximum force of the mouse joint.
         */
        float getMaxForce() const;

        /**
         * Sets the stiffness of the mouse joint.
         *
         * This is the stiffness of the spring that connects the body to the mouse.
         *
         * @param stiffness The stiffness of the mouse joint.
         */
        void setStiffness(float stiffness);

        /**
         * Gets the stiffness of the mouse joint.
         *
         * @returns The stiffness of the mouse joint.
         */
        float getStiffness() const;

        /**
         * Sets the damping of the mouse joint.
         *
         * This is the damping of the spring that connects the body to the mouse.
         *
         * @param damping The damping of the mouse joint.
         */
        void setDamping(float damping);

        /**
         * Gets the damping of the mouse joint.
         *
         * @returns The damping of the mouse joint.
         */
        float getDamping() const;

        /**
         * Sets wether or not the target position of the mouse joint should be updated every update to the current position of the mouse.
         *
         * By default this is enabled.
         *
         * @param autoUpdate Wether or not the target position of the mouse joint should be updated every update to the current position of the mouse.
         */
        void setAutoUpdate(bool autoUpdate);

        /**
         * Gets wether or not the target position of the mouse joint should be updated every update to the current position of the mouse.
         *
         * @returns Wether or not the target position of the mouse joint should be updated every update to the current position of the mouse.
         */
        bool getAutoUpdate() const;

        /**
         * Sets the underlying Box2D joint.
         *
         * Must be a b2MouseJoint.
         *
         * @param joint The Box2D joint.
         */
        void setJoint(b2Joint *joint) override;

        /**
         * Creates the box2d mouse joint.
         *
         * This does not set the joint component's joint variable.
         *
         * @param world The world the entity is in.
         * @param entity The entity that owns the joint.
         * @param box2dWorld The box2d world to create the joint in.
         * @param owner The box2d body of the owning entity.
         * @param connected The box2d body of the connected entity, must be a static body.
         *
         * @returns The box2d mouse joint.
         */
        b2MouseJoint *createBox2DJoint(World::World &world, ECS::Entity entity, b2World *box2dWorld, b2Body *owner, b2Body *connected) override;

        /**
         * Gets the underlying Box2D joint.
         *
         * May be nullptr if the joint has not been created yet.
         *
         * @warning Do not use this unless you know what you are doing.
         *
         * @returns The underlying Box2D joint.
         */
        b2MouseJoint *getJoint() override;

        /**
         * Gets the underlying Box2D joint.
         *
         * May be nullptr if the joint has not been created yet.
         *
         * @warning Do not use this unless you know what you are doing.
         *
         * @returns The underlying Box2D joint.
         */
        const b2MouseJoint *getJoint() const override;

    private:
        b2MouseJoint *joint = nullptr;

        bool autoUpdate = true;
        glm::vec2 target;
        float maxForce = 0.0f;
        float stiffness = 1.0f;
        float damping = 0.0f;
    };
}