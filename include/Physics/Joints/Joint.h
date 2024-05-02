#pragma once

#include "../../ECS/Entity.h"
#include "../../Core/Timestep.h"
#include "../../World/World.h"

#include <box2d/b2_world.h>
#include <box2d/b2_joint.h>
#include <glm/vec2.hpp>

namespace Physics
{
    /**
     * Represents the type of joint.
     */
    enum JointType
    {
        DISTANCE,
        REVOLUTE,
        PRISMATIC,
        PULLEY,
        GEAR,
        MOUSE,
        WHEEL,
    };

    /**
     * The user data attached to the b2Joint by the physics world.
     */
    struct JointUserData
    {
        ECS::Entity owner;
        ECS::Entity connected;
    };

    /**
     * Represents a joint between 2 entities.
     *
     * If the connected entity is destroyed or is no longer a valid body then the joint will be destroyed, and removed from the owning entity.
     */
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
         * The entity must be a valid rigidbody.
         *
         * @param connected The entity the joint is connected to.
         */
        virtual void setConnected(ECS::Entity connected);

        /**
         * Gets the entity the joint is connected to.
         *
         * @returns The entity the joint is connected to.
         */
        virtual ECS::Entity getConnected() const;

        /**
         * Sets wether or not the connected bodies should collide with eachother.
         *
         * @param collideConnected Wether or not the connected bodies should collide with eachother.
         */
        virtual void setCollideConnected(bool collideConnected);

        /**
         * Gets wether or not the connected bodies should collide with eachother.
         *
         * @returns Wether or not the connected bodies should collide with eachother.
         */
        virtual bool getCollideConnected() const;

        /**
         * Sets the anchor point on the first body (the one owning the joint component).
         *
         * This is in local space of the first body.
         *
         * @param anchor The anchor point.
         */
        virtual void setAnchorA(glm::vec2 anchor);

        /**
         * Gets the anchor point on the first body (the one owning the joint component).
         *
         * This is in local space of the first body.
         *
         * @returns The anchor point.
         */
        virtual const glm::vec2 &getAnchorA() const;

        /**
         * Sets the anchor point on the second body (the connected body).
         *
         * This is in local space of the second body.
         *
         * @param anchor The anchor point.
         */
        virtual void setAnchorB(glm::vec2 anchor);

        /**
         * Gets the anchor point on the second body (the connected body).
         *
         * This is in local space of the second body.
         *
         * @returns The anchor point.
         */
        virtual const glm::vec2 &getAnchorB() const;

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
         * This does not set the joint component's joint variable.
         *
         * @param world The world the entity is in.
         * @param entity The entity that owns the joint.
         * @param box2dWorld The box2d world to create the joint in.
         * @param owner The box2d body of the owning entity.
         * @param connected The box2d body of the connected entity.
         *
         * @returns The box2d joint.
         */
        virtual b2Joint *createBox2DJoint(World::World &world, ECS::Entity entity, b2World *box2dWorld, b2Body *owner, b2Body *connected) = 0;

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
         * @param type The type of the joint.
         */
        Joint(ECS::Entity connected, JointType type);

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