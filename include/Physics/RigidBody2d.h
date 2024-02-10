#pragma once

#include <glm/vec2.hpp>

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
     * Represents a 2D rigid body.
     */
    struct RigidBody2D
    {
        /**
         * Creates a new 2D rigid body.
         */
        RigidBody2D();

        /**
         * Destroys the 2D rigid body.
         */
        ~RigidBody2D();

        /**
         * The type of the body.
         */
        RigidBodyType type;

        /**
         * The linear velocity of the body.
         */
        glm::vec2 velocity;

        /**
         * The angular velocity of the body.
         */
        float angularVelocity;

        /**
         * The linear damping of the body.
         */
        float linearDamping;

        /**
         * The angular damping of the body.
         */
        float angularDamping;

        /**
         * Whether or not the body is allowed to sleep.
         *
         * If this is false it can increase process time.
         */
        bool allowSleep;

        /**
         * Whether or not the body is initially awake.
         */
        bool isAwake;

        /**
         * If this is true the body will never rotate unless manually rotate.
         *
         * This is useful for characters and other objects that should not rotate.
         */
        bool fixedRotation;

        /**
         * Whether or not the body is a bullet.
         *
         * A bullet is a high-speed body that should be prevented from tunneling through other dynamic bodies.
         */
        bool isBullet;

        /**
         * Whether or not the body is enabled.
         */
        bool isEnabled;

        /**
         * The scale applied to gravity for this body.
         */
        float gravityScale;
    };
}