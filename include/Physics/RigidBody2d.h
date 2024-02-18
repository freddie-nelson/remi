#pragma once

#include "../ECS/Entity.h"
#include "../World/World.h"
#include "ContactInfo.h"

#include <functional>
#include <glm/vec2.hpp>
#include <box2d/b2_body.h>

namespace Physics
{
    /**
     * The type of a rigid body.
     *
     * STATIC: A body that does not move.
     * DYNAMIC: A body that is affected by forces and collisions.
     * KINEMATIC: A body that is not affected by forces but can be moved manually.
     *
     * This matches the order of b2BodyType in Box2D.
     */
    enum RigidBodyType
    {
        STATIC = 0,
        KINEMATIC,
        DYNAMIC,
    };

    /**
     * A callback for when two rigid bodies collide.
     *
     * @param contactInfo The information about the contact.
    */
    using RigidBodyCollisionCallback = std::function<void(const ContactInfo &)>;

    /**
     * Represents a 2D rigid body.
     *
     * @note Static and kinematic bodies do not collide with each other, they only collide with other dynamic bodies.
     */
    class RigidBody2D
    {
    public:
        /**
         * Creates a new 2D rigid body.
         */
        RigidBody2D();

        /**
         * Destroys the 2D rigid body.
         */
        ~RigidBody2D();

        /**
         * Applies a force to the body at its center.
         *
         * If wake is true the body will be woken up if it is sleeping.
         *
         * The velocities of the body may not be updated until the next physics updated.
         *
         * @param force The force to apply.
         * @param wake Whether or not to wake the body up.
         *
         * @throws std::runtime_error If the box2d body has not been created yet.
         */
        void applyForce(const glm::vec2 &force, bool wake = true);

        /**
         * Applies a force to the body at the given point.
         *
         * If wake is true the body will be woken up if it is sleeping.
         *
         * The point should be in world space.
         *
         * The velocities of the body may not be updated until the next physics update.
         *
         * @param force The force to apply.
         * @param point The point to apply the force at.
         * @param wake Whether or not to wake the body up.
         *
         * @throws std::runtime_error If the box2d body has not been created yet.
         */
        void applyForce(const glm::vec2 &force, const glm::vec2 &point, bool wake = true);

        /**
         * Applies a linear impulse to the body at its center.
         * 
         * If wake is true the body will be woken up if it is sleeping.
         * 
         * The velocities of the body may not be updated until the next physics update.
         *
         * @param impulse The impulse to apply.
         * @param wake Whether or not to wake the body up.
        */
        void applyLinearImpulse(const glm::vec2 &impulse, bool wake = true);

        /**
         * Applies a linear impulse to the body at the given point.
         *
         * If wake is true the body will be woken up if it is sleeping.
         *
         * The point should be in world space.
         *
         * The velocities of the body may not be updated until the next physics update.
         *
         * @param impulse The impulse to apply.
         * @param point The point to apply the impulse at.
         * @param wake Whether or not to wake the body up.
         */
        void applyLinearImpulse(const glm::vec2 &impulse, const glm::vec2 &point, bool wake = true);

        /**
         * Applies a torque to the body.
         *
         * If wake is true the body will be woken up if it is sleeping.
         *
         * The angular velocity of the body may not be updated until the next physics update.
         *
         * @param torque The torque to apply.
         * @param wake Whether or not to wake the body up.
         */
        void applyTorque(float torque, bool wake = true);  

        /**
         * Applies an angular impulse to the body.
         *
         * If wake is true the body will be woken up if it is sleeping.
         *
         * The angular velocity of the body may not be updated until the next physics update.
         *
         * @param impulse The impulse to apply.
         * @param wake Whether or not to wake the body up.
         */
        void applyAngularImpulse(float impulse, bool wake = true);

        /**
         * Starts contact between two rigid bodies.
         * 
         * This informs the rigid body that it has begun contact/collision with another rigid body.
         *
         * This will call the begin contact callback if it is set.
         *
         * @param contactInfo The information about the contact.
        */
        void beginContact(const ContactInfo &contactInfo);

        /**
         * Ends contact between two rigid bodies.
         * 
         * This informs the rigid body that it has stopped contact/collision with another rigid body.
         *
         * This will call the end contact callback if it is set.
         *
         * @param contactInfo The information about the contact.
        */
        void endContact(const ContactInfo &contactInfo);

        /**
         * Sets the callback for when two rigid bodies start colliding.
         *
         * This is called when the bodies first start touching / have just collided.
         *
         * @param callback The callback for when two rigid bodies start contact.
        */
        void setBeginContactCallback(RigidBodyCollisionCallback callback);

        /**
         * Sets the callback for when two rigid bodies stop colliding.
         *
         * This is called when the bodies stop touching / separate.
         *
         * @param callback The callback for when two rigid bodies stop contact.
        */
        void setEndContactCallback(RigidBodyCollisionCallback callback);

        /**
         * Gets the type of the body.
         *
         * @returns The type of the body.
         */
        RigidBodyType getType() const;

        /**
         * Sets the type of the body.
         *
         * @note Static and kinematic bodies do not collide with each other, they only collide with other dynamic bodies.
         *
         * @param type The type of the body.
         */
        void setType(RigidBodyType type);

        /**
         * Gets the linear velocity of the body.
         *
         * @returns The linear velocity of the body.
         */
        const glm::vec2 &getVelocity() const;

        /**
         * Sets the linear velocity of the body.
         *
         * @param velocity The linear velocity of the body.
         */
        void setVelocity(glm::vec2 velocity);

        /**
         * Sets the linear x velocity of the body.
         *
         * @param x The linear x velocity of the body.
         */
        void setVelocityX(float x);

