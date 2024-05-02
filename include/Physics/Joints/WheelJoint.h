#pragma once

#include "Joint.h"

#include <box2d/b2_wheel_joint.h>

namespace Physics
{
    /**
     * A wheel joint.
     *
     * A wheel joint is used to simulate a wheel-like joint. It can be used to simulate a car wheel or a caster wheel.
     *
     * The wheel joint restricts a point on the first body to a line on the second body. The wheel joint also provides a suspension spring.
     *
     * The first/owning body is the wheel.
     */
    class WheelJoint : public Joint
    {
    public:
        /**
         * Creates a new wheel joint.
         *
         * @param connected The entity the joint is connected to. This is the car/wheel owner entity.
         * @param anchor The anchor point on the first body (the one owning the joint component). In local space.
         * @param axis The axis of the wheel joint. In local space.
         */
        WheelJoint(ECS::Entity connected, glm::vec2 anchor, glm::vec2 axis);

        /**
         * Sets the anchor point of the wheel joint.
         *
         * @param anchor The anchor point of the wheel joint. In local space.
         */
        void setAnchor(glm::vec2 anchor);

        /**
         * Gets the anchor point of the wheel joint.
         *
         * @returns The anchor point of the wheel joint. In local space.
         */
        const glm::vec2 &getAnchor() const;

        /**
         * Sets the axis of the wheel joint.
         *
         * @param axis The axis of the wheel joint. In local space.
         */
        void setAxis(glm::vec2 axis);

        /**
         * Gets the axis of the wheel joint.
         *
         * @returns The axis of the wheel joint. In local space.
         */
        const glm::vec2 &getAxis() const;

        /**
         * Sets whether the joint has a limit.
         *
         * @param enable Whether the joint has a limit.
         */
        void enableLimit(bool enable);

        /**
         * Gets whether the joint has a limit.
         *
         * @returns Whether the joint has a limit.
         */
        bool isLimitEnabled() const;

        /**
         * Sets the lower and translation limits of the joint.
         *
         * @param lowerTranslation The lower translation limit.
         */
        void setLowerTranslation(float lowerTranslation);

        /**
         * Gets the lower translation limit of the joint.
         *
         * @returns The lower translation limit.
         */
        float getLowerTranslation() const;

        /**
         * Sets the upper translation limit of the joint.
         *
         * @param upperTranslation The upper translation limit.
         */
        void setUpperTranslation(float upperTranslation);

        /**
         * Gets the upper translation limit of the joint.
         *
         * @returns The upper translation limit.
         */
        float getUpperTranslation() const;

        /**
         * Sets whether the joint has a motor.
         *
         * @param enable Whether the joint has a motor.
         */
        void enableMotor(bool enable);

        /**
         * Gets whether the joint has a motor enabled.
         *
         * @returns Whether the joint has a motor enabled.
         */
        bool isMotorEnabled() const;

        /**
         * Sets the maximum motor torque of the joint.
         *
         * @param maxMotorTorque The maximum motor torque.
         */
        void setMaxMotorTorque(float maxMotorTorque);

        /**
         * Gets the maximum motor torque of the joint.
         *
         * @returns The maximum motor torque.
         */
        float getMaxMotorTorque() const;

        /**
         * Sets the motor speed of the joint.
         *
         * @param motorSpeed The motor speed.
         */
        void setMotorSpeed(float motorSpeed);

        /**
         * Gets the motor speed of the joint.
         *
         * @returns The maximum motor speed.
         */
        float getMotorSpeed() const;

        /**
         * Sets the spring stiffness of the joint.
         *
         * @param stiffness The spring stiffness.
         */
        void setStiffness(float stiffness);

        /**
         * Gets the spring stiffness of the joint.
         *
         * @returns The spring stiffness.
         */
        float getStiffness() const;

        /**
         * Sets the spring damping of the joint.
         *
         * @param damping The spring damping.
         */
        void setDamping(float damping);

        /**
         * Gets the spring damping of the joint.
         *
         * @returns The spring damping.
         */
        float getDamping() const;

        /**
         * Sets the underlying Box2D joint.
         *
         * Must be a b2WheelJoint.
         *
         * @warning Do not use this unless you know what you are doing.
         *
         * @param joint The Box2D joint.
         */
        void setJoint(b2Joint *joint) override;

        /**
         * Creates the box2d wheel joint.
         *
         * This does not set the joint component's joint variable.
         *
         * @param world The world the entity is in.
         * @param entity The entity that owns the joint.
         * @param box2dWorld The box2d world to create the joint in.
         * @param owner The box2d body of the owning entity.
         * @param connected The box2d body of the connected entity.
         *
         * @returns The box2d wheel joint.
         */
        b2WheelJoint *createBox2DJoint(World::World &world, ECS::Entity entity, b2World *box2dWorld, b2Body *owner, b2Body *connected) override;

        /**
         * Gets the underlying Box2D joint.
         *
         * May be nullptr if the joint has not been created yet.
         *
         * @warning Do not use this unless you know what you are doing.
         *
         * @returns The underlying Box2D joint.
         */
        b2WheelJoint *getJoint() override;

        /**
         * Gets the underlying Box2D joint.
         *
         * May be nullptr if the joint has not been created yet.
         *
         * @warning Do not use this unless you know what you are doing.
         *
         * @returns The underlying Box2D joint.
         */
        const b2WheelJoint *getJoint() const override;

    private:
        b2WheelJoint *joint = nullptr;

        glm::vec2 anchor;
        glm::vec2 axis;

        bool isEnableLimit = false;
        float lowerTranslation = 0.0f;
        float upperTranslation = 0.0f;

        bool isEnableMotor = false;
        float maxMotorTorque = 0.0f;
        float motorSpeed = 0.0f;

        float stiffness = 0.0f;
        float damping = 0.0f;
    };
}