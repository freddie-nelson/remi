#pragma once

#include "../../ECS/Entity.h"
#include "../../Core/Timestep.h"
#include "../../World/World.h"
#include "../PhysicsWorld.h"

#include <box2d/b2_joint.h>
#include <glm/vec2.hpp>

namespace Physics
{
    enum JointType
    {
        DISTANCE,
    };

    class Joint
    {
    public:
        virtual ~Joint() = default;

        /**
         * Gets the type of the joint.
         *
         * @returns The type of the joint.
         */
        JointType getType() const;

        /**
         * Sets the entity the joint is connected to.
         *
         * @param connected The entity the joint is connected to.
         */
        void setConnected(ECS::Entity connected);

        /**
         * Gets the entity the joint is connected to.
         *
         * @returns The entity the joint is connected to.
         */
        ECS::Entity getConnected() const;

        /**
         * Sets wether or not the connected bodies should collide with eachother.
         *
         * @param collideConnected Wether or not the connected bodies should collide with eachother.
         */
        void setCollideConnected(bool collideConnected);

        /**
         * Gets wether or not the connected bodies should collide with eachother.
         *
         * @returns Wether or not the connected bodies should collide with eachother.
         */
        bool getCollideConnected() const;

        /**
         * Sets the anchor point on the first body (the one owning the joint component).
         *
         * This is in local space of the first body.
         *
         * @param anchor The anchor point.
         */
        void setAnchorA(glm::vec2 anchor);

        /**
         * Gets the anchor point on the first body (the one owning the joint component).
         *
         * This is in local space of the first body.
         *
         * @returns The anchor point.
         */
        const glm::vec2 &getAnchorA() const;

        /**
         * Sets the anchor point on the second body (the connected body).
         *
         * This is in local space of the second body.
         *
         * @param anchor The anchor point.
         */
        void setAnchorB(glm::vec2 anchor);

        /**
         * Gets the anchor point on the second body (the connected body).
         *
         * This is in local space of the second body.
         *
         * @returns The anchor point.
         */
        const glm::vec2 &getAnchorB() const;

        /**
         * Sets the maximum reaction force the joint can withstand before breaking.
         *
         * 0.0f means it will never break from force.
         *
         * @param force The maximum reaction force the joint can withstand before breaking.
         */
        void setBreakForce(float force);

        /**
         * Gets the maximum reaction force the joint can withstand before breaking.
         *
         * 0.0f means it will never break from force.
         *
         * @returns The maximum reaction force the joint can withstand before breaking.
         */
        float getBreakForce() const;

        /**
         * Sets the maximum reaction torque the joint can withstand before breaking.
         *
         * 0.0f means it will never break from torque.
         *
         * @param torque The maximum reaction torque the joint can withstand before breaking.
         */
        void setBreakTorque(float torque);

        /**
         * Gets the maximum reaction torque the joint can withstand before breaking.
         *
         * 0.0f means it will never break from torque.
         *
         * @returns The maximum reaction torque the joint can withstand before breaking.
         */
        float getBreakTorque() const;

        /**
         * Gets the reaction force on the joint.
         *
         * This may perform some calculations.
         *
         * @param timestep The current timestep
         *
         * @returns The reaction force on the joint.
         */
        glm::vec2 getReactionForce(const Core::Timestep &timestep) const;

        /**
         * Gets the reaction torque on the joint.
         *
         * This may perform some calculations.
         *
         * @param timestep The current timestep
         *
         * @returns The reaction torque on the joint.
         */
        float getReactionTorque(const Core::Timestep &timestep) const;

        /**
         * Creates the box2d joint.
         *
         * This creates the specific box2d joint type and returns it.
         *
         * @param world The world the joint is in.
         * @param physicsWorld The physics world the joint is in.
         * @param owner The entity that owns the joint. (The first body in the joint, not connected body)
         *
         * @returns The box2d joint.
         */
        virtual b2Joint *createBox2DJoint(World::World &world, PhysicsWorld &physicsWorld, ECS::Entity owner) = 0;

        /**
         * Sets the underlying box2d joint.
         *
         * @warning Do not use this unless you know what you are doing.
         *
         * @param joint The box2d joint.
         */
        virtual void setJoint(b2Joint *joint) = 0;

        /**
         * Gets the underlying box2d joint.
         *
         * May be nullptr if the joint has not been created yet.
         *
         * @warning Do not use this unless you know what you are doing.
         *
         * @returns The box2d joint.
         */
        virtual b2Joint *getJoint() = 0;

        /**
         * Gets the underlying box2d joint.
         *
         * May be nullptr if the joint has not been created yet.
         *
         * @warning Do not use this unless you know what you are doing.
         *
         * @returns The box2d joint.
         */
        virtual const b2Joint *getJoint() const = 0;

    protected:
        /**
         * Creates a new joint.
         *
         * Only needs to know about connected body as the joint is added as a component to the first body.
         *
         * @param connected The entity to connect to (must have a rigidbody).
         */
        Joint(ECS::Entity connected);

        /**
         * The type of the joint.
         */
        JointType type;

        /**
         * The connected body.
         */
        ECS::Entity connected;

        /**
         * Wether or not the connected bodies should collide with eachother.
         */
        bool collideConnected = false;

        /**
         * The anchor point on the first body (the one owning the joint component).
         *
         * This is in local space of the first body.
         */
        glm::vec2 anchorA = glm::vec2(0.0f);

        /**
         * The anchor point on the second body (the connected body).
         *
         * This is in local space of the second body.
         */
        glm::vec2 anchorB = glm::vec2(0.0f);

        /**
         * The maximum force the joint can withstand before breaking.
         *
         * 0.0f means it will never break from force.
         *
         * Should be set in sub classes.
         */
        float breakForce = 0.0f;

        /**
         * The maximum torque the joint can withstand before breaking.
         *
         * 0.0f means it will never break from torque.
         *
         * Should be set in sub classes.
         */
        float breakTorque = 0.0f;
    };
}