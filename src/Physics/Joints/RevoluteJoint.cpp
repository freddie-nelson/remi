#include "../../../include/Physics/Joints/RevoluteJoint.h"

#include <stdexcept>

Physics::RevoluteJoint::RevoluteJoint(ECS::Entity connected, glm::vec2 anchorA, glm::vec2 anchorB) : Joint(connected)
{
    this->anchorA = anchorA;
    this->anchorB = anchorB;
}

void Physics::RevoluteJoint::setReferenceAngle(float referenceAngle)
{
    this->referenceAngle = referenceAngle;

    if (joint != nullptr)
    {
        setJoint(nullptr);
    }
}

float Physics::RevoluteJoint::getReferenceAngle() const
{
    return referenceAngle;
}

void Physics::RevoluteJoint::enableLimit(bool enable)
{
    isEnableLimit = enable;

    if (joint != nullptr)
    {
        joint->EnableLimit(enable);
    }
}

bool Physics::RevoluteJoint::isLimitEnabled() const
{
    return isEnableLimit;
}

void Physics::RevoluteJoint::setLowerAngle(float lowerAngle)
{
    this->lowerAngle = lowerAngle;

    if (joint != nullptr)
    {
        joint->SetLimits(lowerAngle, upperAngle);
    }
}

float Physics::RevoluteJoint::getLowerAngle() const
{
    return lowerAngle;
}

void Physics::RevoluteJoint::setUpperAngle(float upperAngle)
{
    this->upperAngle = upperAngle;

    if (joint != nullptr)
    {
        joint->SetLimits(lowerAngle, upperAngle);
    }
}

float Physics::RevoluteJoint::getUpperAngle() const
{
    return upperAngle;
}

void Physics::RevoluteJoint::enableMotor(bool enable)
{
    isEnableMotor = enable;

    if (joint != nullptr)
    {
        joint->EnableMotor(enable);
    }
}

bool Physics::RevoluteJoint::isMotorEnabled() const
{
    return isEnableMotor;
}

void Physics::RevoluteJoint::setMotorSpeed(float speed)
{
    this->motorSpeed = speed;

    if (joint != nullptr)
    {
        joint->SetMotorSpeed(speed);
    }
}

float Physics::RevoluteJoint::getMotorSpeed() const
{
    return motorSpeed;
}

void Physics::RevoluteJoint::setMaxMotorTorque(float torque)
{
    this->maxMotorTorque = torque;

    if (joint != nullptr)
    {
        joint->SetMaxMotorTorque(torque);
    }
}

float Physics::RevoluteJoint::getMaxMotorTorque() const
{
    return maxMotorTorque;
}

void Physics::RevoluteJoint::setJoint(b2Joint *joint)
{
    if (joint->GetType() != b2JointType::e_revoluteJoint)
    {
        throw std::invalid_argument("RevoluteJoint (setJoint): joint must be a revolute joint");
    }

    this->joint = reinterpret_cast<b2RevoluteJoint *>(joint);
}

b2RevoluteJoint *Physics::RevoluteJoint::createBox2DJoint(World::World &world, ECS::Entity entity, b2World *box2dWorld, b2Body *owner, b2Body *connected)
{
    b2RevoluteJointDef jointDef;
    jointDef.bodyA = owner;
    jointDef.bodyB = connected;
    jointDef.collideConnected = getCollideConnected();
    jointDef.localAnchorA = b2Vec2(anchorA.x, anchorA.y);
    jointDef.localAnchorB = b2Vec2(anchorB.x, anchorB.y);
    jointDef.referenceAngle = referenceAngle;
    jointDef.enableLimit = isEnableLimit;
    jointDef.lowerAngle = lowerAngle;
    jointDef.upperAngle = upperAngle;
    jointDef.enableMotor = isEnableMotor;
    jointDef.motorSpeed = motorSpeed;
    jointDef.maxMotorTorque = maxMotorTorque;

    auto userData = new JointUserData{entity, this->connected};
    jointDef.userData.pointer = reinterpret_cast<uintptr_t>(userData);

    auto joint = box2dWorld->CreateJoint(&jointDef);
    if (joint->GetType() != b2JointType::e_revoluteJoint)
    {
        throw std::runtime_error("RevoluteJoint (createBox2DJoint): The created joint is not a revolute joint. Something has gone terribly wrong.");
    }

    return reinterpret_cast<b2RevoluteJoint *>(joint);
}

b2RevoluteJoint *Physics::RevoluteJoint::getJoint()
{
    return joint;
}

const b2RevoluteJoint *Physics::RevoluteJoint::getJoint() const
{
    return joint;
}