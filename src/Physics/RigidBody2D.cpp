#include "../../include/Physics/RigidBody2D.h"

#include <stdexcept>

Physics::RigidBody2D::RigidBody2D()
{
}

Physics::RigidBody2D::~RigidBody2D()
{
}

void Physics::RigidBody2D::applyForce(const glm::vec2 &force, bool wake)
{
    if (body == nullptr)
    {
        throw std::runtime_error("RigidBody2D (applyForce): Box2d body has not been created yet.");
    }

    body->ApplyForceToCenter(b2Vec2(force.x, force.y), wake);
}

void Physics::RigidBody2D::applyForce(const glm::vec2 &force, const glm::vec2 &point, bool wake)
{
    if (body == nullptr)
    {
        throw std::runtime_error("RigidBody2D (applyForce): Box2d body has not been created yet.");
    }

    body->ApplyForce(b2Vec2(force.x, force.y), b2Vec2(point.x, point.y), wake);
}

void Physics::RigidBody2D::applyLinearImpulse(const glm::vec2 &impulse, bool wake)
{
    if (body == nullptr)
    {
        throw std::runtime_error("RigidBody2D (applyLinearImpulse): Box2d body has not been created yet.");
    }

    body->ApplyLinearImpulseToCenter(b2Vec2(impulse.x, impulse.y), wake);
}

void Physics::RigidBody2D::applyLinearImpulse(const glm::vec2 &impulse, const glm::vec2 &point, bool wake)
{
    if (body == nullptr)
    {
        throw std::runtime_error("RigidBody2D (applyLinearImpulse): Box2d body has not been created yet.");
    }

    body->ApplyLinearImpulse(b2Vec2(impulse.x, impulse.y), b2Vec2(point.x, point.y), wake);
}

void Physics::RigidBody2D::applyTorque(float torque, bool wake)
{
    if (body == nullptr)
    {
        throw std::runtime_error("RigidBody2D (applyTorque): Box2d body has not been created yet.");
    }

    body->ApplyTorque(torque, wake);
}

void Physics::RigidBody2D::applyAngularImpulse(float impulse, bool wake)
{
    if (body == nullptr)
    {
        throw std::runtime_error("RigidBody2D (applyAngularImpulse): Box2d body has not been created yet.");
    }

    body->ApplyAngularImpulse(impulse, wake);
}

void Physics::RigidBody2D::beginContact(const ContactInfo &contactInfo) {
    if (beginContactCallback != nullptr) {
        beginContactCallback(contactInfo);
    }
}

void Physics::RigidBody2D::endContact(const ContactInfo &contactInfo) {
    if (endContactCallback != nullptr) {
        endContactCallback(contactInfo);
    }
}

void Physics::RigidBody2D::setBeginContactCallback(RigidBodyCollisionCallback callback) {
    beginContactCallback = callback;
}

void Physics::RigidBody2D::setEndContactCallback(RigidBodyCollisionCallback callback) {
    endContactCallback = callback;
}

Physics::RigidBodyType Physics::RigidBody2D::getType() const
{
    return type;
}

void Physics::RigidBody2D::setType(RigidBodyType type)
{
    this->type = type;

    if (body != nullptr)
    {
        body->SetType(static_cast<b2BodyType>(type));
        velocity.x = body->GetLinearVelocity().x;
        velocity.y = body->GetLinearVelocity().y;
    }
}

const glm::vec2 &Physics::RigidBody2D::getVelocity() const
{
    return velocity;
}

void Physics::RigidBody2D::setVelocity(glm::vec2 velocity)
{
    this->velocity = velocity;

    if (body != nullptr)
    {
        body->SetLinearVelocity(b2Vec2(velocity.x, velocity.y));
    }
}

void Physics::RigidBody2D::setVelocityX(float x)
{
    velocity.x = x;

    if (body != nullptr)
    {
        body->SetLinearVelocity(b2Vec2(x, velocity.y));
    }
}

float Physics::RigidBody2D::getVelocityX() const
{
    return velocity.x;
}

void Physics::RigidBody2D::setVelocityY(float y)
{
    velocity.y = y;

    if (body != nullptr)
    {
        body->SetLinearVelocity(b2Vec2(velocity.x, y));
    }
}

float Physics::RigidBody2D::getVelocityY() const
{
    return velocity.y;
}

float Physics::RigidBody2D::getAngularVelocity() const
{
    return angularVelocity;
}

void Physics::RigidBody2D::setAngularVelocity(float angularVelocity)
{
    this->angularVelocity = angularVelocity;

    if (body != nullptr)
    {
        body->SetAngularVelocity(angularVelocity);
    }
}

float Physics::RigidBody2D::getLinearDamping() const
{
    return linearDamping;
}

void Physics::RigidBody2D::setLinearDamping(float damping)
{
    linearDamping = damping;

    if (body != nullptr)
    {
        body->SetLinearDamping(damping);
    }
}

float Physics::RigidBody2D::getAngularDamping() const
{
    return angularDamping;
}

void Physics::RigidBody2D::setAngularDamping(float damping)
{
    angularDamping = damping;

    if (body != nullptr)
    {
        body->SetAngularDamping(damping);
    }
}

bool Physics::RigidBody2D::getFixedRotation() const
{
    return fixedRotation;
}

void Physics::RigidBody2D::setFixedRotation(bool fixedRotation)
{
    this->fixedRotation = fixedRotation;

    if (body != nullptr)
    {
        body->SetFixedRotation(fixedRotation);
    }
}

bool Physics::RigidBody2D::getAllowSleep() const
{
    return allowSleep;
}

void Physics::RigidBody2D::setAllowSleep(bool allowSleep)
{
    this->allowSleep = allowSleep;

    if (body != nullptr)
    {
        body->SetSleepingAllowed(allowSleep);
    }
}

bool Physics::RigidBody2D::getIsAwake() const
{
    return isAwake;
}

void Physics::RigidBody2D::setIsAwake(bool isAwake)
{
    this->isAwake = isAwake;

    if (body != nullptr)
    {
        body->SetAwake(isAwake);
    }
}

bool Physics::RigidBody2D::getIsBullet() const
{
    return isBullet;
}

void Physics::RigidBody2D::setIsBullet(bool isBullet)
{
    this->isBullet = isBullet;

    if (body != nullptr)
    {
        body->SetBullet(isBullet);
    }
}

bool Physics::RigidBody2D::getIsEnabled() const
{
    return isEnabled;
}

void Physics::RigidBody2D::setIsEnabled(bool isEnabled)
{
    this->isEnabled = isEnabled;

    if (body != nullptr)
    {
        body->SetEnabled(isEnabled);
    }
}

float Physics::RigidBody2D::getGravityScale() const
{
    return gravityScale;
}

void Physics::RigidBody2D::setGravityScale(float gravityScale)
{
    this->gravityScale = gravityScale;

    if (body != nullptr)
    {
        body->SetGravityScale(gravityScale);
    }
}

b2Body *Physics::RigidBody2D::getBody()
{
    return body;
}

void Physics::RigidBody2D::setBody(b2Body *body)
{
    this->body = body;
}

float Physics::RigidBody2D::getMass() const
{
    if (body != nullptr)
    {
        return body->GetMass();
    }

    return 0.0f;
}