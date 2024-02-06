#include "../../include/Physics/RigidBody2D.h"

Physics::RigidBody2D::RigidBody2D()
{
    type = RigidBodyType::DYNAMIC;

    velocity = glm::vec2(0.0f);
    angularVelocity = 0.0f;
    linearDamping = 0.0f;
    angularDamping = 0.0f;

    fixedRotation = false;
    allowSleep = true;
    isInitiallyAwake = true;

    isBullet = false;
    startsEnabled = true;
    gravityScale = 1.0f;
}

Physics::RigidBody2D::~RigidBody2D()
{
}