#include "../../../include/Physics/Joints/Joint.h"

Physics::JointType Physics::Joint::getType() const
{
    return type;
}

void Physics::Joint::setConnected(ECS::Entity connected)
{
    this->connected = connected;
}

ECS::Entity Physics::Joint::getConnected() const
{
    return connected;
}

void Physics::Joint::setCollideConnected(bool collideConnected)
{
    this->collideConnected = collideConnected;
}

bool Physics::Joint::getCollideConnected() const
{
    return collideConnected;
}

void Physics::Joint::setAnchorA(glm::vec2 anchor)
{
    anchorA = anchor;
}

const glm::vec2 &Physics::Joint::getAnchorA() const
{
    return anchorA;
}

void Physics::Joint::setAnchorB(glm::vec2 anchor)
{
    anchorB = anchor;
}

const glm::vec2 &Physics::Joint::getAnchorB() const
{
    return anchorB;
}

void Physics::Joint::setBreakForce(float force)
{
    breakForce = force;
}

float Physics::Joint::getBreakForce() const
{
    return breakForce;
}

void Physics::Joint::setBreakTorque(float torque)
{
    breakTorque = torque;
}

float Physics::Joint::getBreakTorque() const
{
    return breakTorque;
}

glm::vec2 Physics::Joint::getReactionForce(const Core::Timestep &timestep) const
{
    auto v = getJoint()->GetReactionForce(1 / timestep.getSeconds());

    return glm::vec2(v.x, v.y);
}

float Physics::Joint::getReactionTorque(const Core::Timestep &timestep) const
{
    return getJoint()->GetReactionTorque(1 / timestep.getSeconds());
}

Physics::Joint::Joint(ECS::Entity connected) : connected(connected) {}