        /**
         * Gets the linear x velocity of the body.
         *
         * @returns The linear x velocity of the body.
         */
        float getVelocityX() const;

        /**
         * Sets the linear y velocity of the body.
         *
         * @param y The linear y velocity of the body.
         */
        void setVelocityY(float y);

        /**
         * Gets the linear y velocity of the body.
         *
         * @returns The linear y velocity of the body.
         */
        float getVelocityY() const;

        /**
         * Gets the angular velocity of the body.
         *
         * @returns The angular velocity of the body.
         */
        float getAngularVelocity() const;

        /**
         * Sets the angular velocity of the body.
         *
         * @param velocity The angular velocity of the body.
         */
        void setAngularVelocity(float velocity);

        /**
         * Gets the linear damping of the body.
         *
         * @returns The linear damping of the body.
         */
        float getLinearDamping() const;

        /**
         * Sets the linear damping of the body.
         *
         * @param damping The linear damping of the body.
         */
        void setLinearDamping(float damping);

        /**
         * Gets the angular damping of the body.
         *
         * @returns The angular damping of the body.
         */
        float getAngularDamping() const;

        /**
         * Sets the angular damping of the body.
         *
         * @param damping The angular damping of the body.
         */
        void setAngularDamping(float damping);

        /**
         * Gets whether or not the body is allowed to sleep.
         *
         * @returns Whether or not the body is allowed to sleep.
         */
        bool getAllowSleep() const;

        /**
         * Sets whether or not the body is allowed to sleep.
         *
         * @param allowSleep Whether or not the body is allowed to sleep.
         */
        void setAllowSleep(bool allowSleep);

        /**
         * Gets whether or not the body is awake.
         *
         * @returns Whether or not the body is awake.
         */
        bool getIsAwake() const;

        /**
         * Sets whether or not the body is awake.
         *
         * @param isAwake Whether or not the body is awake.
         */
        void setIsAwake(bool isAwake);

        /**
         * Gets whether or not the body has fixed rotation.
         *
         * @returns Whether or not the body has fixed rotation.
         */
        bool getFixedRotation() const;

        /**
         * Sets whether or not the body has fixed rotation.
         *
         * If this is true the body will never rotate unless manually rotated.
         *
         * This is useful for characters and other objects that should not rotate.
         *
         * @param fixedRotation Whether or not the body has fixed rotation.
         */
        void setFixedRotation(bool fixedRotation);

        /**
         * Gets whether or not the body is a bullet.
         *
         * @returns Whether or not the body is a bullet.
         */
        bool getIsBullet() const;

        /**
         * Sets whether or not the body is a bullet.
         *
         * A bullet is a high-speed body that should be prevented from tunneling through other dynamic bodies.
         *
         * @param isBullet Whether or not the body is a bullet.
         */
        void setIsBullet(bool isBullet);

        /**
         * Gets whether or not the body is enabled.
         *
         * @returns Whether or not the body is enabled.
         */
        bool getIsEnabled() const;

        /**
         * Sets whether or not the body is enabled.
         *
         * @param isEnabled Whether or not the body is enabled.
         */
        void setIsEnabled(bool isEnabled);

        /**
         * Gets the scale applied to gravity for this body.
         *
         * @returns The scale applied to gravity for this body.
         */
        float getGravityScale() const;

        /**
         * Sets the scale applied to gravity for this body.
         *
         * @param scale The scale applied to gravity for this body.
         */
        void setGravityScale(float scale);

        /**
         * Gets the mass of the body.
         *
         * This is calculated from the bodies fixtures/colliders.
         *
         * If the box2d body has not been created yet this will return 0.
         *
         * @returns The mass of the body.
         */
        float getMass() const;

        /**
         * Gets the underlying Box2D body.
         *
         * @warning Do not use this unless you know what you are doing.
         *
         * @returns The underlying Box2D body.
         */
        b2Body *getBody();

        /**
         * Sets the underlying Box2D body.
         *
         * @warning Do not use this unless you know what you are doing.
         *
         * @param body The underlying Box2D body.
         */
        void setBody(b2Body *body);

    protected:
        /**
         * The underlying Box2D body.
         */
        b2Body *body = nullptr;

        /**
         * The callback for when two rigid bodies collide.
        */
        RigidBodyCollisionCallback beginContactCallback = nullptr;

        /**
         * The callback for when two rigid bodies stop colliding.
        */
        RigidBodyCollisionCallback endContactCallback = nullptr;

        /**
         * The type of the body.
         */
        RigidBodyType type = RigidBodyType::DYNAMIC;

        /**
         * The linear velocity of the body.
         */
        glm::vec2 velocity = glm::vec2(0.0f);

        /**
         * The angular velocity of the body.
         */
        float angularVelocity = 0.0f;

        /**
         * The linear damping of the body.
         */
        float linearDamping = 0.0f;

        /**
         * The angular damping of the body.
         */
        float angularDamping = 0.0f;

        /**
         * Whether or not the body is allowed to sleep.
         *
         * If this is false it can increase process time.
         */
        bool allowSleep = true;

        /**
         * Whether or not the body is awake.
         */
        bool isAwake = true;

        /**
         * If this is true the body will never rotate unless manually rotated.
         *
         * This is useful for characters and other objects that should not rotate.
         */
        bool fixedRotation = false;

        /**
         * Whether or not the body is a bullet.
         *
         * A bullet is a high-speed body that should be prevented from tunneling through other dynamic bodies.
         */
        bool isBullet = false;

        /**
         * Whether or not the body is enabled.
         */
        bool isEnabled = true;

        /**
         * The scale applied to gravity for this body.
         */
        float gravityScale = 1.0f;
    };
}