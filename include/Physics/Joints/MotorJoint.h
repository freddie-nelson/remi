#pragma once

#include "./Joint.h"

#include <box2d/b2_motor_joint.h>

namespace Physics
{
    /**
     * A motor joint.
     *
     * This joint is used to control the relative motion between two bodies.
     *
     * It let's you control the motion of a body by specifying a linear and angular offset.
     *
     * A common use case is to control the motion of a dynamic body relative to the ground.
     */
    class MotorJoint : public Joint
    {
    public:
        MotorJoint(ECS::Entity connected);

        /**
         * Sets the linear offset of the motor joint.
         *
         * This is the target linear offset in the owning body's local space.
         *
         * @param linearOffset The linear offset of the motor joint.
         */
        void setLinearOffset(glm::vec2 linearOffset);

        /**
         * Gets the linear offset of the motor joint.
         *
         * This is the target linear offset in the owning body's local space.
         *
         * @returns The linear offset of the motor joint.
         */
        const glm::vec2 &getLinearOffset() const;

        /**
         * Sets the angular offset of the motor joint.
         *
         * This is the target angular offset of the motor joint.
         *
         * This is in radians.
         *
         * @param angularOffset The angular offset of the motor joint.
         */
        void setAngularOffset(float angularOffset);

        /**
         * Gets the angular offset of the motor joint.
         *
         * This is the target angular offset of the motor joint.
         *
         * This is in radians.
         *
         * @returns The angular offset of the motor joint.
         */
        float getAngularOffset() const;

        /**
         * Sets the maximum force the motor joint can apply.
         *
         * @param maxForce The maximum force the motor joint can apply.
         */
        void setMaxForce(float maxForce);

        /**
         * Gets the maximum force the motor joint can apply.
         *
         * @returns The maximum force the motor joint can apply.
         */
        float getMaxForce() const;

        /**
         * Sets the maximum torque the motor joint can apply.
         *
         * @param maxTorque The maximum torque the motor joint can apply.
         */
        void setMaxTorque(float maxTorque);

        /**
         * Gets the maximum torque the motor joint can apply.
         *
         * @returns The maximum torque the motor joint can apply.
         */
        float getMaxTorque() const;

        /**
         * Sets the correction factor of the motor joint.
         *
         * This is the position correction factor in the range [0, 1].
         *
         * @param correctionFactor The correction factor of the motor joint.
         */
        void setCorrectionFactor(float correctionFactor);

        /**
         * Gets the correction factor of the motor joint.
         *
         * This is the position correction factor in the range [0, 1].
         *
         * @returns The correction factor of the motor joint.
         */
        float getCorrectionFactor() const;

        /**
         * Sets the underlying Box2D joint.
         *
         * Must be a b2MotorJoint.
         *
         * @warning Do not use this unless you know what you are doing.
         *
         * @param joint The Box2D joint.
         */
        void setJoint(b2Joint *joint) override;

        /**
         * Creates the box2d motor joint.
         *
         * This does not set the joint component's joint variable.
         *
         * @param world The world the entity is in.
         * @param entity The entity that owns the joint.
         * @param box2dWorld The box2d world to create the joint in.
         * @param owner The box2d body of the owning entity.
         * @param connected The box2d body of the connected entity.
         *
         * @returns The box2d motor joint.
         */
        b2MotorJoint *createBox2DJoint(World::World &world, ECS::Entity entity, b2World *box2dWorld, b2Body *owner, b2Body *connected) override;

        /**
         * Gets the underlying Box2D joint.
         *
         * May be nullptr if the joint has not been created yet.
         *
         * @warning Do not use this unless you know what you are doing.
         *
         * @returns The underlying Box2D joint.
         */
        b2MotorJoint *getJoint() override;

        /**
         * Gets the underlying Box2D joint.
         *
         * May be nullptr if the joint has not been created yet.
         *
         * @warning Do not use this unless you know what you are doing.
         *
         * @returns The underlying Box2D joint.
         */
        const b2MotorJoint *getJoint() const override;

    private:
        b2MotorJoint *joint = nullptr;

        glm::vec2 linearOffset = glm::vec2(0);
        float angularOffset = 0;

        float maxForce = 1.0f;
        float maxTorque = 1.0f;
        float correctionFactor = 0.3f;
    };
}
