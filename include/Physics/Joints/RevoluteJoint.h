#pragma once

#include "Joint.h"

#include <box2d/b2_revolute_joint.h>

namespace Physics
{
    class RevoluteJoint : public Joint
    {
    public:
        /**
         * Creates a new revolute joint.
         *
         * Anchor points are in local space of the respective body.
         *
         * @param connected The entity the joint is connected to.
         * @param anchorA The anchor point on the first body (the one owning the joint component).
         * @param anchorB The anchor point on the second body (the connected entity).
         */
        RevoluteJoint(ECS::Entity connected, glm::vec2 anchorA = glm::vec2(0), glm::vec2 anchorB = glm::vec2(0));

        /**
         * Sets the reference angle of the revolute joint.
         *
         * This is the angle between the bodies when the joint is created.
         *
         * @param referenceAngle The reference angle of the revolute joint.
         *
         * @returns The actual reference angle of the revolute joint.
         */
        void setReferenceAngle(float referenceAngle);

        /**
         * Gets the reference angle of the revolute joint.
         *
         * @returns The reference angle of the revolute joint.
         */
        float getReferenceAngle() const;

        /**
         * Sets wether or not the revolute joint has limits enabled (lower and upper angle limits).
         *
         * @param enable Wether or not the revolute joint has limits enabled.
         */
        void enableLimit(bool enable);

        /**
         * Gets wether or not the revolute joint has limits enabled (lower and upper angle limits).
         *
         * @returns Wether or not the revolute joint has limits enabled.
         */
        bool isLimitEnabled() const;

        /**
         * Sets the lower angle limit of the revolute joint.
         *
         * This is the minimum angle the joint will allow the bodies to rotate to.
         *
         * @param lowerAngle The lower angle limit of the revolute joint.
         */
        void setLowerAngle(float lowerAngle);

        /**
         * Gets the lower angle limit of the revolute joint.
         *
         * @returns The lower angle limit of the revolute joint.
         */
        float getLowerAngle() const;

        /**
         * Sets the upper angle limit of the revolute joint.
         *
         * This is the maximum angle the joint will allow the bodies to rotate to.
         *
         * @param upperAngle The upper angle limit of the revolute joint.
         */
        void setUpperAngle(float upperAngle);

        /**
         * Gets the upper angle limit of the revolute joint.
         *
         * @returns The upper angle limit of the revolute joint.
         */
        float getUpperAngle() const;

        /**
         * Sets wether or not the revolute joint has a motor enabled.
         *
         * @param enable Wether or not the revolute joint has a motor enabled.
         */
        void enableMotor(bool enable);

        /**
         * Gets wether or not the revolute joint has a motor enabled.
         *
         * @returns Wether or not the revolute joint has a motor enabled.
         */
        bool isMotorEnabled() const;

        /**
         * Sets the motor speed of the revolute joint.
         *
         * This is the desired speed the joint will try to achieve.
         *
         * This is measured in radians per second.
         *
         * @param motorSpeed The motor speed of the revolute joint.
         */
        void setMotorSpeed(float motorSpeed);

        /**
         * Gets the motor speed of the revolute joint.
         *
         * This is the desired speed the joint will try to achieve.
         *
         * This is measured in radians per second.
         *
         * @returns The motor speed of the revolute joint.
         */
        float getMotorSpeed() const;

        /**
         * Sets the maximum motor torque of the revolute joint.
         *
         * This is the maximum torque the motor can apply to achieve the desired speed.
         *
         * This is measured in newton meters.
         *
         * @param maxMotorTorque The maximum motor torque of the revolute joint.
         */
        void setMaxMotorTorque(float maxMotorTorque);

        /**
         * Gets the maximum motor torque of the revolute joint.
         *
         * This is the maximum torque the motor can apply to achieve the desired speed.
         *
         * This is measured in newton meters.
         *
         * @returns The maximum motor torque of the revolute joint.
         */
        float getMaxMotorTorque() const;

        /**
         * Sets the underlying Box2D joint.
         *
         * Must be a b2RevoluteJoint.
         *
         * @warning Do not use this unless you know what you are doing.
         *
         * @param joint The Box2D joint.
         */
        void setJoint(b2Joint *joint) override;

        /**
         * Creates the box2d revolute joint.
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
        b2RevoluteJoint *createBox2DJoint(World::World &world, ECS::Entity entity, b2World *box2dWorld, b2Body *owner, b2Body *connected) override;

        /**
         * Gets the underlying Box2D joint.
         *
         * May be nullptr if the joint has not been created yet.
         *
         * @warning Do not use this unless you know what you are doing.
         *
         * @returns The underlying Box2D joint.
         */
        b2RevoluteJoint *getJoint() override;

        /**
         * Gets the underlying Box2D joint.
         *
         * May be nullptr if the joint has not been created yet.
         *
         * @warning Do not use this unless you know what you are doing.
         *
         * @returns The underlying Box2D joint.
         */
        const b2RevoluteJoint *getJoint() const override;

    private:
        b2RevoluteJoint *joint = nullptr;

        float referenceAngle = 0.0f;

        bool isEnableLimit = false;
        float lowerAngle = 0.0f;
        float upperAngle = 0.0f;

        bool isEnableMotor = false;
        float motorSpeed = 0.0f;
        float maxMotorTorque = 0.0f;
    };
}