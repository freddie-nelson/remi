#pragma once

#include <box2d/b2_prismatic_joint.h>

#include "Joint.h"

namespace Physics
{
    /**
     * A prismatic joint.
     *
     * This joint connects the bodies at the anchor points and allows them to move along a specified axis.
     */
    class PrismaticJoint : public Joint
    {
    public:
        /**
         * Creates a new prismatic joint.
         *
         * @param connected The entity the joint is connected to.
         * @param worldAnchor The anchor point on the connected entity in world space.
         * @param worldAxis The axis the bodies are allowed to move along in world space.
         */
        PrismaticJoint(ECS::Entity connected, glm::vec2 worldAnchor, glm::vec2 worldAxis = glm::vec2(1, 0));

        /**
         * Sets the world anchor point of the prismatic joint.
         *
         * @param worldAnchor The world anchor point of the prismatic joint.
         */
        void setWorldAnchor(glm::vec2 worldAnchor);

        /**
         * Gets the world anchor point of the prismatic joint.
         *
         * @returns The world anchor point of the prismatic joint.
         */
        const glm::vec2 &getWorldAnchor() const;

        /**
         * Sets the world axis of the prismatic joint.
         *
         * @param worldAxis The world axis of the prismatic joint.
         */
        void setWorldAxis(glm::vec2 worldAxis);

        /**
         * Gets the world axis of the prismatic joint.
         *
         * @returns The world axis of the prismatic joint.
         */
        const glm::vec2 &getWorldAxis() const;

        /**
         * Sets the local axis of body a.
         *
         * The axis is in local space of the respective body.
         *
         * @param localAxisA The local axis of body a.
         */
        void setLocalAxisA(glm::vec2 localAxisA);

        /**
         * Gets the local axis of body a.
         *
         * The axis is in local space of the respective body.
         *
         * @returns The local axis of body a.
         */
        const glm::vec2 &getLocalAxisA() const;

        /**
         * Sets the anchor point of body a.
         *
         * The anchor point is in local space of the respective body.
         *
         * @param anchorA The anchor point of body a.
         */
        void setAnchorA(glm::vec2 anchorA);

        /**
         * Gets the anchor point of body a.
         *
         * The anchor point is in local space of the respective body.
         *
         * @returns The anchor point of body a.
         */
        const glm::vec2 &getAnchorA() const;

        /**
         * Sets the anchor point of body b.
         *
         * The anchor point is in local space of the respective body.
         *
         * @param anchorB The anchor point of body b.
         */
        void setAnchorB(glm::vec2 anchorB);

        /**
         * Gets the anchor point of body b.
         *
         * The anchor point is in local space of the respective body.
         *
         * @returns The anchor point of body b.
         */
        const glm::vec2 &getAnchorB() const;

        /**
         * Sets the reference angle of the prismatic joint.
         *
         * This is the angle between the bodies when the joint is created.
         *
         * @param referenceAngle The reference angle of the prismatic joint.
         */
        void setReferenceAngle(float referenceAngle);

        /**
         * Gets the reference angle of the prismatic joint.
         *
         * @returns The reference angle of the prismatic joint.
         */
        float getReferenceAngle() const;

        /**
         * Sets wether or not the prismatic joint has limits enabled (lower and upper translation limits).
         *
         * @param enable Wether or not the prismatic joint has limits enabled.
         */
        void enableLimit(bool enable);

        /**
         * Gets wether or not the prismatic joint has limits enabled (lower and upper translation limits).
         *
         * @returns Wether or not the prismatic joint has limits enabled.
         */
        bool isLimitEnabled() const;

        /**
         * Sets the lower translation limit of the prismatic joint.
         *
         * This is the minimum translation the joint will allow the bodies to move to.
         *
         * @param lowerTranslation The lower translation limit of the prismatic joint.
         */
        void setLowerTranslation(float lowerTranslation);

        /**
         * Gets the lower translation limit of the prismatic joint.
         *
         * @returns The lower translation limit of the prismatic joint.
         */
        float getLowerTranslation() const;

        /**
         * Sets the upper translation limit of the prismatic joint.
         *
         * This is the maximum translation the joint will allow the bodies to move to.
         *
         * @param upperTranslation The upper translation limit of the prismatic joint.
         */
        void setUpperTranslation(float upperTranslation);

        /**
         * Gets the upper translation limit of the prismatic joint.
         *
         * @returns The upper translation limit of the prismatic joint.
         */
        float getUpperTranslation() const;

        /**
         * Sets wether or not the prismatic joint has a motor enabled.
         *
         * @param enable Wether or not the prismatic joint has a motor enabled.
         */
        void enableMotor(bool enable);

        /**
         * Gets wether or not the prismatic joint has a motor enabled.
         *
         * @returns Wether or not the prismatic joint has a motor enabled.
         */
        bool isMotorEnabled() const;

        /**
         * Sets the maximum motor force of the prismatic joint.
         *
         * This is the maximum force the joint will apply to the bodies
         *
         * @param force The maximum motor force of the prismatic joint.
         */
        void setMaxMotorForce(float force);

        /**
         * Gets the maximum motor force of the prismatic joint.
         *
         * @returns The maximum motor force of the prismatic joint.
         */
        float getMaxMotorForce() const;

        /**
         * Sets the motor speed of the prismatic joint.
         *
         * This is the speed the joint will try to maintain.
         *
         * @param speed The motor speed of the prismatic joint.
         */
        void setMotorSpeed(float speed);

        /**
         * Gets the motor speed of the prismatic joint.
         *
         * @returns The motor speed of the prismatic joint.
         */
        float getMotorSpeed() const;

        /**
         * Sets the underlying Box2D joint.
         *
         * Must be a b2PrismaticJoint.
         *
         * @warning Do not use this unless you know what you are doing.
         *
         * @param joint The Box2D joint.
         */
        void setJoint(b2Joint *joint) override;

        /**
         * Creates the box2d prismatic joint.
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
        b2PrismaticJoint *createBox2DJoint(World::World &world, ECS::Entity entity, b2World *box2dWorld, b2Body *owner, b2Body *connected) override;

        /**
         * Gets the underlying Box2D joint.
         *
         * May be nullptr if the joint has not been created yet.
         *
         * @warning Do not use this unless you know what you are doing.
         *
         * @returns The underlying Box2D joint.
         */
        b2PrismaticJoint *getJoint() override;

        /**
         * Gets the underlying Box2D joint.
         *
         * May be nullptr if the joint has not been created yet.
         *
         * @warning Do not use this unless you know what you are doing.
         *
         * @returns The underlying Box2D joint.
         */
        const b2PrismaticJoint *getJoint() const override;

    private:
        b2PrismaticJoint *joint;

        bool useWorldAnchor = false;
        glm::vec2 worldAnchor;

        bool useWorldAxis = false;
        glm::vec2 worldAxis;

        glm::vec2 localAxisA;

        float referenceAngle = 0.0f;

        bool isEnableLimit = false;
        float lowerTranslation = 0.0f;
        float upperTranslation = 0.0f;

        bool isEnableMotor = false;
        float maxMotorForce = 0.0f;
        float motorSpeed = 0.0f;
    };
}